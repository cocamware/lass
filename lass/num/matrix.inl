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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_INL

#include "num_common.h"
#include "matrix.h"
#include "impl/matrix_solve.h"
#include "../meta/meta_assert.h"

#include <cstddef>

#define LASS_NUM_MATRIX_ENFORCE_EQUAL_DIMENSION(a, b)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::TruePredicate,\
		::lass::util::impl::DefaultRaiser,\
		(a).rows() == (b).rows() && (a).columns() == (b).columns(),\
		int(0), \
		"Matrices '" LASS_STRINGIFY(a) "' and '" LASS_STRINGIFY(b) "' have different dimensions in '" LASS_HERE "'.")

#define LASS_NUM_MATRIX_ENFORCE_ADJACENT_DIMENSION(a, b)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::EqualPredicate,\
		::lass::util::impl::DefaultRaiser,\
		(a).columns(),\
		(b).rows(),\
		"Matrices '" LASS_STRINGIFY(a) "' and '" LASS_STRINGIFY(b) "' have no matching dimensions for operation at '" LASS_HERE "'.")

namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

/** constructs an empty matrix
 *
 *  @par Exception Safety: 
 *		strong guarentee.
 */
template <typename T, typename S>
Matrix<T, S>::Matrix():
	storage_()
{
}



/** Construct a matrix of dimension @a rows x @a columns.
 *  Can be used as default constructor for storage in containers.
 *  @param rows the number of rows of the matrix to be created.
 *  @param columns the number of columns of the matrix to be created.
 *  By default both @a rows and @a columns are zero, what creates an empty matrix.
 *  Not very usefull though, except as place holder.  So, it's safe to pass
 *  zero as arguments, but you shouldn't pass negative values.
 *
 *  @par Complexity:
 *		O(rows * columns)
 *
 *  @par Exception Safety: 
 *		strong guarentee.
 */
template <typename T, typename S>
Matrix<T, S>::Matrix(TSize rows, TSize columns):
	storage_(rows, columns)
{
}



template <typename T, typename S>
Matrix<T, S>::Matrix(const TStorage& storage):
	storage_(storage)
{
}



/** assign storage/expression matrix to this (this should be a storage matrix).
 *
 *  @pre @c this must be an l-value (storage matrix).
 *
 *  @par Complexity:
 *		O(other.rows() * other.columns())
 *
 *  @par Exception Safety: 
 *		basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Matrix<T, S>::Matrix(const Matrix<T2, S2>& other):
	storage_(other.rows(), other.columns())
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);

	const S2& b = other.storage();
	const TSize m = rows();
	const TSize n = columns();
	for (TSize i = 0; i < m; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			storage_(i, j) = b(i, j);
		}
	}
}



/** assign storage/expression matrix to this (this should be a storage matrix).
 *
 *  @pre @c this must be an l-value (storage matrix).
 *
 *  @par Complexity:
 *		O(other.rows() * other.columns())
 *
 *  @par Exception Safety: 
 *		basic guarentee.
 */
template <typename T, typename S>
template <typename T2, typename S2>
Matrix<T, S>& Matrix<T, S>::operator=(const Matrix<T2, S2>& other)
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);

	const S2& b = other.storage();
	const TSize m = other.rows();
	const TSize n = other.columns();
	storage_.resize(m, n);
	for (TSize i = 0; i < m; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			storage_(i, j) = b(i, j);
		}
	}
	return *this;
}



/** return number of rows in matrix.
 *
 *  @post should never return be a negative value.
 *
 *  @par Complexity:
 *		O(1)
 *
 *  @par Exception safety:
 *		no-fail
 */
template <typename T, typename S> inline
const typename Matrix<T, S>::TSize
Matrix<T, S>::rows() const
{
	return storage_.rows();
}



/** return number of columns in matrix.
 *
 *  @post should never return be a negative value.
 *
 *  @par Complexity:
 *		O(1)
 *
 *  @par Exception safety:
 *		no-fail
 */
template <typename T, typename S> inline
const typename Matrix<T, S>::TSize
Matrix<T, S>::columns() const
{
	return storage_.columns();
}



/** return the component value at position (@a row, @a column)
 *
 *  @pre (@a row, @a column) shouldn't be out of the range [0, this->rows()) x [0, this->columns()], unless
 *  you're asking for trouble.
 */
template <typename T, typename S> inline
const typename Matrix<T, S>::TValue
Matrix<T, S>::operator()(TSize row, TSize column) const
{
	LASS_ASSERT(row < rows() && column < columns());
	return storage_(row, column);
}



/** access the component value at position (@a row, @a column)
 *
 *  @pre (@a row, @a column) shouldn't be out of the range [0, this->rows()) x [0, this->columns()], unless
 *  you're asking for trouble.
 *
 *  @pre @c this must be an l-value (storage matrix).
 */
template <typename T, typename S> inline
typename Matrix<T, S>::TReference
Matrix<T, S>::operator()(TSize iRow, TSize iColumn)
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);
	LASS_ASSERT(iRow < rows() && iColumn < columns());
	return storage_(iRow, iColumn);
}



/** return the component value at position (@a row, @a column)
 *  (@a row, @a column) will be wrapped to range [0, this->rows()) x [0, this->columns()] by using a
 *  modulus operator
 */
template <typename T, typename S> inline
const typename Matrix<T, S>::TValue
Matrix<T, S>::at(TSigned row, TSigned column) const
{
	return storage_(mod(row, rows()), mod(column, columns()));
}



/** access the component value at position (@a row, @a column)
 *  (@a row, @a column) will be wrapped to range [0, this->rows()) x [0, this->columns()] by using a
 *  modulus operator
 *
 *  @pre @c this must be an l-value (storage matrix).
 */
template <typename T, typename S> inline
typename Matrix<T, S>::TReference
Matrix<T, S>::at(TSigned row, TSigned column)
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);
	return storage_(mod(row, rows()), mod(column, columns()));
}



/** return a row of matrix.
 *  @a index will be wrapped to range [0, this->rows()) by using a modulus operator
 */
template <typename T, typename S> inline
typename Matrix<T, S>::ConstRow
Matrix<T, S>::row(TSigned index) const
{
	return ConstRow(*this, mod(index, rows()));
}



/** return a row of matrix.
 *  @a index will be wrapped to range [0, this->rows()) by using a modulus operator.
 *  THIS MUST BE LVALUE (storage matrix).
 */
template <typename T, typename S> inline
typename Matrix<T, S>::Row
Matrix<T, S>::row(TSigned index)
{
	return Row(*this, mod(index, rows()));
}



/** return a column of matrix.
 *  @a index will be wrapped to range [0, this->columns()) by using a modulus operator
 */
template <typename T, typename S> inline
typename Matrix<T, S>::ConstColumn
Matrix<T, S>::column(TSigned index) const
{
	return ConstColumn(*this, mod(index, columns()));
}



/** return a column of matrix.
 *  @a index will be wrapped to range [0, this->columns()) by using a modulus operator
 *  THIS MUST BE LVALUE (storage matrix).
 */
template <typename T, typename S> inline
typename Matrix<T, S>::Column
Matrix<T, S>::column(TSigned index)
{
	return Column(*this, mod(index, columns()));
}



/** A weird way to get back the same object
 *
 *  @par Complexity:
 *		O(1)
 *
 *  @par Exception safety:
 *		no-fail
 */
template <typename T, typename S> inline
const Matrix<T, S>& Matrix<T, S>::operator+() const
{
	return *this;
}



/** return a vector with all components negated
 *  (-a)(i, j) == -(a(i, j)).
 *
 *  @par Complexity:
 *		O(1)
 *
 *  @par Exception safety:
 *		no-fail
 */
template <typename T, typename S>
const Matrix<T, impl::MNeg<T, S> >
Matrix<T, S>::operator-() const
{
	typedef impl::MNeg<T, S> TExpression;
	return Matrix<T, TExpression>(TExpression(storage_));
}



/** componentswise addition assignment of two matrices
 *
 *  <i>Matrix Addition: Denote the sum of two matrices @n A and @n B (of the same dimensions) by
 *  @n C=A+B. The sum is defined by adding entries with the same indices @n Cij=Aij+Bij over all
 *  @n i and @n j.</i>
 *  http://mathworld.wolfram.com/MatrixAddition.html
 *
 *  This method implements the assignment addition what reduces to @n Aij+=Bij over all @n i and
 *  @n j.
 *
 *  @pre @c this must be an l-value (storage matrix).
 *
 *  @par Complexity:
 *		O(this->rows() * this->columns())
 *
 *  @throw an exception is thrown if @a *this and @a iB are not of the same dimensions
 */
template <typename T, typename S>
template <typename S2>
Matrix<T, S>& Matrix<T, S>::operator+=(const Matrix<T, S2>& other)
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);
	LASS_NUM_MATRIX_ENFORCE_EQUAL_DIMENSION(*this, other);
	const S2& b = other.storage();
	const TSize m = rows();
	const TSize n = columns();
	for (TSize i = 0; i < m; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			storage_(i, j) += b(i, j);
		}
	}
	return *this;
}



/** componentswise subtraction assignment of two matrices
 *  This method is equivalent to @n this+=(-iB) .
 *
 *  @sa Matrix<T>::operator+=
 *
 *  @pre @c this must be an l-value (storage matrix).
 *
 *  @par Complexity:
 *		O(this->rows() * this->columns())
 *
 *  @throw an exception is thrown if @a *this and @a iB are not of the same dimensions
 */
template <typename T, typename S>
template <typename S2>
Matrix<T, S>& Matrix<T, S>::operator-=(const Matrix<T, S2>& other)
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);
	LASS_NUM_MATRIX_ENFORCE_EQUAL_DIMENSION(*this, other);
	const S2& b = other.storage();
	const TSize m = rows();
	const TSize n = columns();
	for (TSize i = 0; i < m; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			storage_(i, j) -= b(i, j);
		}
	}
	return *this;
}



/** scalar multiplication assignment of matrix
 *
 *  @pre @c this must be an l-value (storage matrix).
 *
 *  @par Complexity:
 *		O(this->rows() * this->columns())
 */
template <typename T, typename S>
Matrix<T, S>& Matrix<T, S>::operator*=(TParam scalar)
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);
	const TSize m = rows();
	const TSize n = columns();
	for (TSize i = 0; i < m; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			storage_(i, j) *= scalar;
		}
	}
	return *this;
}



/** scalar multiplication assignment of matrix
 *
 *  @pre @c this must be an l-value (storage matrix).
 */
template <typename T, typename S>
Matrix<T, S>& Matrix<T, S>::operator /=(TParam scalar)
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);
	const TSize m = rows();
	const TSize n = columns();
	for (TSize i = 0; i < m; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			storage_(i, j) /= scalar;
		}
	}
	return *this;
}



/** set this to a zero matrix of @a rows rows and @a columns columns.
 *  @sa Matrix<T>::isZero
 *
 *  @pre this should be an lvalue
 *
 *  @par Complexity:
 *		O(this->rows() * this->columns())
 */
template <typename T, typename S>
void Matrix<T, S>::setZero(TSize rows, TSize columns)
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);
	storage_.resize(rows, columns);
	for (TSize i = 0; i < rows; ++i)
	{
		for (TSize j = 0; j < columns; ++j)
		{
			storage_(i, j) = TNumTraits::zero;
		}
	}
}



/** set matrix to an identity matrix.
 *  @sa Matrix<T>::isIdentity
 *
 *  @pre this should be an lvalue
 *
 *  @par Complexity:
 *		O(this->rows() * this->columns())
 */
template <typename T, typename S>
void Matrix<T, S>::setIdentity(TSize iSize)
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);
	storage_.resize(iSize, iSize);
	for (TSize i = 0; i < iSize; ++i)
	{
		for (TSize j = 0; j < iSize; ++j)
		{
			storage_(i, j) = i == j ? TNumTraits::one : TNumTraits::zero;
		}
	}
}



/** return true if this is a (0�0) matrix
 *
 *  @par Complexity:
 *		O(1)
 *
 *  @par Exception safety:
 *		no-fail
 */
template <typename T, typename S>
bool Matrix<T, S>::isEmpty() const
{
	return storage_.rows() == 0 || storage_.columns() == 0;
}



/** test if this matrix equals a zero matrix.
 *
 *  <i>A zero matrix is an @n m�n matrix consisting of all 0s (MacDuffee 1943, p. 27), denoted @n 0.
 *  Zero matrices are sometimes also known as null matrices (Akivis and Goldberg 1972, p. 71).</i>
 *  http://mathworld.wolfram.com/ZeroMatrix.html
 *
 *  an empty matrix is also a zero matrix.
 *
 *  @par Complexity:
 *		O(this->rows() * this->columns())
 */
template <typename T, typename S>
bool Matrix<T, S>::isZero() const
{
	const TSize m = rows();
	const TSize n = columns();
	for (TSize i = 0; i < m; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			if (storage_(i, j) != TNumTraits::zero)
			{
				return false;
			}
		}
	}
	return true;
}



/** test if this matrix equals a identity matrix
 *
 *  <i>The identity matrix is a the simplest nontrivial diagonal matrix, defined such that @n I(X)=X
 *  for all vectors @n X. An identity matrix may be denoted @n 1, @n I, or @n E (the latter being an
 *  abbreviation for the German term "Einheitsmatrix"; Courant and Hilbert 1989, p. 7). Identity
 *  matrices are sometimes also known as unit matrices (Akivis and Goldberg 1972, p. 71). The
 *  @n n�n identity matrix is given explicitly by @n Iij=dij for @n i, @n j = 1, 2, ..., n, where
 *  @n dij is the Kronecker delta.</i>
 *  http://mathworld.wolfram.com/IdentityMatrix.html
 *
 *  an empty matrix is also an identity matrix.
 *
 *  @par Complexity:
 *		O(this->rows() * this->columns())
 */
template <typename T, typename S>
bool Matrix<T, S>::isIdentity() const
{
	if (!isSquare())
	{
		return false;
	}
	const TSize m = rows();
	const TSize n = columns();
	for (TSize i = 0; i < m; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			if (storage_(i, j) != ((i == j) ? TNumTraits::one : TNumTraits::zero))
			{
				return false;
			}
		}
	}
	return true;
}



/** test if this matrix is an diagonal matrix
 *
 *  <i>A diagonal matrix is a square matrix @n A of the form @n Aij=Ci*dij where @n dij is the
 *  Kronecker delta, @n Ci are constants, and @n i, @n j = 1, 2, ..., n, with is no implied
 *  summation over indices.</i>
 *  http://mathworld.wolfram.com/DiagonalMatrix.html
 *
 *  @note both square zero matrices and identity matrices will yield true on this one.
 *
 *  @par Complexity:
 *		O(this->rows() * this->columns())
 */
template <typename T, typename S>
bool Matrix<T, S>::isDiagonal() const
{
	if (!isSquare())
	{
		return false;
	}
	const TSize m = rows();
	const TSize n = columns();
	for (TSize i = 0; i < m; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			if (i != j && storage_(i, j) != TNumTraits::zero)
			{
				return false;
			}
		}
	}
	return true;
}



/** return true if matrix has as many rows as columns
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S>
bool Matrix<T, S>::isSquare() const
{
	return storage_.rows() == storage_.columns();
}



/** return transposed matrix
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S>
const Matrix<T, impl::MTrans<T, S> >
Matrix<T, S>::transposed() const
{
	typedef impl::MTrans<T, S> TExpression;
	return Matrix<T, TExpression>(TExpression(storage_));
}



/** replace matrix by its inverse.
 *
 *  If matrix has no inverse (this is singular), no exception is thrown.  Instead, an empty matrix
 *  is made.
 *
 *  @pre @c this must be an l-value (storage matrix).
 *
 *  @result true if succeeded, false if not.
 *  @throw an exception is thrown if this is not a square matrix
 */
template <typename T, typename S>
void Matrix<T, S>::invert()
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);
	LASS_ENFORCE(isSquare());

	const TSize size = rows();
	Matrix<T> lu(*this);
	std::vector<std::ptrdiff_t> index(size);
	const std::ptrdiff_t n = static_cast<std::ptrdiff_t>(size);
	int d;

	if (!impl::ludecomp<T>(lu.storage().rowMajor(), index.begin(), n, d))
	{
		LASS_THROW_EX(util::SingularityError, "failed to invert matrix");
	}

	setIdentity(size);
	for (std::ptrdiff_t i = 0; i < n; ++i)
	{
		impl::lusolve<T>(lu.storage().rowMajor(), index.begin(), column(i).begin(), n);
	}
}



template <typename T, typename S> inline
const typename Matrix<T, S>::TStorage&
Matrix<T, S>::storage() const
{
	return storage_;
}



template <typename T, typename S> inline
typename Matrix<T, S>::TStorage&
Matrix<T, S>::storage()
{
	return storage_;
}



/** swap two matrices
 *
 *  @pre @c this and @a other must be an l-value (storage matrix).
 *
 *  @par Complexity:
 *		O(1)
 *
 *  @par Exception safety:
 *		no-fail
 */
template <typename T, typename S> inline
void Matrix<T, S>::swap(Matrix<T, S>& other)
{
	LASS_META_ASSERT(TStorage::lvalue, this_is_not_an_lvalue);
	storage_.swap(other.storage_);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(a.rows() * a.columns())
 */
template <typename T, typename S1, typename S2>
bool operator==(const Matrix<T, S1>& a, const Matrix<T, S2>& b)
{
	if (a.rows() != b.rows() || a.columns() != b.columns())
	{
		return false;
	}
	typedef typename Matrix<T, S1>::TSize TSize;
	const TSize m = a.rows();
	const TSize n = a.columns();
	for (TSize i = 0; i < m; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			if (a(i, j) != b(i, j))
			{
				return false;
			}
		}
	}
	return true;
}



/** @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(a.rows() * a.columns())
 */
template <typename T, typename S1, typename S2>
bool operator!=(const Matrix<T, S1>& a, const Matrix<T, S2>& b)
{
	return !(a == b);
}



/** componentwise addition
 *  @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S1, typename S2>
const Matrix<T, impl::MAdd<T, S1, S2> >
operator+(const Matrix<T, S1>& a, const Matrix<T, S2>& b)
{
	LASS_NUM_MATRIX_ENFORCE_EQUAL_DIMENSION(a, b);
	typedef impl::MAdd<T, S1, S2> TExpression;
	return Matrix<T, TExpression>(TExpression(a.storage(), b.storage()));
}



/** componentwise addition
 *  @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S1, typename S2>
const Matrix<T, impl::MSub<T, S1, S2> >
operator-(const Matrix<T, S1>& a, const Matrix<T, S2>& b)
{
	LASS_NUM_MATRIX_ENFORCE_EQUAL_DIMENSION(a, b);
	typedef impl::MSub<T, S1, S2> TExpression;
	return Matrix<T, TExpression>(TExpression(a.storage(), b.storage()));
}



/** Matrix multiplication
 *  @relates lass::num::Matrix
 *
 *  <i>The product @n C of two matrices @n A and @n B is defined by @n Cik=Aij*Bjk, where @n j is
 *  summed over for all possible values of @n i and @n k. The implied summation over repeated
 *  indices without the presence of an explicit sum sign is called Einstein summation, and is
 *  commonly used in both matrix and tensor analysis. Therefore, in order for matrix multiplication
 *  to be defined, the dimensions of the matrices must satisfy @n (n�m)*(m�p)=(n�p) where @n (a�b)
 *  denotes a matrix with @n a rows and @n b columns.</i>
 *  http://mathworld.wolfram.com/MatrixMultiplication.html
 *
 *  @throw an exception is throw in @a a and @a b don't meet the requirement
 *         @n a.columns()==b.rows() .
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S1, typename S2>
const Matrix<T, impl::MProd<T, S1, S2> >
operator*(const Matrix<T, S1>& a, const Matrix<T, S2>& b)
{
	LASS_NUM_MATRIX_ENFORCE_ADJACENT_DIMENSION(a, b);
	typedef impl::MProd<T, S1, S2> TExpression;
	return Matrix<T, TExpression>(TExpression(a.storage(), b.storage()));
}



/** add @a a to all components of @a b
 *  @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S>
const Matrix<T, impl::MAdd<T, impl::MScalar<T>, S> >
operator+(const T& a, const Matrix<T, S>& b)
{
	typedef impl::MAdd<T, impl::MScalar<T>, S> TExpression;
	return Matrix<T, TExpression>(TExpression(
		impl::MScalar<T>(a, b.rows(), b.cols()), b.storage()));
}



/** add @a a to all negated components of @a b
 *  @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S>
const Matrix<T, impl::MSub<T, impl::MScalar<T>, S> >
operator-(const T& a, const Matrix<T, S>& b)
{
	typedef impl::MSub<T, impl::MScalar<T>, S> TExpression;
	return Matrix<T, TExpression>(TExpression(
		impl::MScalar<T>(a, b.rows(), b.cols()), b.storage()));
}



/** multiply @a a with all components of @a b
 *  @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S>
const Matrix<T, impl::MMul<T, impl::MScalar<T>, S> >
operator*(const T& a, const Matrix<T, S>& b)
{
	typedef impl::MMul<T, impl::MScalar<T>, S> TExpression;
	return Matrix<T, TExpression>(TExpression(
		impl::MScalar<T>(a, b.rows(), b.columns()), b.storage()));
}



/** add @a b to all components of @a a
 *  @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S>
const Matrix<T, impl::MAdd<T, S, impl::MScalar<T> > >
operator+(const Matrix<T, S>& a, typename Matrix<T, S>::TParam b)
{
	typedef impl::MAdd<T, S, impl::MScalar<T> > TExpression;
	return Matrix<T, TExpression>(TExpression(
		a.storage(), impl::MScalar<T>(b, a.rows(), a.cols())));
}



/** subtract @a b from all components of @a a
 *  @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S>
const Matrix<T, impl::MAdd<T, S, impl::MScalar<T> > >
operator-(const Matrix<T, S>& a, typename Matrix<T, S>::TParam b)
{
	typedef impl::MSub<T, S, impl::MScalar<T> > TExpression;
	return Matrix<T, TExpression>(TExpression(
		a.storage(), impl::MScalar<T>(-b, a.rows(), a.cols())));
}



/** multiply all components of @a a with @a b.
 *  @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S>
const Matrix<T, impl::MMul<T, S, impl::MScalar<T> > >
operator*(const Matrix<T, S>& a, typename Matrix<T, S>::TParam b)
{
	typedef impl::MMul<T, S, impl::MScalar<T> > TExpression;
	return Matrix<T, TExpression>(TExpression(
		a.storage(), impl::MScalar<T>(b, a.rows(), a.columns())));
}



/** divide all components of @a a by @a b.
 *  @relates lass::num::Matrix
 *
 *  @par Complexity:
 *		O(1)
 */
template <typename T, typename S>
const Matrix<T, impl::MMul<T, S, impl::MScalar<T> > >
operator/(const Matrix<T, S>& a, typename Matrix<T, S>::TParam b)
{
	typedef impl::MMul<T, S, impl::MScalar<T> > TExpression;
	return Matrix<T, TExpression>(TExpression(
		a.storage(), impl::MScalar<T>(num::inv(b), a.rows(), a.columns())));
}



/** solve equation A * X = B
 *
 *  @param a [in] matrix A
 *  @param bx [in,out] serves both as the input matrix B and the output matrix X.
 *  @param improve [in] if true, it will perform an extra iteration to improve the solution, 
 *      but it comes at the cost of having to take an extra copy of @a a.
 *
 *  @pre @a bx must be an l-value (storage matrix)
 *
 *  @relates lass::num::Matrix
 *  @throw an exception is thrown if dimensions don't match
 *         (this->isSquare() && this->columns() == bx.rows())
 */
template <typename T, typename S, typename S2>
void solve(const Matrix<T, S>& a, Matrix<T, S2>& bx, bool improve)
{
	LASS_META_ASSERT(S2::lvalue, bx_isnt_an_lvalue);
	LASS_ENFORCE(a.isSquare());
	LASS_NUM_MATRIX_ENFORCE_ADJACENT_DIMENSION(a, bx);

	const size_t n = a.rows();
	Matrix<T> lu(a);
	std::vector<std::ptrdiff_t> pivot(n);
	int d;

	Matrix<T> aa;
	std::vector<T> bcol;
	if (improve)
	{
		aa = a;
		bcol.resize(n);
	}

	if (!impl::ludecomp<T>(lu.storage().rowMajor(), pivot.begin(), static_cast<std::ptrdiff_t>(n), d))
	{
		LASS_THROW_EX(util::SingularityError, "failed to solve matrix equation");
	}

	for (size_t i = 0; i < bx.columns(); ++i)
	{
		LASS_ASSERT(static_cast<int>(i) >= 0);
		typename Matrix<T, S2>::Column xcol = bx.column(static_cast<int>(i));
		if (improve)
		{
			for (size_t j = 0; j < n; ++j)
			{
				bcol[j] = xcol[j];
			}
		}
		impl::lusolve<T>(lu.storage().rowMajor(), pivot.begin(), xcol.begin(), static_cast<std::ptrdiff_t>(n));
		if (improve)
		{
			impl::lumprove<T>(aa.storage().rowMajor(), lu.storage().rowMajor(), pivot.begin(), bcol.begin(), xcol.begin(), static_cast<std::ptrdiff_t>(n));
		}
	}
}



/** @relates lass::num::Matrix
 */
template <typename T, typename S, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& stream, const Matrix<T, S>& matrix)
{
	typedef typename Matrix<T, S>::TSize TSize;
	const TSize m = matrix.rows();
	const TSize n = matrix.columns();

	LASS_ENFORCE_STREAM(stream) << "(";
	for (TSize i = 0; i < m; ++i)
	{
		if (i != 0)
		{
			LASS_ENFORCE_STREAM(stream) << ", ";
		}
		LASS_ENFORCE_STREAM(stream) << "(";
		if (n > 0)
		{
			LASS_ENFORCE_STREAM(stream) << matrix(i, 0);
		}
		for (TSize j = 1; j < n; ++j)
		{
			LASS_ENFORCE_STREAM(stream) << ", " << matrix(i, j);
		}
		LASS_ENFORCE_STREAM(stream) << ")";
	}
	LASS_ENFORCE_STREAM(stream) << ")";
	return stream;
}



}

}

#endif

// EOF
