/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
 *
 *	*** BEGIN LICENSE INFORMATION ***
 *	
 *	The contents of this file are subject to the Common Public Attribution License 
 *	Version 1.0 (the "License"); you may not use this file except in compliance with 
 *	the License. You may obtain a copy of the License at 
 *	http://lass.sourceforge.net/cpal-license. The License is based on the 
 *	Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *	use of software over a computer network and provide for limited attribution for 
 *	the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *	with Exhibit B.
 *	
 *	Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *	WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *	language governing rights and limitations under the License.
 *	
 *	The Original Code is LASS - Library of Assembled Shared Sources.
 *	
 *	The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *	The Original Developer is the Initial Developer.
 *	
 *	All portions of the code written by the Initial Developer are:
 *	Copyright (C) 2004-2023 the Initial Developer.
 *	All Rights Reserved.
 *	
 *	Contributor(s):
 *
 *	Alternatively, the contents of this file may be used under the terms of the 
 *	GNU General Public License Version 2 or later (the GPL), in which case the 
 *	provisions of GPL are applicable instead of those above.  If you wish to allow use
 *	of your version of this file only under the terms of the GPL and not to allow 
 *	others to use your version of this file under the CPAL, indicate your decision by 
 *	deleting the provisions above and replace them with the notice and other 
 *	provisions required by the GPL License. If you do not delete the provisions above,
 *	a recipient may use your version of this file under either the CPAL or the GPL.
 *	
 *	*** END LICENSE INFORMATION ***
 */

#include "test_common.h"

#include "../lass/util/shared_ptr.h"
#include "../lass/stde/lock_free_spmc_ring_buffer.h"

#include <thread>

namespace
{

class Gizmo: lass::util::NonCopyable
{
public:
    explicit Gizmo(size_t value): value_(value)
    {
        constructed_.fetch_add(1, std::memory_order_relaxed);
    }
    ~Gizmo()
    {
        value_ = 0;
        deconstructed_.fetch_add(1, std::memory_order_relaxed);
    }
    operator size_t() const { return value_; }
    static size_t constructed() { return constructed_.load(std::memory_order_relaxed); }
    static size_t deconstructed() { return deconstructed_.load(std::memory_order_relaxed); }
private:
    size_t value_;
    static std::atomic<size_t> constructed_;
    static std::atomic<size_t> deconstructed_;
};

alignas(LASS_LOCK_FREE_ALIGNMENT) std::atomic<size_t> Gizmo::constructed_{ 0 };
alignas(LASS_LOCK_FREE_ALIGNMENT) std::atomic<size_t> Gizmo::deconstructed_{ 0 };

class GizmoWithCounter: lass::util::NonCopyable
{
public:
    explicit GizmoWithCounter(size_t value): value_(value)
    {
        constructed_.fetch_add(1, std::memory_order_relaxed);
    }
    ~GizmoWithCounter()
    {
        value_ = 0;
        deconstructed_.fetch_add(1, std::memory_order_relaxed);
    }
    operator size_t() const { return value_; }
    static size_t constructed() { return constructed_.load(std::memory_order_relaxed); }
    static size_t deconstructed() { return deconstructed_.load(std::memory_order_relaxed); }
    std::atomic<int> refCount;
private:
    size_t value_;
    static std::atomic<size_t> constructed_;
    static std::atomic<size_t> deconstructed_;
};

alignas(LASS_LOCK_FREE_ALIGNMENT) std::atomic<size_t> GizmoWithCounter::constructed_{ 0 };
alignas(LASS_LOCK_FREE_ALIGNMENT) std::atomic<size_t> GizmoWithCounter::deconstructed_{ 0 };

}

namespace lass
{
namespace test
{

void testUtilSharedPtr()
{
    using namespace lass::util;

    typedef SharedPtr<float> TSharedFloat;

    float* p = new float(3.14f);
    TSharedFloat a(p);
    LASS_TEST_CHECK_EQUAL(a.get(), p);
    LASS_TEST_CHECK_EQUAL(a.count(), static_cast<TSharedFloat::TCount>(1));
    TSharedFloat b = a;
    LASS_TEST_CHECK_EQUAL(a.get(), p);
    LASS_TEST_CHECK_EQUAL(a.count(), static_cast<TSharedFloat::TCount>(2));
    LASS_TEST_CHECK_EQUAL(b.get(), p);
    LASS_TEST_CHECK_EQUAL(b.count(), static_cast<TSharedFloat::TCount>(2));
    TSharedFloat c = std::move(a);
    LASS_TEST_CHECK_EQUAL(a.get(), static_cast<float*>(nullptr));
    LASS_TEST_CHECK_EQUAL(a.count(), static_cast<TSharedFloat::TCount>(0));
    LASS_TEST_CHECK_EQUAL(b.get(), p);
    LASS_TEST_CHECK_EQUAL(b.count(), static_cast<TSharedFloat::TCount>(2));
    LASS_TEST_CHECK_EQUAL(c.get(), p);
    LASS_TEST_CHECK_EQUAL(c.count(), static_cast<TSharedFloat::TCount>(2));
    a = c;
    LASS_TEST_CHECK_EQUAL(a.get(), p);
    LASS_TEST_CHECK_EQUAL(a.count(), static_cast<TSharedFloat::TCount>(3));
    LASS_TEST_CHECK_EQUAL(b.get(), p);
    LASS_TEST_CHECK_EQUAL(b.count(), static_cast<TSharedFloat::TCount>(3));
    LASS_TEST_CHECK_EQUAL(c.get(), p);
    LASS_TEST_CHECK_EQUAL(c.count(), static_cast<TSharedFloat::TCount>(3));
    a = std::move(c);
    LASS_TEST_CHECK_EQUAL(a.get(), p);
    LASS_TEST_CHECK_EQUAL(a.count(), static_cast<TSharedFloat::TCount>(2));
    LASS_TEST_CHECK_EQUAL(b.get(), p);
    LASS_TEST_CHECK_EQUAL(b.count(), static_cast<TSharedFloat::TCount>(2));
    LASS_TEST_CHECK_EQUAL(c.get(), static_cast<float*>(nullptr));
    LASS_TEST_CHECK_EQUAL(c.count(), static_cast<TSharedFloat::TCount>(0));
}


void testUtilSharedPtrConcurent()
{
    using namespace lass;

    const size_t c = std::max<size_t>(std::thread::hardware_concurrency() - 1, 2);
    LASS_COUT << "#producers = 1, #consumers = " << c << std::endl;

    constexpr size_t n = sizeof(size_t) == 4
        ? 10'000 // numWorkers * (n - 1) * (n / 2) must fit in a 32 bit unsigned int
        : 100'000;
    static_assert(n % 2 == 0, "n must be even");

    using Task = util::SharedPtr<Gizmo>;
    using Ring = stde::lock_free_spmc_ring_buffer<Task>;

    Ring ring(32);
    std::atomic<bool> done { false };
    std::atomic<size_t> total { 0 };

    auto consumer = [&ring, &done, &total]()
    {
        size_t subTotal = 0;
        while (true)
        {
            Task task;
            if (!ring.try_pop(task))
            {
                if (!done)
                {
                    LASS_SPIN_PAUSE;
                    continue;
                }
                // try again. the ring could temporarily have been empty, but between
                // try_pop and done-check more tasks could have been pushed into the ring ...
                if (!ring.try_pop(task))
                {
                    // ok, it's really empty ... we're done.
                    total += subTotal;
                    return;
                }
            }
            subTotal += *task;
        }
    };
    std::vector<std::thread> consumers;
    for (size_t i = 0; i < c; ++i)
    {
        consumers.emplace_back(consumer);
    }

    for (size_t value = 1; value < n; ++value)
    {
        Task task(new Gizmo(value));
        for (size_t i = 0; i < c; ++i)
        {
            while (!ring.try_push(task))
            {
                LASS_SPIN_PAUSE;
            }
        }
    }
    done = true;
    for (size_t i = 0; i < c; ++i)
    {
        consumers[i].join();
    }

    LASS_TEST_CHECK_EQUAL(Gizmo::constructed(), n - 1);
    LASS_TEST_CHECK_EQUAL(Gizmo::deconstructed(), n - 1);
    LASS_TEST_CHECK_EQUAL(total, c * (n - 1) * (n / 2));
}


void testUtilSharedPtrConcurentIntrinsic()
{
    using namespace lass;

    const size_t c = std::max<size_t>(std::thread::hardware_concurrency() - 1, 2);
    LASS_COUT << "#producers = 1, #consumers = " << c << std::endl;

    constexpr size_t n = sizeof(size_t) == 4
        ? 10'000 // numWorkers * (n - 1) * (n / 2) must fit in a 32 bit unsigned int
        : 100'000;
    static_assert(n % 2 == 0, "n must be even");

    using Task = util::SharedPtr<
        GizmoWithCounter,
        util::ObjectStorage,
        util::IntrusiveCounter<GizmoWithCounter, std::atomic<int>, &GizmoWithCounter::refCount>
    >;
    using Ring = stde::lock_free_spmc_ring_buffer<Task>;

    Ring ring(32);
    std::atomic<bool> done { false };
    std::atomic<size_t> total { 0 };

    auto consumer = [&ring, &done, &total]()
    {
        size_t subTotal = 0;
        while (true)
        {
            Task task;
            if (!ring.try_pop(task))
            {
                if (!done)
                {
                    LASS_SPIN_PAUSE;
                    continue;
                }
                // try again. the ring could temporarily have been empty, but between
                // try_pop and done-check more tasks could have been pushed into the ring ...
                if (!ring.try_pop(task))
                {
                    // ok, it's really empty ... we're done.
                    total += subTotal;
                    return;
                }
            }
            subTotal += *task;
        }
    };
    std::vector<std::thread> consumers;
    for (size_t i = 0; i < c; ++i)
    {
        consumers.emplace_back(consumer);
    }

    for (size_t value = 1; value < n; ++value)
    {
        Task task(new GizmoWithCounter(value));
        for (size_t i = 0; i < c; ++i)
        {
            while (!ring.try_push(task))
            {
                LASS_SPIN_PAUSE;
            }
        }
    }
    done = true;
    for (size_t i = 0; i < c; ++i)
    {
        consumers[i].join();
    }

    LASS_TEST_CHECK_EQUAL(GizmoWithCounter::constructed(), n - 1);
    LASS_TEST_CHECK_EQUAL(GizmoWithCounter::deconstructed(), n - 1);
    LASS_TEST_CHECK_EQUAL(total, c * (n - 1) * (n / 2));
}

TUnitTest test_util_shared_ptr()
{
    return TUnitTest
    {
        LASS_TEST_CASE(testUtilSharedPtr),
        LASS_TEST_CASE(testUtilSharedPtrConcurent),
        LASS_TEST_CASE(testUtilSharedPtrConcurentIntrinsic),
    };
}

}
}

// EOF
