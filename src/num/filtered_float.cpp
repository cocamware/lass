/** @file
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

#include "num_common.h"
#include "filtered_float.h"

namespace lass
{
namespace num
{

/*
#define LASS_NUM_EXTENDED_PRECISION_DEFINE_NUMTRAITS(type)\
	const int NumTraits< FilteredFloat< type > >::memorySize = sizeof(FilteredFloat< type >);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::one = FilteredFloat< type >(NumTraits< type >::one);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::zero = FilteredFloat< type >(NumTraits< type >::zero);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::qNaN = FilteredFloat< type >(NumTraits< type >::qNaN);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sNaN = FilteredFloat< type >(NumTraits< type >::sNaN);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::infinity = FilteredFloat< type >(NumTraits< type >::infinity);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::epsilon = FilteredFloat< type >(NumTraits< type >::epsilon);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::min = FilteredFloat< type >(NumTraits< type >::min);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::max = FilteredFloat< type >(NumTraits< type >::max);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::minStrictPositive = FilteredFloat< type >(NumTraits< type >::minStrictPositive);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::pi = FilteredFloat< type >(NumTraits< type >::pi);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::e = FilteredFloat< type >(NumTraits< type >::e);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sqrt2 = FilteredFloat< type >(NumTraits< type >::sqrt2);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sqrtPi = FilteredFloat< type >(NumTraits< type >::sqrtPi);
 */
#define LASS_NUM_EXTENDED_PRECISION_DEFINE_NUMTRAITS(type)\
	const int NumTraits< FilteredFloat< type > >::memorySize = sizeof(FilteredFloat< type >);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::one = FilteredFloat< type >(NumTraits< type >::one);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::zero = FilteredFloat< type >(NumTraits< type >::zero);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::qNaN = FilteredFloat< type >(NumTraits< type >::qNaN);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sNaN = FilteredFloat< type >(NumTraits< type >::sNaN);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::infinity = FilteredFloat< type >(NumTraits< type >::infinity);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::epsilon = FilteredFloat< type >(NumTraits< type >::epsilon);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::min = FilteredFloat< type >(NumTraits< type >::min);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::max = FilteredFloat< type >(NumTraits< type >::max);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::minStrictPositive = FilteredFloat< type >(NumTraits< type >::minStrictPositive);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::pi = FilteredFloat< type >(NumTraits< type >::pi);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::e = FilteredFloat< type >(NumTraits< type >::e);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sqrt2 = FilteredFloat< type >(NumTraits< type >::sqrt2);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sqrtPi = FilteredFloat< type >(NumTraits< type >::sqrtPi);

LASS_NUM_EXTENDED_PRECISION_DEFINE_NUMTRAITS(float)
LASS_NUM_EXTENDED_PRECISION_DEFINE_NUMTRAITS(double)
LASS_NUM_EXTENDED_PRECISION_DEFINE_NUMTRAITS(long double)

}
}

// EOF
