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
Vector4D<T>::Vector4D(TParam iX, TParam iY, TParam iZ, TParam iW) :
	x(iX),
	y(iY),
	z(iZ),
	w(iW)
{
}



template<typename T> inline
typename Vector4D<T>::TConstReference Vector4D<T>::operator[](unsigned iIndex) const  
{
	LASS_ASSERT(iIndex < dimension);
	return *(&x + iIndex);
}



template<typename T> inline
typename Vector4D<T>::TReference Vector4D<T>::operator[](unsigned iIndex) 
{
	LASS_ASSERT(iIndex < dimension);
	return *(&x + iIndex);
}



/** Wrap index around range. 
 */
template<typename T> inline
typename Vector4D<T>::TConstReference Vector4D<T>::at(signed iIndex) const 
{
	return *(&x + num::mod(iIndex, dimension));
}



/** Wrap index around range. 
 */
template<typename T> inline
typename Vector4D<T>::TReference Vector4D<T>::at(signed iIndex) 
{
	return *(&x + num::mod(iIndex, dimension));
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
Vector4D<T>& Vector4D<T>::operator+=(const Vector4D<T>& iB)
{
	x += iB.x;
	y += iB.y;
	z += iB.z;
	w += iB.w;
	return *this;
}



/** componentwise subtraction
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator-=(const Vector4D<T>& iB)
{
	x -= iB.x;
	y -= iB.y;
	z -= iB.z;
	w -= iB.w;
	return *this;
}



/** Componentwise multiplication.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator*=(const Vector4D<T>& iB)
{
	x *= iB.x;
	y *= iB.y;
	z *= iB.z;
	w *= iB.w;
	return *this;
}



/** Componentwise division.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator/=(const Vector4D<T>& iB)
{
	x /= iB.x;
	y /= iB.y;
	z /= iB.z;
	w /= iB.w;
	return *this;
}



/** add iB to each component of this.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator+=(TParam iB)
{
	x += iB;
	y += iB;
	z += iB;
	w += iB;
	return *this;
}



/** subtract iB of each component of this.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator-=(TParam iB)
{
	x -= iB;
	y -= iB;
	z -= iB;
	w -= iB;
	return *this;
}



/** multiply each component of this with iB.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator*=(TParam iB)
{
	x *= iB;
	y *= iB;
	z *= iB;
	w *= iB;
	return *this;
}



/** divide each component of this by iB.
 */
template<typename T> inline
Vector4D<T>& Vector4D<T>::operator/=(TParam iB)
{
	x /= iB;
	y /= iB;
	z /= iB;
	w /= iB;
	return *this;
}



/** Return true if all the components are (exactly!) zero
 */
template<typename T> inline
const bool Vector4D<T>::isZero() const
{
	return	x == TNumTraits::zero && y == TNumTraits::zero &&
			z == TNumTraits::zero && w == TNumTraits::zero;
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
const Vector4D<T> Vector4D<T>::project(const Vector4D<T>& iB) const 
{
	Vector4D<T> result;
	result *= dot(iB, *this);
	result /= squaredNorm();
	return result;
}



/** Project vector on this one
 */
template<typename T> inline
const Vector4D<T> Vector4D<T>::reject(const Vector4D<T>& iB) const 
{
	return iB - project(iB);
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
typename Vector4D<T>::TValue dot(const Vector4D<T>& iA, const Vector4D<T>& iB)
{
	return iA.x * iB.x + iA.y * iB.y + iA.z * iB.z + iA.w * iB.w;
}



/** @relates lass::prim::Vector4D
 */
template<typename T> inline 
bool operator==(const Vector4D<T>& iA, const Vector4D<T>& iB)
{
	return iA.x == iB.x && iA.y == iB.y && iA.z == iB.z && iA.w == iB.w;
}



/** @relates lass::prim::Vector4D
 */
template<typename T> inline
bool operator!=(const Vector4D<T>& iA, const Vector4D<T>& iB)
{
	return !(iA == iB);
}



/** componentwise addition
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator+(const Vector4D<T>& iA, const Vector4D<T>& iB)
{
	Vector4D<T> result(iA);
	result += iB;
	return result;
}



/** componentwise subtraction
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator-(const Vector4D<T>& iA, const Vector4D<T>& iB)
{
	Vector4D<T> result(iA);
	result -= iB;
	return result;
}



/** Componentwise multiplication
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator*(const Vector4D<T>& iA, const Vector4D<T>& iB)
{
	Vector4D<T> result(iA);
	result *= iB;
	return result;
}



/** Componentwise division
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator/(const Vector4D<T>& iA, const Vector4D<T>& iB)
{
	Vector4D<T> result(iA);
	result /= iB;
	return result;
}



/** add iB to all components of iA.
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator+(const Vector4D<T>& iA, typename Vector4D<T>::TParam iB)
{
	Vector4D<T> result(iA);
	result += iB;
	return result;
}



/** subtract iB of all components of iA.
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator-(const Vector4D<T>& iA, typename Vector4D<T>::TParam iB)
{
	Vector4D<T> result(iA);
	result -= iB;
	return result;
}



/** muliply all components of iA by iB
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator*(const Vector4D<T>& iA, typename Vector4D<T>::TParam iB)
{
	Vector4D<T> result(iA);
	result *= iB;
	return result;
}



/** divide all components of iA by iB
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator/(const Vector4D<T>& iA, typename Vector4D<T>::TParam iB)
{
	Vector4D<T> result(iA);
	result /= iB;
	return result;
}



/** add iA to all components of iB
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator+(typename Vector4D<T>::TParam iA, const Vector4D<T>& iB)
{
	Vector4D<T> result(iB);
	result += iA;
	return result;
}



/** subtract iA of all components of iB
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator-(typename Vector4D<T>::TParam iA, const Vector4D<T>& iB)
{
	Vector4D<T> result(-iB);
	result += iA;
	return result;
}



/** multiply all components of iB with iA
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> operator*(typename Vector4D<T>::TParam iA, const Vector4D<T>& iB)
{
	Vector4D<T> result(iB);
	result *= iA;
	return result;
}



/** return a vector with, for each coordinate, the minimum value of @a iA and @a iB
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> pointwiseMin(const Vector4D<T>& iA, const Vector4D<T>& iB)
{
    return Vector4D<T>(std::min(iA.x, iB.x), std::min(iA.y, iB.y), 
                       std::min(iA.z, iB.z), std::min(iA.w, iB.w));
}



/** return a vector with, for each coordinate, the maximum value of @a iA and @a iB
 *  @relates lass::prim::Vector4D
 */
template<typename T> inline
Vector4D<T> pointwiseMax(const Vector4D<T>& iA, const Vector4D<T>& iB)
{
    return Vector4D<T>(std::max(iA.x, iB.x), std::max(iA.y, iB.y), 
                       std::max(iA.z, iB.z), std::max(iA.w, iB.w));
}



/** @relates lass::prim::Vector4D
 */
template<typename T, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& oOStream, 
                                             const Vector4D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) << "(" << iB.x << ", " << iB.y << ", " << iB.z << ", " << iB.w << ")";
	return oOStream;
}



/** @relates lass::prim::Vector4D
 */
template<typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& ioIStream, 
                                             Vector4D<T>& oB)
{
	Vector4D<T> result;

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
	if (c != ',')
	{
		ioIStream.clear(std::ios::failbit);
		return ioIStream;
	}

	c = 0;
	ioIStream >> result.w >> c;
	if (c != ')')
	{
		ioIStream.clear(std::ios::failbit);
		return ioIStream;
	}

	oB = result;
	return ioIStream;
}



/** @relates lass::prim::Vector4D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Vector4D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) 
		<< "<Vector4D>" << iB.x << " " << iB.y << " " << iB.z << " " << iB.w << "</Vector4D>\n";
	return oOStream;
}



}

}

#endif
