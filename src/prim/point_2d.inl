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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_INL

#include "point_2d.h"

namespace lass
{
namespace prim
{

template<typename T> inline
Point2D<T>::Point2D():
	x(TNumTraits::zero),
	y(TNumTraits::zero)
{
	LASS_ASSERT(isZero());
}



template<typename T> inline
Point2D<T>::Point2D(TParam iX, TParam iY):
	x(iX),
	y(iY)
{
}



template<typename T>
template<typename U>
Point2D<T>::Point2D(const Point2D<U>& iOther):
	x(static_cast<TValue>(iOther.x)),
	y(static_cast<TValue>(iOther.y))
{
}



template<typename T>
template<typename U>
Point2D<T>::Point2D(const Vector2D<U>& iPositionVector):
	x(static_cast<TValue>(iPositionVector.x)),
	y(static_cast<TValue>(iPositionVector.y))
{
}



template<typename T>
template<typename U>
Point2D<T>::Point2D(const U& iX, const U& iY):
	x(static_cast<TValue>(iX)),
	y(static_cast<TValue>(iY))
{
}



template <typename T> inline
const typename Point2D<T>::TVector
Point2D<T>::position() const
{
	return TVector(x, y);
}



template<typename T> inline
typename Point2D<T>::TConstReference
Point2D<T>::operator[](unsigned iIndex) const
{
	LASS_ASSERT(iIndex < dimension);
	return *(&x + iIndex);
}



template<typename T> inline
typename Point2D<T>::TReference
Point2D<T>::operator[](unsigned iIndex)
{
	LASS_ASSERT(iIndex < dimension);
	return *(&x + iIndex);
}



/** Wrap index around range.
 */
template<typename T> inline
typename Point2D<T>::TConstReference
Point2D<T>::at(signed iIndex) const
{
	return *(&x + num::mod(iIndex, dimension));
}



/** Wrap index around range.
 */
template<typename T> inline
typename Point2D<T>::TReference
Point2D<T>::at(signed iIndex)
{
	return *(&x + num::mod(iIndex, dimension));
}



template<typename T>
Point2D<T>&
Point2D<T>::operator+=(const Vector2D<T>& iB)
{
	x += iB.x;
	y += iB.y;
	return *this;
}



template<typename T>
Point2D<T>&
Point2D<T>::operator-=(const Vector2D<T>& iB)
{
	x -= iB.x;
	y -= iB.y;
	return *this;
}



template<typename T>
const bool Point2D<T>::isZero() const
{
	return x == TNumTraits::zero && y == TNumTraits::zero;
}



// --- FREE FUNCTIONS ---------------------------------------------------------------------------

/** @relates lass::prim::Point2D
 */
template<typename T>
bool operator==(const Point2D<T>& iA, const Point2D<T>& iB)
{
	return iA.x == iB.x && iA.y == iB.y;
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
bool operator!=(const Point2D<T>& iA, const Point2D<T>& iB)
{
	return !(iA == iB);
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
Point2D<T> operator+(const Point2D<T>& iA, const Vector2D<T>& iB)
{
	Point2D<T> result(iA);
	result += iB;
	return result;
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
Point2D<T> operator+(const Vector2D<T>& iA, const Point2D<T>& iB)
{
	Point2D<T> result(iB);
	result += iA;
	return result;
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
Point2D<T> operator-(const Point2D<T>& iA, const Vector2D<T>& iB)
{
	Point2D<T> result(iA);
	result -= iB;
	return result;
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
Vector2D<T> operator-(const Point2D<T>& iA, const Point2D<T>& iB)
{
	return Vector2D<T>(iA.x - iB.x, iA.y - iB.y);
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
typename Point2D<T>::TValue distance(const Point2D<T>& iA, const Point2D<T>& iB)
{
	const Vector2D<T> difference = iA - iB;
	return difference.norm();
}

/** @relates lass::prim::Point2D
 */
template<typename T> inline
typename Point2D<T>::TValue squaredDistance(const Point2D<T>& iA, const Point2D<T>& iB)
{
	const Vector2D<T> difference = iA - iB;
	return difference.squaredNorm();
}


/** return a point with, for each coordinate, the minimum value of iA and iB
 *  @relates lass::prim::Point2D
 */
template<typename T>
Point2D<T> pointwiseMin(const Point2D<T>& iA, const Point2D<T>& iB)
{
	return Point2D<T>(std::min(iA.x, iB.x), std::min(iA.y, iB.y));
}



/** return a point with, for each coordinate, the maximum value of iA and iB
 *  @relates lass::prim::Point2D
 */
template<typename T>
Point2D<T> pointwiseMax(const Point2D<T>& iA, const Point2D<T>& iB)
{
	return Point2D<T>(std::max(iA.x, iB.x), std::max(iA.y, iB.y));
}



/** @relates lass::prim::Point2D
 */
template<typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Point2D<T>& iB)
{
	LASS_ENFORCE_STREAM(ioOStream) << iB.position();
	return ioOStream;
}



/** @relates lass::prim::Point2D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point2D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream)
		<< "<Point2D>" << iB.x << " " << iB.y << "</Point2D>\n";
	return oOStream;
}



/** @relates lass::prim::Point2D
 */
template<typename T>
lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& ioOStream, const Point2D<T>& iB)
{
	LASS_ENFORCE_STREAM(ioOStream) << "lasthandle = line(";
	ioOStream << iB.x << "," << iB.y << ");" << std::endl;
	ioOStream << "set(lasthandle,'Marker','o');" << std::endl;
	ioOStream << "set(lasthandle,'markersize',2);" << std::endl;
	return ioOStream;
}


/** @relates lass::prim::Point2D
 */
template<typename T>
std::istream& operator>>(std::istream& ioIStream, Point2D<T>& oB)
{
	Vector2D<T> temp;
	LASS_ENFORCE_STREAM(ioIStream) >> temp;
	oB = Point2D<T>(temp);
	return ioIStream;
}

/** returns twice signed area of triangle iA,iB,iC
 *  @relates lass::prim::Point2D
 */
template<typename T>
T doubleTriangleArea( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC )
{
	const Vector2D<T> a = iA.position();
	const Vector2D<T> b = iB.position();
	const Vector2D<T> c = iC.position();
	return perpDot(b, c) - perpDot(a, c) + perpDot(a, b);
}



/** returns true when the line iB->iC is counter clockwise oriented with respect to iA->iB
 *  @relates lass::prim::Point2D
 */
template<typename T>
bool ccw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC )
{
	return  doubleTriangleArea(iA,iB,iC) > num::NumTraits<T>::zero;
}

/** returns true when the line iB->iC is clockwise oriented with respect to iA->iB
 *  @relates lass::prim::Point2D
 */
template<typename T>
bool cw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC )
{
	return  doubleTriangleArea(iA,iB,iC) < num::NumTraits<T>::zero;
}


/** returns true when the line iB->iC is counter clockwise oriented with respect to iA->iB.
 *  When iC is in line of iA and iB also returns true.
 *  @relates lass::prim::Point2D
 */
template<typename T>
bool weakCcw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC )
{
	return  doubleTriangleArea(iA,iB,iC) >= num::NumTraits<T>::zero;
}

/** returns true when the line iB->iC is counter clockwise oriented with respect to iA->iB.
 *  When iC is in line of iA and iB also returns true.
 *  @relates lass::prim::Point2D
 */
template<typename T>
bool weakCw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC )
{
	return  doubleTriangleArea(iA,iB,iC) <= num::NumTraits<T>::zero;
}

/** returns true when the point iD is strictly (within numerical precision) in the circle
 *  going through iA, iB and iC.
 *  @relates lass::prim::Point2D
 *  @note this test is used for establishing Delaunay neighborhoods and this tests
 *        numerical stability determines the overall stability of the Delaunay meshers
 */
template<typename T>
bool inCircle( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC, const Point2D<T>& iD )
{
	const T az = iA.position().squaredNorm();
	const T bz = iB.position().squaredNorm();
	const T cz = iC.position().squaredNorm();
	const T dz = iD.position().squaredNorm();

	T det = (az * doubleTriangleArea(iB, iC, iD) - bz * doubleTriangleArea(iA, iC, iD)
		+    cz * doubleTriangleArea(iA, iB, iD) - dz * doubleTriangleArea(iA, iB, iC));

	return (det > 0);
}


}

}

#endif
