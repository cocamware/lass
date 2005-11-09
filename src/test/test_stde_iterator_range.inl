/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_ITERATOR_RANGE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_ITERATOR_RANGE_INL

#include "test_common.h"
#include "../stde/iterator_range.h"

namespace lass
{
namespace test
{

void testStdeIteratorRange()
{
	typedef std::vector<int> TContainer;
	typedef stde::iterator_range<TContainer::iterator> TRange;

	TRange range;
	LASS_TEST_CHECK(range.begin() == range.end());
	LASS_TEST_CHECK_EQUAL(range.size(), 0);
	LASS_TEST_CHECK(range.empty());
	LASS_TEST_CHECK(!range);
	LASS_TEST_CHECK(range ? false : true);

	TContainer container;
	const int n = 10;
	for (int i = 0; i < n; ++i)
	{
		container.push_back(i);
	}

	range = TRange(container.begin(), container.end());
	LASS_TEST_CHECK(range.begin() == container.begin());
	LASS_TEST_CHECK(range.end() == container.end());
	LASS_TEST_CHECK_EQUAL(range.size(), n);
	LASS_TEST_CHECK(!range.empty());
	LASS_TEST_CHECK_EQUAL(!range, false);
	LASS_TEST_CHECK(range);

	for (int i = 0; i < n; ++i)
	{
		LASS_TEST_CHECK_EQUAL(range[i], i);
	}

	for (int i = 0; range; ++range, ++i)
	{
		LASS_TEST_CHECK_EQUAL(*range, i);
	}

	TContainer other_container;
	for (int i = 100; i < 200; ++i)
	{
		other_container.push_back(i);
	}
	TRange other(other_container.begin(), other_container.end());

	LASS_TEST_CHECK(range == TRange());
	LASS_TEST_CHECK(range != other);
	
	range = other++;
	LASS_TEST_CHECK_EQUAL(range.size(), other.size() + 1);
	
	range.swap(other);
	LASS_TEST_CHECK_EQUAL(range.size() + 1, other.size());
}

}

}

#endif

// EOF
