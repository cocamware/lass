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
#include "../lass/stde/lock_free_spsc_ring_buffer.h"

#include <thread>

namespace
{

class Gizmo
{
public:
	Gizmo() : index_(lass::num::NumTraits<size_t>::max)
	{
		++constructed_;
	}
	explicit Gizmo(size_t index) : index_(index)
	{
		++constructed_;
	}
	Gizmo(const Gizmo&) = delete;
	Gizmo(Gizmo&& other) : index_(other.index_)
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

}

namespace lass
{
namespace test
{

void testLockFreeSpscValueRingBuffer()
{
	LASS_COUT << "#producers = 1, #consumers = 1" << std::endl;
	const size_t k = 4;
	const size_t n = 1'000'000;
	std::vector<char> flag(n, 0);

	stde::lock_free_spsc_ring_buffer<size_t> ring(k);
	std::atomic<bool> stop { false };

	LASS_TEST_CHECK(ring.empty());

	std::thread popper([&ring, &flag, &stop]
	{
		while (true)
		{
			size_t index;
			if (ring.try_pop(index))
			{
				++flag[index];
			}
			else if (stop.load(std::memory_order_acquire))
			{
				break;
			}
			else
			{
				std::this_thread::yield();
			}
		}
	});
	
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_CLANG
	// Because Clang considers n to be a constexpr, it complains with
	// "lambda capture 'n' is not required to be captured for this use"
	// But other compilers won't compile without it.
#	pragma GCC diagnostic ignored "-Wunused-lambda-capture"
#endif

	std::thread pusher([&ring, n] 
	{
		for (size_t i = 0; i < n; ++i)
		{
			while (!ring.try_push(i))
			{
				std::this_thread::yield();
			}
		}
	});

	pusher.join();
	stop.store(true, std::memory_order_release);	
	popper.join();

	LASS_TEST_CHECK(ring.empty());

	for (size_t i = 0; i < n; ++i)
	{
		LASS_TEST_CHECK_EQUAL(flag[i], 1);
	}
}


void testLockFreeSpscObjectRingBuffer()
{
	LASS_COUT << "#producers = 1, #consumers = 1" << std::endl;
	const size_t k = 4;
	const size_t n = 1'000'000;
	std::vector<char> flag(n, 0);

	stde::lock_free_spsc_ring_buffer<std::unique_ptr<Gizmo>> ring(k);
	std::atomic<bool> stop { false };

	LASS_TEST_CHECK(ring.empty());

	std::thread popper([&ring, &flag, &stop]
	{
		while (true)
		{
			std::unique_ptr<Gizmo> p;
			if (ring.try_pop(p))
			{
				++flag[p->index()];
			}
			else if (stop.load(std::memory_order_acquire))
			{
				break;
			}
			else
			{
				std::this_thread::yield();
			}
		}
	});

	std::thread pusher([&ring, n]
	{
		for (size_t i = 0; i < n; ++i)
		{
			while (ring.full())
			{
				std::this_thread::yield();
			}
			std::unique_ptr<Gizmo> p(new Gizmo(i));
			ring.try_push(std::move(p));
		}
	});

	pusher.join();
	stop.store(true, std::memory_order_release);
	popper.join();

	LASS_TEST_CHECK(ring.empty());

	LASS_TEST_CHECK_EQUAL(Gizmo::constructed(), n);
	LASS_TEST_CHECK_EQUAL(Gizmo::deconstructed(), n);
	for (size_t i = 0; i < n; ++i)
	{
		LASS_TEST_CHECK_EQUAL(flag[i], 1);
	}
}

TUnitTest test_stde_lock_free_spsc_ring_buffer()
{
	return TUnitTest{
		LASS_TEST_CASE(testLockFreeSpscValueRingBuffer),
		LASS_TEST_CASE(testLockFreeSpscObjectRingBuffer),
	};
}

}

}

// EOF
