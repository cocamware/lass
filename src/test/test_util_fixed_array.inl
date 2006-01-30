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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_FIXED_ARRAY_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_FIXED_ARRAY_INL

#include "test_common.h"

#include "../util/fixed_array.h"

namespace lass
{
namespace test
{

template <typename T>
void testUtilFixedArray()
{
	using namespace util;

	typedef FixedArray<T, 5> TFixedArray;

	TFixedArray array;

	LASS_TEST_CHECK_EQUAL(array.size(), 5);
	LASS_TEST_CHECK_EQUAL(array.max_size(), 5);
	LASS_TEST_CHECK_EQUAL(array.empty(), false);

	T value(1);
	for (typename TFixedArray::iterator it = array.begin(); it != array.end(); ++it)
	{
		*it = value;
		value += value;
	}

	LASS_TEST_CHECK_EQUAL(array[0], 1);
	LASS_TEST_CHECK_EQUAL(array[1], 2);
	LASS_TEST_CHECK_EQUAL(array[2], 4);
	LASS_TEST_CHECK_EQUAL(array[3], 8);
	LASS_TEST_CHECK_EQUAL(array[4], 16);

	for (unsigned i = 0; i < array.size(); ++i)
	{
		LASS_TEST_CHECK_EQUAL(array[i], array.at(i));
	}

	LASS_TEST_CHECK_EQUAL(array[0], array.front());
	LASS_TEST_CHECK_EQUAL(*array.begin(), array.front());
	LASS_TEST_CHECK_EQUAL(*array.rbegin(), array.back());

	LASS_TEST_CHECK_THROW(array.at(static_cast<typename TFixedArray::size_type>(-1)) = 37, std::exception);
	LASS_TEST_CHECK_THROW(array.at(5) = 37, std::exception);

	FixedArray<T, 0> nullArray;

	LASS_TEST_CHECK_EQUAL(nullArray.size(), 0);
	LASS_TEST_CHECK_EQUAL(nullArray.max_size(), 0);
	LASS_TEST_CHECK_EQUAL(nullArray.empty(), true);
	LASS_TEST_CHECK(nullArray.begin() == nullArray.end());
	LASS_TEST_CHECK(nullArray.rbegin() == nullArray.rend());

	LASS_TEST_CHECK_THROW(nullArray.at(0) = 37, std::exception);
}

}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL
#	pragma warning(pop)
#endif

#endif

// EOF
