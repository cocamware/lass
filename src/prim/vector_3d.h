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



/** @struct lass::prim::Vector3D
 *	@brief 3D Vector
 *	@author BdG
 *	@date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_3D_H




#include "prim_common.h"
#include "../io/xml_o_stream.h"



namespace lass
{

namespace prim
{

template<typename T>
struct Vector3D
{
    typedef Vector3D<T> TSelf;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits<T> TNumTraits;

	enum { dimension = 3 };	/**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Vector3D<U> Type;
	};


	// public data 

	TValue x;
	TValue y;
	TValue z;


	// methods

	Vector3D():
		x(T()),
		y(T()),
		z(T())
	{
	}
	Vector3D(TParam iX, TParam iY, TParam iZ);

	template <typename U> explicit Vector3D(const Vector3D<U>& iOther): 
		x(iOther.x), 
		y(iOther.y),
		z(iOther.z)
	{
	}

	typename Vector3D::TConstReference operator[](unsigned iIndex) const;
	typename Vector3D::TReference operator[](unsigned iIndex) ;
	typename Vector3D::TConstReference at(signed iIndex) const;
	typename Vector3D::TReference at(signed iIndex);

	const Vector3D<T>& operator+() const;
	const Vector3D<T> operator-() const;
	Vector3D<T>& operator+=(const Vector3D<T>& iB);
	Vector3D<T>& operator-=(const Vector3D<T>& iB);
	Vector3D<T>& operator*=(const Vector3D<T>& iB);
	Vector3D<T>& operator/=(const Vector3D<T>& iB);
	Vector3D<T>& operator+=(TParam iB);
	Vector3D<T>& operator-=(TParam iB);
	Vector3D<T>& operator*=(TParam iB);
	Vector3D<T>& operator/=(TParam iB);

	const bool isZero() const;
	const TValue squaredNorm() const;
	const TValue norm() const;
	const Vector3D<T> normal() const;
	const Vector3D<T> reciprocal() const;
	const Vector3D<T> project(const Vector3D<T>& iB) const;
	const Vector3D<T> reject(const Vector3D<T>& iB) const;

	void normalize();

    template <class RandomGenerator> static Vector3D<T> random(RandomGenerator& ioGenerator);
};

template<typename T> typename Vector3D<T>::TValue dot(const Vector3D<T>& iA, const Vector3D<T>& iB);
template<typename T> Vector3D<T> cross(const Vector3D<T>& iA, const Vector3D<T>& iB);

template<typename T> Vector3D<T> transform(const Vector3D<T>& iA, T (*iF)(T));

template<typename T> bool operator==(const Vector3D<T>& iA, const Vector3D<T>& iB);
template<typename T> bool operator!=(const Vector3D<T>& iA, const Vector3D<T>& iB);

template<typename T> Vector3D<T> operator+(const Vector3D<T>& iA, const Vector3D<T>& iB);
template<typename T> Vector3D<T> operator-(const Vector3D<T>& iA, const Vector3D<T>& iB);
template<typename T> Vector3D<T> operator*(const Vector3D<T>& iA, const Vector3D<T>& iB);
template<typename T> Vector3D<T> operator/(const Vector3D<T>& iA, const Vector3D<T>& iB);

template<typename T> Vector3D<T> operator+(const Vector3D<T>& iA, typename Vector3D<T>::TParam iB);
template<typename T> Vector3D<T> operator-(const Vector3D<T>& iA, typename Vector3D<T>::TParam iB);
template<typename T> Vector3D<T> operator*(const Vector3D<T>& iA, typename Vector3D<T>::TParam iB);
template<typename T> Vector3D<T> operator/(const Vector3D<T>& iA, typename Vector3D<T>::TParam iB);

template<typename T> Vector3D<T> operator+(typename Vector3D<T>::TParam iA, const Vector3D<T>& iB);
template<typename T> Vector3D<T> operator-(typename Vector3D<T>::TParam iA, const Vector3D<T>& iB);
template<typename T> Vector3D<T> operator*(typename Vector3D<T>::TParam iA, const Vector3D<T>& iB);
// we don't allow scalar / vector.  you can do scalar * vector.reciprocal() for that.

template<typename T> Vector3D<T> pointwiseMin(const Vector3D<T>& iA, const Vector3D<T>& iB);
template<typename T> Vector3D<T> pointwiseMax(const Vector3D<T>& iA, const Vector3D<T>& iB);

template<typename T> std::ostream& operator<<(std::ostream& ioOStream, const Vector3D<T>& iB);
template<typename T> io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Vector3D<T>& iB);
template<typename T> std::istream& operator>>(std::istream& ioIStream, Vector3D<T>& iB);



}

}

#include "vector_3d.inl"

#define LASS_PRIM_PYOBJECT_UTIL_VECTOR_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif
