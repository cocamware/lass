/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_ATOMIC_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_ATOMIC_INL

#include "test_common.h"
#include "../util/atomic.h"
#include "../num/basic_types.h"

namespace lass
{
namespace test
{

namespace atomic
{

template <typename T>
void testUtilAtomicType()
{
	const T old1 = 1;
	const T new1 = 2;

	T a = old1;
	util::atomicIncrement(a);
	LASS_TEST_CHECK_EQUAL(a, new1);	
	util::atomicDecrement(a);
	LASS_TEST_CHECK_EQUAL(a, old1);
	
	const T wrong1 = 10;
	LASS_TEST_CHECK(!util::atomicCompareAndSwap(a, wrong1, new1));
	LASS_TEST_CHECK_EQUAL(a, old1);
	LASS_TEST_CHECK(util::atomicCompareAndSwap(a, old1, new1));
	LASS_TEST_CHECK_EQUAL(a, new1);
}

template <typename T>
void testUtilAtomicAdjacentCas()
{
	const T old1 = 1, old2 = 2;
	const T new1 = 3, new2 = 4;
	const T wrong1 = 10, wrong2 = 11;

	T a[2] = { old1, old2 };
	LASS_TEST_CHECK(!util::atomicCompareAndSwap(a[0], wrong1, wrong2, new1, new2));
	LASS_TEST_CHECK_EQUAL(a[0], old1);
	LASS_TEST_CHECK_EQUAL(a[1], old2);
	LASS_TEST_CHECK(util::atomicCompareAndSwap(a[0], old1, old2, new1, new2));
	LASS_TEST_CHECK_EQUAL(a[0], new1);
	LASS_TEST_CHECK_EQUAL(a[1], new2);
}

}

void testUtilAtomic()
{
	using namespace atomic;
	
	testUtilAtomicType<num::Tuint8>();	
	testUtilAtomicType<num::Tuint16>();	
	testUtilAtomicType<num::Tuint32>();	
	testUtilAtomicType<num::Tuint64>();

	testUtilAtomicType<num::Tint8>();	
	testUtilAtomicType<num::Tint16>();	
	testUtilAtomicType<num::Tint32>();	
	testUtilAtomicType<num::Tint64>();

	testUtilAtomicAdjacentCas<num::Tuint8>();
	testUtilAtomicAdjacentCas<num::Tuint16>();
	testUtilAtomicAdjacentCas<num::Tuint32>();

	testUtilAtomicAdjacentCas<num::Tint8>();
	testUtilAtomicAdjacentCas<num::Tint16>();
	testUtilAtomicAdjacentCas<num::Tint32>();
}

}
}

#endif

// EOF
 
