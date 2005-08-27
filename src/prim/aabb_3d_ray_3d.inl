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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_RAY_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_RAY_3D_INL

#include "aabb_3d_ray_3d.inl"
#include "impl/aabb_slab.h"

namespace lass
{
namespace prim
{

/** Find the intersection of an AABB and ray by their parameter t on the ray.
 *  @relates lass::prim::Aabb3D
 *  @relates lass::prim::Ray3D
 *
 *  @param iAabb the AABB
 *  @param iRay the ray
 *  @param oT the parameter of the intersection point with @a t > 0.  May be null.
 *  @return @arg rNone      no intersections with @a t > 0 found
 *                          @a oT is not assigned.
 *          @arg rOne       exactly one intersection with t > 0 found
 *                          @a oT represents it.
 *          @arg rTwo       exactly one intersection with t > 0 found
 *                          @a oT represents it.
 *          @arg rInfinite  infinite many intersections found (ray is coincident with plane),
 *                          @a oT is not assigned.
 *          @arg rInvalid   @a iPlane or @a iRay is invalid, no intersection.
 *                          @a oT is not assigned.
 */
template<typename T, typename MMPAabb, typename NPRay, typename PPRay>
Result intersect(const Aabb3D<T, MMPAabb>& iAabb,
				 const Ray3D<T, NPRay, PPRay>& iRay,
				 T& oTNear, T& oTFar)
{
	if (iAabb.isEmpty())
	{
		return rNone;
	}

	typedef num::NumTraits<T> TNumTraits;
	typedef Point3D<T> TPoint;
	typedef Vector3D<T> TVector;

	const TPoint& min = iAabb.min();
	const TPoint& max = iAabb.max();
	const TPoint& support = iRay.support();
	const TVector& direction = iRay.direction();

	T tMin = TNumTraits::zero;
	T tMax = TNumTraits::infinity;
	bool good = true;
	for (size_t i = 0; i < 3 && good; ++i)
	{
		good &= impl::interectSlab(min[i], max[i], support[i], direction[i], tMin, tMax);
	}

	if (good)
	{
		if (tMin > TNumTraits::zero)
		{
			oTNear = tMin;
			oTFar = tMax;
			return rTwo;
		}
		else
		{
			oTNear = tMax;
			return rOne;
		}
	}
	return rNone;
}



template<typename T, typename MMPAabb, typename NPRay, typename PPRay>
Result intersect(const Aabb3D<T, MMPAabb>& iAabb,
				 const Ray3D<T, NPRay, PPRay>& iRay,
				 T& oTNear)
{
	T dummy;
	return intersect(iAabb, iRay, oTNear, dummy);
}



template<typename T, typename MMPAabb, typename NPRay, typename PPRay>
Result intersect(const Ray3D<T, NPRay, PPRay>& iRay,
				 const Aabb3D<T, MMPAabb>& iAabb,				 
				 T& oTNear, T& oTFar)
{
	return intersect(iAabb, iRay, oTNear, oTFar);
}



template<typename T, typename MMPAabb, typename NPRay, typename PPRay>
Result intersect(const Ray3D<T, NPRay, PPRay>& iRay,
				 const Aabb3D<T, MMPAabb>& iAabb,
				 T& oTNear)
{
	T dummy;
	return intersect(iAabb, iRay, oTNear, dummy);
}



}

}

#endif

// EOF