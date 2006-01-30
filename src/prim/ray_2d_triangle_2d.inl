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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_TRIANGLE_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_TRIANGLE_2D_INL

#include "ray_2d_triangle_2d.h"
#include "impl/intersect_edge_2d.h"

namespace lass
{
namespace prim
{

/** Find the intersection of a ray and a triangle by their parameter t on the ray.
 *  @relates lass::prim::Ray2D
 *  @relates lass::prim::Triangle2D
 *
 *  A maximum of two possible intersections with t > 0.
 *
 *  @param iTriangle [in] the triangle
 *  @param iRay [in] the ray
 *  @param oT [out] the parameter of the intersection point > @a iMinT.
 *  @param iMinT [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a oT > @a iMinT found
 *                          @a oT is not assigned.
 *          @arg rOne       a intersection with @a oT > @a iMinT is found
 *							@a oT is assigned.
 */
template<typename T, class NP, class PP>
Result intersect(const Triangle2D<T>& iTriangle, 
				 const Ray2D<T, NP, PP>& iRay, 
				 T& oT, const T& iMinT)
{
	typedef Point2D<T> TPoint;
	typedef Vector2D<T> TVector;
	typedef typename TVector::TValue TValue;
	typedef typename TVector::TNumTraits TNumTraits;

	const TPoint& support = iRay.support();
	const TVector& direction = iRay.direction();

	TValue tNear = TNumTraits::infinity;
	bool good = false;
	good |= impl::intersectEdge2D(support, direction, iTriangle[0], iTriangle[1], tNear, iMinT);
	good |= impl::intersectEdge2D(support, direction, iTriangle[1], iTriangle[2], tNear, iMinT);
	good |= impl::intersectEdge2D(support, direction, iTriangle[2], iTriangle[0], tNear, iMinT);

	if (good)
	{
		LASS_ASSERT(tNear > iMinT);
		oT = tNear;
		return rOne;
	}
	return rNone;
}

}

}

#endif

// EOF
