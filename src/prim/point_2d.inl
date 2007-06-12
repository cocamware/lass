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
Point2D<T>::Point2D(TParam x, TParam y):
	x(x),
	y(y)
{
}



template<typename T>
template<typename U>
Point2D<T>::Point2D(const Point2D<U>& other):
	x(static_cast<TValue>(other.x)),
	y(static_cast<TValue>(other.y))
{
}



template<typename T>
template<typename U>
Point2D<T>::Point2D(const Vector2D<U>& position):
	x(static_cast<TValue>(position.x)),
	y(static_cast<TValue>(position.y))
{
}



template<typename T>
template<typename U>
Point2D<T>::Point2D(const U& x, const U& y):
	x(static_cast<TValue>(x)),
	y(static_cast<TValue>(y))
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
Point2D<T>::operator[](size_t index) const
{
	LASS_ASSERT(index < dimension);
	return *(&x + index);
}



template<typename T> inline
typename Point2D<T>::TReference
Point2D<T>::operator[](size_t index)
{
	LASS_ASSERT(index < dimension);
	return *(&x + index);
}



/** Wrap index around range.
 */
template<typename T> inline
typename Point2D<T>::TConstReference
Point2D<T>::at(signed index) const
{
	return *(&x + num::mod(index, dimension));
}



/** Wrap index around range.
 */
template<typename T> inline
typename Point2D<T>::TReference
Point2D<T>::at(signed index)
{
	return *(&x + num::mod(index, dimension));
}



template<typename T>
Point2D<T>&
Point2D<T>::operator+=(const Vector2D<T>& offset)
{
	x += offset.x;
	y += offset.y;
	return *this;
}



template<typename T>
Point2D<T>&
Point2D<T>::operator-=(const Vector2D<T>& offset)
{
	x -= offset.x;
	y -= offset.y;
	return *this;
}



template<typename T>
const bool Point2D<T>::isZero() const
{
	return x == TNumTraits::zero && y == TNumTraits::zero;
}



/** Return true if at least one of the components is NaN
 */
template<typename T> inline
const bool Point2D<T>::isNaN() const
{
	return num::isNaN(x) || num::isNaN(y);
}



// --- FREE FUNCTIONS ---------------------------------------------------------------------------

/** @relates lass::prim::Point2D
 */
template<typename T>
bool operator==(const Point2D<T>& a, const Point2D<T>& b)
{
	return a.x == b.x && a.y == b.y;
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
bool operator!=(const Point2D<T>& a, const Point2D<T>& b)
{
	return !(a == b);
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
Point2D<T> operator+(const Point2D<T>& a, const Vector2D<T>& b)
{
	Point2D<T> result(a);
	result += b;
	return result;
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
Point2D<T> operator+(const Vector2D<T>& a, const Point2D<T>& b)
{
	Point2D<T> result(b);
	result += a;
	return result;
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
Point2D<T> operator-(const Point2D<T>& a, const Vector2D<T>& b)
{
	Point2D<T> result(a);
	result -= b;
	return result;
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
Vector2D<T> operator-(const Point2D<T>& a, const Point2D<T>& b)
{
	return Vector2D<T>(a.x - b.x, a.y - b.y);
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline
typename Point2D<T>::TValue distance(const Point2D<T>& a, const Point2D<T>& b)
{
	const Vector2D<T> difference = a - b;
	return difference.norm();
}

/** @relates lass::prim::Point2D
 */
template<typename T> inline
typename Point2D<T>::TValue squaredDistance(const Point2D<T>& a, const Point2D<T>& b)
{
	const Vector2D<T> difference = a - b;
	return difference.squaredNorm();
}


/** return a point with, for each coordinate, the minimum value of a and b
 *  @relates lass::prim::Point2D
 */
template<typename T>
Point2D<T> pointwiseMin(const Point2D<T>& a, const Point2D<T>& b)
{
	return Point2D<T>(std::min(a.x, b.x), std::min(a.y, b.y));
}



/** return a point with, for each coordinate, the maximum value of a and b
 *  @relates lass::prim::Point2D
 */
template<typename T>
Point2D<T> pointwiseMax(const Point2D<T>& a, const Point2D<T>& b)
{
	return Point2D<T>(std::max(a.x, b.x), std::max(a.y, b.y));
}



/** interpolate linearly between two points: a + t * (b - a)
 *  @relates lass::prim::Point2D
 */
template<typename T>
inline Point2D<T> lerp(const Point2D<T>& a, const Point2D<T>& b, typename Point2D<T>::TParam t)
{
	return Point2D<T>(lerp(a.position(), b.position(), t));
}



/** @relates lass::prim::Point2D
 */
template<typename T>
std::ostream& operator<<(std::ostream& stream, const Point2D<T>& b)
{
	LASS_ENFORCE_STREAM(stream) << b.position();
	return stream;
}



/** @relates lass::prim::Point2D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& stream, const Point2D<T>& b)
{
	LASS_ENFORCE_STREAM(stream)
		<< "<Point2D>" << b.x << " " << b.y << "</Point2D>\n";
	return stream;
}



/** @relates lass::prim::Point2D
 */
template<typename T>
lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& stream, const Point2D<T>& b)
{
	LASS_ENFORCE_STREAM(stream) << "lasthandle = line(";
	stream << b.x << "," << b.y << ",";
	stream << "'Color'," << stream.color() << ");\n";
	stream << "set(lasthandle,'Marker','o');\n";
	stream << "set(lasthandle,'markersize',2);\n";
	return stream;
}


/** @relates lass::prim::Point2D
 */
template<typename T>
std::istream& operator>>(std::istream& stream, Point2D<T>& b)
{
	Vector2D<T> temp;
	LASS_ENFORCE_STREAM(stream) >> temp;
	b = Point2D<T>(temp);
	return stream;
}

/** returns twice signed area of triangle a,b,c
 *  @relates lass::prim::Point2D
 */
// based on floating point number theory the ordening of computation is optimised
// for floating point or filtered arithmetic
// when T is an exact type, a faster computation could be devised
// see work from Shewchuk, Fortune and Van Wyck
// the basic idea is to translate points a and b by c
template<typename T>
T doubleTriangleArea( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return perpDot(a-c,b-c);
	/* more intuitive (and faster) but less precise version...
	const Vector2D<T> a = a.position();
	const Vector2D<T> b = b.position();
	const Vector2D<T> c = c.position();
	return perpDot(b, c) - perpDot(a, c) + perpDot(a, b);
	*/
}

// based on floating point number theory the ordening of computation is optimised
// for floating point or filtered arithmetic
// when T is an exact type, a faster computation could be devised
// see work from Shewchuk, Fortune and Van Wyck
// the basic idea is to translate points a and b by c
template<typename T>
T preciseDoubleTriangleArea( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return perpDot(a-c,b-c);
}





/** returns true when the line b->c is counter clockwise oriented with respect to a->b
 *  @relates lass::prim::Point2D
 */
template<typename T>
bool ccw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return  doubleTriangleArea(a,b,c) > num::NumTraits<T>::zero;
}

/** returns true when the line b->c is clockwise oriented with respect to a->b
 *  @relates lass::prim::Point2D
 */
template<typename T>
bool cw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return  doubleTriangleArea(a,b,c) < num::NumTraits<T>::zero;
}


/** returns true when the line b->c is counter clockwise oriented with respect to a->b.
 *  When c is in line of a and b also returns true.
 *  @relates lass::prim::Point2D
 */
template<typename T>
bool weakCcw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return  doubleTriangleArea(a,b,c) >= num::NumTraits<T>::zero;
}

/** returns true when the line b->c is counter clockwise oriented with respect to a->b.
 *  When c is in line of a and b also returns true.
 *  @relates lass::prim::Point2D
 */
template<typename T>
bool weakCw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return  doubleTriangleArea(a,b,c) <= num::NumTraits<T>::zero;
}

/** returns true when the point d is strictly (within numerical precision) in the circle
 *  going through a, b and c.
 *  @relates lass::prim::Point2D
 *  @note this test is used for establishing Delaunay neighborhoods and this tests
 *        numerical stability determines the overall stability of the Delaunay meshers
 */
template<typename T>
bool inCircle( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c, const Point2D<T>& d )
{
	const T az = a.position().squaredNorm();
	const T bz = b.position().squaredNorm();
	const T cz = c.position().squaredNorm();
	const T dz = d.position().squaredNorm();

	T det = (az * doubleTriangleArea(b, c, d) - bz * doubleTriangleArea(a, c, d)
		+    cz * doubleTriangleArea(a, b, d) - dz * doubleTriangleArea(a, b, c));

	return (det > T(0));
}


}

}

#endif
