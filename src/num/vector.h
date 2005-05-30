/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



/** @class lass::num::Vector
 *  @brief a dynamic sized n-dimensional vector with vector expression templates
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_H

#include "num_common.h"
#include "num_traits.h"
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

	const TSize size() const;

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

	const bool isEmpty() const;
	const bool isZero() const;
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
