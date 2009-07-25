/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
 *
 *	*** BEGIN LICENSE INFORMATION ***
 *	
 *	The contents of this file are subject to the Common Public Attribution License 
 *	Version 1.0 (the "License"); you may not use this file except in compliance with 
 *	the License. You may obtain a copy of the License at 
 *	http://lass.sourceforge.net/cpal-license. The License is based on the 
 *	Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *	use of software over a computer network and provide for limited attribution for 
 *	the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *	with Exhibit B.
 *	
 *	Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *	WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *	language governing rights and limitations under the License.
 *	
 *	The Original Code is LASS - Library of Assembled Shared Sources.
 *	
 *	The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *	The Original Developer is the Initial Developer.
 *	
 *	All portions of the code written by the Initial Developer are:
 *	Copyright (C) 2004-2009 the Initial Developer.
 *	All Rights Reserved.
 *	
 *	Contributor(s):
 *
 *	Alternatively, the contents of this file may be used under the terms of the 
 *	GNU General Public License Version 2 or later (the GPL), in which case the 
 *	provisions of GPL are applicable instead of those above.  If you wish to allow use
 *	of your version of this file only under the terms of the GPL and not to allow 
 *	others to use your version of this file under the CPAL, indicate your decision by 
 *	deleting the provisions above and replace them with the notice and other 
 *	provisions required by the GPL License. If you do not delete the provisions above,
 *	a recipient may use your version of this file under either the CPAL or the GPL.
 *	
 *	*** END LICENSE INFORMATION ***
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SPHERE3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SPHERE3D_INL


#include "sphere_3d.h"
#include "../num/floating_point_comparison.h"

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
Side Sphere3D<T>::classify(const TPoint& iPoint) const
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
	return difference.squaredNorm() - num::sqr(radius_);
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



/** return squared distance of point to surface of sphere.
 */
template<typename T>
const typename Sphere3D<T>::TValue
Sphere3D<T>::squaredDistance(const TPoint& iPoint) const
{
	return num::abs(equation(iPoint));
}



/** returns if point is on inside or on surface
 *  @return <tt>classify(iPoint) != sOutside</tt> but may be faster
 */
template<typename T>
bool Sphere3D<T>::contains(const TPoint& iPoint) const
{
	const TValue eq = equation(iPoint);
	return eq <= TNumTraits::zero;
}



/** Classify a point and tell and what side of the sphere surface it is.
 *  @return sInside, sSurface, sOutside
 */
template<typename T>
Side Sphere3D<T>::classify(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	const TValue eq = equation(iPoint, iRelativeTolerance);
	return eq > TNumTraits::zero ? sOutside : (eq < TNumTraits::zero ? sInside : sSurface);
}



/**
 * (P - C)² - r²
 */
template<typename T>
const typename Sphere3D<T>::TValue
Sphere3D<T>::equation(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	const TVector difference = iPoint - center_;
	const TValue d2 = difference.squaredNorm();
	const TValue r2 = num::sqr(radius_);
	return num::almostEqual(d2, r2, iRelativeTolerance) ? TNumTraits::zero : (d2 - r2);
}



/** return signed distance of point to surface of sphere.
 *  negative distance means point is inside the sphere.
 */
template<typename T>
const typename Sphere3D<T>::TValue
Sphere3D<T>::signedDistance(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	const TValue eq = equation(iPoint, iRelativeTolerance);
	return eq >= TNumTraits::zero ? num::sqrt(eq) : -num::sqrt(-eq);
}



/** return squared distance of point to surface of sphere.
 */
template<typename T>
const typename Sphere3D<T>::TValue
Sphere3D<T>::squaredDistance(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	return num::abs(equation(iPoint, iRelativeTolerance));
}



/** returns if point is on inside or on surface
 *  @return <tt>classify(iPoint, iRelativeTolerance) != sOutside</tt> but may be faster
 */
template<typename T>
bool Sphere3D<T>::contains(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	const TValue eq = equation(iPoint, iRelativeTolerance);
	return eq <= TNumTraits::zero;
}



/** return true if sphere has a non-negative radius
 */
template <typename T>
bool Sphere3D<T>::isValid() const
{
	return radius_ >= TNumTraits::zero;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::Sphere3D
 */
template <typename T>
const T squaredDistance(const Sphere3D<T>& sphere, const Point3D<T>& point)
{
	return sphere.squaredDistance(point);
}



/** @relates lass::prim::Sphere3D
 */
template <typename T>
const T distance(const Sphere3D<T>& sphere, const Point3D<T>& point)
{
	return num::sqrt(sphere.squaredDistance(point));
}



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
		<< "<Sphere3D>\n"
		<< "<center>" << iSphere.center() << "</center>\n"
		<< "<radius>" << iSphere.radius() << "</radius>\n"
		<< "</Sphere3D>\n";

	return ioOStream;
}



}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------
