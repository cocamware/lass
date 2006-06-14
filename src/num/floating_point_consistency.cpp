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
#include "floating_point_consistency.h"

namespace lass
{
namespace num
{

#define LASS_NUM_FLOATING_POINT_CONSISTENCY_DEFINE_NUMTRAITS(type)\
	const int NumTraits< Consistent< type > >::memorySize = sizeof(Consistent< type >);\
	const Consistent< type > NumTraits< Consistent< type > >::one = Consistent< type >(NumTraits< type >::one);\
	const Consistent< type > NumTraits< Consistent< type > >::zero = Consistent< type >(NumTraits< type >::zero);\
	const Consistent< type > NumTraits< Consistent< type > >::qNaN = Consistent< type >(NumTraits< type >::qNaN);\
	const Consistent< type > NumTraits< Consistent< type > >::sNaN = Consistent< type >(NumTraits< type >::sNaN);\
	const Consistent< type > NumTraits< Consistent< type > >::infinity = Consistent< type >(NumTraits< type >::infinity);\
	const Consistent< type > NumTraits< Consistent< type > >::epsilon = Consistent< type >(NumTraits< type >::epsilon);\
	const Consistent< type > NumTraits< Consistent< type > >::min = Consistent< type >(NumTraits< type >::min);\
	const Consistent< type > NumTraits< Consistent< type > >::max = Consistent< type >(NumTraits< type >::max);\
	const Consistent< type > NumTraits< Consistent< type > >::minStrictPositive = Consistent< type >(NumTraits< type >::minStrictPositive);\
	const Consistent< type > NumTraits< Consistent< type > >::pi = Consistent< type >(NumTraits< type >::pi);\
	const Consistent< type > NumTraits< Consistent< type > >::e = Consistent< type >(NumTraits< type >::e);\
	const Consistent< type > NumTraits< Consistent< type > >::sqrt2 = Consistent< type >(NumTraits< type >::sqrt2);\
	const Consistent< type > NumTraits< Consistent< type > >::sqrtPi = Consistent< type >(NumTraits< type >::sqrtPi);
 
LASS_NUM_FLOATING_POINT_CONSISTENCY_DEFINE_NUMTRAITS(float)
LASS_NUM_FLOATING_POINT_CONSISTENCY_DEFINE_NUMTRAITS(double)
LASS_NUM_FLOATING_POINT_CONSISTENCY_DEFINE_NUMTRAITS(long double)

}
}

// EOF
