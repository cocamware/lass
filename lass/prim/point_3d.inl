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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3D_INL




#include "point_3d.h"



namespace lass
{

namespace prim
{

template<typename T> inline
Point3D<T>::Point3D() :
	x(TNumTraits::zero),
	y(TNumTraits::zero),
	z(TNumTraits::zero)
{
	LASS_ASSERT(isZero());
}



template<typename T> inline
Point3D<T>::Point3D(TParam x, TParam y, TParam z) :
	x(x),
	y(y),
	z(z)
{
}



template<typename T>
template<typename U> inline
Point3D<T>::Point3D(const Point3D<U>& other):
	x(static_cast<TValue>(other.x)),
	y(static_cast<TValue>(other.y)),
	z(static_cast<TValue>(other.z))
{
}



template<typename T>
template<typename U> inline
Point3D<T>::Point3D(const Vector3D<U>& position):
	x(static_cast<TValue>(position.x)),
	y(static_cast<TValue>(position.y)),
	z(static_cast<TValue>(position.z))
{
}



template<typename T>
template<typename U> inline
Point3D<T>::Point3D(const U& x, const U& y, const U& z):
	x(static_cast<TValue>(x)),
	y(static_cast<TValue>(y)),
	z(static_cast<TValue>(z))
{
}



template <typename T> inline
const typename Point3D<T>::TVector
Point3D<T>::position() const
{
	return TVector(x, y, z);
}



template<typename T> inline
typename Point3D<T>::TConstReference
Point3D<T>::operator[](size_t index) const
{
	LASS_ASSERT(index < dimension);
	return *(&x + index);
}



template<typename T> inline
typename Point3D<T>::TReference
Point3D<T>::operator[](size_t index)
{
	LASS_ASSERT(index < dimension);
	return *(&x + index);
}



/** Wrap index around range.
 */
template<typename T> inline
typename Point3D<T>::TConstReference
Point3D<T>::at(signed index) const
{
	return *(&x + num::mod(index, dimension));
}



/** Wrap index around range.
 */
template<typename T> inline
typename Point3D<T>::TReference
Point3D<T>::at(signed index)
{
	return *(&x + num::mod(index, dimension));
}




template<typename T> inline
Point3D<T>& Point3D<T>::operator+=(const TVector& offset)
{
	x += offset.x;
	y += offset.y;
	z += offset.z;
	return *this;
}



template<typename T> inline
Point3D<T>& Point3D<T>::operator-=(const TVector& offset)
{
	x -= offset.x;
	y -= offset.y;
	z -= offset.z;
	return *this;
}



template<typename T> inline
bool Point3D<T>::isZero() const
{
	return x == TNumTraits::zero && y == TNumTraits::zero && z == TNumTraits::zero;
}



/** Return true if at least one of the components is NaN
 */
template<typename T> inline
bool Point3D<T>::isNaN() const
{
	return num::isNaN(x) || num::isNaN(y) || num::isNaN(z);
}



// --- FREE FUNCTIONS ---------------------------------------------------------------------------

/** @relates lass::prim::Point3D
 */
template<typename T> inline
bool operator==(const Point3D<T>& a, const Point3D<T>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline
bool operator!=(const Point3D<T>& a, const Point3D<T>& b)
{
	return !(a == b);
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline
Point3D<T> operator+(const Point3D<T>& a, const Vector3D<T>& b)
{
	Point3D<T> result(a);
	result += b;
	return result;
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline
Point3D<T> operator-(const Point3D<T>& a, const Vector3D<T>& b)
{
	Point3D<T> result(a);
	result -= b;
	return result;
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline
Point3D<T> operator+(const Vector3D<T>& a, const Point3D<T>& b)
{
	Point3D<T> result(b);
	result += a;
	return result;
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline
Vector3D<T> operator-(const Point3D<T>& a, const Point3D<T>& b)
{
	return Vector3D<T>(a.x - b.x, a.y - b.y, a.z - b.z);
}



/** return the distance between two points
 *  @relates lass::prim::Point3D
 */
template<typename T> inline
typename Point3D<T>::TValue distance(const Point3D<T>& a, const Point3D<T>& b)
{
	return (a - b).norm();
}

/** @relates lass::prim::Point2D
 */
template<typename T> inline
typename Point3D<T>::TValue squaredDistance(const Point3D<T>& a, const Point3D<T>& b)
{
	return (a - b).squaredNorm();
}



/** return a point with, for each coordinate, the minimum value of a and b
 *  @relates lass::prim::Point3D
 */
template<typename T>
Point3D<T> pointwiseMin(const Point3D<T>& a, const Point3D<T>& b)
{
	return Point3D<T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}



/** interpolate linearly between two points: a + t * (b - a)
 *  @relates lass::prim::Point3D
 */
template<typename T> inline
Point3D<T> lerp(const Point3D<T>& a, const Point3D<T>& b, typename Point3D<T>::TParam t)
{
	return Point3D<T>(lerp(a.position(), b.position(), t));
}



/** return a point with, for each coordinate, the maximum value of a and b
 *  @relates lass::prim::Point3D
 */
template<typename T>
Point3D<T> pointwiseMax(const Point3D<T>& a, const Point3D<T>& b)
{
	return Point3D<T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}



/** @relates lass::prim::Point3D
 */
template<typename T>
std::ostream& operator<<(std::ostream& stream, const Point3D<T>& b)
{
	LASS_ENFORCE_STREAM(stream) << b.position();
	return stream;
}



/** @relates lass::prim::Point3D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& stream, const Point3D<T>& b)
{
	LASS_ENFORCE_STREAM(stream)
		<< "<Point3D>" << b.x << " " << b.y << " " << b.z << "</Point3D>\n";
	return stream;
}


/** @relates lass::prim::Point3D
 */
template<typename T>
std::istream& operator>>(std::istream& stream, Point3D<T>& b)
{
	Vector3D<T> temp;
	LASS_ENFORCE_STREAM(stream) >> temp;
	b = Point3D<T>(temp);
	return stream;
}



}

}

#endif
