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



/** @struct lass::prim::Vector2D
 *	@brief 2D Vector
 *	@author BdG
 *	@date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_2D_H
#pragma once



#include "prim_common.h"
#include "../io/xml_o_stream.h"



namespace lass
{

namespace prim
{

template<typename T>
struct Vector2D
{
    typedef Vector2D<T> TSelf;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef typename num::NumTraits<T> TNumTraits;

	enum { dimension = 2 };	/**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef typename Vector2D<U> Type;
	};


	// public data 

	TValue x;
	TValue y;


	// methods

	Vector2D() throw();
	Vector2D(TParam iX, TParam iY) throw();

	template <typename U> explicit Vector2D(const Vector2D<U>& iOther) throw(): 
		x(iOther.x), 
		y(iOther.y)
	{
	}

	typename Vector2D::TConstReference operator[](unsigned iIndex) const;
	typename Vector2D::TReference operator[](unsigned iIndex);
	typename Vector2D::TConstReference at(signed iIndex) const throw();
	typename Vector2D::TReference at(signed iIndex) throw();

	const Vector2D<T>& operator+() const throw();
	const Vector2D<T> operator-() const throw();
	Vector2D<T>& operator+=(const Vector2D<T>& iB) throw();
	Vector2D<T>& operator-=(const Vector2D<T>& iB) throw();
	Vector2D<T>& operator*=(const Vector2D<T>& iB) throw();
	Vector2D<T>& operator/=(const Vector2D<T>& iB) throw();
	Vector2D<T>& operator+=(TParam iB) throw();
	Vector2D<T>& operator-=(TParam iB) throw();
	Vector2D<T>& operator*=(TParam iB) throw();
	Vector2D<T>& operator/=(TParam iB) throw();

	const bool isZero() const throw();
	const TValue squaredNorm() const throw();
	const TValue norm() const throw();
	const Vector2D<T> normal() const throw();
	const Vector2D<T> reciprocal() const throw();
    const Vector2D<T> perp() const throw();
	const Vector2D<T> project(const Vector2D<T>& iB) const throw();
	const Vector2D<T> reject(const Vector2D<T>& iB) const throw();

	void normalize();

    template <class RandomGenerator> static Vector2D<T> random(RandomGenerator& ioGenerator);
};

template<typename T> typename Vector2D<T>::TValue dot(const Vector2D<T>& iA, const Vector2D<T>& iB) throw();
template<typename T> typename Vector2D<T>::TValue perpDot(const Vector2D<T>& iA, const Vector2D<T>& iB) throw();
template<typename T> Vector2D<T> transform(const Vector2D<T>& iA, T (*iF)(T));

template<typename T> bool operator==(const Vector2D<T>& iA, const Vector2D<T>& iB) throw();
template<typename T> bool operator!=(const Vector2D<T>& iA, const Vector2D<T>& iB) throw();

template<typename T> Vector2D<T> operator+(const Vector2D<T>& iA, const Vector2D<T>& iB) throw();
template<typename T> Vector2D<T> operator-(const Vector2D<T>& iA, const Vector2D<T>& iB) throw();
template<typename T> Vector2D<T> operator*(const Vector2D<T>& iA, const Vector2D<T>& iB) throw();
template<typename T> Vector2D<T> operator/(const Vector2D<T>& iA, const Vector2D<T>& iB) throw();

template<typename T> Vector2D<T> operator+(const Vector2D<T>& iA,typename  Vector2D<T>::TParam iB) throw();
template<typename T> Vector2D<T> operator-(const Vector2D<T>& iA,typename  Vector2D<T>::TParam iB) throw();
template<typename T> Vector2D<T> operator*(const Vector2D<T>& iA,typename  Vector2D<T>::TParam iB) throw();
template<typename T> Vector2D<T> operator/(const Vector2D<T>& iA,typename  Vector2D<T>::TParam iB) throw();

template<typename T> Vector2D<T> operator+(typename Vector2D<T>::TParam iA, const Vector2D<T>& iB) throw();
template<typename T> Vector2D<T> operator-(typename Vector2D<T>::TParam iA, const Vector2D<T>& iB) throw();
template<typename T> Vector2D<T> operator*(typename Vector2D<T>::TParam iA, const Vector2D<T>& iB) throw();
// we don't allow scalar / vector.  you can do scalar * vector.reciprocal() for that.

template<typename T> Vector2D<T> pointwiseMin(const Vector2D<T>& iA, const Vector2D<T>& iB) throw();
template<typename T> Vector2D<T> pointwiseMax(const Vector2D<T>& iA, const Vector2D<T>& iB) throw();

template<typename T> std::ostream& operator<<(std::ostream& ioOStream, const Vector2D<T>& iB);
template<typename T> std::istream& operator>>(std::istream& ioIStream, Vector2D<T>& iB);
template<typename T> io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Vector2D<T>& iB);



}

}

#include "vector_2d.inl"

#define LASS_PRIM_PYOBJECT_UTIL_VECTOR_2D
#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif
