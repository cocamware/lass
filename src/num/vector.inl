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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_INL

#include "num_common.h"
#include "vector.h"

#define LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(iA, iB)\
	(*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
									 lass::util::impl::DefaultRaiser>\
	((iA).dimension() == (iB).dimension(), "Vectors '" LASS_STRINGIFY(iA) "' and '" \
	 LASS_STRINGIFY(iB) "' have different dimensions in '" LASS_HERE "'."))


namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

/** constructs an empty vector
 */
template <typename T>
Vector<T>::Vector():
	dimension_(0)
{
}



/** Construct a vector of dimension @a iDimension.
 *  @param iDimension the dimension of the vector to be created.  You can pass zero, but you 
 *         shouldn't pass negative dimensions though.
 *  @param iConstructInitialized by default this is false what means the contructor will initialize
 *                               all component values explicitely to zero.  If you don't want this
 *                               (to save time?), you can set this true.
 */
template <typename T>
Vector<T>::Vector(TSize iDimension, bool iConstructAsZero):
	dimension_(iDimension)
{
	LASS_ASSERT(dimension_ >= 0);
	if (dimension_ > 0)
	{
		values_.reset(new T[dimension_]);
		if (iConstructAsZero)
		{
			for (TSize i = 0; i < dimension_; ++i)
			{
				values_[i] = T();
			}
		}
	}
}



template <typename T>
Vector<T>::Vector(const Vector<T>& iOther): 
	dimension_(iOther.dimension())
{
	if (dimension_ > 0)
	{
		values_.reset(new T[dimension_]);
		for (unsigned i = 0; i < dimension_; ++i)
		{
			values_[i] = iOther.values_[i];
		}
	}
}



template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& iOther)
{
	Vector<T> temp(iOther);
	swap(temp);
	return *this;
}
	


/** return dimension of vector.
 *  this should never be a negative value.
 */
template <typename T>
typename Vector<T>::TSize Vector<T>::dimension() const
{
	LASS_ASSERT(dimension_ >= 0);
	return dimension_;
}



/** return the iIndex'th component value.
 *  iIndex shouldn't be out of the range [0, this->dimension()), unless you're asking for trouble.
 */
template <typename T>
typename Vector<T>::TConstReference Vector<T>::operator[](TSize iIndex) const
{
	LASS_ASSERT(iIndex >= 0 && iIndex < dimension_);
	return values_[iIndex];
}



/** access the iIndex'th component value.
 *  iIndex shouldn't be out of the range [0, this->dimension()), unless you're asking for trouble.
 */
template <typename T>
typename Vector<T>::TReference Vector<T>::operator[](TSize iIndex)
{
	LASS_ASSERT(iIndex < dimension_);
	return values_[iIndex];
}



/** return the iIndex'th component value and wrap index if necessary.
 *  if iIndex is out of the range [0, this->dimension()), it will be wrapped to map in this range.
 *  This is simply a modulus operation: mod(iIndex, this->dimension()).
 */
template <typename T>
typename Vector<T>::TConstReference Vector<T>::at(TSize iIndex) const
{
	return values_[mod(iIndex, dimension_)];
}



/** access the iIndex'th component value and wrap index if necessary.
 *  if iIndex is out of the range [0, this->dimension()), it will be wrapped to map in this range.
 *  This is simply a modulus operation: mod(iIndex, this->dimension()).
 */
template <typename T>
typename Vector<T>::TReference Vector<T>::at(TSize iIndex)
{
	return values_[num::mod(iIndex, dimension_)];
}



/** A weird way to get back the same object 
 */
template <typename T>
Vector<T>& Vector<T>::operator+() const
{
	return *this;
}



/** return a vector with all components negated
 *  (-v)[i] == -(v[i]). 
 */
template <typename T>
Vector<T> Vector<T>::operator-() const
{
	Vector<T> result(dimension_, false); // construct without initializing
	for (TSize i = 0; i < dimension_; ++i)
	{
		result.values_[i] = -values_[i];
	}
	return result;
}



/** componentwise addition
 */
template <typename T>
Vector<T>& Vector<T>::operator+=(const Vector<T>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	for (TSize i = 0; i < dimension_; ++i)
	{
		values_[i] += iB.values_[i];
	}
	return *this;
}



/** componentwise subtraction
 */
template <typename T>
Vector<T>& Vector<T>::operator-=(const Vector<T>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	for (TSize i = 0; i < dimension_; ++i)
	{
		values_[i] -= iB.values_[i];
	}
	return *this;
}



/** multiply each component of this with iB.
 */
template <typename T>
Vector<T>& Vector<T>::operator*=(TParam iB)
{
	for (TSize i = 0; i < dimension_; ++i)
	{
		values_[i] *= iB;
	}
	return *this;
}



/** divide each component of this by iB.
 */
template <typename T>
Vector<T>& Vector<T>::operator/=(TParam iB)
{
	for (TSize i = 0; i < dimension_; ++i)
	{
		values_[i] /= iB;
	}
	return *this;
}



template <typename T>
bool Vector<T>::isEmpty() const
{
	return dimension_ == 0;
}



/** Return true if all the components are (exactly!) zero
 */
template <typename T>
bool Vector<T>::isZero() const
{
	for (TSize i = 0; i < dimension_; ++i)
	{
        // if you get your compiler error here, you'll be using a broken STLport version. Ask [BdG].
        //
		if (values_[i] != TNumTraits::zero)
		{
			return false;
		}
	}
	return true;
}



/** Return squared norm of vector.
 *  @return dot(*this, *this)
 */
template <typename T>
typename Vector<T>::TValue Vector<T>::squaredNorm() const
{
	TValue result = TNumTraits::zero;
	for (TSize i = 0; i < dimension_; ++i)
	{
		result += num::sqr(values_[i]);
	}
	return result;
}



/** Return norm of vector.
 *  @return sqrt(this->squaredNorm())
 */
template <typename T>
typename Vector<T>::TValue Vector<T>::norm() const
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
template <typename T>
Vector<T> Vector<T>::normal() const
{
	Vector<T> result(*this);
	result.normalize();
	return result;
}



/** return a vector with each component being the reciprocal value of this vector.
 */
template <typename T>
Vector<T> Vector<T>::reciprocal() const
{
	Vector<T> result(dimension_, false);
	for (TSize i = 0; i < dimension_; ++i)
	{
		result.values_[i] = TNumTraits::one / values_[i];
	}
	return result;
}



/** Project vector on this one
 */
template <typename T>
Vector<T> Vector<T>::project(const Vector<T>& iB) const
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(*this, iB);
	Vector<T> result;
	result *= dot(iB, *this);
	result /= squaredNorm();
	return result;
}



/** Project vector on this one
 */
template <typename T>
Vector<T> Vector<T>::reject(const Vector<T>& iB) const
{
	return iB - project(iB);
}



/** Normalize vector.
 */
template <typename T>
void Vector<T>::normalize()
{
	*this /= norm();
}



/** get a pointer to the raw vector data, use with care!
 */
template <typename T>
typename Vector<T>::TPointer Vector<T>::data() const
{
	return values_.get();
}



/** swap content of two vectors
 */
template <typename T>
void Vector<T>::swap(Vector<T>& iOther)
{
	values_.swap(iOther.values_);
	std::swap(dimension_, iOther.dimension_);
}



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** dot product.
 *  @relates lass::prim::Vector
 */
template <typename T>
typename Vector<T>::TValue dot(const Vector<T>& iA, const Vector<T>& iB)
{
	LASS_NUM_VECTOR_ENFORCE_EQUAL_DIMENSION(iA, iB);
	typedef typename Vector<T>::TSize TSize;
	const TSize size = iA.dimension();

	TValue result = TNumTraits::zero;
	for (TSize i = 0; i < size; ++i)
	{
		result += iA.values_[i] * iB.values_[i];
	}
	return result;
}



/** transfrom a vector by applying an operator (well, actually a function) 'iOp' to each of each
 *  component values.
 *  @relates lass::prim::Vector
 */
template <typename T> 
Vector<T> transform(const Vector<T>& iA, T (*iF)(T))
{
	typedef typename Vector<T>::TSize TSize;
	const TSize size = iA.dimension();

	Vector<T> result(size, false);
	for (TSize i = 0; i < size; ++i)
	{
		result[i] = iF(iA_[i]);
	}
	return result;
}



/** @relates lass::prim::Vector
 */
template <typename T>
bool operator==(const Vector<T>& iA, const Vector<T>& iB)
{
	typedef typename Vector<T>::TSize TSize;
	const TSize size = iA.dimension();

	if (size != iB.dimension())
	{
		return false;
	}
	for (TSize i = 0; i < size; ++i)
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
template <typename T>
bool operator!=(const Vector<T>& iA, const Vector<T>& iB)
{
	return !(iA == iB);
}



/** componentwise addition
 *  @relates lass::prim::Vector
 */
template <typename T>
Vector<T> operator+(const Vector<T>& iA, const Vector<T>& iB)
{
	Vector<T> result(iA);
	result += iB;
	return result;
}



/** componentwise subtraction
 *  @relates lass::prim::Vector
 */
template <typename T>
Vector<T> operator-(const Vector<T>& iA, const Vector<T>& iB)
{
	Vector<T> result(iA);
	result -= iB;
	return result;
}



/** muliply all components of iA by iB
 *  @relates lass::prim::Vector
 */
template <typename T>
Vector<T> operator*(const Vector<T>& iA, const T& iB)
{
	Vector<T> result(iA);
	result *= iB;
	return result;
}



/** divide all components of iA by iB
 *  @relates lass::prim::Vector
 */
template <typename T>
Vector<T> operator/(const Vector<T>& iA, const T& iB)
{
	Vector<T> result(iA);
	result /= iB;
	return result;
}



/** multiply all components of iB with iA
 *  @relates lass::prim::Vector
 */
template <typename T>
Vector<T> operator*(const T& iA, const Vector<T>& iB)
{
	Vector<T> result(iB);
	result *= iA;
	return result;
}



/** @relates lass::prim::Vector
 */
template <typename T>
std::ostream& operator<<(std::ostream& oOStream, const Vector<T>& iB)
{
	typedef typename Vector<T>::TSize TSize;
	const TSize size = iB.dimension();

	LASS_ENFORCE_STREAM(oOStream) << "(";
	if (size > 0)
	{
		LASS_ENFORCE_STREAM(oOStream) << iB[0];
	}
	for (TSize i = 1; i < size; ++i)
	{
		LASS_ENFORCE_STREAM(oOStream) << ", " << iB[i];
	}
	LASS_ENFORCE_STREAM(oOStream) << ")";
	return oOStream;
}



/** @relates lass::prim::Vector
 */
template <typename T>
std::istream& operator>>(std::istream& ioIStream, Vector<T>& oB)
{
	typedef typename Vector<T>::TValue TValue;
	typedef typename Vector<T>::TSize TSize;
	typedef std::list<TValue> Buffer;
	
	Buffer buffer;
	TValue value;
	TSize size = 0;

	char c = 0;
	LASS_ENFORCE_STREAM(ioIStream) >> c;
	if (c != '(')
	{
		ioIStream.clear(std::ios_base::failbit);
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
			ioIStream.clear(std::ios_base::failbit);
			return ioIStream;
		}
	}
	while (c != ')');

	TVector<T> temp(size, false);
	for (TSize i = 0, Buffer::const_iterator it = buffer.begin(); i < size; ++i, ++bit)
	{
		LASS_ASSERT(it != buffer.end());
		temp[i] = *buffer;
	}

	result.swap(temp);
	return ioIStream;
}



}

}

#endif

// EOF
