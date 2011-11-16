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



/** @class lass::num::Vector
 *  @brief a dynamic sized n-dimensional vector with vector expression templates
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_H

#include "num_common.h"
#include "num_traits.h"
#include "../meta/bool.h"
#include "../meta/wrap.h"
#include "../util/call_traits.h"
#include "impl/vector_expressions.h"

namespace lass
{
namespace num
{

template
<
	typename T,
	typename S = impl::VStorage<T>
>
class Vector
{
public:

	typedef Vector<T, S> TSelf;
	typedef S TStorage;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits<T> TNumTraits;
	typedef size_t TSize;

	template <typename U> struct Rebind
	{
		typedef Vector<U, S> Type;
	};

	Vector();
	explicit Vector(TSize iDimension, TParam iInitialValue = TNumTraits::zero);
	explicit Vector(const TStorage& iStorage);
	template <typename VectorType> explicit Vector(const VectorType& iVector);
	template <typename T2, typename S2> Vector(const Vector<T2, S2>& iOther);

	template <typename T2, typename S2> Vector<T, S>& operator=(const Vector<T2, S2>& iOther);

	TSize size() const;

	const TValue operator[](TSize iIndex) const;
	TReference operator[](TSize iIndex);
	const TValue at(TSize iIndex) const;
	TReference at(TSize iIndex);

	const Vector<T, S>& operator+() const;
	const Vector<T, impl::VNeg<T, S> > operator-() const;
	template <typename T2, typename S2> Vector<T, S>& operator+=(const Vector<T2, S2>& iB);
	template <typename T2, typename S2> Vector<T, S>& operator-=(const Vector<T2, S2>& iB);
	template <typename T2, typename S2> Vector<T, S>& operator*=(const Vector<T2, S2>& iB);
	template <typename T2, typename S2> Vector<T, S>& operator/=(const Vector<T2, S2>& iB);
	template <typename T2> Vector<T, S>& operator+=(const T2& iB);
	template <typename T2> Vector<T, S>& operator-=(const T2& iB);
	template <typename T2> Vector<T, S>& operator*=(const T2& iB);
	template <typename T2> Vector<T, S>& operator/=(const T2& iB);

	bool isEmpty() const;
	bool isZero() const;
	const TValue sum() const;
	const TValue min() const;
	const TValue max() const;
	const TValue squaredNorm() const;
	const TValue norm() const;
	const Vector<T, impl::VMul<T, S, impl::VScalar<T> > > normal() const;
	const Vector<T, impl::VRec<T, S> > reciprocal() const;

	template <typename S2> const Vector<T, impl::VMul<T, S, impl::VScalar<T> > >
		project(const Vector<T, S2>& iB) const;
	template <typename S2> const Vector<T, impl::VSub<T, S2, impl::VMul<T, S, impl::VScalar<T> > > >
		reject(const Vector<T, S2>& iB) const;

	const Vector<T, impl::VFun<T, S> > transform(T (*iOperator)(T));

	void normalize();

	const TStorage& storage() const;
	TStorage& storage();
	void swap(Vector<T, S>& iOther);

private:

	template <typename T2, typename S2> friend class Vector;

	template <typename IntegralType> void init(IntegralType iDimension, meta::Wrap<meta::True>);
	template <typename VectorType> void init(const VectorType& iVector, meta::Wrap<meta::False>);
 
	TStorage storage_;
};

template <typename T, typename S1, typename S2>
bool operator==(const Vector<T>& iA, const Vector<T>& iB);
template <typename T, typename S1, typename S2>
inline bool operator!=(const Vector<T>& iA, const Vector<T>& iB);

template <typename T, typename S1, typename S2>
const T dot(const Vector<T, S1>& iA, const Vector<T, S2>& iB);

template <typename T, typename S1, typename S2>
const Vector<T, impl::VAdd<T, S1, S2> > operator+(const Vector<T, S1>& iA, const Vector<T, S2>& iB);
template <typename T, typename S1, typename S2>
const Vector<T, impl::VSub<T, S1, S2> > operator-(const Vector<T, S1>& iA, const Vector<T, S2>& iB);
template <typename T, typename S1, typename S2>
const Vector<T, impl::VMul<T, S1, S2> > operator*(const Vector<T, S1>& iA, const Vector<T, S2>& iB);
template <typename T, typename S1, typename S2>
const Vector<T, impl::VDiv<T, S1, S2> > operator/(const Vector<T, S1>& iA, const Vector<T, S2>& iB);

template <typename T, typename S>
const Vector<T, impl::VAdd<T, impl::VScalar<T>, S> > operator+(const T& iA, const Vector<T, S>& iB);
template <typename T, typename S>
const Vector<T, impl::VSub<T, impl::VScalar<T>, S> > operator-(const T& iA, const Vector<T, S>& iB);
template <typename T, typename S>
const Vector<T, impl::VMul<T, impl::VScalar<T>, S> > operator*(const T& iA, const Vector<T, S>& iB);
template <typename T, typename S>
const Vector<T, impl::VDiv<T, impl::VScalar<T>, S> > operator/(const T& iA, const Vector<T, S>& iB);

template <typename T, typename S>
const Vector<T, impl::VAdd<T, S, impl::VScalar<T> > > operator+(const Vector<T, S>& iA, const T& iB);
template <typename T, typename S>
const Vector<T, impl::VSub<T, S, impl::VScalar<T> > > operator-(const Vector<T, S>& iA, const T& iB);
template <typename T, typename S>
const Vector<T, impl::VMul<T, S, impl::VScalar<T> > > operator*(const Vector<T, S>& iA, const T& iB);
template <typename T, typename S>
const Vector<T, impl::VDiv<T, S, impl::VScalar<T> > > operator/(const Vector<T, S>& iA, const T& iB);


template <typename T, typename S, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& iS, const Vector<T, S>& iA);

template <typename T, typename S, typename Char, typename Traits>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& iS, Vector<T>& iB);

}

}

#include "vector.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_H
#include "matrix_vector.h"
#endif

#endif

// EOF
