/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_INL

#include "num_common.h"
#include "matrix.h"
#include "impl/matrix_solve.h"

#define LASS_NUM_MATRIX_ENFORCE_EQUAL_DIMENSION(iA, iB)\
	(*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
									 lass::util::impl::DefaultRaiser>\
	((iA).rows() == (iB).rows() && (iA).cols() == (iB).cols(), "Matrices '" LASS_STRINGIFY(iA)\
	 "' and '" LASS_STRINGIFY(iB) "' have different dimensions in '" LASS_HERE "'."))

#define LASS_NUM_MATRIX_ENFORCE_ADJACENT_DIMENSION(iA, iB)\
	(*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
									 lass::util::impl::DefaultRaiser>\
	((iA).cols() == (iB).rows(), "Matrices '" LASS_STRINGIFY(iA) "' and '" LASS_STRINGIFY(iB)\
	 "' have no matchin dimensions for operation at '" LASS_HERE "'."))

namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

/** constructs an empty matrix
 */
template <typename T>
Matrix<T>::Matrix():
	rows_(0),
	cols_(0)
{
}



/** Construct a matrix of dimension @a iRows x @a iCols.
 *  Can be used as default constructor for storage in containers.
 *  @param iRows the number of rows of the matrix to be created.  By default this is zero, which
 *         is a matrix with no data in it.  Not very usefull except as a placeholder.  You shouldn't
 *         pass a negative number though.
 *  @param iCols the number of columns of the matrix to be created.  By default this is zero, which
 *         means a square matrix must be created.  i.e. the same number as iRows must be used.  You
 *         shouldn't pass a negative number either.
 *  @param iConstructAsZero by default this is true what means the contructor will initialize
 *							all component values explicitely to zero.  If you don't want this
 *							(to save time?), you can set this false.
 */
template <typename T>
Matrix<T>::Matrix(TSize iRows, TSize iCols, bool iConstructAsZero):
	rows_(iRows),
	cols_(iCols == 0 ? iRows : iCols)
{
	LASS_ASSERT(rows_ >= 0 && cols_ >= 0);
	const TSize size = rows_ * cols_;
	if (size > 0)
	{
		values_.reset(new T[size]);
		if (iConstructAsZero)
		{
			for (TSize i = 0; i < size; ++i)
			{
				values_[i] = T();
			}
		}
	}
}



template <typename T>
Matrix<T>::Matrix(const Matrix<T>& iOther): 
	rows_(iOther.rows_),
	cols_(iOther.cols_)
{
	const TSize size = rows_ * cols_;
	if (size > 0)
	{
		values_.reset(new T[size]);
		for (unsigned i = 0; i < size; ++i)
		{
			values_[i] = iOther.values_[i];
		}
	}
}



template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& iOther)
{
	Matrix<T> temp(iOther);
	swap(temp);
	return *this;
}



/** return number of rows in matrix.
 *  this should never be a negative value.
 */
template <typename T>
typename Matrix<T>::TSize Matrix<T>::rows() const
{
	LASS_ASSERT(rows_ >= 0);
	return rows_;
}



/** return number of columns in matrix.
 *  this should never be a negative value.
 */
template <typename T>
typename Matrix<T>::TSize Matrix<T>::cols() const
{
	LASS_ASSERT(cols_ >= 0);
	return cols_;
}



/** return the component value at position (iRow, iColumn)
 *  (iRow, iColumn) shouldn't be out of the range [0, this->rows()) x [0, this->cols()], unless 
 *  you're asking for trouble.
 */
template <typename T>
typename Matrix<T>::TConstReference Matrix<T>::operator()(TSize iRow, TSize iCol) const
{
	return values_[flatIndex(iRow, iCol)];
}



/** access the component value at position (iRow, iColumn)
 *  (iRow, iColumn) shouldn't be out of the range [0, this->rows()) x [0, this->cols()], unless 
 *  you're asking for trouble.
 */
template <typename T>
 typename Matrix<T>::TReference Matrix<T>::operator()(TSize iRow, TSize iCol)
{
	return values_[flatIndex(iRow, iCol)];
}



/** return the component value at position (iRow, iColumn)
 *  (iRow, iColumn) will be wrapped to range [0, this->rows()) x [0, this->cols()] by using a
 *  modulus operator
 */
template <typename T>
typename Matrix<T>::TConstReference Matrix<T>::at(TSize iRow, TSize iCol) const
{
	return values_[flatIndex(num::mod(iRow, rows_), num::mod(iCol, cols_))];
}



/** access the component value at position (iRow, iColumn)
 *  (iRow, iColumn) will be wrapped to range [0, this->rows()) x [0, this->cols()] by using a
 *  modulus operator
 */
template <typename T>
typename Matrix<T>::TReference Matrix<T>::at(TSize iRow, TSize iCol)
{
	return values_[flatIndex(num::mod(iRow, rows_), num::mod(iCol, cols_))];
}



/** A weird way to get back the same object 
 */
template <typename T>
Matrix<T>& Matrix<T>::operator+() const
{
	return *this;
}



/** return a vector with all components negated
 *  (-a)(i, j) == -(a(i, j)). 
 */
template <typename T>
Matrix<T> Matrix<T>::operator-() const
{
	Matrix<T> result(rows_, cols_, false); // construct without initializing
	const TSize size = rows_ * cols_;
	for (TSize i; i < size; ++i)
	{
		result.values_[i] = -values_[i];
	}
	return result;
}



/** componentswise addition assignment of two matrices
 *
 *  <i>Matrix Addition: Denote the sum of two matrices @c A and @c B (of the same dimensions) by 
 *  @c C=A+B. The sum is defined by adding entries with the same indices @c Cij=Aij+Bij over all 
 *  @c i and @c j.</i>
 *  http://mathworld.wolfram.com/MatrixAddition.html
 *
 *  This method implements the assignment addition what reduces to @c Aij+=Bij over all @c i and 
 *  @c j.
 *
 *  @throw an exception is thrown if @a *this and @a iB are not of the same dimensions
 */
template <typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& iB)
{
	LASS_NUM_MATRIX_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TSize size = rows_ * cols_;
	for (TSize i = 0; i < size; ++i)
	{
		values_[i] += iB.values_[i];
	}
	return *this;
}



/** componentswise subtraction assignment of two matrices
 *  This method is equivalent to @c this+=(-iB) .
 *
 *  @sa Matrix<T>::operator+=
 *
 *  @throw an exception is thrown if @a *this and @a iB are not of the same dimensions
 */
template <typename T>
Matrix<T>& Matrix<T>::operator-=(const Matrix<T>& iB)
{
	LASS_NUM_MATRIX_ENFORCE_EQUAL_DIMENSION(*this, iB);
	const TSize size = rows_ * cols_;
	for (TSize i = 0; i < size; ++i)
	{
		values_[i] -= iB.values_[i];
	}
	return *this;
}



/** scalar multiplication assignment of matrix
 */
template <typename T>
Matrix<T>& Matrix<T>::operator *=(TParam iB)
{
	const TSize size = rows_ * cols_;
	for (TSize i = 0; i < size; ++i)
	{
		values_[i] *= iB;
	}
	return *this;
}



/** scalar multiplication assignment of matrix
 */
template <typename T>
Matrix<T>& Matrix<T>::operator /=(TParam iB)
{
	const TSize size = rows_ * cols_;
	for (TSize i = 0; i < size; ++i)
	{
		values_[i] /= iB;
	}
	return *this;
}



/** set this to a zero matrix of @a iRows rows and @a iCols columns.
 *  @sa Matrix<T>::isZero
 */
template <typename T>
void Matrix<T>::setZero(TSize iRows, TSize iCols)
{
	Matrix<T> temp(iRows, iCols);
	swap(temp);
}



/** set matrix to an identity matrix of size @a iSize.
 *  @sa Matrix<T>::isIdentity
 */
template <typename T>
void Matrix<T>::setIdentity(TSize iSize)
{
	Matrix<T> temp(iSize, iSize);
	for (TSize i = 0; i < iSize; ++i)
	{
		temp(i, i) = TNumTraits::one;
	}
	swap(temp);
}



/** return true if this is a (0×0) matrix
 */
template <typename T>
bool Matrix<T>::isEmpty() const
{
	return rows_ == 0 || cols_ == 0;
}



/** test if this matrix equals a zero matrix.
 *
 *  <i>A zero matrix is an @c m×n matrix consisting of all 0s (MacDuffee 1943, p. 27), denoted @c 0.  
 *  Zero matrices are sometimes also known as null matrices (Akivis and Goldberg 1972, p. 71).</i>
 *  http://mathworld.wolfram.com/ZeroMatrix.html
 */
template <typename T>
bool Matrix<T>::isZero() const
{
	const TSize size = rows_ * cols_;
	for (TSize i = 0; i < size; ++i)
	{
		if (values_[i] != TNumTraits::zero)
		{
			return false;
		}
	}
	return true;
}



/** test if this matrix equals a identity matrix
 *
 *  <i>The identity matrix is a the simplest nontrivial diagonal matrix, defined such that @c I(X)=X 
 *  for all vectors @c X. An identity matrix may be denoted @c 1, @c I, or @c E (the latter being an
 *  abbreviation for the German term "Einheitsmatrix"; Courant and Hilbert 1989, p. 7). Identity 
 *  matrices are sometimes also known as unit matrices (Akivis and Goldberg 1972, p. 71). The  
 *  @c n×n identity matrix is given explicitly by @c Iij=dij for @c i, @c j = 1, 2, ..., n, where 
 *  @c dij is the Kronecker delta.</i>
 *  http://mathworld.wolfram.com/IdentityMatrix.html
 */
template <typename T>
bool Matrix<T>::isIdentity() const
{
	if (!isSquare())
	{
		return false;
	}
	for (TSize i = 0; i < rows_; ++i)
	{
		for (TSize j = 0; j < cols_; ++j)
		{
			if ((*this)(i, j) != ((i == j) ? TNumTraits::one : TNumTraits::zero))
			{
				return false;
			}
		}
	}
	return true;
}



/** test if this matrix is an diagonal matrix
 *
 *  <i>A diagonal matrix is a square matrix @c A of the form @c Aij=Ci*dij where @c dij is the 
 *  Kronecker delta, @c Ci are constants, and @c i, @c j = 1, 2, ..., n, with is no implied 
 *  summation over indices.</i>
 *  http://mathworld.wolfram.com/DiagonalMatrix.html
 *
 *  @note both square zero matrices and identity matrices will yield true on this one.
 */
template <typename T>
bool Matrix<T>::isDiagonal() const
{
	if (!isSquare())
	{
		return false;
	}
	for (TSize i = 0; i < rows_; ++i)
	{
		for (TSize j = 0; j < cols_; ++j)
		{
			if (i != j && operator()(i, j) != TNumTraits::zero)
			{
				return false;
			}
		}
	}
	return true;
}



template <typename T>
bool Matrix<T>::isSquare() const
{
	return rows_ == cols_;
}



/** return transposed matrix
 */
template <typename T>
Matrix<T> Matrix<T>::transpose() const
{
	Matrix<T> result(cols_, rows_, false);
	for (TSize i = 0; i < rows_; ++i)
	{
		for (TSize j = 0; j < cols_; ++j)
		{
			result(j, i) = operator()(i, j);
		}
	}
	return result;
}



/** return inverse of matrix
 *
 *  If matrix has no inverse (this is singular), no exception is thrown.  Instead, an empty matrix
 *  is returned.  You can test for empty matrices with isEmpty().
 *
 *  @throw an exception is thrown if this is not a square matrix
 */
template <typename T>
Matrix<T> Matrix<T>::inverse() const
{
	LASS_ENFORCE(isSquare());
    
    Matrix<T> lu(*this);
    Matrix<size_t> index(rows_, 1);
    int d;

	if (!impl::cludecomp(lu, index, d))
    {
        return Matrix<T>(); // empty solution
    }

    Matrix<T> result;
	result.setIdentity(n);
    clusolve(lu, index, result);
    return result;
}



/** solve equation A * X = B
 *  @return true if solution is found, false if set has no solution.
 *  @throw an exception is thrown if dimensions don't match 
 *         (this->isSquare() && this->cols() == ioB.rows()) 
 */
template <typename T>
bool Matrix<T>::solve(Matrix<T>& ioB) const
{
	LASS_ENFORCE(isSquare());
	LASS_NUM_MATRIX_ENFORCE_ADJACENT_DIMENSION(*this, ioB);
    
    Matrix<T> lu(*this);
    Matrix<size_t> index(rows_, 1);
    int d;

	if (!impl::ludecomp(lu, index, d))
    {
        return false; // empty solution
    }

	impl::lusolve(lu, index, ioB);
    return true;
}



/** get a pointer to the raw matrix data (row major), use with care!
 */
template <typename T>
typename Matrix<T>::TPointer Matrix<T>::data() const
{
	return values_.get();
}



/** swap content (data and dimensions) of two matrices
 */
template <typename T>
void Matrix<T>::swap(Matrix<T>& iOther)
{
	values_.swap(iOther.values_);
	std::swap(rows_, iOther.rows_);
	std::swap(cols_, iOther.cols_);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename T>
typename Matrix<T>::TSize Matrix<T>::flatIndex(TSize iRow, TSize iCol) const
{
	LASS_ASSERT(iRow >= 0 && iRow < rows_ && iCol >= 0 && iCol < cols_);
	return iRow * cols_ + iCol;
}



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::num::Matrix
 */
template <typename T>
bool operator==(const Matrix<T>& iA, const Matrix<T>& iB)
{
	if (iA.rows() != iB.rows() || iA.cols() != iB.cols())
	{
		return false;
	}
	typedef typename Matrix<T>::TSize TSize;
	const TSize size = iA.rows() * iA.cols();
	const Matrix<T>::TPointer a = iA.data();
	const Matrix<T>::TPointer b = iB.data();
	for (TSize i = 0; i < size; ++i)
	{
		if (a[i] != b[i])
		{
			return false;
		}
	}
	return true;
}



/** @relates lass::num::Matrix
 */
template <typename T>
bool operator!=(const Matrix<T>& iA, const Matrix<T>& iB)
{
	return !(iA == iB)
}



/** Matrix Addtion
 *  @relates lass::num::Matrix
 *  @sa Matrix<T>::operator+=
 */
template <typename T>
Matrix<T> operator+(const Matrix<T>& iA, const Matrix<T>& iB)
{
	Matrix<T> result(iA);
	result += iB;
	return result;
}



/** Matrix Subtraction
 *  @relates lass::num::Matrix
 *  @sa Matrix<T>::operator-=
 */
template <typename T>
Matrix<T> operator-(const Matrix<T>& iA, const Matrix<T>& iB)
{
	Matrix<T> result(iA);
	result -= iB;
	return result;
}



/** @relates lass::num::Matrix
 *  @sa Matrix<T>::operator*=
 */
template <typename T>
Matrix<T> operator*(const Matrix<T>& iA, const T& iB)
{
	Matrix<T> result(iA);
	result *= iB;
	return result;
}



/** @relates lass::num::Matrix
 *  @sa Matrix<T>::operator/=
 */
template <typename T>
Matrix<T> operator/(const Matrix<T>& iA, const T& iB)
{
	Matrix<T> result(iA);
	result /= iB;
	return result;
}



/** @relates lass::num::Matrix
 *  @sa Matrix<T>::operator*=
 */
template <typename T>
Matrix<T> operator*(const T& iA, const Matrix<T>& iB)
{
	Matrix<T> result(iB);
	result *= iA;
	return result;
}



/** Matrix multiplication
 *  @relates lass::num::Matrix
 *
 *  <i>The product @c C of two matrices @c A and @c B is defined by @c Cik=Aij*Bjk, where @c j is
 *  summed over for all possible values of @c i and @c k. The implied summation over repeated 
 *  indices without the presence of an explicit sum sign is called Einstein summation, and is 
 *  commonly used in both matrix and tensor analysis. Therefore, in order for matrix multiplication
 *  to be defined, the dimensions of the matrices must satisfy @c (n×m)*(m×p)=(n×p) where @c (a×b)
 *  denotes a matrix with @c a rows and @c b columns.</i>
 *  http://mathworld.wolfram.com/MatrixMultiplication.html
 *
 *  @throw an exception is throw in @a iA and @a iB don't meet the requirement 
 *         @c iA.cols()==iB.rows() .
 */
template <typename T>
Matrix<T> operator*(const Matrix<T>& iA, const Matrix<T>& iB)
{
	LASS_NUM_MATRIX_ENFORCE_ADJACENT_DIMENSION(iA, iB);
	typedef typename Matrix<T>::TSize TSize;
	typedef typename Matrix<T>::TNumTraits TNumTraits;
	const TSize n = iA.rows();
	const TSize m = iA.cols();
	const TSize p = iB.cols();

	Matrix<T> result(n, p, false);
	for (TSize i = 0; i < n; ++i)
	{
		for (TSize k = 0; i < p; ++i)
		{
			result(i, k) = TNumTraits::zero;
			for (TSize j = 0; j < m; ++i)
			{
				result(i, k) += iA(i, j) * iB(j, k);
			}
		}
	}
	return result;
}



/** @relates lass::num::Matrix
 */
template <typename T>
std::ostream& operator<<(std::ostream& oOStream, const Matrix<T>& iB)
{
	typedef typename Matrix<T>::TSize TSize;
	const TSize m = iB.rows();
	const TSize n = iB.cols();

	LASS_ENFORCE_STREAM(oOStream) << "(";
	for (TSize i = 0; i < m; ++i)
	{
		if (i != 0)
		{
			LASS_ENFORCE_STREAM(oOStream) << ", ";
		}
		LASS_ENFORCE_STREAM(oOStream) << "(";
		if (n > 0)
		{
			LASS_ENFORCE_STREAM(oOStream) << iB(i, 0);
		}
		for (TSize j = 1; j < n; ++j)
		{
			LASS_ENFORCE_STREAM(oOStream) << ", " << iB(i, j);
		}
		LASS_ENFORCE_STREAM(oOStream) << ")";
	}
	LASS_ENFORCE_STREAM(oOStream) << ")";
	return oOStream;
}



}

}

#endif

// EOF
