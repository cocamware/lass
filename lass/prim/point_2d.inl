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
bool Point2D<T>::isZero() const
{
	return x == TNumTraits::zero && y == TNumTraits::zero;
}



/** Return true if at least one of the components is NaN
 */
template<typename T> inline
bool Point2D<T>::isNaN() const
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
template<typename T> inline
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
template<typename T> inline
T preciseDoubleTriangleArea( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return perpDot(a-c,b-c);
}





/** returns true when the line b->c is counter clockwise oriented with respect to a->b
 *  @relates lass::prim::Point2D
 */
template<typename T> inline
bool ccw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return  doubleTriangleArea(a,b,c) > T();
}

/** returns true when the line b->c is clockwise oriented with respect to a->b
 *  @relates lass::prim::Point2D
 */
template<typename T> inline
bool cw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return  doubleTriangleArea(a,b,c) < T();
}


/** returns true when the line b->c is counter clockwise oriented with respect to a->b.
 *  When c is in line of a and b also returns true.
 *  @relates lass::prim::Point2D
 */
template<typename T> inline
bool weakCcw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return  doubleTriangleArea(a,b,c) >= T();
}

/** returns true when the line b->c is counter clockwise oriented with respect to a->b.
 *  When c is in line of a and b also returns true.
 *  @relates lass::prim::Point2D
 */
template<typename T> inline
bool weakCw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c )
{
	return  doubleTriangleArea(a,b,c) <= T();
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
