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



/** @class lass::num::Vector
 *  @brief a dynamic sized n-dimensional vector
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_H

#include "num_common.h"
#include "num_traits.h"
#include "../util/call_traits.h"
#include "../util/scoped_ptr.h"

namespace lass
{
namespace num
{

template<typename T>
class Vector
{
public:

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef TValue* TPointer;
	typedef typename num::NumTraits<T> TNumTraits;
	typedef size_t TSize;

	template <typename U> struct Rebind
	{
		typedef typename Vector<U> Type;
	};

	Vector() throw();
	explicit Vector(TSize iDimension, bool iConstructAsZero = true);
	Vector(const Vector<T>& iOther);
	/*
	template <typename U> explicit Vector(const Vector<U>& iOther): 
	{
		dimension_ = iOther.dimension();
		if (dimension_ > 0)
		{
			values_.reset(new T[dimension_]);
			for (unsigned i = 0; i < dimension_; ++i)
			{
				values_[i] = iOther.values_[i];
			}
		}
	}
*/
	Vector<T>& operator=(const Vector<T>& iOther);

	TSize dimension() const throw(); 

	inline typename Vector::TConstReference operator[](TSize iIndex) const;
	inline typename Vector::TReference operator[](TSize iIndex);
	inline typename Vector::TConstReference at(TSize iIndex) const throw();
	inline typename Vector::TReference at(TSize iIndex) throw();

	Vector<T>& operator+() const throw();
	Vector<T> operator-() const;
	Vector<T>& operator+=(const Vector<T>& iB);
	Vector<T>& operator-=(const Vector<T>& iB);
	Vector<T>& operator*=(TParam iB) throw();
	Vector<T>& operator/=(TParam iB) throw();

	bool isEmpty() const throw();
	bool isZero() const throw();
	TValue squaredNorm() const throw();
	TValue norm() const throw();
	Vector<T> normal() const;
	Vector<T> reciprocal() const;
	Vector<T> project(const Vector<T>& iB) const;
	Vector<T> reject(const Vector<T>& iB) const;

	void normalize() throw();

	TPointer data() const throw();
	void swap(Vector<T>& iOther) throw();

private:

	typedef util::ScopedPtr<T, util::ArrayStorage> TValues;

	TValues values_;
	TSize dimension_;
};



template <typename T> typename Vector<T>::TValue dot(const Vector<T>& iA, const Vector<T>& iB) throw();
template <typename T> Vector<T> transform(const Vector<T>& iA, T (*iF)(T));

template <typename T> bool operator==(const Vector<T>& iA, const Vector<T>& iB) throw();
template <typename T> inline bool operator!=(const Vector<T>& iA, const Vector<T>& iB) throw();

template <typename T> inline Vector<T> operator+(const Vector<T>& iA, const Vector<T>& iB);
template <typename T> inline Vector<T> operator-(const Vector<T>& iA, const Vector<T>& iB);
template <typename T> inline Vector<T> operator*(const Vector<T>& iA, const T& iB);
template <typename T> inline Vector<T> operator/(const Vector<T>& iA, const T& iB);
template <typename T> inline Vector<T> operator*(const T& iA, const Vector<T>& iB);

template <typename T> std::ostream& operator<<(std::ostream& ioOStream, const Vector<T>& iB);
template <typename T> std::istream& operator>>(std::istream& ioIStream, Vector<T>& iB);



}

}

#include "vector.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_H
#include "matrix_vector.h"
#endif

#endif

// EOF
