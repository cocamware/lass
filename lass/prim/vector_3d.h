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
 *	Copyright (C) 2004-2009 the Initial Developer.
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



/** @struct lass::prim::Vector3D
 *  @brief 3D Vector
 *  @author BdG
 *  @date 2003
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

	enum { dimension = 3 }; /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Vector3D<U> Type;
	};


	// public data

	TValue x;
	TValue y;
	TValue z;


	// methods

	Vector3D();
	Vector3D(TParam x, TParam y, TParam z);
	template <typename U> explicit Vector3D(const Vector3D<U>& other);
	template <typename U> Vector3D(const U& x, const U& y, const U& z);

	typename Vector3D::TConstReference operator[](size_t index) const;
	typename Vector3D::TReference operator[](size_t index) ;
	typename Vector3D::TConstReference at(signed index) const;
	typename Vector3D::TReference at(signed index);

	const Vector3D<T>& operator+() const;
	const Vector3D<T> operator-() const;
	Vector3D<T>& operator+=(const Vector3D<T>& other);
	Vector3D<T>& operator-=(const Vector3D<T>& other);
	Vector3D<T>& operator*=(const Vector3D<T>& other);
	Vector3D<T>& operator/=(const Vector3D<T>& other);
	Vector3D<T>& operator+=(TParam other);
	Vector3D<T>& operator-=(TParam other);
	Vector3D<T>& operator*=(TParam other);
	Vector3D<T>& operator/=(TParam other);

	const bool isZero() const;
	const bool isNaN() const;
	const TValue squaredNorm() const;
	const TValue norm() const;
	const Vector3D<T> normal() const;
	const Vector3D<T> reciprocal() const;
	const Vector3D<T> project(const Vector3D<T>& other) const;
	const Vector3D<T> reject(const Vector3D<T>& other) const;
	const Vector3D<T> reflect(const Vector3D<T>& other) const;
	const Vector3D<T> transform(T (*op)(T)) const;

	void normalize();

	template <class RandomGenerator> static Vector3D<T> random(RandomGenerator& generator);
};

template<typename T> typename Vector3D<T>::TValue dot(const Vector3D<T>& a, const Vector3D<T>& b);
template<typename T> Vector3D<T> cross(const Vector3D<T>& a, const Vector3D<T>& b);

template<typename T> bool operator==(const Vector3D<T>& a, const Vector3D<T>& b);
template<typename T> bool operator!=(const Vector3D<T>& a, const Vector3D<T>& b);

template<typename T> Vector3D<T> operator+(const Vector3D<T>& a, const Vector3D<T>& b);
template<typename T> Vector3D<T> operator-(const Vector3D<T>& a, const Vector3D<T>& b);
template<typename T> Vector3D<T> operator*(const Vector3D<T>& a, const Vector3D<T>& b);
template<typename T> Vector3D<T> operator/(const Vector3D<T>& a, const Vector3D<T>& b);

template<typename T> Vector3D<T> operator+(const Vector3D<T>& a, typename Vector3D<T>::TParam b);
template<typename T> Vector3D<T> operator-(const Vector3D<T>& a, typename Vector3D<T>::TParam b);
template<typename T> Vector3D<T> operator*(const Vector3D<T>& a, typename Vector3D<T>::TParam b);
template<typename T> Vector3D<T> operator/(const Vector3D<T>& a, typename Vector3D<T>::TParam b);

template<typename T> Vector3D<T> operator+(typename Vector3D<T>::TParam a, const Vector3D<T>& b);
template<typename T> Vector3D<T> operator-(typename Vector3D<T>::TParam a, const Vector3D<T>& b);
template<typename T> Vector3D<T> operator*(typename Vector3D<T>::TParam a, const Vector3D<T>& b);
// we don't allow scalar / vector.  you can do scalar * vector.reciprocal() for that.

template<typename T> Vector3D<T> pointwiseMin(const Vector3D<T>& a, const Vector3D<T>& b);
template<typename T> Vector3D<T> pointwiseMax(const Vector3D<T>& a, const Vector3D<T>& b);

template <typename T> Vector3D<T> lerp(const Vector3D<T>& a, const Vector3D<T>& b, typename Vector3D<T>::TParam t);

template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const Vector3D<T>& b);
template<typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& stream, Vector3D<T>& b);

template<typename T> io::XmlOStream& operator<<(io::XmlOStream& stream, const Vector3D<T>& b);



}

}

#include "vector_3d.inl"

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_VECTOR_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "../python/export_traits_prim.h"
#endif

#endif
