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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_INL

#include "num_common.h"
#include "vector.h"
#include "../meta/is_integral.h"

#define LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(a, b)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::EqualPredicate,\
		::lass::util::impl::DefaultRaiser,\
		(a).size(), \
		(b).size(), \
		"Vectors '" LASS_STRINGIFY(a) "' and '" LASS_STRINGIFY(b) "' have different dimensions in '" LASS_HERE "'.")

namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

/** constructs an empty vector
 *  
 *  @par Exception safety: 
 *		strong guarentee.
 */
template <typename T, typename S>
Vector<T, S>::Vector():
	storage_()
{
	LASS_ASSERT(size() == 0);
}



/** Construct a vector of dimension @a iDimension.
 *  @param iDimension the dimension of the vector to be created.  You can pass zero, but you
 *         shouldn't pass negative dimensions though.
 *  @param iInitialValue the initial value of all vector components, zero by default.
 *
 *  @par Complexity: 
 *		O(iDimension)
 *
 *  @par Exception safety: 
 *		strong guarentee.
 */
template <typename T, typename S>
Vector<T, S>::Vector(TSize iDimension, TParam iInitialValue):
	storage_(iDimension, iInitialValue)
{
}



/** construct vector from storage type
 *
 *  @par Complexity: 
 *		O(iStorage.size())
 *
 *  @par Exception safety: 
 *		strong guarentee.
 */
template <typename T, typename S>
Vector<T, S>::Vector(const TStorage& iStorage):
	storage_(iStorage)
{
}



/** contruct by any particular type supporting [] and size().
 *  Should only be used with writable storage type (like std::vector which is the default).
 *
 *  @par Complexity: 
 *		O(iVector.size())
 *
 *  @par Exception safety: 
 *		strong guarentee.
 */
template <typename T, typename S>
template <typename VectorType>
Vector<T, S>::Vector(const VectorType& iVector)
{
	init(iVector, meta::Wrap<typename meta::IsIntegral<VectorType>::Type>());
}



/** construct storage/expression vector to this (this should be a storage vector).
 *
 *  @pre @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(iOther.size())
 *
 *  @par Exception safety: 
 *		strong guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>::Vector(const Vector<T2, S2>& iOther)
{
	TSize n = iOther.storage_.size();
	storage_.resize(n);
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] = iOther.storage_[i];
	}
}



/** assign storage/expression vector to this (this should be a storage vector).
 *
 *  @pre @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(iOther.size())
 *
 *  @par Exception safety: 
 *		basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>& Vector<T, S>::operator=(const Vector<T2, S2>& iOther)
{
	TSize n = iOther.storage_.size();
	storage_.resize(n);
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] = iOther.storage_[i];
	}
	return *this;
}



/** return dimension of vector.
 *  this should never be a negative value.
 *
 *  @par Exception safety: 
 *		nofail.
 */
template <typename T, typename S> inline
typename Vector<T, S>::TSize
Vector<T, S>::size() const
{
	return storage_.size();
}



/** return the iIndex'th component value.
 *
 *  @pre @a iIndex must be in [0, this->size()), unless you're asking for trouble.
 *
 *  @par Exception safety: 
 *		strong guarentee.
 */
template <typename T, typename S> inline
const typename Vector<T, S>::TValue
Vector<T, S>::operator[](TSize iIndex) const
{
	LASS_ASSERT(iIndex < size());
	return storage_[iIndex];
}



/** access the iIndex'th component value.
 *
 *  @pre 
 *		@arg @a iIndex must be in [0, this->size()), unless you're asking for trouble.
 *		@arg @c this must be an l-value.
 *
 *  @par Exception safety: 
 *		strong guarentee.
 */
template <typename T, typename S> inline
typename Vector<T, S>::TReference
Vector<T, S>::operator[](TSize iIndex)
{
	LASS_ASSERT(iIndex < size());
	return storage_[iIndex];
}


/** return the iIndex'th component value and wrap index if necessary.
 *  if iIndex is out of the range [0, this->size()), it will be wrapped to map in this range.
 *  This is simply a modulus operation: mod(iIndex, this->size()).
 *
 *  @par Exception safety: 
 *		strong guarentee.
 */
template <typename T, typename S> inline
const typename Vector<T, S>::TValue
Vector<T, S>::at(TSize iIndex) const
{
	return storage_[mod(iIndex, storage_.size())];
}



/** access the iIndex'th component value and wrap index if necessary.
 *  if iIndex is out of the range [0, this->size()), it will be wrapped to map in this range.
 *  This is simply a modulus operation: mod(iIndex, this->size()).
 *
 *  @pre @c this must be an l-value.
 *
 *  @par Exception safety: 
 *		strong guarentee.
 */
template <typename T, typename S> inline
typename Vector<T, S>::TReference
Vector<T, S>::at(TSize iIndex)
{
	return storage_[num::mod(iIndex, storage_.size())];
}



/** A weird way to get back the same object
 *
 *  @par Complexity: 
 *		O(1)
 *
 *  @par Exception safety: 
 *		nofail guarentee.
 */
template <typename T, typename S> inline
const Vector<T, S>&
Vector<T, S>::operator+() const
{
	return *this;
}



/** return a vector with all components negated
 *  (-v)[i] == -(v[i]).
 *
 *  @par Complexity: 
 *		O(1)
 *
 *  @par Exception safety: 
 *		nofail.
 */
template <typename T, typename S>
const Vector<T, impl::VNeg<T, S> >
Vector<T, S>::operator-() const
{
	typedef impl::VNeg<T, S> TExpression;
	return Vector<T, TExpression>(TExpression(storage_));
}



/** add storage/expression vector to this (this should be a storage vector).
 *
 *  @pre 
 *		@arg this->size() == iB.size()
 *		@arg @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(this->size())
 *
 *  @par Exception safety: 
 *		basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>& Vector<T, S>::operator+=(const Vector<T2, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TSize n = storage_.size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] += iB[i];
	}
	return *this;
}



/** subtract storage/expression vector from this (this should be a storage vector).
 *
 *  @pre 
 *		@arg this->size() == iB.size()
 *		@arg @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(this->size())
 *
 *  @par Exception safety: 
 *		basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>& Vector<T, S>::operator-=(const Vector<T2, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TSize n = storage_.size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] -= iB[i];
	}
	return *this;
}



/** multiply storage/expression vector with this (this should be a storage vector).
 *
 *  @pre 
 *		@arg this->size() == iB.size()
 *		@arg @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(this->size())
 *
 *  @par Exception safety: 
 *		basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>& Vector<T, S>::operator*=(const Vector<T2, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TSize n = storage_.size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] *= iB[i];
	}
	return *this;
}



/** divide this by storage/expression vector (this should be a storage vector).
 *
 *  @pre 
 *		@arg this->size() == iB.size()
 *		@arg @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(this->size())
 *
 *  @par Exception safety: 
 *		basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>& Vector<T, S>::operator/=(const Vector<T2, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TSize n = storage_.size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] /= iB[i];
	}
	return *this;
}



/** add @a iB to all components
 *
 *  @pre @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
template <typename T2>
Vector<T, S>& Vector<T, S>::operator+=(const T2& iB)
{
	const TSize n = storage_.size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] += iB;
	}
	return *this;
}



/** subtract @a iB from all components
 *
 *  @pre @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
template <typename T2>
Vector<T, S>& Vector<T, S>::operator-=(const T2& iB)
{
	const TSize n = storage_.size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] -= iB;
	}
	return *this;
}



/** multiply all components with @a iB.
 *
 *  @pre @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
template <typename T2>
Vector<T, S>& Vector<T, S>::operator*=(const T2& iB)
{
	const TSize n = storage_.size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] *= iB;
	}
	return *this;
}



/** divide all components by @a iB.
 *
 *  @pre @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
template <typename T2>
Vector<T, S>& Vector<T, S>::operator/=(const T2& iB)
{
	const TSize n = storage_.size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] /= iB;
	}
	return *this;
}


/** return true if vector contains no dataa at all
 *
 *  @par Complexity: 
 *		O(1)
 *
 *  @par Exception safety: 
 *		nofail.
 */
template <typename T, typename S> inline
bool Vector<T, S>::isEmpty() const
{
	return storage_.size() == 0;
}



/** Return true if all the components are (exactly!) zero
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
bool Vector<T, S>::isZero() const
{
	const TSize n = storage_.size();
	for (TSize i = 0; i < n; ++i)
	{
		// if you get your compiler error here, you'll be using a broken STLport version. Ask [Bramz].
		//
		if (storage_[i] != TNumTraits::zero)
		{
			return false;
		}
	}
	return true;
}



/** Return sum of all components of vector.
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
const typename Vector<T, S>::TValue
Vector<T, S>::sum() const
{
	const TSize n = storage_.size();
	TValue result = TNumTraits::zero;
	for (TSize i = 0; i < n; ++i)
	{
		result += storage_[i];
	}
	return result;
}



/** Return minimum of all components of vector.
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
const typename Vector<T, S>::TValue
Vector<T, S>::min() const
{
	const TSize n = storage_.size();
	if (n == 0)
	{
		return TNumTraits::zero;
	}
	TValue result = storage_[0];
	for (TSize i = 1; i < n; ++i)
	{
		result = std::min(result, storage_[i]);
	}
	return result;
}



/** Return maximum of all components of vector.
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
const typename Vector<T, S>::TValue
Vector<T, S>::max() const
{
	const TSize n = storage_.size();
	if (n == 0)
	{
		return TNumTraits::zero;
	}
	TValue result = storage_[0];
	for (TSize i = 1; i < n; ++i)
	{
		result = std::max(result, storage_[i]);
	}
	return result;
}



/** Return squared norm of vector.
 *  @return dot(*this, *this)
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
const typename Vector<T, S>::TValue
Vector<T, S>::squaredNorm() const
{
	const TSize n = storage_.size();
	TValue result = TNumTraits::zero;
	for (TSize i = 0; i < n; ++i)
	{
		result += num::sqr(storage_[i]);
	}
	return result;
}



/** Return norm of vector.
 *  @return sqrt(this->squaredNorm())
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
const typename Vector<T, S>::TValue
Vector<T, S>::norm() const
{
	return num::sqrt(squaredNorm());
}



/** return a unit vector with same direction/sense as this vector.
 *
 *	<i>The normalized vector of <b>X</b> is a vector in the same direction but with norm (length) 1.
 *	It is denoted <b>X^</b> and given by <b>X^</b> = <b>X</b> / |<b>X</b>|</i>,
 *	http://mathworld.wolfram.com/dimension_ormalizedVector.html.
 *
 *	@return *this / this->norm()
 *
 *	@par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
const Vector<T, impl::VMul<T, S, impl::VScalar<T> > >
Vector<T, S>::normal() const
{
	const TValue scale = TNumTraits::one / norm();
	typedef impl::VMul<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(storage_, impl::VScalar<T>(storage_.size(), scale)));
}



/** return a vector with each component being the reciprocal value of this vector.
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S>
const Vector<T, impl::VRec<T, S> >
Vector<T, S>::reciprocal() const
{
	typedef impl::VRec<T, S> TExpression;
	return Vector<T, TExpression>(TExpression(storage_));
}



/** Project vector on this one
 *
 *  @pre this->size() == iB.size()
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
template <typename S2>
const Vector<T, impl::VMul<T, S, impl::VScalar<T> > >
Vector<T, S>::project(const Vector<T, S2>& iB) const
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TValue scale = dot(iB, *this) / squaredNorm();
	typedef impl::VMul<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(storage_, impl::VScalar<T>(storage_.size(), scale)));
}



/** Project vector on this one
 *
 *  @pre this->size() == iB.size()
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
template <typename S2>
const Vector<T, impl::VSub<T, S2, impl::VMul<T, S, impl::VScalar<T> > > >
Vector<T, S>::reject(const Vector<T, S2>& iB) const
{
	return iB - project(iB);
}



/** Normalize vector.
 *
 *  @pre @c this must be an l-value.
 *
 *  @par Complexity: 
 *		O(this->size())
 */
template <typename T, typename S>
void Vector<T, S>::normalize()
{
	*this /= norm();
}



template <typename T, typename S> inline
const typename Vector<T, S>::TStorage&
Vector<T, S>::storage() const
{
	return storage_;
}



template <typename T, typename S> inline
typename Vector<T, S>::TStorage&
Vector<T, S>::storage()
{
	return storage_;
}



/** swap storage of two vectors
 *
 *  @pre @c this and @ a iOther must be l-values
 *
 *  @par Complexity: 
 *		O(1)
 *
 *  @par Exception safety: 
 *		no-fail
 */
template <typename T, typename S>
void Vector<T, S>::swap(Vector<T, S>& iOther)
{
	storage_.swap(iOther.storage_);
}



// --- private -------------------------------------------------------------------------------------

template <typename T, typename S>
template <typename IntegralType>
void Vector<T, S>::init(IntegralType iDimension, meta::Wrap<meta::True>)
{
	TStorage temp(iDimension, T());
	storage_.swap(temp);
}



template <typename T, typename S>
template <typename VectorType>
void Vector<T, S>::init(const VectorType& iVector, meta::Wrap<meta::False>)
{
	TSize n = iVector.size();
	TStorage temp(n, T());
	for (TSize i = 0; i < n; ++i)
	{
		temp[i] = iVector[i];
	}
	storage_.swap(temp);
}
	


// --- free ----------------------------------------------------------------------------------------



/** @relates lass::prim::Vector
 *
 *  @par Complexity: 
 *		O(iA->size())
 */
template <typename T, typename S1, typename S2>
bool operator==(const Vector<T, S1>& iA, const Vector<T, S2>& iB)
{
	typedef typename Vector<T, S1>::TSize TSize;
	const TSize n = iA.size();

	if (n != iB.size())
	{
		return false;
	}
	for (TSize i = 0; i < n; ++i)
	{
		if (iA[i] != iB[i])
		{
			return false;
		}
	}
	return true;
}



/** @relates lass::prim::Vector
 *
 *  @par Complexity: 
 *		O(iA.size())
 */
template <typename T, typename S1, typename S2>
bool operator!=(const Vector<T, S1>& iA, const Vector<T, S2>& iB)
{
	return !(iA == iB);
}



/** dot product.
 *  @relates lass::num::Vector
 *
 *  @pre iA.size() == iB.size()
 *
 *  @par Complexity: 
 *		O(iA.size())
 */
template <typename T, typename S1, typename S2>
const T dot(const Vector<T, S1>& iA, const Vector<T, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(iA, iB);
	typedef typename Vector<T, S1>::TSize TSize;
	const TSize n = iA.size();

	T result = NumTraits<T>::zero;
	for (TSize i = 0; i < n; ++i)
	{
		result += iA[i] * iB[i];
	}
	return result;
}



/** componentwise addition
 *  @relates lass::num::Vector
 *
 *  @pre iA.size() == iB.size()
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S1, typename S2>
const Vector<T, impl::VAdd<T, S1, S2> >
operator+(const Vector<T, S1>& iA, const Vector<T, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(iA, iB);
	typedef impl::VAdd<T, S1, S2> TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), iB.storage()));
}



/** componentwise subtraction
 *  @relates lass::num::Vector
 *
 *  @pre iA.size() == iB.size()
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S1, typename S2>
const Vector<T, impl::VSub<T, S1, S2> >
operator-(const Vector<T, S1>& iA, const Vector<T, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(iA, iB);
	typedef impl::VSub<T, S1, S2> TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), iB.storage()));
}



/** componentwise multiplication
 *  @relates lass::num::Vector
 *
 *  @pre iA.size() == iB.size()
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S1, typename S2>
const Vector<T, impl::VMul<T, S1, S2> >
operator*(const Vector<T, S1>& iA, const Vector<T, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(iA, iB);
	typedef impl::VMul<T, S1, S2> TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), iB.storage()));
}



/** componentwise division
 *  @relates lass::num::Vector
 *
 *  @pre iA.size() == iB.size()
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S1, typename S2>
const Vector<T, impl::VDiv<T, S1, S2> >
operator/(const Vector<T, S1>& iA, const Vector<T, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(iA, iB);
	typedef impl::VDiv<T, S1, S2> TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), iB.storage()));
}



/** add @a iA to all components of @a iB
 *  @relates lass::num::Vector
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S>
const Vector<T, impl::VAdd<T, impl::VScalar<T>, S> >
operator+(const T& iA, const Vector<T, S>& iB)
{
	typedef impl::VAdd<T, impl::VScalar<T>, S> TExpression;
	return Vector<T, TExpression>(TExpression(impl::VScalar<T>(iB.size(), iA), iB.storage()));
}



/** add @a iA to all negated components of @a iB
 *  @relates lass::num::Vector
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S>
const Vector<T, impl::VSub<T, impl::VScalar<T>, S> >
operator-(const T& iA, const Vector<T, S>& iB)
{
	typedef impl::VSub<T, impl::VScalar<T>, S> TExpression;
	return Vector<T, TExpression>(TExpression(impl::VScalar<T>(iB.size(), iA), iB.storage()));
}



/** multiply @a iA with all components of @a iB
 *  @relates lass::num::Vector
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S>
const Vector<T, impl::VMul<T, impl::VScalar<T>, S> >
operator*(const T& iA, const Vector<T, S>& iB)
{
	typedef impl::VMul<T, impl::VScalar<T>, S> TExpression;
	return Vector<T, TExpression>(TExpression(impl::VScalar<T>(iB.size(), iA), iB.storage()));
}



/** multiply @a iA with all reciprocal components of @a iB
 *  @relates lass::num::Vector
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S>
const Vector<T, impl::VDiv<T, impl::VScalar<T>, S> >
operator/(const T& iA, const Vector<T, S>& iB)
{
	typedef impl::VDiv<T, impl::VScalar<T>, S> TExpression;
	return Vector<T, TExpression>(TExpression(impl::VScalar<T>(iB.size(), iA), iB.storage()));
}



/** add @a iB to all components of @a iA
 *  @relates lass::num::Vector
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S>
const Vector<T, impl::VAdd<T, S, impl::VScalar<T> > >
operator+(const Vector<T, S>& iA, const T& iB)
{
	typedef impl::VAdd<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), impl::VScalar<T>(iA.size(), iB)));
}



/** subtract @a iB from all components of @a iA
 *  @relates lass::num::Vector
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S>
const Vector<T, impl::VSub<T, S, impl::VScalar<T> > >
operator-(const Vector<T, S>& iA, const T& iB)
{
	typedef impl::VSub<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), impl::VScalar<T>(iA.size(), iB)));
}



/** multiply all components of @a iA with @a iB.
 *  @relates lass::num::Vector
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S>
const Vector<T, impl::VMul<T, S, impl::VScalar<T> > >
operator*(const Vector<T, S>& iA, const T& iB)
{
	typedef impl::VMul<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), impl::VScalar<T>(iA.size(), iB)));
}



/** multiply all components of @a iA with @a iB.
 *  @relates lass::num::Vector
 *
 *  @par Complexity: 
 *		O(1)
 */
template <typename T, typename S>
const Vector<T, impl::VDiv<T, S, impl::VScalar<T> > >
operator/(const Vector<T, S>& iA, const T& iB)
{
	typedef impl::VDiv<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), impl::VScalar<T>(iA.size(), iB)));
}



/** @relates lass::prim::Vector
 *
 *  @par Complexity: 
 *		O(iB.size())
 */
template <typename T, typename S, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& oOStream, const Vector<T, S>& iB)
{
	typedef typename Vector<T, S>::TSize TSize;
	const TSize n = iB.size();

	LASS_ENFORCE_STREAM(oOStream) << "(";
	if (n > 0)
	{
		LASS_ENFORCE_STREAM(oOStream) << iB[0];
	}
	for (TSize i = 1; i < n; ++i)
	{
		LASS_ENFORCE_STREAM(oOStream) << ", " << iB[i];
	}
	LASS_ENFORCE_STREAM(oOStream) << ")";
	return oOStream;
}



/** @relates lass::prim::Vector
 */
template <typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& ioIStream, Vector<T>& oB)
{
	typedef typename Vector<T>::TValue TValue;
	typedef typename Vector<T>::TSize TSize;
	typedef typename Vector<T>::TStorage TStorage;

	TStorage buffer;
	TValue value;
	TSize size = 0;

	char c = 0;
	LASS_ENFORCE_STREAM(ioIStream) >> c;
	if (c != '(')
	{
		ioIStream.clear(std::ios::failbit);
		return ioIStream;
	}

	// TODO peek for empty vectors '()'

	// assume no empty vector here
	do
	{
		c = 0;
		LASS_ENFORCE_STREAM(ioIStream) >> value >> c;
		buffer.push_back(value);
		++size;
		if (c != ',' && c != ')')
		{
			ioIStream.clear(std::ios::failbit);
			return ioIStream;
		}
	}
	while (c != ')');

	oB.storage().swap(buffer);
	return ioIStream;
}



}

}

#endif

// EOF
