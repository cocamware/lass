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
 *  @brief a dynamic sized n-dimensional matrix with expression templates
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_H

#include "num_common.h"
#include "num_traits.h"
#include "impl/matrix_expressions.h"
#include "../util/call_traits.h"
#include "../util/scoped_ptr.h"

namespace lass
{
namespace num
{

template
<
    typename T,
    typename S = impl::MStorage<T>
>
class Matrix
{
public:

	typedef Matrix<T, S> TSelf;
	typedef S TStorage;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits<T> TNumTraits;
	typedef TValue* TPointer;
	typedef size_t TSize;

	template <typename U> struct Rebind
	{
		typedef Matrix<U, S> Type;
	};

	Matrix();
	explicit Matrix(TSize iRows, TSize iCols);
    explicit Matrix(const TStorage& iStorage);

	template <typename T2, typename S2> Matrix<T, S>& operator=(const Matrix<T2, S2>& iOther);

    const TSize rows() const;
	const TSize columns() const; 

	const TValue operator()(TSize iRow, TSize iCol) const;
	TReference operator()(TSize iRow, TSize iCol);
	const TValue at(TSize iRow, TSize iCol) const;
	TReference at(TSize iRow, TSize iCol);

	const Matrix<T, S>& operator+() const;
	const Matrix<T, impl::MNeg<T, S> > operator-() const;

	template <typename S2> Matrix<T, S>& operator+=(const Matrix<T, S2>& iB);
	template <typename S2> Matrix<T, S>& operator-=(const Matrix<T, S2>& iB);
	Matrix<T, S>& operator*=(TParam iB);
	Matrix<T, S>& operator/=(TParam iB);

	void setZero(TSize iRows, TSize iCols);
	void setIdentity(TSize iSize);

	bool isEmpty() const;
	bool isZero() const;
	bool isIdentity() const;
	bool isDiagonal() const;
	bool isSquare() const;

    const Matrix<T, impl::MTrans<T, S> > transpose() const;
	
    bool inverse();

	const TStorage& storage() const;
	void swap(Matrix<T, S>& iOther);

private:

	TStorage storage_;
};

template <typename T, typename S1, typename S2> 
bool operator==(const Matrix<T, S1>& iA, const Matrix<T, S2>& iB);
template <typename T, typename S1, typename S2> 
inline bool operator!=(const Matrix<T, S1>& iA, const Matrix<T, S2>& iB);

template <typename T, typename S1, typename S2> 
const Matrix<T, impl::MAdd<T, S1, S2> > operator+(const Matrix<T, S1>& iA, const Matrix<T, S2>& iB);
template <typename T, typename S1, typename S2> 
const Matrix<T, impl::MSub<T, S1, S2> > operator-(const Matrix<T, S1>& iA, const Matrix<T, S2>& iB);
template <typename T, typename S1, typename S2> 
const Matrix<T, impl::MMul<T, S1, S2> > operator*(const Matrix<T, S1>& iA, const Matrix<T, S2>& iB);

template <typename T, typename S> 
const Matrix<T, impl::MAdd<T, impl::MScalar<T>, S> > operator+(const T& iA, const Matrix<T, S>& iB);
template <typename T, typename S> 
const Matrix<T, impl::MSub<T, impl::MScalar<T>, S> > operator-(const T& iA, const Matrix<T, S>& iB);
template <typename T, typename S> 
const Matrix<T, impl::MMul<T, impl::MScalar<T>, S> > operator*(const T& iA, const Matrix<T, S>& iB);

template <typename T, typename S> 
const Matrix<T, impl::MAdd<T, S, impl::MScalar<T> > > operator+(const Matrix<T, S>& iA, const T& iB);
template <typename T, typename S> 
const Matrix<T, impl::MAdd<T, S, impl::MScalar<T> > > operator-(const Matrix<T, S>& iA, const T& iB);
template <typename T, typename S> 
const Matrix<T, impl::MMul<T, S, impl::MScalar<T> > > operator*(const Matrix<T, S>& iA, const T& iB);
template <typename T, typename S> 
const Matrix<T, impl::MMul<T, S, impl::MScalar<T> > > operator/(const Matrix<T, S>& iA, const T& iB);

template <typename T, typename S> 
bool solve(const Matrix<T, S>& iA, Matrix<T>& ioB);


template <typename T, typename S, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& iS, const Matrix<T, S>& iA);

}

}

#include "matrix.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_H
#include "matrix_vector.h"
#endif

#endif

// EOF
