/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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
Vector2D<T>::Vector2D(TParam x, TParam y):
	x(x),
	y(y)
{
}



template <typename T>
template <typename U>
Vector2D<T>::Vector2D(const Vector2D<U>& other):
	x(static_cast<TValue>(other.x)),
	y(static_cast<TValue>(other.y))
{
}



template <typename T>
template <typename U>
Vector2D<T>::Vector2D(const U& x, const U& y):
	x(static_cast<TValue>(x)),
	y(static_cast<TValue>(y))
{
}



template<typename T> inline
typename Vector2D<T>::TConstReference Vector2D<T>::operator[](size_t index) const
{
	LASS_ASSERT(index < dimension);
	return *(&x + index);
}



template<typename T> inline
typename Vector2D<T>::TReference Vector2D<T>::operator[](size_t index)
{
	LASS_ASSERT(index < dimension);
	return *(&x + index);
}



/** Wrap index around range.
 */
template<typename T> inline
typename Vector2D<T>::TConstReference Vector2D<T>::at(signed index) const
{
	return *(&x + num::mod(index, dimension));
}



/** Wrap index around range.
 */
template<typename T> inline
typename Vector2D<T>::TReference Vector2D<T>::at(signed index)
{
	return *(&x + num::mod(index, dimension));
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
Vector2D<T>& Vector2D<T>::operator+=(const Vector2D<T>& other)
{
	x += other.x;
	y += other.y;
	return *this;
}



/** componentwise subtraction
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator-=(const Vector2D<T>& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}



/** Componentwise multiplication.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator*=(const Vector2D<T>& other)
{
	x *= other.x;
	y *= other.y;
	return *this;
}



/** Componentwise division.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator/=(const Vector2D<T>& other)
{
	x /= other.x;
	y /= other.y;
	return *this;
}



/** add other to each component of this.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator+=(TParam other)
{
	x += other;
	y += other;
	return *this;
}



/** subtract other of each component of this.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator-=(TParam other)
{
	x -= other;
	y -= other;
	return *this;
}



/** multiply each component of this with other.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator*=(TParam other)
{
	x *= other;
	y *= other;
	return *this;
}



/** divide each component of this by other.
 */
template<typename T> inline
Vector2D<T>& Vector2D<T>::operator/=(TParam other)
{
	x /= other;
	y /= other;
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
const Vector2D<T> Vector2D<T>::project(const Vector2D<T>& other) const
{
	Vector2D<T> result(*this);
	result *= dot(other, *this);
	result /= squaredNorm();
	return result;
}



/** Project vector on this one
 */
template<typename T> inline
const Vector2D<T> Vector2D<T>::reject(const Vector2D<T>& other) const
{
	return other - project(other);
}



template<typename T> inline
const Vector2D<T> Vector2D<T>::reflect(const Vector2D<T>& other) const
{
	return 2 * project(other) - other;
}



/** apply a function to every component
 */
template <typename T>
const Vector2D<T> Vector2D<T>::transform(T (*op)(T)) const
{
	return Vector2D<T>(op(x), op(y));
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
Vector2D<T> Vector2D<T>::random(RandomGenerator& generator)
{
	num::DistributionUniform<T, RandomGenerator, num::rtRightOpen> distribution(
		generator, TNumTraits::zero, 2 * TNumTraits::pi);
	const TValue theta = distribution();
	return Vector2D<T>(num::cos(theta), num::sin(theta));
}



/** dot product.
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
typename Vector2D<T>::TValue dot(const Vector2D<T>& a, const Vector2D<T>& b)
{
	return a.x * b.x + a.y * b.y;
}



/** returns cosine of angle between both vectors.
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
typename Vector2D<T>::TValue cos(const Vector2D<T>& a, const Vector2D<T>& b)
{
	return dot(a, b) / num::sqrt(a.squaredNorm() * b.squaredNorm());
}



/** perp dot product (cross product for 2D vectors).
 *  @relates lass::prim::Vector2D
 *
 *  <i>The "perp dot product" for <b>a</b> and <b>b</b> vectors in the plane is a modification of
 *  the two-dimensional dot product in which a is replaced by the perpendicular vector rotated 90°
 *  to the left defined by Hill (1994)</i>, http://mathworld.wolfram.com/PerpDotProduct.html.
 *
 *  It reminds a lot to the 3D cross product, as its result is equal to the z-value of the
 *  cross product of a and b extended to 3D space by setting their z-value to 0:
 *  Vector3D<T> c = cross(Vector3D<T>(a.x, a.y, 0), Vector3D<T>(b.x, b.y, 0)).  We know of this
 *  that c.x and c.y are both zero, and that c.z equals the perp dot product between a and b.
 */
template<typename T> inline
typename Vector2D<T>::TValue perpDot(const Vector2D<T>& a, const Vector2D<T>& b)
{
	return a.x * b.y - a.y * b.x;
}



/** @relates lass::prim::Vector2D
 */
template<typename T> inline
bool operator==(const Vector2D<T>& a, const Vector2D<T>& b)
{
	return a.x == b.x && a.y == b.y;
}



/** @relates lass::prim::Vector2D
 */
template<typename T> inline
bool operator!=(const Vector2D<T>& a, const Vector2D<T>& b)
{
	return !(a == b);
}



/** componentwise addition
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator+(const Vector2D<T>& a, const Vector2D<T>& b)
{
	Vector2D<T> result(a);
	result += b;
	return result;
}



/** componentwise subtraction
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator-(const Vector2D<T>& a, const Vector2D<T>& b)
{
	Vector2D<T> result(a);
	result -= b;
	return result;
}



/** Componentwise multiplication
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator*(const Vector2D<T>& a, const Vector2D<T>& b)
{
	Vector2D<T> result(a);
	result *= b;
	return result;
}



/** Componentwise division
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator/(const Vector2D<T>& a, const Vector2D<T>& b)
{
	Vector2D<T> result(a);
	result /= b;
	return result;
}



/** add b to all components of a.
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator+(const Vector2D<T>& a, typename Vector2D<T>::TParam b)
{
	Vector2D<T> result(a);
	result += b;
	return result;
}



/** subtract b of all components of a.
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator-(const Vector2D<T>& a, typename Vector2D<T>::TParam b)
{
	Vector2D<T> result(a);
	result -= b;
	return result;
}



/** muliply all components of a by b
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator*(const Vector2D<T>& a, typename Vector2D<T>::TParam b)
{
	Vector2D<T> result(a);
	result *= b;
	return result;
}



/** divide all components of a by b
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator/(const Vector2D<T>& a, typename Vector2D<T>::TParam b)
{
	Vector2D<T> result(a);
	result /= b;
	return result;
}



/** add a to all components of b
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator+(typename Vector2D<T>::TParam a, const Vector2D<T>& b)
{
	Vector2D<T> result(b);
	result += a;
	return result;
}



/** subtract a of all components of b
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator-(typename Vector2D<T>::TParam a, const Vector2D<T>& b)
{
	Vector2D<T> result(-b);
	result += a;
	return result;
}



/** multiply all components of b with a
 *  @relates lass::prim::Vector2D
 */
template<typename T> inline
Vector2D<T> operator*(typename Vector2D<T>::TParam a, const Vector2D<T>& b)
{
	Vector2D<T> result(b);
	result *= a;
	return result;
}



/** return a vector with, for each coordinate, the minimum value of @a a and @a b
 *  @relates lass::prim::Vector2D
 */
template<typename T>
inline Vector2D<T> pointwiseMin(const Vector2D<T>& a, const Vector2D<T>& b)
{
	return Vector2D<T>(std::min(a.x, b.x), std::min(a.y, b.y));
}



/** return a vector with, for each coordinate, the maximum value of @a a and @a b
 *  @relates lass::prim::Vector2D
 */
template<typename T>
inline Vector2D<T> pointwiseMax(const Vector2D<T>& a, const Vector2D<T>& b)
{
	return Vector2D<T>(std::max(a.x, b.x), std::max(a.y, b.y));
}



/** interpolate linearly between two vectors: a + t * (b - a)
 *  @relates lass::prim::Vector2D
 */
template<typename T>
inline Vector2D<T> lerp(const Vector2D<T>& a, const Vector2D<T>& b, typename Vector2D<T>::TParam t)
{
	Vector2D<T> result = b;
	result -= a;
	result *= t;
	result += a;
	return result;
}



/** @relates lass::prim::Vector2D
 */
template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream, const Vector2D<T>& b)
{
	LASS_ENFORCE_STREAM(stream) << "(" << b.x << ", " << b.y  << ")";

	return stream;
}



/** @relates lass::prim::Vector2D
 */
template<typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream, Vector2D<T>& b)
{
	Vector2D<T> result;

	Char c = 0;
	stream >> c;
	if (c != '(')
	{
		stream.clear(std::ios::failbit);
		return stream;
	}

	c = 0;
	stream >> result.x >> c;

	if (c != ',')
	{
		stream.clear(std::ios::failbit);
		return stream;
	}

	c = 0;
	stream >> result.y >> c;
	if (c != ')')
	{
		stream.clear(std::ios::failbit);
		return stream;
	}

	b = result;
	return stream;
}



/** @relates lass::prim::Vector2D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& stream, const Vector2D<T>& b)
{
	LASS_ENFORCE_STREAM(stream)
		<< "<Vector2D>" << b.x << " " << b.y << "</Vector2D>\n";
	return stream;
}



}

}

#endif
