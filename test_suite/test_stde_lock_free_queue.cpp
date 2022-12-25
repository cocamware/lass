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
 *	Copyright (C) 2004-2022 the Initial Developer.
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
#include "../lass/stde/lock_free_queue.h"

#include <thread>

namespace
{

class Gizmo
{
public:
	Gizmo(): index_(lass::num::NumTraits<size_t>::max)
	{
		++constructed_;
	}
	explicit Gizmo(size_t index): index_(index)
	{
		++constructed_;
	}
	Gizmo(const Gizmo&) = delete;
	Gizmo(Gizmo&& other): index_(other.index_)
	{
		++constructed_;
	}
	Gizmo& operator=(const Gizmo&) = delete;
	Gizmo& operator=(Gizmo&&) = default;
	~Gizmo()
	{
		++deconstructed_;
	}
	size_t index() const { return index_; }
	static size_t constructed() { return constructed_; }
	static size_t deconstructed() { return deconstructed_; }
private:
	size_t index_;
	static std::atomic<size_t> constructed_;
	static std::atomic<size_t> deconstructed_;
};

alignas(LASS_LOCK_FREE_ALIGNMENT) std::atomic<size_t> Gizmo::constructed_{ 0 };
alignas(LASS_LOCK_FREE_ALIGNMENT) std::atomic<size_t> Gizmo::deconstructed_{ 0 };



class AllocatorFixedStats: public lass::util::AllocatorFixed<>
{
public:
	AllocatorFixedStats(size_t size): lass::util::AllocatorFixed<>(size) {}
	void* allocate()
	{
		++allocated_;
		return lass::util::AllocatorFixed<>::allocate();
	}
	void deallocate(void* p)
	{
		++deallocated_;
		lass::util::AllocatorFixed<>::deallocate(p);
	}
	static size_t allocated() { return allocated_; }
	static size_t deallocated() { return deallocated_; }
private:
	static std::atomic<size_t> allocated_;
	static std::atomic<size_t> deallocated_;
};

alignas(LASS_LOCK_FREE_ALIGNMENT) std::atomic<size_t> AllocatorFixedStats::allocated_{ 0 };
alignas(LASS_LOCK_FREE_ALIGNMENT) std::atomic<size_t> AllocatorFixedStats::deallocated_{ 0 };

}

namespace lass
{
namespace test
{

void testLockFreeQueue()
{
	const size_t numWorkers = std::max<size_t>(std::thread::hardware_concurrency() / 2, 2);
	LASS_COUT << "#producers = " << numWorkers << ", #consumers = " << numWorkers << std::endl;
	const size_t n = 1'000'000;
	static_assert(n % 2 == 0, "n must be even");

	stde::lock_free_queue<size_t, AllocatorFixedStats> queue;
	std::atomic<bool> stop(false);
	std::atomic<size_t> total(0);
	std::atomic<size_t> consumed(0);

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_CLANG
		// Because Clang considers n to be a constexpr, it complains with
		// "lambda capture 'n' is not required to be captured for this use"
		// But other compilers won't compile without it.
#		pragma GCC diagnostic ignored "-Wunused-lambda-capture"
#endif

	auto producer = [&queue, n]
	{
		for (size_t i = 0; i < n; ++i)
		{
			queue.push(i);
		}
	};
	auto consumer = [&queue, &stop, &total, &consumed] {
		size_t subTotal = 0;
		size_t subConsumed = 0;
		while (true)
		{
			size_t value;
			if (queue.pop(value))
			{
				subTotal += value;
				++subConsumed;
			}
			else if (stop.load(std::memory_order_acquire))
			{
				total += subTotal;
				consumed += subConsumed;
				break;
			}
			else
			{
				std::this_thread::yield();
			}
		}
	};

	std::vector<std::thread> consumers;
	for (size_t i = 0; i < numWorkers; ++i)
	{
		consumers.emplace_back(consumer);
	}

	std::vector<std::thread> producers;
	for (size_t i = 0; i < numWorkers; ++i)
	{
		producers.emplace_back(producer);
	}
	for (size_t i = 0; i < numWorkers; ++i)
	{
		producers[i].join();
	}
	stop.store(true, std::memory_order_release);

	for (size_t i = 0; i < numWorkers; ++i)
	{
		consumers[i].join();
	}

	LASS_TEST_CHECK_EQUAL(consumed.load(std::memory_order_acquire), numWorkers * n);
	LASS_TEST_CHECK_EQUAL(total.load(std::memory_order_acquire), numWorkers * (n - 1) * (n / 2));
}

void testLockFreeQueueObject()
{
	const size_t numWorkers = std::max<size_t>(std::thread::hardware_concurrency() / 2, 2);
	LASS_COUT << "#producers = " << numWorkers << ", #consumers = " << numWorkers << std::endl;
	const size_t n = 1'000'000;
	static_assert(n % 2 == 0, "n must be even");

	{
		stde::lock_free_queue<Gizmo, AllocatorFixedStats> queue;
		std::atomic<bool> stop(false);
		std::atomic<size_t> total(0);
		std::atomic<size_t> consumed(0);

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_CLANG
		// Because Clang considers n to be a constexpr, it complains with
		// "lambda capture 'n' is not required to be captured for this use"
		// But other compilers won't compile without it.
#		pragma GCC diagnostic ignored "-Wunused-lambda-capture"
#endif

		auto producer = [&queue, n]
		{
			for (size_t i = 0; i < n; i += 2)
			{
				queue.push(Gizmo(i));
				queue.emplace(i + 1);
			}
		};
		auto consumer = [&queue, &stop, &total, &consumed] {
			size_t subTotal = 0;
			size_t subConsumed = 0;
			while (true)
			{
				Gizmo value;
				if (queue.pop(value))
				{
					subTotal += value.index();
					++subConsumed;
				}
				else if (stop.load(std::memory_order_acquire))
				{
					total += subTotal;
					consumed += subConsumed;
					break;
				}
				else
				{
					std::this_thread::yield();
				}
			}
		};

		std::vector<std::thread> consumers;
		for (size_t i = 0; i < numWorkers; ++i)
		{
			consumers.emplace_back(consumer);
		}

		std::vector<std::thread> producers;
		for (size_t i = 0; i < numWorkers; ++i)
		{
			producers.emplace_back(producer);
		}
		for (size_t i = 0; i < numWorkers; ++i)
		{
			producers[i].join();
		}
		stop.store(true, std::memory_order_release);

		for (size_t i = 0; i < numWorkers; ++i)
		{
			consumers[i].join();
		}

		LASS_TEST_CHECK_EQUAL(consumed.load(std::memory_order_acquire), numWorkers * n);
		LASS_TEST_CHECK_EQUAL(total.load(std::memory_order_acquire), numWorkers * (n - 1) * (n / 2));
	}

	LASS_TEST_CHECK_EQUAL(Gizmo::constructed(), Gizmo::deconstructed());
	LASS_TEST_CHECK_EQUAL(AllocatorFixedStats::allocated(), AllocatorFixedStats::deallocated());
}



TUnitTest test_stde_lock_free_queue()
{
	return TUnitTest{
		LASS_TEST_CASE(testLockFreeQueue),
		LASS_TEST_CASE(testLockFreeQueueObject),
	};
}

}

}

// EOF
