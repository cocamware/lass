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
Point3D<T>::Point3D(TParam iX, TParam iY, TParam iZ) :
	x(iX),
	y(iY),
	z(iZ)
{
}



template<typename T>
template<typename U>
Point3D<T>::Point3D(const Point3D<U>& iOther):
	x(static_cast<TValue>(iOther.x)),
	y(static_cast<TValue>(iOther.y)),
	z(static_cast<TValue>(iOther.z))
{
}



template<typename T>
template<typename U>
Point3D<T>::Point3D(const Vector3D<U>& iPositionVector):
	x(static_cast<TValue>(iPositionVector.x)),
	y(static_cast<TValue>(iPositionVector.y)),
	z(static_cast<TValue>(iPositionVector.z))
{
}



template<typename T>
template<typename U>
Point3D<T>::Point3D(const U& iX, const U& iY, const U& iZ):
	x(static_cast<TValue>(iX)),
	y(static_cast<TValue>(iY)),
	z(static_cast<TValue>(iZ))
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
Point3D<T>::operator[](size_t iIndex) const
{
	LASS_ASSERT(iIndex < dimension);
	return *(&x + iIndex);
}



template<typename T> inline
typename Point3D<T>::TReference
Point3D<T>::operator[](size_t iIndex)
{
	LASS_ASSERT(iIndex < dimension);
	return *(&x + iIndex);
}



/** Wrap index around range.
 */
template<typename T> inline
typename Point3D<T>::TConstReference
Point3D<T>::at(signed iIndex) const
{
	return *(&x + num::mod(iIndex, dimension));
}



/** Wrap index around range.
 */
template<typename T> inline
typename Point3D<T>::TReference
Point3D<T>::at(signed iIndex)
{
	return *(&x + num::mod(iIndex, dimension));
}




template<typename T>
Point3D<T>& Point3D<T>::operator+=(const TVector& iB)
{
	x += iB.x;
	y += iB.y;
	z += iB.z;
	return *this;
}



template<typename T>
Point3D<T>& Point3D<T>::operator-=(const TVector& iB)
{
	x -= iB.x;
	y -= iB.y;
	z -= iB.z;
	return *this;
}



template<typename T>
const bool Point3D<T>::isZero() const
{
	return x == TNumTraits::zero && y == TNumTraits::zero && z == TNumTraits::zero;
}



// --- FREE FUNCTIONS ---------------------------------------------------------------------------

/** @relates lass::prim::Point3D
 */
template<typename T>
bool operator==(const Point3D<T>& iA, const Point3D<T>& iB)
{
	return iA.x == iB.x && iA.y == iB.y && iA.z == iB.z;
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline
bool operator!=(const Point3D<T>& iA, const Point3D<T>& iB)
{
	return !(iA == iB);
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline
Point3D<T> operator+(const Point3D<T>& iA, const Vector3D<T>& iB)
{
	Point3D<T> result(iA);
	result += iB;
	return result;
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline
Point3D<T> operator-(const Point3D<T>& iA, const Vector3D<T>& iB)
{
	Point3D<T> result(iA);
	result -= iB;
	return result;
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline
Point3D<T> operator+(const Vector3D<T>& iA, const Point3D<T>& iB)
{
	Point3D<T> result(iB);
	result += iA;
	return result;
}



/** @relates lass::prim::Point3D
 */
template<typename T>
Vector3D<T> operator-(const Point3D<T>& iA, const Point3D<T>& iB)
{
	return Vector3D<T>(iA.x - iB.x, iA.y - iB.y, iA.z - iB.z);
}



/** return the distance between two points
 *  @relates lass::prim::Point3D
 */
template<typename T>
typename Point3D<T>::TValue distance(const Point3D<T>& iA, const Point3D<T>& iB)
{
	const Vector3D<T> difference = iA - iB;
	return difference.norm();
}

/** @relates lass::prim::Point2D
 */
template<typename T>
typename Point3D<T>::TValue squaredDistance(const Point3D<T>& iA, const Point3D<T>& iB)
{
	const Vector3D<T> difference = iA - iB;
	return difference.squaredNorm();
}



/** return a point with, for each coordinate, the minimum value of iA and iB
 *  @relates lass::prim::Point3D
 */
template<typename T>
Point3D<T> pointwiseMin(const Point3D<T>& iA, const Point3D<T>& iB)
{
	return Point3D<T>(std::min(iA.x, iB.x), std::min(iA.y, iB.y), std::min(iA.z, iB.z));
}



/** return a point with, for each coordinate, the maximum value of iA and iB
 *  @relates lass::prim::Point3D
 */
template<typename T>
Point3D<T> pointwiseMax(const Point3D<T>& iA, const Point3D<T>& iB)
{
	return Point3D<T>(std::max(iA.x, iB.x), std::max(iA.y, iB.y), std::max(iA.z, iB.z));
}



/** @relates lass::prim::Point3D
 */
template<typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Point3D<T>& iB)
{
	LASS_ENFORCE_STREAM(ioOStream) << iB.position();
	return ioOStream;
}



/** @relates lass::prim::Point3D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point3D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream)
		<< "<Point3D>" << iB.x << " " << iB.y << " " << iB.z << "</Point3D>\n";
	return oOStream;
}


/** @relates lass::prim::Point3D
 */
template<typename T>
std::istream& operator>>(std::istream& ioIStream, Point3D<T>& oB)
{
	Vector3D<T> temp;
	LASS_ENFORCE_STREAM(ioIStream) >> temp;
	oB = Point3D<T>(temp);
	return ioIStream;
}



}

}

#endif
