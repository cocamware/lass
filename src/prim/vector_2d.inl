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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_2D_INL




#include "vector_2d.h"
#include "../num/basic_ops.h"
#include "../num/distribution.h"

namespace lass
{

namespace prim
{

template<typename T> inline
Vector2D<T>::Vector2D():
	x(T()),
	y(T())
{
}



template<typename T> inline
Vector2D<T>::Vector2D(TParam iX, TParam iY):
	x(iX),
	y(iY)
{
}



template <typename T>
template <typename U>
Vector2D<T>::Vector2D(const Vector2D<U>& iOther):
	x(static_cast<TValue>(iOther.x)),
	y(static_cast<TValue>(iOther.y))
{
}



template <typename T>
template <typename U>
Vector2D<T>::Vector2D(const U& iX, const U& iY):
	x(static_cast<TValue>(iX)),
	y(static_cast<TValue>(iY))
{
}



template<typename T> inline
typename Vector2D<T>::TConstReference Vector2D<T>::operator[](size_t iIndex) const
{
	LASS_ASSERT(iIndex < dimension);
	return *(&x + iIndex);
}



template<typename T> inline
typename Vector2D<T>::TReference Vector2D<T>::operator[](size_t iIndex)
{
	LASS_ASSERT(iIndex < dimension);
	return *(&x + iIndex);
}



/** Wrap index around range.
 */
template<typename T> inline
typename Vector2D<T>::TConstReference Vector2D<T>::at(signed iIndex) const
{
	return *(&x + num::mod(iIndex, dimension));
}



/** Wrap index around range.
 */
template<typename T> inline
typename Vector2D<T>::TReference Vector2D<T>::at(signed iIndex)
{
	return *(&x + num::mod(iIndex, dimension));
}



/** A weird way to get back the same object
 */
template<typename T> inline
const Vector2D<T>& Vector2D<T>::operator+() const
{
	return *this;
}



template<typename T> inline
const Vector2D<T> Vector2D<T>::operator-() const
{
	return Vector2D(-x, -y);
}



/** componentwise addition
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator+=(const Vector2D<T>& iB)
{
	x += iB.x;
	y += iB.y;
	return *this;
}



/** componentwise subtraction
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator-=(const Vector2D<T>& iB)
{
	x -= iB.x;
	y -= iB.y;
	return *this;
}



/** Componentwise multiplication.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator*=(const Vector2D<T>& iB)
{
	x *= iB.x;
	y *= iB.y;
	return *this;
}



/** Componentwise division.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator/=(const Vector2D<T>& iB)
{
	x /= iB.x;
	y /= iB.y;
	return *this;
}



/** add iB to each component of this.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator+=(TParam iB)
{
	x += iB;
	y += iB;
	return *this;
}



/** subtract iB of each component of this.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator-=(TParam iB)
{
	x -= iB;
	y -= iB;
	return *this;
}



/** multiply each component of this with iB.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator*=(TParam iB)
{
	x *= iB;
	y *= iB;
	return *this;
}



/** divide each component of this by iB.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator/=(TParam iB)
{
	x /= iB;
	y /= iB;
	return *this;
}



/** Return true if all the components are (exactly!) zero
 */
template<typename T> inline
const bool Vector2D<T>::isZero() const
{
	return x == TNumTraits::zero && y == TNumTraits::zero;
}



/** Return true if at least one of the components is NaN
 */
template<typename T> inline
const bool Vector2D<T>::isNaN() const
{
	return num::isNaN(x) || num::isNaN(y);
}



/** Return squared norm of vector.
 */
template<typename T> inline
const typename Vector2D<T>::TValue Vector2D<T>::squaredNorm() const
{
	return dot(*this, *this);
}



/** Return norm of vector.
 */
template<typename T> inline
const typename Vector2D<T>::TValue Vector2D<T>::norm() const
{
	return ::lass::num::sqrt(squaredNorm());
}



/** return a unit vector with same direction/sense as this vector.
 *
 * <i>The normalized vector of <b>X</b> is a vector in the same direction but with norm (length) 1.
 * It is denoted <b>X^</b> and given by <b>X^</b> = <b>X</b> / |<b>X</b>|</i>,
 * http://mathworld.wolfram.com/NormalizedVector.html.
 */
template<typename T>
const Vector2D<T> Vector2D<T>::normal() const
{
	Vector2D<T> result(*this);
	result.normalize();
	return result;
}



/** return the reciprocal version of this vector
 */
template<typename T>
const Vector2D<T> Vector2D<T>::reciprocal() const
{
	Vector2D<T> result(TNumTraits::one, TNumTraits::one);
	result /= *this;
	return result;
}



/** return the vector perpendicular to this one, 90° to the left.
 */
template<typename T> inline
const Vector2D<T> Vector2D<T>::perp() const
{
	return Vector2D<T>(-y, x);
}



/** Project vector on this one
 */
template <typename T>
const Vector2D<T> Vector2D<T>::project(const Vector2D<T>& iB) const
{
	Vector2D<T> result(*this);
	result *= dot(iB, *this);
	result /= squaredNorm();
	return result;
}



/** Project vector on this one
 */
template<typename T> inline
const Vector2D<T> Vector2D<T>::reject(const Vector2D<T>& iB) const
{
	return iB - project(iB);
}



template<typename T> inline
const Vector2D<T> Vector2D<T>::reflect(const Vector2D<T>& iB) const
{
	return 2 * project(iB) - iB;
}



/** apply a function to every component
 */
template <typename T>
const Vector2D<T> Vector2D<T>::transform(T (*iOperator)(T)) const
{
	return Vector2D<T>(iOperator(x), iOperator(y));
}



/** Normalize vector.
 */
template<typename T> inline
void Vector2D<T>::normalize()
{
	*this /= norm();
}



/** Random unit vector
 */
template <typename T>
template <class RandomGenerator>
Vector2D<T> Vector2D<T>::random(RandomGenerator& ioGenerator)
{
	num::DistributionUniform<T, RandomGenerator, num::rtRightOpen> distribution(
		ioGenerator, TNumTraits::zero, 2 * TNumTraits::pi);
	const TValue theta = distribution();
	return Vector2D<T>(num::cos(theta), num::sin(theta));
}



/** dot product.
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
typename Vector2D<T>::TValue dot(const Vector2D<T>& iA, const Vector2D<T>& iB)
{
	return iA.x * iB.x + iA.y * iB.y;
}



/** perp dot product (cross product for 2D vectors).
 *  @relates lass::prim::Vector2D
 *
 *  <i>The "perp dot product" for <b>a</b> and <b>b</b> vectors in the plane is a modification of
 *  the two-dimensional dot product in which a is replaced by the perpendicular vector rotated 90°
 *  to the left defined by Hill (1994)</i>, http://mathworld.wolfram.com/PerpDotProduct.html.
 *
 *  It reminds a lot to the 3D cross product, as its result is equal to the z-value of the
 *  cross product of iA and iB extended to 3D space by setting their z-value to 0:
 *  Vector3D<T> c = cross(Vector3D<T>(a.x, a.y, 0), Vector3D<T>(b.x, b.y, 0)).  We know of this
 *  that c.x and c.y are both zero, and that c.z equals the perp dot product between iA and iB.
 */
template<typename T> inline
typename Vector2D<T>::TValue perpDot(const Vector2D<T>& iA, const Vector2D<T>& iB)
{
	return iA.x * iB.y - iA.y * iB.x;
}



/** @relates lass::prim::Vector2D
 */
template<typename T> inline
bool operator==(const Vector2D<T>& iA, const Vector2D<T>& iB)
{
	return iA.x == iB.x && iA.y == iB.y;
}



/** @relates lass::prim::Vector2D
 */
template<typename T> inline
bool operator!=(const Vector2D<T>& iA, const Vector2D<T>& iB)
{
	return !(iA == iB);
}



/** componentwise addition
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator+(const Vector2D<T>& iA, const Vector2D<T>& iB)
{
	Vector2D<T> result(iA);
	result += iB;
	return result;
}



/** componentwise subtraction
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator-(const Vector2D<T>& iA, const Vector2D<T>& iB)
{
	Vector2D<T> result(iA);
	result -= iB;
	return result;
}



/** Componentwise multiplication
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator*(const Vector2D<T>& iA, const Vector2D<T>& iB)
{
	Vector2D<T> result(iA);
	result *= iB;
	return result;
}



/** Componentwise division
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator/(const Vector2D<T>& iA, const Vector2D<T>& iB)
{
	Vector2D<T> result(iA);
	result /= iB;
	return result;
}



/** add iB to all components of iA.
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator+(const Vector2D<T>& iA, typename Vector2D<T>::TParam iB)
{
	Vector2D<T> result(iA);
	result += iB;
	return result;
}



/** subtract iB of all components of iA.
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator-(const Vector2D<T>& iA, typename Vector2D<T>::TParam iB)
{
	Vector2D<T> result(iA);
	result -= iB;
	return result;
}



/** muliply all components of iA by iB
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator*(const Vector2D<T>& iA, typename Vector2D<T>::TParam iB)
{
	Vector2D<T> result(iA);
	result *= iB;
	return result;
}



/** divide all components of iA by iB
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator/(const Vector2D<T>& iA, typename Vector2D<T>::TParam iB)
{
	Vector2D<T> result(iA);
	result /= iB;
	return result;
}



/** add iA to all components of iB
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator+(typename Vector2D<T>::TParam iA, const Vector2D<T>& iB)
{
	Vector2D<T> result(iB);
	result += iA;
	return result;
}



/** subtract iA of all components of iB
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator-(typename Vector2D<T>::TParam iA, const Vector2D<T>& iB)
{
	Vector2D<T> result(-iB);
	result += iA;
	return result;
}



/** return a vector with, for each coordinate, the minimum value of @a iA and @a iB
 *  @relates lass::prim::Vector2D
 */
template<typename T>
inline Vector2D<T> pointwiseMin(const Vector2D<T>& iA, const Vector2D<T>& iB)
{
	return Vector2D<T>(std::min(iA.x, iB.x), std::min(iA.y, iB.y));
}



/** return a vector with, for each coordinate, the maximum value of @a iA and @a iB
 *  @relates lass::prim::Vector2D
 */
template<typename T>
inline Vector2D<T> pointwiseMax(const Vector2D<T>& iA, const Vector2D<T>& iB)
{
	return Vector2D<T>(std::max(iA.x, iB.x), std::max(iA.y, iB.y));
}



/** multiply all components of iB with iA
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator*(typename Vector2D<T>::TParam iA, const Vector2D<T>& iB)
{
	Vector2D<T> result(iB);
	result *= iA;
	return result;
}



/** @relates lass::prim::Vector2D
 */
template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& oOStream,
											 const Vector2D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) << "(" << iB.x << ", " << iB.y  << ")";

	return oOStream;
}



/** @relates lass::prim::Vector2D
 */
template<typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& ioIStream,
											 Vector2D<T>& oB)
{
	Vector2D<T> result;

	Char c = 0;
	ioIStream >> c;
	if (c != '(')
	{
		ioIStream.clear(std::ios::failbit);
		return ioIStream;
	}

	c = 0;
	ioIStream >> result.x >> c;

	if (c != ',')
	{
		ioIStream.clear(std::ios::failbit);
		return ioIStream;
	}

	c = 0;
	ioIStream >> result.y >> c;
	if (c != ')')
	{
		ioIStream.clear(std::ios::failbit);
		return ioIStream;
	}

	oB = result;
	return ioIStream;
}



/** @relates lass::prim::Vector2D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Vector2D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream)
		<< "<Vector2D>" << iB.x << " " << iB.y << "</Vector2D>\n";
	return oOStream;
}



}

}

#endif
