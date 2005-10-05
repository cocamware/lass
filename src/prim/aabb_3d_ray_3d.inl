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
 *  @param iAabb [in] the AABB
 *  @param iRay [in] the ray
 *  @param iMinT [in] the minimum t that may be returned as valid intersection.
 *  @param oT [out] the parameter of the intersection point >= @a iMinT.
 *  @return @arg rNone      no intersections with @a >= @a iMinT found
 *                          @a oT is not assigned.
 *          @arg rOne       a intersection with @a oT >= @a iMinT is found
 *							@a oT is assigned.
 */
template<typename T, typename MMPAabb, typename NPRay, typename PPRay>
Result intersect(const Aabb3D<T, MMPAabb>& iAabb,
				 const Ray3D<T, NPRay, PPRay>& iRay,
				 T& oT, const T& iMinT)
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

	T tNear = iMinT;
	T tFar = TNumTraits::infinity;
	bool good = true;
	good &= impl::interectSlab(min[0], max[0], support[0], direction[0], tNear, tFar);
	good &= impl::interectSlab(min[1], max[1], support[1], direction[1], tNear, tFar);
	good &= impl::interectSlab(min[2], max[2], support[2], direction[2], tNear, tFar);

	if (good)
	{
		if (tNear > iMinT)
		{
			oT = tNear;
			return rOne;
		}
		if (tFar > iMinT)
		{
			oT = tFar;
			return rOne;
		}
	}
	return rNone;
}



}

}

#endif

// EOF