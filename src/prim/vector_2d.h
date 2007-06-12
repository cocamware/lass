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



/** @struct lass::prim::Vector2D
 *  @brief 2D Vector
 *  @author BdG
 *  @date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_2D_H




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

	enum { dimension = 2 }; /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Vector2D<U> Type;
	};


	// public data

	TValue x;
	TValue y;


	// methods

	Vector2D();
	Vector2D(TParam x, TParam y);
	template <typename U> explicit Vector2D(const Vector2D<U>& other);
	template <typename U> Vector2D(const U& x, const U& y);

	typename Vector2D::TConstReference operator[](size_t index) const;
	typename Vector2D::TReference operator[](size_t index);
	typename Vector2D::TConstReference at(signed index) const;
	typename Vector2D::TReference at(signed index);

	const Vector2D<T>& operator+() const;
	const Vector2D<T> operator-() const;
	Vector2D<T>& operator+=(const Vector2D<T>& other);
	Vector2D<T>& operator-=(const Vector2D<T>& other);
	Vector2D<T>& operator*=(const Vector2D<T>& other);
	Vector2D<T>& operator/=(const Vector2D<T>& other);
	Vector2D<T>& operator+=(TParam other);
	Vector2D<T>& operator-=(TParam other);
	Vector2D<T>& operator*=(TParam other);
	Vector2D<T>& operator/=(TParam other);

	const bool isZero() const;
	const bool isNaN() const;
	const TValue squaredNorm() const;
	const TValue norm() const;
	const Vector2D<T> normal() const;
	const Vector2D<T> reciprocal() const;
	const Vector2D<T> perp() const;
	const Vector2D<T> project(const Vector2D<T>& other) const;
	const Vector2D<T> reject(const Vector2D<T>& other) const;
	const Vector2D<T> reflect(const Vector2D<T>& other) const;
	const Vector2D<T> transform(T (*iOperator)(T)) const;

	void normalize();

	template <class RandomGenerator> static Vector2D<T> random(RandomGenerator& generator);
};

template<typename T> typename Vector2D<T>::TValue dot(const Vector2D<T>& a, const Vector2D<T>& b);
template<typename T> typename Vector2D<T>::TValue perpDot(const Vector2D<T>& a, const Vector2D<T>& b);

template<typename T> bool operator==(const Vector2D<T>& a, const Vector2D<T>& b);
template<typename T> bool operator!=(const Vector2D<T>& a, const Vector2D<T>& b);

template<typename T> Vector2D<T> operator+(const Vector2D<T>& a, const Vector2D<T>& b);
template<typename T> Vector2D<T> operator-(const Vector2D<T>& a, const Vector2D<T>& b);
template<typename T> Vector2D<T> operator*(const Vector2D<T>& a, const Vector2D<T>& b);
template<typename T> Vector2D<T> operator/(const Vector2D<T>& a, const Vector2D<T>& b);

template<typename T> Vector2D<T> operator+(const Vector2D<T>& a, typename Vector2D<T>::TParam b);
template<typename T> Vector2D<T> operator-(const Vector2D<T>& a, typename Vector2D<T>::TParam b);
template<typename T> Vector2D<T> operator*(const Vector2D<T>& a, typename Vector2D<T>::TParam b);
template<typename T> Vector2D<T> operator/(const Vector2D<T>& a, typename Vector2D<T>::TParam b);

template<typename T> Vector2D<T> operator+(typename Vector2D<T>::TParam a, const Vector2D<T>& b);
template<typename T> Vector2D<T> operator-(typename Vector2D<T>::TParam a, const Vector2D<T>& b);
template<typename T> Vector2D<T> operator*(typename Vector2D<T>::TParam a, const Vector2D<T>& b);
// we don't allow scalar / vector.  you can do scalar * vector.reciprocal() for that.

template<typename T> Vector2D<T> pointwiseMin(const Vector2D<T>& a, const Vector2D<T>& b);
template<typename T> Vector2D<T> pointwiseMax(const Vector2D<T>& a, const Vector2D<T>& b);

template <typename T> Vector2D<T> lerp(typename Vector2D<T>::TParam a, const Vector2D<T>& b, typename Vector2D<T>::TParam t);

template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const Vector2D<T>& b);
template<typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& ioIStream, Vector2D<T>& b);

template<typename T> io::XmlOStream& operator<<(io::XmlOStream& stream, const Vector2D<T>& b);



}

}

#include "vector_2d.inl"
#include "pyobject_util.inl"

#endif
