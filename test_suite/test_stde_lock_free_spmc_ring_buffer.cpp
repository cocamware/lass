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
#include "../lass/stde/lock_free_spmc_ring_buffer.h"

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
	Gizmo(const Gizmo& other) : index_(other.index_)
	{
		++constructed_;
	}
	Gizmo(Gizmo&& other): index_(other.index_)
	{
		++constructed_;
	}
	Gizmo& operator=(const Gizmo&) = default;
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

void testLockFreeSpmcValueRingBuffer()
{
	const size_t c = std::max<size_t>(std::thread::hardware_concurrency() - 1, 2);
	LASS_COUT << "#producers = 1, #consumers = " << c << std::endl;
	const size_t k = 4;
	const size_t n = 1'000'000;
	std::vector<char> flag(n, 0);

	LASS_TEST_CHECK((std::is_base_of<
		stde::impl::lock_free_spmc_value_ring_buffer<size_t>,
		stde::lock_free_spmc_ring_buffer<size_t>
	>::value));

	stde::impl::lock_free_spmc_value_ring_buffer<size_t> ring(k);
	std::atomic<bool> stop { false };

	LASS_TEST_CHECK(ring.empty());

	auto consumer = [&ring, &flag, &stop]
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
	};

	std::vector<std::thread> consumers;
	for (size_t i = 0; i < c; ++i)
	{
		consumers.emplace_back(consumer);
	}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_CLANG
	// Because Clang considers n to be a constexpr, it complains with
	// "lambda capture 'n' is not required to be captured for this use"
	// But other compilers won't compile without it.
#	pragma GCC diagnostic ignored "-Wunused-lambda-capture"
#endif

	std::thread producer([&ring, n]
	{
		for (size_t i = 0; i < n; ++i)
		{
			switch (i % 3)
			{
			case 0:
				while (!ring.try_push(i))
				{
					std::this_thread::yield();
				}
				break;
			case 1:
				while (true)
				{
					size_t ii = i;
					if (ring.try_push(std::move(ii)))
					{
						break;
					}
					std::this_thread::yield();
				}
				break;
			case 2:
				while (!ring.try_emplace(i))
				{
					std::this_thread::yield();
				}
				break;
			};
		}
	});

	producer.join();
	stop.store(true, std::memory_order_release);
	for (size_t i = 0; i < c; ++i)
	{
		consumers[i].join();
	}

	LASS_TEST_CHECK(ring.empty());

	for (size_t i = 0; i < n; ++i)
	{
		LASS_TEST_CHECK_EQUAL(static_cast<int>(flag[i]), 1);
	}
}



void testLockFreeSpmcObjectRingBuffer()
{
	const size_t c = std::max<size_t>(std::thread::hardware_concurrency() - 1, 2);
	LASS_COUT << "#producers = 1, #consumers = " << c << std::endl;
	const size_t k = 4;
	const size_t n = 1'000'000;
	std::vector<char> flag(n, 0);

	{
		stde::impl::lock_free_spmc_object_ring_buffer<Gizmo, util::AllocatorConcurrentFreeList<AllocatorFixedStats> > ring(k);
		std::atomic<bool> stop { false };

		LASS_TEST_CHECK(ring.empty());
		LASS_TEST_CHECK_EQUAL(AllocatorFixedStats::allocated(), size_t(0));
		LASS_TEST_CHECK_EQUAL(AllocatorFixedStats::deallocated(), size_t(0));

		auto consumer = [&ring, &flag, &stop]
		{
			while (true)
			{
				Gizmo gizmo(0);
				if (ring.try_pop(gizmo))
				{
					++flag[gizmo.index()];
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
		};

		std::vector<std::thread> consumers;
		for (size_t i = 0; i < c; ++i)
		{
			consumers.emplace_back(consumer);
		}

		std::thread producer([&ring, n]
		{
			for (size_t i = 0; i < n; ++i)
			{
				switch (i % 3)
				{
				case 0:
					{
						Gizmo gizmo(i);
						while (!ring.try_push(gizmo))
						{
							std::this_thread::yield();
						}
					}
					break;
				case 1:
					while (true)
					{
						Gizmo gizmo(i);
						if (ring.try_push(std::move(gizmo)))
						{
							break;
						}
						std::this_thread::yield();
					}
					break;
				case 2:
					while (!ring.try_emplace(i))
					{
						std::this_thread::yield();
					}
					break;
				};
			}
		});

		producer.join();
		stop.store(true, std::memory_order_release);
		for (size_t i = 0; i < c; ++i)
		{
			consumers[i].join();
		}

		LASS_TEST_CHECK(ring.empty());

	}

	LASS_TEST_CHECK_EQUAL(Gizmo::constructed(), Gizmo::deconstructed());
	LASS_TEST_CHECK_EQUAL(AllocatorFixedStats::allocated(), AllocatorFixedStats::deallocated());
	LASS_TEST_CHECK(AllocatorFixedStats::allocated() >= k);
	LASS_TEST_CHECK(AllocatorFixedStats::deallocated() <= k + c + 1);

	for (size_t i = 0; i < n; ++i)
	{
		LASS_TEST_CHECK_EQUAL(static_cast<int>(flag[i]), 1);
	}
}



TUnitTest test_stde_lock_free_spmc_ring_buffer()
{
	return TUnitTest{
		LASS_TEST_CASE(testLockFreeSpmcValueRingBuffer),
		LASS_TEST_CASE(testLockFreeSpmcObjectRingBuffer)
	};
}

}

}

// EOF
