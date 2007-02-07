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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_TRIANGLE_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_TRIANGLE_2D_H

#include "prim_common.h"
#include "ray_2d.h"
#include "triangle_2d.h"
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
 *  @param triangle [in] the triangle
 *  @param ray [in] the ray
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a t > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       a intersection with @a t > @a tMin is found
 *							@a t is assigned.
 */
template<typename T, class NP, class PP>
Result intersect(
		const Triangle2D<T>& triangle, const Ray2D<T, NP, PP>& ray, 
		T& t, const T& tMin = T())
{
	typedef Point2D<T> TPoint;
	typedef Vector2D<T> TVector;
	typedef typename TVector::TValue TValue;
	typedef typename TVector::TNumTraits TNumTraits;

	const TPoint& support = ray.support();
	const TVector& direction = ray.direction();

	TValue tNear = TNumTraits::infinity;
	bool good = false;
	good |= impl::intersectEdge2D(support, direction, triangle[0], triangle[1], tNear, tMin);
	good |= impl::intersectEdge2D(support, direction, triangle[1], triangle[2], tNear, tMin);
	good |= impl::intersectEdge2D(support, direction, triangle[2], triangle[0], tNear, tMin);

	if (good)
	{
		LASS_ASSERT(tNear > tMin);
		t = tNear;
		return rOne;
	}
	return rNone;
}

}
}

#endif

// EOF

