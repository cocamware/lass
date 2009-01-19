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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_4D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_4D_INL




#include "vector_4d.h"



namespace lass
{

namespace prim
{

template<typename T> inline
Vector4D<T>::Vector4D() :
	x(T()),
	y(T()),
	z(T()),
	w(T())
{
}



template<typename T> inline
Vector4D<T>::Vector4D(TParam x, TParam y, TParam z, TParam w) :
	x(x),
	y(y),
	z(z),
	w(w)
{
}



template <typename T>
template <typename U>
Vector4D<T>::Vector4D(const Vector4D<U>& other):
	x(static_cast<TValue>(other.x)),
	y(static_cast<TValue>(other.y)),
	z(static_cast<TValue>(other.z)),
	w(static_cast<TValue>(other.w))
{
}



template <typename T>
template <typename U>
Vector4D<T>::Vector4D(const U& x, const U& y, const U& z, const U& w):
	x(static_cast<TValue>(x)),
	y(static_cast<TValue>(y)),
	z(static_cast<TValue>(z)),
	w(static_cast<TValue>(w))
{
}



template<typename T> inline
typename Vector4D<T>::TConstReference Vector4D<T>::operator[](size_t index) const
{
	LASS_ASSERT(index < dimension);
	return *(&x + index);
}



template<typename T> inline
typename Vector4D<T>::TReference Vector4D<T>::operator[](size_t index)
{
	LASS_ASSERT(index < dimension);
	return *(&x + index);
}



/** Wrap index around range.
 */
template<typename T> inline
typename Vector4D<T>::TConstReference Vector4D<T>::at(signed index) const
{
	return *(&x + num::mod(index, dimension));
}



/** Wrap index around range.
 */
template<typename T> inline
typename Vector4D<T>::TReference Vector4D<T>::at(signed index)
{
	return *(&x + num::mod(index, dimension));
}



/** A weird way to get back the same object
 */
template<typename T> inline
const Vector4D<T>& Vector4D<T>::operator+() const
{
	return *this;
}



template<typename T> inline
const Vector4D<T> Vector4D<T>::operator-() const
{
	return Vector4D(-x, -y, -z, -w);
}



/** componentwise addition
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator+=(const Vector4D<T>& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	return *this;
}



/** componentwise subtraction
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator-=(const Vector4D<T>& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	return *this;
}



/** Componentwise multiplication.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator*=(const Vector4D<T>& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	w *= other.w;
	return *this;
}



/** Componentwise division.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator/=(const Vector4D<T>& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	w /= other.w;
	return *this;
}



/** add other to each component of this.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator+=(TParam other)
{
	x += other;
	y += other;
	z += other;
	w += other;
	return *this;
}



/** subtract other of each component of this.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator-=(TParam other)
{
	x -= other;
	y -= other;
	z -= other;
	w -= other;
	return *this;
}



/** multiply each component of this with other.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator*=(TParam other)
{
	x *= other;
	y *= other;
	z *= other;
	w *= other;
	return *this;
}



/** divide each component of this by other.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator/=(TParam other)
{
	x /= other;
	y /= other;
	z /= other;
	w /= other;
	return *this;
}



/** Return true if all the components are (exactly!) zero
 */
template<typename T> inline
const bool Vector4D<T>::isZero() const
{
	return  x == TNumTraits::zero && y == TNumTraits::zero &&
			z == TNumTraits::zero && w == TNumTraits::zero;
}



/** Return true if at least one of the components is NaN
 */
template<typename T> inline
const bool Vector4D<T>::isNaN() const
{
	return num::isNaN(x) || num::isNaN(y) || num::isNaN(z) || num::isNaN(w);
}



/** Return squared norm of vector.
 */
template<typename T> inline
const typename Vector4D<T>::TValue Vector4D<T>::squaredNorm() const
{
	return dot(*this, *this);
}



/** Return norm of vector.
 */
template<typename T> inline
const typename  Vector4D<T>::TValue Vector4D<T>::norm() const
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
const Vector4D<T> Vector4D<T>::normal() const
{
	Vector4D<T> result(*this);
	result.normalize();
	return result;
}



/** Project vector on this one
 */
template <typename T>
const Vector4D<T> Vector4D<T>::project(const Vector4D<T>& other) const
{
	Vector4D<T> result(*this);
	result *= dot(other, *this);
	result /= squaredNorm();
	return result;
}



/** Project vector on this one
 */
template<typename T> inline
const Vector4D<T> Vector4D<T>::reject(const Vector4D<T>& other) const
{
	return other - project(other);
}



template<typename T> inline
const Vector4D<T> Vector4D<T>::reflect(const Vector4D<T>& other) const
{
	return 2 * project(other) - other;
}



/** apply a function to every component
 */
template <typename T>
const Vector4D<T> Vector4D<T>::transform(T (*iOperator)(T)) const
{
	return Vector4D<T>(iOperator(x), iOperator(y), iOperator(z), iOperator(w));
}



/** Normalize vector.
 */
template<typename T> inline
void Vector4D<T>::normalize()
{
	*this /= norm();
}



/** dot product.
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
typename Vector4D<T>::TValue dot(const Vector4D<T>& a, const Vector4D<T>& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}



/** @relates lass::prim::Vector4D
 */
template<typename T> inline
bool operator==(const Vector4D<T>& a, const Vector4D<T>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}



/** @relates lass::prim::Vector4D
 */
template<typename T> inline
bool operator!=(const Vector4D<T>& a, const Vector4D<T>& b)
{
	return !(a == b);
}



/** componentwise addition
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator+(const Vector4D<T>& a, const Vector4D<T>& b)
{
	Vector4D<T> result(a);
	result += b;
	return result;
}



/** componentwise subtraction
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator-(const Vector4D<T>& a, const Vector4D<T>& b)
{
	Vector4D<T> result(a);
	result -= b;
	return result;
}



/** Componentwise multiplication
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator*(const Vector4D<T>& a, const Vector4D<T>& b)
{
	Vector4D<T> result(a);
	result *= b;
	return result;
}



/** Componentwise division
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator/(const Vector4D<T>& a, const Vector4D<T>& b)
{
	Vector4D<T> result(a);
	result /= b;
	return result;
}



/** add b to all components of a.
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator+(const Vector4D<T>& a, typename Vector4D<T>::TParam b)
{
	Vector4D<T> result(a);
	result += b;
	return result;
}



/** subtract b of all components of a.
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator-(const Vector4D<T>& a, typename Vector4D<T>::TParam b)
{
	Vector4D<T> result(a);
	result -= b;
	return result;
}



/** muliply all components of a by b
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator*(const Vector4D<T>& a, typename Vector4D<T>::TParam b)
{
	Vector4D<T> result(a);
	result *= b;
	return result;
}



/** divide all components of a by b
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator/(const Vector4D<T>& a, typename Vector4D<T>::TParam b)
{
	Vector4D<T> result(a);
	result /= b;
	return result;
}



/** add a to all components of b
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator+(typename Vector4D<T>::TParam a, const Vector4D<T>& b)
{
	Vector4D<T> result(b);
	result += a;
	return result;
}



/** subtract a of all components of b
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator-(typename Vector4D<T>::TParam a, const Vector4D<T>& b)
{
	Vector4D<T> result(-b);
	result += a;
	return result;
}



/** multiply all components of b with a
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator*(typename Vector4D<T>::TParam a, const Vector4D<T>& b)
{
	Vector4D<T> result(b);
	result *= a;
	return result;
}



/** return a vector with, for each coordinate, the minimum value of @a a and @a b
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> pointwiseMin(const Vector4D<T>& a, const Vector4D<T>& b)
{
	return Vector4D<T>(std::min(a.x, b.x), std::min(a.y, b.y),
					   std::min(a.z, b.z), std::min(a.w, b.w));
}



/** return a vector with, for each coordinate, the maximum value of @a a and @a b
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> pointwiseMax(const Vector4D<T>& a, const Vector4D<T>& b)
{
	return Vector4D<T>(std::max(a.x, b.x), std::max(a.y, b.y),
					   std::max(a.z, b.z), std::max(a.w, b.w));
}



/** interpolate linearly between two vectors: a + t * (b - a)
 *  @relates lass::prim::Vector4D
 */
template<typename T>
inline Vector4D<T> lerp(const Vector4D<T>& a, const Vector4D<T>& b, typename Vector4D<T>::TParam t)
{
	Vector4D<T> result = b;
	result -= a;
	result *= t;
	result += a;
	return result;
}



/** @relates lass::prim::Vector4D
 */
template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream, const Vector4D<T>& b)
{
	LASS_ENFORCE_STREAM(stream) << "(" << b.x << ", " << b.y << ", " << b.z << ", " << b.w << ")";
	return stream;
}



/** @relates lass::prim::Vector4D
 */
template<typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream, Vector4D<T>& b)
{
	Vector4D<T> result;

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
	if (c != ',')
	{
		stream.clear(std::ios::failbit);
		return stream;
	}

	c = 0;
	stream >> result.w >> c;
	if (c != ')')
	{
		stream.clear(std::ios::failbit);
		return stream;
	}

	b = result;
	return stream;
}



/** @relates lass::prim::Vector4D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& stream, const Vector4D<T>& b)
{
	LASS_ENFORCE_STREAM(stream)
		<< "<Vector4D>" << b.x << " " << b.y << " " << b.z << " " << b.w << "</Vector4D>\n";
	return stream;
}



}

}

#endif
