/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_RAY_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_RAY_3D_INL
#pragma once



#include "plane_3d_ray_3d.inl"



namespace lass
{

namespace prim
{

/** Find the intersection of a plane and ray by their parameter t on the ray.
 *  @relates lass::prim::Plane3D
 *  @relates lass::prim::Ray3D
 *
 *  @param iPlane the plane
 *  @param iRay the ray
 *  @param oT the parameter of the intersection point with @a t > 0.  May be null.
 *  @return @arg rNone      no intersections with @a t > 0 found
 *                          @a oT is not assigned.
 *          @arg rOne       exactly one intersection with t > 0 found
 *                          @a oT represents it.
 *          @arg rInfinite  infinite many intersections found (ray is coincident with plane),
 *                          @a oT is not assigned.
 *          @arg rInvalid   @a iPlane or @a iRay is invalid, no intersection.
 *                          @a oT is not assigned.
 */ 
template<typename T, class EPPlane, class NPPlane, class NPRay, class PPRay> 
Result intersect(const Plane3D<T, EPPlane, NPPlane>& iPlane,
                 const Ray3D<T, NPRay, PPRay>& iRay, 
                 T& oT)
{
    typedef typename Vector3D<T>::TValue TValue;
    typedef typename Vector3D<T>::TNumTraits TNumTraits;

    if (!iPlane.isValid() || !iRay.isValid())
    {
        return rInvalid;
    }

    const TValue d = dot(iPlane.normal(), iRay.direction());
    if (d == TNumTraits::zero)
    {
		// ray is parallel to plane, but is it also coincident?
		const Side side = iPlane.classify(iRay.support());
		LASS_ASSERT(side == sFront || side == sSurface || side == sBack);
		return side == sSurface ? rInfinite : rNone;
    }
    else
    {
        const TValue t = iPlane.equation(iRay.support()) / d;
		LASS_ASSERT(!num::isNaN(t));
        if (t < TNumTraits::zero)
        {
			return rNone;
        }
        else
        {
            oT = t;
            return rOne;
        }
    }        
}



}

}

#endif
