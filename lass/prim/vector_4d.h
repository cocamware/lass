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

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_VECTOR_4D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "pyobject_util.h"
#endif

#endif
