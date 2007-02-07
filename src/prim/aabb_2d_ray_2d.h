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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_RAY_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_RAY_2D_H

#include "prim_common.h"
#include "aabb_2d.h"
#include "ray_2d.h"
#include "impl/aabb_slab.h"

namespace lass
{
namespace prim
{

/** Find the intersection of an AABB and ray by their parameter t on the ray.
 *  @relates lass::prim::Aabb2D
 *  @relates lass::prim::Ray2D
 *
 *  @param aabb [in] the AABB
 *  @param ray [in] the ray
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a t > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       a intersection with @a t > @a tMin is found
 *							@a t is assigned.
 */
template<typename T, typename MMPAabb, typename NPRay, typename PPRay>
Result intersect(
		const Aabb2D<T, MMPAabb>& aabb, const Ray2D<T, NPRay, PPRay>& ray, 
		T& t, const T& tMin = T())
{
	if (aabb.isEmpty())
	{
		return rNone;
	}

	typedef num::NumTraits<T> TNumTraits;
	typedef Point2D<T> TPoint;
	typedef Vector2D<T> TVector;

	const TPoint& min = aabb.min();
	const TPoint& max = aabb.max();
	const TPoint& support = ray.support();
	const TVector& direction = ray.direction();

	T tNear = tMin;
	T tFar = TNumTraits::infinity;
	bool good = true;
	good &= impl::interectSlab(min[0], max[0], support[0], direction[0], tNear, tFar);
	good &= impl::interectSlab(min[1], max[1], support[1], direction[1], tNear, tFar);

	if (good)
	{
		if (tNear > tMin)
		{
			t = tNear;
			return rOne;
		}
		if (tFar > tMin)
		{
			t = tFar;
			return rOne;
		}
	}
	return rNone;
}

}

}

#endif

// EOF
