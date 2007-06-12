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



/** @struct lass::prim::Vector4D
 *  @brief 4D Vector
 *  @author BdG
 *  @date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_4D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_4D_H




#include "prim_common.h"
#include "../io/xml_o_stream.h"


namespace lass
{

namespace prim
{

template<typename T>
struct Vector4D
{
	typedef Vector4D<T> TSelf;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef typename num::NumTraits<T> TNumTraits;

	enum { dimension = 4 }; /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Vector4D<U> Type;
	};


	// public data

	TValue x;
	TValue y;
	TValue z;
	TValue w;


	// methods

	Vector4D();
	Vector4D(TParam x, TParam y, TParam z, TParam w);
	template <typename U> explicit Vector4D(const Vector4D<U>& other);
	template <typename U> Vector4D(const U& x, const U& y, const U& z, const U& w);

	typename Vector4D::TConstReference operator[](size_t index) const;
	typename Vector4D::TReference operator[](size_t index) ;
	typename Vector4D::TConstReference at(signed index) const;
	typename Vector4D::TReference at(signed index);

	const Vector4D<T>& operator+() const;
	const Vector4D<T> operator-() const;
	Vector4D<T>& operator+=(const Vector4D<T>& other);
	Vector4D<T>& operator-=(const Vector4D<T>& other);
	Vector4D<T>& operator*=(const Vector4D<T>& other);
	Vector4D<T>& operator/=(const Vector4D<T>& other);
	Vector4D<T>& operator+=(TParam other);
	Vector4D<T>& operator-=(TParam other);
	Vector4D<T>& operator*=(TParam other);
	Vector4D<T>& operator/=(TParam other);

	const bool isZero() const;
	const bool isNaN() const;
	const TValue squaredNorm() const;
	const TValue norm() const;
	const Vector4D<T> normal() const;
	const Vector4D<T> reciprocal() const;
	const Vector4D<T> project(const Vector4D<T>& other) const;
	const Vector4D<T> reject(const Vector4D<T>& other) const;
	const Vector4D<T> reflect(const Vector4D<T>& other) const;
	const Vector4D<T> transform(T (*op)(T)) const;

	void normalize();
};

template<typename T> typename Vector4D<T>::TValue dot(const Vector4D<T>& a, const Vector4D<T>& b);

template<typename T> bool operator==(const Vector4D<T>& a, const Vector4D<T>& b);
template<typename T> bool operator!=(const Vector4D<T>& a, const Vector4D<T>& b);

template<typename T> Vector4D<T> operator+(const Vector4D<T>& a, const Vector4D<T>& b);
template<typename T> Vector4D<T> operator-(const Vector4D<T>& a, const Vector4D<T>& b);
template<typename T> Vector4D<T> operator*(const Vector4D<T>& a, const Vector4D<T>& b);
template<typename T> Vector4D<T> operator/(const Vector4D<T>& a, const Vector4D<T>& b);

template<typename T> Vector4D<T> operator+(const Vector4D<T>& a, typename Vector4D<T>::TParam b);
template<typename T> Vector4D<T> operator-(const Vector4D<T>& a, typename Vector4D<T>::TParam b);
template<typename T> Vector4D<T> operator*(const Vector4D<T>& a, typename Vector4D<T>::TParam b);
template<typename T> Vector4D<T> operator/(const Vector4D<T>& a, typename Vector4D<T>::TParam b);

template<typename T> Vector4D<T> operator+(typename Vector4D<T>::TParam a, const Vector4D<T>& b);
template<typename T> Vector4D<T> operator-(typename Vector4D<T>::TParam a, const Vector4D<T>& b);
template<typename T> Vector4D<T> operator*(typename Vector4D<T>::TParam a, const Vector4D<T>& b);
// we don't allow scalar / vector.  you can do scalar * vector.reciprocal() for that.

template<typename T> Vector4D<T> pointwiseMin(const Vector4D<T>& a, const Vector4D<T>& b);
template<typename T> Vector4D<T> pointwiseMax(const Vector4D<T>& a, const Vector4D<T>& b);

template <typename T> Vector4D<T> lerp(const Vector4D<T>& a, const Vector4D<T>& b, typename Vector4D<T>::TParam t);

template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const Vector4D<T>& b);
template<typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& stream, Vector4D<T>& b);

template<typename T> io::XmlOStream& operator<<(io::XmlOStream& stream, const Vector4D<T>& b);


}

}

#include "vector_4d.inl"
#include "pyobject_util.inl"

#endif
