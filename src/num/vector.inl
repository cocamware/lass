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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_INL

#include "num_common.h"
#include "vector.h"

#define LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(iA, iB)\
	(*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
									 lass::util::impl::DefaultRaiser>\
	((iA).size() == (iB).size(), "Vectors '" LASS_STRINGIFY(iA) "' and '" \
	 LASS_STRINGIFY(iB) "' have different dimensions in '" LASS_HERE "'."))


namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

/** constructs an empty vector
 *  Exception safety: strong guarentee.
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
 *  Exception safety: strong guarentee.
 */
template <typename T, typename S>
Vector<T, S>::Vector(TSize iDimension, TParam iInitialValue):
	storage_(iDimension, iInitialValue)
{
}



/** construct vector from storage type
 *  Exception safety: strong guarentee.
 */
template <typename T, typename S>
Vector<T, S>::Vector(const TStorage& iStorage):
	storage_(iStorage)
{
}



/** contruct by any particular type supporting [] and size().
 *  Should only be used with writable storage type (like std::vector which is the default).
 *  Exception safety: strong guarentee.
 */
template <typename T, typename S>
template <typename VectorType>
Vector<T, S>::Vector(const VectorType& iVector):
	storage_(iVector.size(), T())
{
	TSize n = size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] = iVector[i];
	}
}



/** assign storage/expression vector to this (this should be a storage vector).
 *  This vector should be an lvalue which means it should be a storage vector.
 *  Exception safety: basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>& Vector<T, S>::operator=(const Vector<T2, S2>& iOther)
{
	TSize n = iOther.size();
	storage_.resize(n);
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] = iOther[i];
	}
	return *this;
}



/** return dimension of vector.
 *  this should never be a negative value.
 *  Exception safety: nofail.
 */
template <typename T, typename S> inline
const typename Vector<T, S>::TSize
Vector<T, S>::size() const
{
	return storage_.size();
}



/** return the iIndex'th component value.
 *  iIndex shouldn't be out of the range [0, this->size()), unless you're asking for trouble.
 *  Exception safety: strong guarentee.
 */
template <typename T, typename S> inline
const typename Vector<T, S>::TValue
Vector<T, S>::operator[](TSize iIndex) const
{
	LASS_ASSERT(iIndex < size());
	return storage_[iIndex];
}



/** access the iIndex'th component value.
 *  iIndex shouldn't be out of the range [0, this->size()), unless you're asking for trouble.
 *  Exception safety: strong guarentee.
 */
template <typename T, typename S> inline
typename util::CallTraits<T>::TReference
Vector<T, S>::operator[](TSize iIndex)
{
	LASS_ASSERT(iIndex < size());
	return storage_[iIndex];
}


/** return the iIndex'th component value and wrap index if necessary.
 *  if iIndex is out of the range [0, this->size()), it will be wrapped to map in this range.
 *  This is simply a modulus operation: mod(iIndex, this->size()).
 *  Exception safety: strong guarentee.
 */
template <typename T, typename S> inline
const typename Vector<T, S>::TValue
Vector<T, S>::at(TSize iIndex) const
{
	return storage_[mod(iIndex, size())];
}



/** access the iIndex'th component value and wrap index if necessary.
 *  if iIndex is out of the range [0, this->size()), it will be wrapped to map in this range.
 *  This is simply a modulus operation: mod(iIndex, this->size()).
 *  Exception safety: strong guarentee.
 */
template <typename T, typename S> inline
typename util::CallTraits<T>::TReference
Vector<T, S>::at(TSize iIndex)
{
	return storage_[num::mod(iIndex, size())];
}



/** A weird way to get back the same object
 *  Exception safety: nofail guarentee.
 */
template <typename T, typename S> inline
const Vector<T, S>&
Vector<T, S>::operator+() const
{
	return *this;
}



/** return a vector with all components negated
 *  (-v)[i] == -(v[i]).
 *  Exception safety: nofail.
 */
template <typename T, typename S>
const Vector<T, impl::VNeg<T, S> >
Vector<T, S>::operator-() const
{
	typedef impl::VNeg<T, S> TExpression;
	return Vector<T, TExpression>(TExpression(storage_));
}



/** add storage/expression vector to this (this should be a storage vector).
 *  This vector should be an lvalue which means it should be a storage vector.
 *  Exception safety: basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>& Vector<T, S>::operator+=(const Vector<T2, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TSize n = size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] += iB[i];
	}
	return *this;
}



/** subtract storage/expression vector from this (this should be a storage vector).
 *  This vector should be an lvalue which means it should be a storage vector.
 *  Exception safety: basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>& Vector<T, S>::operator-=(const Vector<T2, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TSize n = size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] -= iB[i];
	}
	return *this;
}



/** multiply storage/expression vector with this (this should be a storage vector).
 *  This vector should be an lvalue which means it should be a storage vector.
 *  Exception safety: basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>& Vector<T, S>::operator*=(const Vector<T2, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TSize n = size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] *= iB[i];
	}
	return *this;
}



/** divide this by storage/expression vector (this should be a storage vector).
 *  This vector should be an lvalue which means it should be a storage vector.
 *  Exception safety: basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Vector<T, S>& Vector<T, S>::operator/=(const Vector<T2, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TSize n = size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] /= iB[i];
	}
	return *this;
}



/** add @a iB to all components
 */
template <typename T, typename S>
template <typename T2>
Vector<T, S>& Vector<T, S>::operator+=(const T2& iB)
{
	const TSize n = size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] += iB;
	}
	return *this;
}



/** subtract @a iB from all components
 */
template <typename T, typename S>
template <typename T2>
Vector<T, S>& Vector<T, S>::operator-=(const T2& iB)
{
	const TSize n = size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] -= iB;
	}
	return *this;
}



/** multiply all components with @a iB.
 */
template <typename T, typename S>
template <typename T2>
Vector<T, S>& Vector<T, S>::operator*=(const T2& iB)
{
	const TSize n = size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] *= iB;
	}
	return *this;
}



/** divide all components by @a iB.
 */
template <typename T, typename S>
template <typename T2>
Vector<T, S>& Vector<T, S>::operator/=(const T2& iB)
{
	const TSize n = size();
	for (TSize i = 0; i < n; ++i)
	{
		storage_[i] /= iB;
	}
	return *this;
}


/**
 *  Exception safety: nofail.
 */
template <typename T, typename S> inline
const bool Vector<T, S>::isEmpty() const
{
	return storage_.size() == 0;
}



/** Return true if all the components are (exactly!) zero
 */
template <typename T, typename S>
const bool Vector<T, S>::isZero() const
{
	const TSize n = size();
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
 */
template <typename T, typename S>
const typename Vector<T, S>::TValue
Vector<T, S>::sum() const
{
	const TSize n = size();
	TValue result = TNumTraits::zero;
	for (TSize i = 0; i < n; ++i)
	{
		result += storage_[i];
	}
	return result;
}



/** Return minimum of all components of vector.
 *  O(N).
 */
template <typename T, typename S>
const typename Vector<T, S>::TValue
Vector<T, S>::min() const
{
	const TSize n = size();
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
 *  O(N).
 */
template <typename T, typename S>
const typename Vector<T, S>::TValue
Vector<T, S>::max() const
{
	const TSize n = size();
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
 */
template <typename T, typename S>
const typename Vector<T, S>::TValue
Vector<T, S>::squaredNorm() const
{
	const TSize n = size();
	TValue result = TNumTraits::zero;
	for (TSize i = 0; i < n; ++i)
	{
		result += num::sqr(storage_[i]);
	}
	return result;
}



/** Return norm of vector.
 *  @return sqrt(this->squaredNorm())
 */
template <typename T, typename S>
const typename Vector<T, S>::TValue
Vector<T, S>::norm() const
{
	return num::sqrt(squaredNorm());
}



/** return a unit vector with same direction/sense as this vector.
 *
 * <i>The normalized vector of <b>X</b> is a vector in the same direction but with norm (length) 1.
 * It is denoted <b>X^</b> and given by <b>X^</b> = <b>X</b> / |<b>X</b>|</i>,
 * http://mathworld.wolfram.com/dimension_ormalizedVector.html.
 *
 * @return *this / this->norm()
 */
template <typename T, typename S>
const Vector<T, impl::VMul<T, S, impl::VScalar<T> > >
Vector<T, S>::normal() const
{
	const TValue scale = TNumTraits::one / norm();
	typedef impl::VMul<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(storage_, impl::VScalar(scale, size())));
}



/** return a vector with each component being the reciprocal value of this vector.
 */
template <typename T, typename S>
const Vector<T, impl::VRec<T, S> >
Vector<T, S>::reciprocal() const
{
	typedef impl::VRec<T, S> TExpression;
	return Vector<T, TExpression>(TExpression(storage_));
}



/** Project vector on this one
 */
template <typename T, typename S>
template <typename S2>
const Vector<T, impl::VMul<T, S, impl::VScalar<T> > >
Vector<T, S>::project(const Vector<T, S2>& iB) const
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TValue scale = dot(iB, *this) / squaredNorm();
	typedef impl::VMul<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(storage_, impl::VScalar<T>(scale, size())));
}



/** Project vector on this one
 */
template <typename T, typename S>
template <typename S2>
const Vector<T, impl::VSub<T, S2, impl::VMul<T, S, impl::VScalar<T> > > >
Vector<T, S>::reject(const Vector<T, S2>& iB) const
{
	return iB - project(iB);
}



/** Normalize vector.
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



/** VECTOR MUST HAVE REAL STORAGE
 */
template <typename T, typename S>
void Vector<T, S>::swap(Vector<T, S>& iOther)
{
	storage_.swap(iOther.storage_);
}



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



/** @relates lass::prim::Vector
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
 */
template <typename T, typename S1, typename S2>
bool operator!=(const Vector<T, S1>& iA, const Vector<T, S2>& iB)
{
	return !(iA == iB);
}



/** dot product.
 *  @relates lass::num::Vector
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
 */
template <typename T, typename S1, typename S2>
const Vector<T, impl::VAdd<T, S1, S2> >
operator+(const Vector<T, S1>& iA, const Vector<T, S2>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(iA, iB);
	typedef impl::VAdd<T, S1, S2> TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), iB.storage()));
}



/** componentwise addition
 *  @relates lass::num::Vector
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
 */
template <typename T, typename S>
const Vector<T, impl::VAdd<T, impl::VScalar<T>, S> >
operator+(const T& iA, const Vector<T, S>& iB)
{
	typedef impl::VAdd<T, impl::VScalar<T>, S> TExpression;
	return Vector<T, TExpression>(TExpression(impl::VScalar(iA, iB.size()), iB.storage()));
}



/** add @a iA to all negated components of @a iB
 *  @relates lass::num::Vector
 */
template <typename T, typename S>
const Vector<T, impl::VSub<T, impl::VScalar<T>, S> >
operator-(const T& iA, const Vector<T, S>& iB)
{
	typedef impl::VSub<T, impl::VScalar<T>, S> TExpression;
	return Vector<T, TExpression>(TExpression(impl::VScalar(iA, iB.size()), iB.storage()));
}



/** multiply @a iA with all components of @a iB
 *  @relates lass::num::Vector
 */
template <typename T, typename S>
const Vector<T, impl::VMul<T, impl::VScalar<T>, S> >
operator*(const T& iA, const Vector<T, S>& iB)
{
	typedef impl::VMul<T, impl::VScalar<T>, S> TExpression;
	return Vector<T, TExpression>(TExpression(impl::VScalar(iA, iB.size()), iB.storage()));
}



/** multiply @a iA with all reciprocal components of @a iB
 *  @relates lass::num::Vector
 */
template <typename T, typename S>
const Vector<T, impl::VDiv<T, impl::VScalar<T>, S> >
operator/(const T& iA, const Vector<T, S>& iB)
{
	typedef impl::VDiv<T, impl::VScalar<T>, S> TExpression;
	return Vector<T, TExpression>(TExpression(impl::VScalar(iA, iB.size()), iB.storage()));
}



/** add @a iB to all components of @a iA
 *  @relates lass::num::Vector
 */
template <typename T, typename S>
const Vector<T, impl::VAdd<T, S, impl::VScalar<T> > >
operator+(const Vector<T, S>& iA, const T& iB)
{
	typedef impl::VAdd<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), impl::VScalar(iB, iA.size())));
}



/** subtract @a iB from all components of @a iA
 *  @relates lass::num::Vector
 */
template <typename T, typename S>
const Vector<T, impl::VSub<T, S, impl::VScalar<T> > >
operator-(const Vector<T, S>& iA, const T& iB)
{
	typedef impl::VSub<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), impl::VScalar(iB, iA.size())));
}



/** multiply all components of @a iA with @a iB.
 *  @relates lass::num::Vector
 */
template <typename T, typename S>
const Vector<T, impl::VMul<T, S, impl::VScalar<T> > >
operator*(const Vector<T, S>& iA, const T& iB)
{
	typedef impl::VMul<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), impl::VScalar(iB, iA.size())));
}



/** multiply all components of @a iA with @a iB.
 *  @relates lass::num::Vector
 */
template <typename T, typename S>
const Vector<T, impl::VDiv<T, S, impl::VScalar<T> > >
operator/(const Vector<T, S>& iA, const T& iB)
{
	typedef impl::VDiv<T, S, impl::VScalar<T> > TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), impl::VScalar(iB, iA.size())));
}



/** @relates lass::prim::Vector
 */
template <typename T, typename S, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& oOStream, const Vector<T, S>& iB)
{
	typedef typename Vector<T, S>::TSize TSize;
	const TSize n = iB.size();

	LASS_ENFORCE_STREAM(oOStream) << "(";
	if (size > 0)
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
