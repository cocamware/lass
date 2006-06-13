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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_RAY_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_RAY_3D_INL

#include "plane_3d_ray_3d.h"
#include "../num/floating_point_consistency.h"

namespace lass
{

namespace prim
{

/** Find the intersection of a plane and ray by their parameter t on the ray.
 *  @relates lass::prim::Plane3D
 *  @relates lass::prim::Ray3D
 *
 *  @param iPlane [in] the plane
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
template<typename T, class EPPlane, class NPPlane, class NPRay, class PPRay>
Result intersect(const Plane3D<T, EPPlane, NPPlane>& iPlane,
				 const Ray3D<T, NPRay, PPRay>& iRay,
				 T& oT, const T& iMinT)
{
	typedef typename Vector3D<T>::TValue TValue;
	typedef typename Vector3D<T>::TNumTraits TNumTraits;
	typedef num::Consistent<T> TConsistent;

	if (!iPlane.isValid() || !iRay.isValid())
	{
		return rInvalid;
	}

	const TValue nd = dot(iPlane.normal(), iRay.direction());
	if (nd == TNumTraits::zero)
	{
		// ray is parallel to plane, but is it also coincident?
		const Side side = iPlane.classify(iRay.support());
		LASS_ASSERT(side == sFront || side == sSurface || side == sBack);
		return side == sSurface ? rInfinite : rNone;
	}
	else
	{
		const TConsistent t = -iPlane.equation(iRay.support()) / nd;
		LASS_ASSERT(!num::isNaN(t));
		if (t > iMinT)
		{
			oT = t.value();
			return rOne;
		}
		return rNone;
	}
}



}

}

#endif
