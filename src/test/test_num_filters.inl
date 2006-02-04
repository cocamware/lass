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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_FILTERS_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_FILTERS_INL

#include "test_common.h"

#include "../num/filters.h"

namespace lass
{
namespace test
{

template <typename T>
void testNumFilters()
{
	typedef num::NumTraits<T> TNumTraits;

	const size_t firOrder = 10;
	std::vector<T> firTaps;
	for (size_t i = 0; i < firOrder; ++i)
	{
		firTaps.push_back(TNumTraits::one / T(1 + i));
	}
	
	const size_t impulsLength = 100;
	std::vector<T> impuls(impulsLength, TNumTraits::zero);
	impuls[0] = TNumTraits::one;

	num::FirFilter<T> firFilter(firTaps);
	std::vector<T> firResponse(impulsLength);
	T* last = firFilter(&impuls[0], &impuls[impulsLength], &firResponse[0]);
	LASS_TEST_CHECK_EQUAL(last, &firResponse[impulsLength]);

	for (size_t i = 0; i < firOrder; ++i)
	{
		LASS_TEST_CHECK_EQUAL(firResponse[i], firTaps[i]);
	}
	for (size_t i = firOrder; i < impulsLength; ++i)
	{
		LASS_TEST_CHECK_EQUAL(firResponse[i], TNumTraits::zero);
	}
}


}
}

#endif

// EOF
