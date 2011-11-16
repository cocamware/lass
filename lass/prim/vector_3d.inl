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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_3D_INL




#include "vector_3d.h"
#include "../num/basic_ops.h"
#include "../num/distribution.h"



namespace lass
{

namespace prim
{


template<typename T> inline
Vector3D<T>::Vector3D():
	x(TNumTraits::zero),
	y(TNumTraits::zero),
	z(TNumTraits::zero)
{
}



template<typename T> inline
Vector3D<T>::Vector3D(TParam x, TParam y, TParam z):
	x(x),
	y(y),
	z(z)
{
}



template <typename T>
template <typename U> inline
Vector3D<T>::Vector3D(const Vector3D<U>& other):
	x(static_cast<TValue>(other.x)),
	y(static_cast<TValue>(other.y)),
	z(static_cast<TValue>(other.z))
{
}



template <typename T>
template <typename U> inline
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
bool Vector3D<T>::isZero() const
{
	return x == TNumTraits::zero && y == TNumTraits::zero && z == TNumTraits::zero;
}



/** Return true if at least one of the components is NaN
 */
template<typename T> inline
bool Vector3D<T>::isNaN() const
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
