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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SPHERE3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SPHERE3D_INL
#pragma once

#include "sphere_3d.h"

namespace lass
{
namespace prim
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
Sphere3D<T>::Sphere3D():
	center_(),
	radius_(TNumTraits::zero)
{
	LASS_ASSERT(center_.isZero());
}



template <typename T>
Sphere3D<T>::Sphere3D(const TPoint& iCenter, TParam iRadius):
	center_(iCenter),
	radius_(iRadius)
{
}



template <typename T>
const typename Sphere3D<T>::TPoint& 
Sphere3D<T>::center() const
{
    return center_;
}



template <typename T>
typename Sphere3D<T>::TPoint& 
Sphere3D<T>::center()
{
    return center_;
}



template<typename T>
typename Sphere3D<T>::TConstReference 
Sphere3D<T>::radius() const
{
    return radius_;
}



template<typename T>
typename Sphere3D<T>::TReference 
Sphere3D<T>::radius()
{
    return radius_;
}



/** return area of surface of sphere
 */
template<typename T>
const typename Sphere3D<T>::TValue 
Sphere3D<T>::area() const
{
    return 4 * TNumTraits::pi * num::sqr(radius_);
}



/** return volume of sphere
 */
template<typename T>
const typename Sphere3D<T>::TValue 
Sphere3D<T>::volume() const
{
    return (4 * TNumTraits::pi * num::sqr(radius_) * radius_) / 3;
}



/** Classify a point and tell and what side of the sphere surface it is.
 *  @return sInside, sSurface, sOutside
 */
template<typename T>
const Side Sphere3D<T>::classify(const TPoint& iPoint) const
{
    const TValue eq = equation(iPoint);
    return eq > TNumTraits::zero ? sOutside : (eq < TNumTraits::zero ? sInside : sSurface);
}



/** 
 * (P - C)² - r²
 */
template<typename T>
const typename Sphere3D<T>::TValue 
Sphere3D<T>::equation(const TPoint& iPoint) const
{
    const TVector difference = iPoint - center_;
    return difference.squaredNorm() - radius_ * radius_;
}



/** return signed distance of point to surface of sphere.
 *  negative distance means point is inside the sphere.
 */
template<typename T>
const typename Sphere3D<T>::TValue 
Sphere3D<T>::signedDistance(const TPoint& iPoint) const
{
	const TValue eq = equation(iPoint);
	return eq >= TNumTraits::zero ? num::sqrt(eq) : -num::sqrt(-eq);
}



/** return true if sphere has a non-negative radius
 */
template <typename T>
const bool Sphere3D<T>::isValid() const
{
    return radius_ >= TNumTraits::zero;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::Sphere3D
 */
template<typename T> 
std::ostream& operator<<(std::ostream& ioOStream, const Sphere3D<T>& iSphere)
{
	LASS_ENFORCE(ioOStream) << "{S=" << iSphere.center() << ", r=" << iSphere.radius() << "}";
	return ioOStream;
}



/** @relates lass::prim::Sphere3D
 */
template<typename T> 
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Sphere3D<T>& iSphere)
{
	LASS_ENFORCE_STREAM(ioOStream) 
		<< "<Sphere3D>" << std::endl
		<< "<center>" << iSphere.center() << "</center>" << std::endl
		<< "<radius>" << iSphere.radius() << "</radius>" << std::endl
		<< "</Sphere3D>" << std::endl;

	return ioOStream;
}



}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------
