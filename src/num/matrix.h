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



/** @class lass::num::Matrix
 *  @brief dynamically sized mxn-dimensional matrix
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_H

#include "num_common.h"
#include "num_traits.h"
#include "../util/call_traits.h"
#include "../util/scoped_ptr.h"

namespace lass
{
namespace num
{

template<typename T>
class Matrix
{
public:

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef typename TValue* TPointer;
	typedef typename num::NumTraits<T> TNumTraits;
	typedef size_t TSize;

	template <typename U> struct Rebind
	{
		typedef typename Matrix<U> Type;
	};

	Matrix() throw();
	explicit Matrix(TSize iRows, TSize iCols = 0, bool iConstructAsZero = true);
	Matrix(const Matrix<T>& iOther);
	/*
	template <typename U> explicit Matrix(const Matrix<U>& iOther): 
	{
		rows_ = iOther.rows();
		cols_ = iOther.cols();
		if (rows_ > 0 && cols_ > 0)
		{
			const TSize size = rows_ * cols_;
			values_.reset(new T[size]);
			for (unsigned i = 0; i < size; ++i)
			{
				values_[i] = iOther.values_[i];
			}
		}
	}
*/
	Matrix<T>& operator=(const Matrix<T>& iOther);

	TSize rows() const throw();
	TSize cols() const throw(); 

	inline typename Matrix::TConstReference operator()(TSize iRow, TSize iCol) const;
	inline typename Matrix::TReference operator()(TSize iRow, TSize iCol);
	inline typename Matrix::TConstReference at(TSize iRow, TSize iCol) const throw();
	inline typename Matrix::TReference at(TSize iRow, TSize iCol) throw();

	Matrix<T>& operator+() const throw();
	Matrix<T> operator-() const;
	Matrix<T>& operator+=(const Matrix<T>& iB);
	Matrix<T>& operator-=(const Matrix<T>& iB);
	Matrix<T>& operator*=(TParam iB) throw();
	Matrix<T>& operator/=(TParam iB) throw();

	void setZero(TSize iRows, TSize iCols);
	void setIdentity(TSize iSize);

	bool isEmpty() const throw();
	bool isZero() const throw();
	bool isIdentity() const throw();
	bool isDiagonal() const throw();
	bool isSquare() const throw();

	Matrix<T> transpose() const;
	Matrix<T> inverse() const;

	bool solve(Matrix<T>& ioB) const;

	TPointer data() const throw();
	void swap(Matrix<T>& iOther) throw();

private:

	typedef util::ScopedPtr<T, util::ArrayStorage> TValues;

	inline TSize flatIndex(TSize iRow, TSize iCol) const throw();

	TValues values_;
	TSize rows_;
	TSize cols_;
};

template <typename T> bool operator==(const Matrix<T>& iA, const Matrix<T>& iB);
template <typename T> inline bool operator!=(const Matrix<T>& iA, const Matrix<T>& iB);

template <typename T> inline Matrix<T> operator+(const Matrix<T>& iA, const Matrix<T>& iB);
template <typename T> inline Matrix<T> operator-(const Matrix<T>& iA, const Matrix<T>& iB);
template <typename T> inline Matrix<T> operator*(const Matrix<T>& iA, const T& iB);
template <typename T> inline Matrix<T> operator/(const Matrix<T>& iA, const T& iB);
template <typename T> inline Matrix<T> operator*(const T& iA, const Matrix<T>& iB);

template <typename T> Matrix<T> operator*(const Matrix<T>& iA, const Matrix<T>& iB);

template <typename T> std::ostream& operator<<(std::ostream& iS, const Matrix<T>& iA);
template <typename T> std::istream& operator>>(std::istream& iS, Matrix<T>& iA);

}

}

#include "matrix.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_H
#include "matrix_vector.h"
#endif

#endif

// EOF
