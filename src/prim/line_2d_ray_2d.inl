/** @file
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_RAY_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_RAY_2D_INL

#include "line_2d_ray_2d.h"

namespace lass
{

namespace prim
{

/** Find the intersection of a line and ray by their parameter t on the ray.
 *  @relates lass::prim::Line2D
 *  @relates lass::prim::Ray2D
 *
 *  @param iLine [in] the line
 *  @param iRay [in] the ray
 *  @param oT [out] the parameter of the intersection point > @a iMinT.
 *  @param iMinT [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections > @a iMinT found
 *                          @a oT is not assigned.
 *          @arg rOne       exactly one intersection > @a iMinT found
 *                          @a oT represents it.
 *          @arg rInfinite  infinite many intersections found (ray is coincident with plane),
 *                          @a oT is not assigned.
 *          @arg rInvalid   @a iPlane or @a iRay is invalid, no intersection.
 *                          @a oT is not assigned.
 */
template<typename T, class EP1, class NP1, class NP2, class PP2>
Result intersect(const Line2D<T, EP1, NP1>& iLine,
				 const Ray2D<T, NP2, PP2>& iRay,
				 T& oT, const T& iMinT)
{
	typedef typename Vector2D<T>::TValue TValue;
	typedef typename Vector2D<T>::TNumTraits TNumTraits;

	if (!iLine.isValid() || !iRay.isValid())
	{
		return rInvalid;
	}

	const TValue nd = perpDot(iLine.normal(), iRay.direction());
	if (nd == TNumTraits::zero)
	{
		// ray is parallel to plane, but is it also coincident?
		const Side side = iLine.classify(iRay.support());
		LASS_ASSERT(side == sFront || side == sSurface || side == sBack);
		return side == sSurface ? rInfinite : rNone;
	}
	else
	{
		const TValue t = -iLine.equation(iRay.support()) / nd;
		LASS_ASSERT(!num::isNaN(t));
		if (t > iMinT)
		{
			oT = t;
			return rOne;
		}
		return rNone;
	}
}



}

}

#endif
