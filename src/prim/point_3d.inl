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
template<typename U>
Point3D<T>::Point3D(const Point3D<U>& other):
	x(static_cast<TValue>(other.x)),
	y(static_cast<TValue>(other.y)),
	z(static_cast<TValue>(other.z))
{
}



template<typename T>
template<typename U>
Point3D<T>::Point3D(const Vector3D<U>& position):
	x(static_cast<TValue>(position.x)),
	y(static_cast<TValue>(position.y)),
	z(static_cast<TValue>(position.z))
{
}



template<typename T>
template<typename U>
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




template<typename T>
Point3D<T>& Point3D<T>::operator+=(const TVector& offset)
{
	x += offset.x;
	y += offset.y;
	z += offset.z;
	return *this;
}



template<typename T>
Point3D<T>& Point3D<T>::operator-=(const TVector& offset)
{
	x -= offset.x;
	y -= offset.y;
	z -= offset.z;
	return *this;
}



template<typename T>
const bool Point3D<T>::isZero() const
{
	return x == TNumTraits::zero && y == TNumTraits::zero && z == TNumTraits::zero;
}



/** Return true if at least one of the components is NaN
 */
template<typename T> inline
const bool Point3D<T>::isNaN() const
{
	return num::isNaN(x) || num::isNaN(y) || num::isNaN(z);
}



// --- FREE FUNCTIONS ---------------------------------------------------------------------------

/** @relates lass::prim::Point3D
 */
template<typename T>
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
template<typename T>
Vector3D<T> operator-(const Point3D<T>& a, const Point3D<T>& b)
{
	return Vector3D<T>(a.x - b.x, a.y - b.y, a.z - b.z);
}



/** return the distance between two points
 *  @relates lass::prim::Point3D
 */
template<typename T>
typename Point3D<T>::TValue distance(const Point3D<T>& a, const Point3D<T>& b)
{
	const Vector3D<T> difference = a - b;
	return difference.norm();
}

/** @relates lass::prim::Point2D
 */
template<typename T>
typename Point3D<T>::TValue squaredDistance(const Point3D<T>& a, const Point3D<T>& b)
{
	const Vector3D<T> difference = a - b;
	return difference.squaredNorm();
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
template<typename T>
inline Point3D<T> lerp(const Point3D<T>& a, const Point3D<T>& b, typename Point3D<T>::TParam t)
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
