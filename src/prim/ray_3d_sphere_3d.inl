/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY3D_SPHERE3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY3D_SPHERE3D_INL
#pragma once



#include "ray_3d_sphere_3d.h"



namespace lass
{

namespace prim
{



#pragma LASS_FIXME("this only works if ray is normalized!")



/** Find the intersection of a ray and a sphere by their parameter t on the ray.
 *  @relates lass::prim::Ray3D
 *  @relates lass::prim::Sphere3D
 *
 *  A maximum of two possible intersections with t > 0.
 *
 *  @param iRay the ray
 *  @param iSphere the sphere
 *  @param oTNear the parameter of the intersection point with the smallest t > 0, if any.
 *  @param oTFar the parameter of the intersection point with the largest t > 0, if any.
 *  @return	- rNone: no intersections with t > 0 found, oTNear and oTFar is invalid.
 *			- rOne: exactly one intersection with t > 0, oTNear represents it, oTFar is invalid.
 *			- rTwo: two intersections with t > 0 found, oTNear and oTFar represent them
 */ 
template<typename T, class NP, class PP> 
Result intersect(const Ray3D<T, NP, PP>& iRay, const Sphere3D<T>& iSphere, T& oTNear, T& oTFar)
{
	return rInvalid;
}



#if !defined(LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/** specialisation of intersect for normalized rays.
 *  @relates lass::prim::Ray3D
 *  @relates lass::prim::Sphere3D
 */
template<typename T, class PP> 
Result intersect(const Ray3D<T, Normalized, PP>& iRay, const Sphere3D<T>& iSphere, 
                 T& oTNear, T& oTFar)
{
    typedef Vector3D<T> TVector;
    typedef typename TVector::TValue TValue;
    typedef typename TVector::TNumTraits TNumTraits;

    const TVector v = iSphere.center() - iRay.support();
    
    TValue d = dot(v, iRay.direction());
    const TValue vSquared = v.squaredNorm();
    const TValue rSquared = num::sqr(iSphere.radius());   
    if (d < TNumTraits::zero && vSquared > rSquared)
    {
        return rNone;
    }
    
    const TValue mSquared = vSquared - num::sqr(d);   
    if (mSquared > rSquared)
    {
        return rNone;
    }
    
    const TValue q = num::sqrt(rSquared - mSquared);
    if (vSquared > rSquared)
    {
		oTNear = d - q;
		oTFar = d + q;
        return rTwo;
    }
    else
    {
        oTNear = d + q;
		return rOne;
    }
}

#endif

}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------