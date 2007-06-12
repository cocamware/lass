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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_3D_INL




#include "vector_3d.h"
#include "../num/basic_ops.h"
#include "../num/distribution.h"



namespace lass
{

namespace prim
{


template<typename T>
Vector3D<T>::Vector3D():
	x(TNumTraits::zero),
	y(TNumTraits::zero),
	z(TNumTraits::zero)
{
}



template<typename T>
Vector3D<T>::Vector3D(TParam x, TParam y, TParam z):
	x(x),
	y(y),
	z(z)
{
}



template <typename T>
template <typename U>
Vector3D<T>::Vector3D(const Vector3D<U>& other):
	x(static_cast<TValue>(other.x)),
	y(static_cast<TValue>(other.y)),
	z(static_cast<TValue>(other.z))
{
}



template <typename T>
template <typename U>
Vector3D<T>::Vector3D(const U& x, const U& y, const U& z):
	x(static_cast<TValue>(x)),
	y(static_cast<TValue>(y)),
	z(static_cast<TValue>(z))
{
}



template<typename T>
typename Vector3D<T>::TConstReference Vector3D<T>::operator[](size_t index) const
{
	LASS_ASSERT(index < dimension);
	return *(&x + index);
}



template<typename T>
typename Vector3D<T>::TReference Vector3D<T>::operator[](size_t index)
{
	LASS_ASSERT(index < dimension);
	return *(&x + index);
}



/** Wrap index around range.
 */
template<typename T>
typename Vector3D<T>::TConstReference Vector3D<T>::at(signed index) const
 {
	return *(&x + num::mod(index, dimension));
}



/** Wrap index around range.
 */
template<typename T>
typename Vector3D<T>::TReference Vector3D<T>::at(signed index)
{
	return *(&x + num::mod(index, dimension));
}



/** A weird way to get back the same object
 */
template<typename T>
const Vector3D<T>& Vector3D<T>::operator+() const
{
	return *this;
}



template<typename T>
const Vector3D<T> Vector3D<T>::operator-() const
{
	return Vector3D(-x, -y, -z);
}



/** componentwise addition
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator+=(const Vector3D<T>& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}



/** componentwise subtraction
 */
template<typename T>
Vector3D<T>& Vector3D<T>::operator-=(const Vector3D<T>& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}



/** Componentwise multiplication.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator*=(const Vector3D<T>& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	return *this;
}



/** Componentwise division.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator/=(const Vector3D<T>& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	return *this;
}



/** add other to each component of this.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator+=(TParam other)
{
	x += other;
	y += other;
	z += other;
	return *this;
}



/** subtract other of each component of this.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator-=(TParam other)
{
	x -= other;
	y -= other;
	z -= other;
	return *this;
}



/** multiply each component of this with other.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator*=(TParam other)
{
	x *= other;
	y *= other;
	z *= other;
	return *this;
}



/** divide each component of this by other.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator/=(TParam other)
{
	x /= other;
	y /= other;
	z /= other;
	return *this;
}



/** Return true if all the components are (exactly!) zero
 */
template<typename T> inline
const bool Vector3D<T>::isZero() const
{
	return x == TNumTraits::zero && y == TNumTraits::zero && z == TNumTraits::zero;
}



/** Return true if at least one of the components is NaN
 */
template<typename T> inline
const bool Vector3D<T>::isNaN() const
{
	return num::isNaN(x) || num::isNaN(y) || num::isNaN(z);
}



/** Return squared norm of vector.
 */
template<typename T> inline
const typename Vector3D<T>::TValue Vector3D<T>::squaredNorm() const
{
	return dot(*this, *this);
}



/** Return norm of vector.
 */
template<typename T> inline
const typename Vector3D<T>::TValue Vector3D<T>::norm() const
{
	return num::sqrt(squaredNorm());
}



/** return a unit vector with same direction/sense as this vector.
 *
 * <i>The normalized vector of <b>X</b> is a vector in the same direction but with norm (length) 1.
 * It is denoted <b>X^</b> and given by <b>X^</b> = <b>X</b> / |<b>X</b>|</i>,
 * http://mathworld.wolfram.com/NormalizedVector.html.
 */
template<typename T>
const Vector3D<T> Vector3D<T>::normal() const
{
	Vector3D<T> result(*this);
	result.normalize();
	return result;
}



/** return the reciprocal version of this vector
 */
template<typename T>
const Vector3D<T> Vector3D<T>::reciprocal() const
{
	Vector3D<T> result(TNumTraits::one, TNumTraits::one, TNumTraits::one);
	result /= *this;
	return result;
}



/** Project vector on this one
 */
template <typename T>
const Vector3D<T> Vector3D<T>::project(const Vector3D<T>& other) const
{
	Vector3D<T> result(*this);
	result *= dot(other, *this);
	result /= squaredNorm();
	return result;
}



/** Reject vector from this one
 */
template<typename T> inline
const Vector3D<T> Vector3D<T>::reject(const Vector3D<T>& other) const
{
	return other - project(other);
}



template<typename T> inline
const Vector3D<T> Vector3D<T>::reflect(const Vector3D<T>& other) const
{
	return 2 * project(other) - other;
}



/** apply a function to every component
 */
template <typename T>
const Vector3D<T> Vector3D<T>::transform(T (*op)(T)) const
{
	return Vector3D<T>(op(x), op(y), op(z));
}



/** Normalize vector.
 */
template<typename T> inline
void Vector3D<T>::normalize()
{
	*this /= norm();
}



/** Random unit vector
 */
template <typename T>
template <class RandomGenerator>
Vector3D<T> Vector3D<T>::random(RandomGenerator& generator)
{
	num::DistributionUniform<TValue, RandomGenerator> zGenerator(
		generator, -TNumTraits::one, TNumTraits::one);
	const TValue z = zGenerator();
	const TValue r = num::sqrt(1 - num::sqr(z));

	num::DistributionUniform<TValue, RandomGenerator> thetaGenerator(
		generator, TNumTraits::zero, 2 * TNumTraits::pi);
	const TValue theta = thetaGenerator();

	return Vector3D<T>(r * num::cos(theta), r * num::sin(theta), z);
}



/** dot product.
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
typename Vector3D<T>::TValue dot(const Vector3D<T>& a, const Vector3D<T>& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}



/** cross product
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> cross(const Vector3D<T>& a, const Vector3D<T>& b)
{
	return Vector3D<T>(a.y * b.z - a.z * b.y,
					   a.z * b.x - a.x * b.z,
					   a.x * b.y - a.y * b.x);
}



/** @relates lass::prim::Vector3D
 */
template<typename T> inline
bool operator==(const Vector3D<T>& a, const Vector3D<T>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}



/** @relates lass::prim::Vector3D
 */
template<typename T> inline
bool operator!=(const Vector3D<T>& a, const Vector3D<T>& b)
{
	return !(a == b);
}



/** componentwise addition
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator+(const Vector3D<T>& a, const Vector3D<T>& b)
{
	Vector3D<T> result(a);
	result += b;
	return result;
}



/** componentwise subtraction
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator-(const Vector3D<T>& a, const Vector3D<T>& b)
{
	Vector3D<T> result(a);
	result -= b;
	return result;
}



/** Componentwise multiplication
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator*(const Vector3D<T>& a, const Vector3D<T>& b)
{
	Vector3D<T> result(a);
	result *= b;
	return result;
}



/** Componentwise division
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator/(const Vector3D<T>& a, const Vector3D<T>& b)
{
	Vector3D<T> result(a);
	result /= b;
	return result;
}



/** add b to all components of a.
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator+(const Vector3D<T>& a, typename Vector3D<T>::TParam b)
{
	Vector3D<T> result(a);
	result += b;
	return result;
}



/** subtract b of all components of a.
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator-(const Vector3D<T>& a, typename Vector3D<T>::TParam b)
{
	Vector3D<T> result(a);
	result -= b;
	return result;
}



/** muliply all components of a by b
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator*(const Vector3D<T>& a, typename Vector3D<T>::TParam b)
{
	Vector3D<T> result(a);
	result *= b;
	return result;
}



/** divide all components of a by b
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator/(const Vector3D<T>& a, typename Vector3D<T>::TParam b)
{
	Vector3D<T> result(a);
	result /= b;
	return result;
}



/** add a to all components of b
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator+(typename Vector3D<T>::TParam a, const Vector3D<T>& b)
{
	Vector3D<T> result(b);
	result += a;
	return result;
}



/** subtract a of all components of b
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator-(typename Vector3D<T>::TParam a, const Vector3D<T>& b)
{
	Vector3D<T> result(-b);
	result += a;
	return result;
}



/** multiply all components of b with a
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator*(typename Vector3D<T>::TParam a, const Vector3D<T>& b)
{
	Vector3D<T> result(b);
	result *= a;
	return result;
}



/** return a vector with, for each coordinate, the minimum value of @a a and @a b
 *  @relates lass::prim::Vector3D
 */
template<typename T>
inline Vector3D<T> pointwiseMin(const Vector3D<T>& a, const Vector3D<T>& b)
{
	return Vector3D<T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}



/** return a vector with, for each coordinate, the maximum value of @a a and @a b
 *  @relates lass::prim::Vector3D
 */
template<typename T>
inline Vector3D<T> pointwiseMax(const Vector3D<T>& a, const Vector3D<T>& b)
{
	return Vector3D<T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}



/** interpolate linearly between two vectors: a + t * (b - a)
 *  @relates lass::prim::Vector3D
 */
template<typename T>
inline Vector3D<T> lerp(const Vector3D<T>& a, const Vector3D<T>& b, typename Vector3D<T>::TParam t)
{
	Vector3D<T> result = b;
	result -= a;
	result *= t;
	result += a;
	return result;
}



/** @relates lass::prim::Vector3D
 */
template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream, const Vector3D<T>& b)
{
	LASS_ENFORCE_STREAM(stream) << "(" << b.x << ", " << b.y << ", " << b.z << ")";
	return stream;
}



/** @relates lass::prim::Vector3D
 */
template<typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream, Vector3D<T>& b)
{
	Vector3D<T> result;

	char c = 0;
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
	if (c != ',')
	{
		stream.clear(std::ios::failbit);
		return stream;
	}

	c = 0;
	stream >> result.z >> c;
	if (c != ')')
	{
		stream.clear(std::ios::failbit);
		return stream;
	}

	b = result;
	return stream;
}



/** @relates lass::prim::Vector3D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& stream, const Vector3D<T>& b)
{
	LASS_ENFORCE_STREAM(stream)
		<< "<Vector3D>" << b.x << " " << b.y << " " << b.z << "</Vector3D>\n";
	return stream;
}



}

}

#endif
