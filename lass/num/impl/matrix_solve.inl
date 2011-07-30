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
 *	Copyright (C) 2004-2010 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_IMPL_MATRIX_SOLVE_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_IMPL_MATRIX_SOLVE_INL

#include "../num_common.h"
#include "matrix_solve.h"

namespace lass
{
namespace num
{
namespace impl
{

/** Given a complex matrix iA, this routine replaces it by the LU decomposition
 *  of a rowwise permutation of itself.
 *  @param ioMatrix [in,out]
 *		@arg Random iterator to first element of of a @e square matrix in row major order.
 *      @arg ioMatrix is replaced by its LU decomposition.
 *		@arg [ioMatrix, ioMatrix + iSize * iSize) must be a valid range
 *  @param oIndex [out]
 *      @arg records the row permutations effected by the partial pivoting.
 *		@arg [oIndex, oIndex + iSize) must be a valid range.
 *	@param iSize [in]
 *		@arg size of matrix
 *  @param oD [out]
 *      @arg indicates the number of row interchanges was even (+1) or odd (-1).
 *  @return - true: LU decomposition completed
 *          - false: matrix ioMatrix is singular
 *
 *  This routine is used in combination with lusolve() to solve linear
 *  equations and lumprove() to improve the solution.
 *
 *  Method: Crout's algorithm with partial pivoting.
 */
template 
<
	typename T, 
	typename RandomIterator1, 
	typename RandomIterator2
>
bool ludecomp(RandomIterator1 ioMatrix,
			  RandomIterator2 oIndex,
			  size_t iSize,
			  int& iD)
{
	typedef NumTraits<T> TNumTraits;
	typedef typename TNumTraits::baseType TBase;
	typedef NumTraits<TBase> TBaseTraits;

	const TBase epsilon = static_cast<TBase>(1e-20);

	std::vector<T> vv(iSize);
	iD = 1;

	for (size_t i = 0; i < iSize; ++i)
	{
		RandomIterator1 rowI = ioMatrix + (i * iSize);
		TBase normMax = TBaseTraits::zero;
		size_t jMax = iSize;
		for (size_t j = 0; j < iSize; ++j)
		{
			const TBase temp = num::norm(rowI[j]);
			if (temp > normMax)
			{
				normMax = temp;
				jMax = j;
			}
		}
		if (normMax == TBaseTraits::zero)
		{
			return false; // no decomposition
		}
		LASS_ASSERT(jMax != iSize);

		vv[i] = num::conj(rowI[jMax]) / normMax;
	}

	for (size_t j = 0; j < iSize; ++j)
	{
		size_t i;
		for (i = 0; i < j; ++i)
		{
			RandomIterator1 rowI = ioMatrix + (i * iSize);
			T sum = rowI[j];
			for (size_t k = 0; k < i; ++k)
			{
				sum -= rowI[k] * ioMatrix[k * iSize + j];
			}
			rowI[j] = sum;
		}

		TBase normMax = TBaseTraits::zero;
		size_t iMax = iSize;
		for (i = j; i < iSize; ++i)
		{
			RandomIterator1 rowI = ioMatrix + (i * iSize);
			T sum = rowI[j];
			for (size_t k = 0; k < j; ++k)
			{
				sum -= rowI[k] * ioMatrix[k * iSize + j];
			}
			rowI[j] = sum;

			const T dum = vv[i] * sum;
			const TBase temp = num::norm(dum);
			if (temp >= normMax)
			{
			   normMax = temp;
			   iMax = i;
			}
		}
		LASS_ASSERT(iMax != iSize);

		if (j != iMax)
		{
		   for (size_t k = 0; k < iSize; ++k)
		   {
			   std::swap(ioMatrix[iMax * iSize + k], ioMatrix[j * iSize + k]);
		   }
		   iD = -iD;
		   vv[iMax] = vv[j];
		}
		oIndex[j] = iMax;

		TBase temp = num::norm(ioMatrix[j * iSize + j]);
		if (temp == TBaseTraits::zero)
		{
			ioMatrix[j * iSize + j] = TNumTraits::one;//CT(T(1), T(1));
			temp = epsilon;
		}

		if (j != iSize - 1)
		{
			const T dum = num::conj(ioMatrix[j * iSize + j]) / temp;
			for (i = j + 1; i < iSize; ++i)
			{
				ioMatrix[i * iSize + j] *= dum;
			}
		}
	}

	return true;
}



/** Solves the set of linear eqautions A X = B.
 *  @internal
 *  @param iMatrix [in] 
 *		@arg Random iterator to first element of A in row major order
 *      @arg A as LU decomposition.
 *      @arg A must be square.
 *		@arg [iMatrix, iMatrx + iSize * iSize) must be a valid range
 *  @param iIndex [in]  
 *		@arg permutation sequence filled by ludecomp().
 *      @arg [iIndex, iIndex + iSize) must be a valid range
 *  @param ioColumn [in,out]     
 *      @arg as input, it is the righthand side vector B.
 *      @arg as output it is the solution vector X.
 *      @arg must be of same length as iA
 *      @arg [ioColumn, ioColumn + iSize) must be a valid range
 *	@param iSize [in]
 *		@arg size of matrix
 *
 *  This routine can be used for successive calls with different right-hand
 *  sides B.
 *
 *  Method: backward and forward substitution, taking into account the leading
 *          zero's in B.
 */
template 
<
	typename T, 
	typename RandomIterator1, 
	typename RandomIterator2,
	typename RandomIterator3
>
void lusolve(RandomIterator1 iMatrix,
			 RandomIterator2 iIndex,
			 RandomIterator3 ioColumn,
			 size_t iSize)
{
	typedef NumTraits<T> TNumTraits;

	size_t ii = iSize;
	for (size_t i = 0; i < iSize; ++i)
	{
		RandomIterator1 rowI = iMatrix + (i * iSize);
		const size_t ip = iIndex[i];
		T sum = ioColumn[ip];
		ioColumn[ip] = ioColumn[i];

		if (ii != iSize)
		{
			for (size_t j = ii; j < i; ++j)
			{
				sum -= rowI[j] * ioColumn[j];
			}
		}
		else
		{
			if (!(sum == TNumTraits::zero)) // BUG in STL???
			{
				ii = i;
			}
		}
		ioColumn[i] = sum;
	}

	for (size_t ic = iSize; ic > 0; --ic)
	{
		const size_t i = ic - 1;
		RandomIterator1 rowI = iMatrix + (i * iSize);
		T sum = ioColumn[i];
		for (size_t j = ic; j < iSize; ++j)
		{
			sum -= rowI[j] * ioColumn[j];
		}
		ioColumn[i] = sum / rowI[i];
	}
}



/** Improves a solution vector X of the linear set of equations A X = B.
 *  @internal
 *  @param iMatrix [in]    
 *		@arg Random iterator to first element of matrix in row major order.
 *      @arg must be square.
 *		@arg [iMatrix, iMatrix + iSize * iSize) must be a valid range
 *  @param iMatrixLU [in]    
 *      @arg LU decompostion of A
 *      @arg must be square.
 *		@arg [iMatrixLU, iMatrixLU + iSize * iSize) must be a valid range
 *  @param iIndex [in]  
 *		@arg [iIndex, iIndex + iSize) must be a valid range
 *  @param iColumn [in]  
 *		@arg [iColumn, iColumn + iSize) must be a valid range
 *  @param ioX [in,out] 
 *		@arg [ioX, ioX + iSize) must be a valid range
 *  @param iSize [in]
 *		@arg size of matrix
 */
template
<
	typename T,
	typename RandomIterator1,
	typename RandomIterator2,
	typename RandomIterator3,
	typename RandomIterator4,
	typename RandomIterator5
>
void lumprove(RandomIterator1 iMatrix,
			  RandomIterator2 iMatrixLU,
			  RandomIterator3 iIndex,
			  RandomIterator4 iColumn,
			  RandomIterator5 ioX,
			  size_t iSize)
{
	typedef NumTraits<T> TNumTraits;

	std::vector<T> r(iSize);
	size_t i;
	for (i = 0; i < iSize; ++i)
	{
		RandomIterator1 rowI = iMatrix + (i * iSize);
		r[i] = -iColumn[i];
		for (size_t j = 0; j < iSize; ++j)
		{
			r[i] += rowI[j] * ioX[j];
		}
	}

	lusolve<T>(iMatrixLU, iIndex, r.begin(), iSize);

	for (i = 0; i < iSize; ++i)
	{
		ioX[i] -= r[i];
	}
}



/** Solve A X = B for 2x2 matrices with Cramer's rule.
 *  @param iMatrix [in]    
 *		@arg Random iterator to first element of matrix A in row major order.
 *      @arg A must be of size 2x2.
 *		@arg [iMatrix, iMatrix + 4) must be a valid range
 *  @param ioColumn [in,out]
 *		@arg Random iterator to first element of B as input
 *		@arg Random iterator to first element of X as output
 *		@arg [iColumn, iColumn + 2) must be a valid range
 */
template 
<
	typename T,
	typename RandomIterator1,
	typename RandomIterator2
> 
bool cramer2(RandomIterator1 iMatrixRowMajor, 
			 RandomIterator2 ioColumnFirst,
			 RandomIterator2 ioColumnLast)
{
	typedef NumTraits<T> TNumTraits;
	RandomIterator1 m = iMatrixRowMajor; // shortcut ;)

	const T determinant = m[0] * m[3] - m[1] * m[2];

	if (determinant == TNumTraits::zero)
	{
		return false;
	}

	const T inverseDeterminant = num::inv(determinant);

	for (RandomIterator2 column = ioColumnFirst; column != ioColumnLast; column += 2)
	{
		LASS_ASSERT(column + 1 != ioColumnLast);
        const T x = column[0];
		const T y = column[1];		
		column[0] = inverseDeterminant * (x * m[3] - y * m[2]);
		column[1] = inverseDeterminant * (m[0] * y - m[1] * x);
	}
	return true;
}



/** Solve A X = B for 3x3 matrices with Cramer's rule.
 *  @internal
 *  @param iMatrix [in]
 *		@arg Random iterator to first element of matrix A in row major order.
 *      @arg A must be of size 3x3.
 *		@arg [iMatrix, iMatrix + 9) must be a valid range
 *  @param ioColumn [in,out]
 *		@arg Random iterator to first element of B as input
 *		@arg Random iterator to first element of X as output
 *		@arg [iColumn, iColumn + 3) must be a valid range
 */
template 
<
	typename T,
	typename RandomIterator1,
	typename RandomIterator2
> 
bool cramer3(RandomIterator1 iMatrixRowMajor, 
			 RandomIterator2 ioColumnFirst,
			 RandomIterator2 ioColumnLast)
{
	typedef NumTraits<T> TNumTraits;
	RandomIterator1 m = iMatrixRowMajor; // shortcut ;)

	const T determinant = 
		m[0] * (m[4] * m[8] - m[7] * m[5]) +
		m[3] * (m[7] * m[2] - m[1] * m[8]) +
		m[6] * (m[1] * m[5] - m[4] * m[2]);
	
	if (determinant == TNumTraits::zero)
	{
		return false;
	}

	const T inverseDeterminant = num::inv(determinant);

	for (RandomIterator2 column = ioColumnFirst; column != ioColumnLast; column += 3)
	{
		LASS_ASSERT(column + 1 != ioColumnLast && column + 2 != ioColumnLast);
		const T x = column[0];
		const T y = column[1];
		const T z = column[2];
		column[0] = inverseDeterminant * (
			x * (m[4] * m[8] - m[7] * m[5]) +
			y * (m[7] * m[2] - m[1] * m[8]) +
			z * (m[1] * m[5] - m[4] * m[2]));
		column[1] = inverseDeterminant * (
			m[0] * (y * m[8] - z * m[5]) +
			m[3] * (z * m[2] - x * m[8]) +
			m[6] * (x * m[5] - y * m[2]));
		column[2] = inverseDeterminant * (
			m[0] * (m[4] * z - m[7] * y) +
			m[3] * (m[7] * x - m[1] * z) +
			m[6] * (m[1] * y - m[4] * x));
	}

	return true;
}



/** Solve system of linear equations with a tridiagonal matrix.
 *  @internal
 *  @param iA [in]
 *		@arg Random iterator to first element of lower diagonal.
 *      @arg This is actually the matrix element in the "zeroth" column of the first row.
 *			So, iA[0] is being unused since it's not a valid matrix position.  The first
 *			valid element is iA[1]
 *		@arg [iA, iA + N) must be a valid range
 *  @param iB [in]
 *		@arg Random iterator to first element of main diagonal.
 *      @arg This is actually the matrix element in the first column of the first row.
 *		@arg [iB, iB + N) must be a valid range
 *  @param iC [in]
 *		@arg Random iterator to first element of upper diagonal.
 *      @arg This is actually the matrix element in the second column of the first row.
 *		@arg [iC, iC + N) must be a valid range
 *		@arg The last element iC[N - 1] isn't really a valid element since it's a position
 *			outside the matrix.
 */
template 
<
	typename T, 
	typename RandomIterator1, 
	typename RandomIterator2,
	typename RandomIterator3
>
bool solveTridiagonal(RandomIterator1 iA, RandomIterator1 iB, RandomIterator1 iC,
					  RandomIterator2 ioSolution, RandomIterator3 ioTemp,
					  std::size_t iSize)
{
	typedef NumTraits<T> TNumTraits;

	T beta = iB[0];
	if (beta == TNumTraits::zero)
	{
		return false;
	}
	ioSolution[0] /= beta;
	for (std::size_t i = 1; i <iSize; ++i)
	{
		ioTemp[i] = iC[i - 1] / beta;
		beta = iB[i] - iA[i] * ioTemp[i];
		if (beta == TNumTraits::zero)
		{
			return false;
		}
		ioSolution[i] = (ioSolution[i] - iA[i] * ioSolution[i - 1]) / beta;
	}
	for (std::size_t i = iSize - 1; i > 0; --i)
	{
		ioSolution[i - 1] -= ioTemp[i] * ioSolution[i];
	}
	return true;
}

}

}

}

#endif

// EOF
