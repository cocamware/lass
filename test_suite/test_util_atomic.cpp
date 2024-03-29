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
#include "../lass/util/atomic.h"
#include "../lass/util/thread_fun.h"
#include "../lass/num/basic_types.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4996) // 'deprecated-declaration': deprecation-message (or "was declared deprecated")
#else
#	pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace lass
{
namespace test
{
namespace atomic
{

template <typename T>
class hex_type
{
public:
	hex_type(T x): x_(x) {}
	operator T() const { return x_; }
private:
	T x_;
};

template <typename T>
hex_type<T> hex(T x)
{
	return hex_type<T>(x);
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, hex_type<T> x)
{
	typedef typename num::BasicType<8 * (sizeof(T) < 4 ? 4 : sizeof(T)) >::Tuint type;
	stream << std::hex << "0x" << static_cast<type>(x);
	return stream;
}

template <typename T>
void testUtilAtomicType()
{
	T pattern = 0;
	for (size_t i = 0; i < sizeof(T); ++i)
	{
		pattern = static_cast<T>((pattern << 8) + 1);
	}

	T old1 = 1;
	T new1 = 2;
	volatile T a = old1;
	util::atomicIncrement(a);
	LASS_TEST_CHECK_EQUAL(a, new1);	
	util::atomicDecrement(a);
	LASS_TEST_CHECK_EQUAL(a, old1);

	a = pattern;
	old1 = pattern;
	new1 = static_cast<T>(2 * pattern);
	T wrong1 = static_cast<T>(10 * pattern);

	LASS_TEST_CHECK(!util::atomicCompareAndSwap(a, wrong1, new1));
	LASS_TEST_CHECK_EQUAL(hex(a), hex(old1));
	LASS_TEST_CHECK(util::atomicCompareAndSwap(a, old1, new1));
	LASS_TEST_CHECK_EQUAL(hex(a), hex(new1));
}

template <typename T>
void testUtilAtomicAdjacentCas()
{
	T pattern = 0;
	for (size_t i = 0; i < sizeof(T); ++i)
	{
		pattern = static_cast<T>((pattern << 8) + 1);
	}

	const T old1 = pattern, old2 = static_cast<T>(2 * pattern);
	const T new1 = static_cast<T>(3 * pattern), new2 = static_cast<T>(4 * pattern);
	const T wrong1 = static_cast<T>(10 * pattern), wrong2 = static_cast<T>(11 * pattern);

#if LASS_ADDRESS_SIZE == 64
	volatile T LASS_ALIGNED(a[2], 16) = { old1, old2 };
#elif defined(LASS_PROCESSOR_ARCHITECTURE_ARM)
	volatile T LASS_ALIGNED(a[2], 8) = { old1, old2 };
#else
	volatile T a[2] = { old1, old2 };
#endif
	LASS_META_ASSERT(sizeof(a) == 2 * sizeof(T), expect_close_packing);

	LASS_TEST_CHECK(!util::atomicCompareAndSwap(a[0], wrong1, wrong2, new1, new2));
	LASS_TEST_CHECK_EQUAL(hex(a[0]), hex(old1));
	LASS_TEST_CHECK_EQUAL(hex(a[1]), hex(old2));
	LASS_TEST_CHECK(!util::atomicCompareAndSwap(a[0], wrong1, old2, new1, new2));
	LASS_TEST_CHECK_EQUAL(hex(a[0]), hex(old1));
	LASS_TEST_CHECK_EQUAL(hex(a[1]), hex(old2));
	LASS_TEST_CHECK(!util::atomicCompareAndSwap(a[0], old1, wrong2, new1, new2));
	LASS_TEST_CHECK_EQUAL(hex(a[0]), hex(old1));
	LASS_TEST_CHECK_EQUAL(hex(a[1]), hex(old2));
	LASS_TEST_CHECK(util::atomicCompareAndSwap(a[0], old1, old2, new1, new2));
	LASS_TEST_CHECK_EQUAL(hex(a[0]), hex(new1));
	LASS_TEST_CHECK_EQUAL(hex(a[1]), hex(new2));
}

template <typename T>
void countingThread(volatile T* counter, T times, volatile bool* startFlag, util::Condition* startCondition)
{
	while (!*startFlag)
	{
		startCondition->wait(500);
	}
	for (T k = 0; k < times; ++k)
	{
		T expected, desired;
		do
		{
			expected = *counter;
			desired = static_cast<T>(expected + 1);
		}
		while (!util::atomicCompareAndSwap(*counter, expected, desired));
	}
}

template <typename T>
void testUtilAtomicMultithreadedCounter()
{
	typedef util::SharedPtr<util::ThreadFun> ThreadPtr;
	const size_t numThreads = std::max<size_t>(util::numberOfAvailableProcessors(), 2);

	volatile T counter = 0;
	T times = static_cast<T>(std::min<num::Tuint64>(num::NumTraits<T>::max, 10000));
	const T timesPerThread = static_cast<T>(times / numThreads);
	times = static_cast<T>(timesPerThread * numThreads);
	
	volatile bool startFlag = false;
	util::Condition startCondition;
	std::vector<ThreadPtr> threads;
	for (size_t k = 0; k < numThreads; ++k)
	{
		threads.push_back(ThreadPtr(util::threadFun( util::bind( &countingThread<T>, &counter, timesPerThread, &startFlag, &startCondition ), util::threadJoinable ) ));
		threads[k]->run();
	}
	startFlag = true;
	startCondition.broadcast();
	
	for (size_t k = 0; k < numThreads; ++k)
	{
		threads[k]->join();
	}
	
	LASS_TEST_CHECK_EQUAL(counter, times);
}

}

TUnitTest test_util_atomic()
{
	using namespace atomic;

	TUnitTest result;
	
	result.push_back(LASS_TEST_CASE(testUtilAtomicType<num::Tuint8>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicType<num::Tuint16>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicType<num::Tuint32>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicType<num::Tuint64>));

	result.push_back(LASS_TEST_CASE(testUtilAtomicType<num::Tint8>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicType<num::Tint16>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicType<num::Tint32>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicType<num::Tint64>));

	result.push_back(LASS_TEST_CASE(testUtilAtomicAdjacentCas<num::Tuint8>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicAdjacentCas<num::Tuint16>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicAdjacentCas<num::Tuint32>));
#if (LASS_ADDRESS_SIZE == 64)
	result.push_back(LASS_TEST_CASE(testUtilAtomicAdjacentCas<num::Tuint64>));
#endif

	result.push_back(LASS_TEST_CASE(testUtilAtomicAdjacentCas<num::Tint8>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicAdjacentCas<num::Tint16>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicAdjacentCas<num::Tint32>));
#if (LASS_ADDRESS_SIZE == 64)
	result.push_back(LASS_TEST_CASE(testUtilAtomicAdjacentCas<num::Tint64>));
#endif

	result.push_back(LASS_TEST_CASE(testUtilAtomicMultithreadedCounter<num::Tuint8>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicMultithreadedCounter<num::Tuint16>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicMultithreadedCounter<num::Tuint32>));
	result.push_back(LASS_TEST_CASE(testUtilAtomicMultithreadedCounter<num::Tuint64>));

	return result;
}

}
}

// EOF
 
