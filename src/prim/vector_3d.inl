/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
Vector3D<T>::Vector3D(TParam iX, TParam iY, TParam iZ):
	x(iX),
	y(iY),
	z(iZ)
{
}



template<typename T> 
typename Vector3D<T>::TConstReference Vector3D<T>::operator[](unsigned iIndex) const
{
	LASS_ASSERT(iIndex < dimension);
	return *(&x + iIndex);
}



template<typename T> 
typename Vector3D<T>::TReference Vector3D<T>::operator[](unsigned iIndex)
{
	LASS_ASSERT(iIndex < dimension);
	return *(&x + iIndex);
}



/** Wrap index around range. 
 */
template<typename T> 
typename Vector3D<T>::TConstReference Vector3D<T>::at(signed iIndex) const
 {
	return *(&x + num::mod(iIndex, dimension));
}



/** Wrap index around range. 
 */
template<typename T> 
typename Vector3D<T>::TReference Vector3D<T>::at(signed iIndex)
{
	return *(&x + num::mod(iIndex, dimension));
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
Vector3D<T>& Vector3D<T>::operator+=(const Vector3D<T>& iB) 
{
	x += iB.x;
	y += iB.y;
	z += iB.z;
	return *this;
}



/** componentwise subtraction
 */
template<typename T> 
Vector3D<T>& Vector3D<T>::operator-=(const Vector3D<T>& iB)
{
	x -= iB.x;
	y -= iB.y;
	z -= iB.z;
	return *this;
}



/** Componentwise multiplication.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator*=(const Vector3D<T>& iB)
{
	x *= iB.x;
	y *= iB.y;
	z *= iB.z;
	return *this;
}



/** Componentwise division.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator/=(const Vector3D<T>& iB)
{
	x /= iB.x;
	y /= iB.y;
	z /= iB.z;
	return *this;
}



/** add iB to each component of this.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator+=(TParam iB)
{
	x += iB;
	y += iB;
	z += iB;
	return *this;
}



/** subtract iB of each component of this.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator-=(TParam iB)
{
	x -= iB;
	y -= iB;
	z -= iB;
	return *this;
}



/** multiply each component of this with iB.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator*=(TParam iB)
{
	x *= iB;
	y *= iB;
	z *= iB;
	return *this;
}



/** divide each component of this by iB.
 */
template<typename T> inline
Vector3D<T>& Vector3D<T>::operator/=(TParam iB)
{
	x /= iB;
	y /= iB;
	z /= iB;
	return *this;
}



/** Return true if all the components are (exactly!) zero
 */
template<typename T> inline
const bool Vector3D<T>::isZero() const 
{
	return x == TNumTraits::zero && y == TNumTraits::zero && z == TNumTraits::zero;
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
const Vector3D<T> Vector3D<T>::project(const Vector3D<T>& iB) const 
{
	Vector3D<T> result;
	result *= dot(iB, *this);
	result /= squaredNorm();
	return result;
}



/** Project vector on this one
 */
template<typename T> inline
const Vector3D<T> Vector3D<T>::reject(const Vector3D<T>& iB) const 
{
	return iB - project(iB);
}



/** apply a function to every component
 */
template <typename T>
const Vector3D<T> Vector3D<T>::transform(T (*iOperator)(T)) const
{
    return Vector3D<T>(iOperator(x), iOperator(y), iOperator(z));
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
Vector3D<T> Vector3D<T>::random(RandomGenerator& ioGenerator)
{
	num::DistributionUniform<TValue, RandomGenerator> zGenerator(
		ioGenerator, -TNumTraits::one, TNumTraits::one);
	const TValue z = zGenerator();
	const TValue r = num::sqrt(1 - num::sqr(z));

	num::DistributionUniform<TValue, RandomGenerator> thetaGenerator(
		ioGenerator, TNumTraits::zero, 2 * TNumTraits::pi);
    const TValue theta = thetaGenerator();

    return Vector3D<T>(r * num::cos(theta), r * num::sin(theta), z);
}



/** dot product.
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
typename Vector3D<T>::TValue dot(const Vector3D<T>& iA, const Vector3D<T>& iB)
{
	return iA.x * iB.x + iA.y * iB.y + iA.z * iB.z;
}



/** cross product
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> cross(const Vector3D<T>& iA, const Vector3D<T>& iB)
{
	return Vector3D<T>(iA.y * iB.z - iA.z * iB.y, 
					   iA.z * iB.x - iA.x * iB.z, 
					   iA.x * iB.y - iA.y * iB.x);
}



/** @relates lass::prim::Vector3D
 */
template<typename T> inline
bool operator==(const Vector3D<T>& iA, const Vector3D<T>& iB)
{
	return iA.x == iB.x && iA.y == iB.y && iA.z == iB.z;
}



/** @relates lass::prim::Vector3D
 */
template<typename T> inline
bool operator!=(const Vector3D<T>& iA, const Vector3D<T>& iB)
{
	return !(iA == iB);
}



/** componentwise addition
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator+(const Vector3D<T>& iA, const Vector3D<T>& iB)
{
	Vector3D<T> result(iA);
	result += iB;
	return result;
}



/** componentwise subtraction
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator-(const Vector3D<T>& iA, const Vector3D<T>& iB)
{
	Vector3D<T> result(iA);
	result -= iB;
	return result;
}



/** Componentwise multiplication
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator*(const Vector3D<T>& iA, const Vector3D<T>& iB)
{
	Vector3D<T> result(iA);
	result *= iB;
	return result;
}



/** Componentwise division
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator/(const Vector3D<T>& iA, const Vector3D<T>& iB)
{
	Vector3D<T> result(iA);
	result /= iB;
	return result;
}



/** add iB to all components of iA.
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator+(const Vector3D<T>& iA, typename Vector3D<T>::TParam iB)
{
	Vector3D<T> result(iA);
	result += iB;
	return result;
}



/** subtract iB of all components of iA.
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator-(const Vector3D<T>& iA, typename Vector3D<T>::TParam iB)
{
	Vector3D<T> result(iA);
	result -= iB;
	return result;
}



/** muliply all components of iA by iB
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator*(const Vector3D<T>& iA, typename Vector3D<T>::TParam iB)
{
	Vector3D<T> result(iA);
	result *= iB;
	return result;
}



/** divide all components of iA by iB
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator/(const Vector3D<T>& iA, typename Vector3D<T>::TParam iB)
{
	Vector3D<T> result(iA);
	result /= iB;
	return result;
}



/** add iA to all components of iB
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator+(typename Vector3D<T>::TParam iA, const Vector3D<T>& iB)
{
	Vector3D<T> result(iB);
	result += iA;
	return result;
}



/** subtract iA of all components of iB
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator-(typename Vector3D<T>::TParam iA, const Vector3D<T>& iB)
{
	Vector3D<T> result(-iB);
	result += iA;
	return result;
}



/** multiply all components of iB with iA
 *  @relates lass::prim::Vector3D
 */
template<typename T> inline
Vector3D<T> operator*(typename Vector3D<T>::TParam iA, const Vector3D<T>& iB)
{
	Vector3D<T> result(iB);
	result *= iA;
	return result;
}



/** return a vector with, for each coordinate, the minimum value of @a iA and @a iB
 *  @relates lass::prim::Vector3D
 */
template<typename T>
inline Vector3D<T> pointwiseMin(const Vector3D<T>& iA, const Vector3D<T>& iB)
{
    return Vector3D<T>(std::min(iA.x, iB.x), std::min(iA.y, iB.y), std::min(iA.z, iB.z));
}



/** return a vector with, for each coordinate, the maximum value of @a iA and @a iB
 *  @relates lass::prim::Vector3D
 */
template<typename T>
inline Vector3D<T> pointwiseMax(const Vector3D<T>& iA, const Vector3D<T>& iB)
{
    return Vector3D<T>(std::max(iA.x, iB.x), std::max(iA.y, iB.y), std::max(iA.z, iB.z));
}



/** @relates lass::prim::Vector3D
 */
template<typename T>
std::ostream& operator<<(std::ostream& oOStream, const Vector3D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) << "(" << iB.x << ", " << iB.y << ", " << iB.z << ")";
	return oOStream;
}



/** @relates lass::prim::Vector3D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Vector3D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) 
		<< "<Vector3D>" << iB.x << " " << iB.y << " " << iB.z << "</Vector3D>\n";
	return oOStream;
}



/** @relates lass::prim::Vector3D
 */
template<typename T>
std::istream& operator>>(std::istream& ioIStream, Vector3D<T>& oB)
{
	Vector3D<T> result;

	char c = 0;
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
	if (c != ',')
	{
		ioIStream.clear(std::ios::failbit);
		return ioIStream;
	}

	c = 0;
	ioIStream >> result.z >> c;
	if (c != ')')
	{
		ioIStream.clear(std::ios::failbit);
		return ioIStream;
	}

	oB = result;
	return ioIStream;
}



}

}

#endif
