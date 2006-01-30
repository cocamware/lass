/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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

// --- public --------------------------------------------------------------------------------------



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** Given a complex matrix iA, this routine replaces it by the LU decomposition
 *  of a rowwise permutation of itself.
 *  @param ioMatrix
 *		- INPUT and OUTPUT.
 *		- Random iterator to first element of of a @e square matrix in row major order.
 *      - ioMatrix is replaced by its LU decomposition.
 *		- [ioMatrix, ioMatrix + iSize * iSize) must be a valid range
 *  @param oIndex 
 *		- OUTPUT.
 *      - records the row permutations effected by the partial pivoting.
 *		- [oIndex, oIndex + iSize) must be a valid range.
 *	@param iSize
 *		- INPUT
 *		- size of matrix
 *  @param oD     
 *		- OUTPUT.
 *      - indicates the number of row interchanges was even (+1) or odd (-1).
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
 *  @param iMatrix     
 *		- INPUT.
 *		- Random iterator to first element of A in row major order
 *      - A as LU decomposition.
 *      - A must be square.
 *		- [iMatrix, iMatrx + iSize * iSize) must be a valid range
 *  @param iIndex 
 *		- INPUT 
 *		- permutation sequence filled by ludecomp().
 *      - [iIndex, iIndex + iSize) must be a valid range
 *  @param ioColumn    
 *		- INPUT and OUTPUT
 *      - as input, it is the righthand side vector B.
 *      - as output it is the solution vector X.
 *      - must be of same length as iA
 *      - [ioColumn, ioColumn + iSize) must be a valid range
 *	@param iSize
 *		- INPUT
 *		- size of matrix
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
 *  @param iMatrix     
 *		- INPUT
 *		- Random iterator to first element of matrix in row major order.
 *      - must be square.
 *		- [iMatrix, iMatrix + iSize * iSize) must be a valid range
 *  @param iMatrixLU   
 *		- INPUT
 *      - LU decompostion of A
 *      - must be square.
 *		- [iMatrixLU, iMatrixLU + iSize * iSize) must be a valid range
 *  @param iIndex 
 *		- INPUT
 *		- [iIndex, iIndex + iSize) must be a valid range
 *  @param iColumn 
 *		- INPUT
 *		- [iColumn, iColumn + iSize) must be a valid range
 *  @param ioX 
 *		- INPUT and OUTPUT
 *		- [ioX, ioX + iSize) must be a valid range
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

	lusolve(iMatrixLU, iIndex, r.begin(), iSize);

	for (i = 0; i < iSize; ++i)
	{
		ioX[i] -= r[i];
	}
}



/** Solve A X = B for 2x2 matrices with Cramer's rule.
 *  @param iMatrix     
 *		- INPUT
 *		- Random iterator to first element of matrix A in row major order.
 *      - A must be of size 2x2.
 *		- [iMatrix, iMatrix + 4) must be a valid range
 *  @param ioColumn 
 *		- INPUT and OUTPUT
 *		- Random iterator to first element of B as input
 *		- Random iterator to first element of X as output
 *		- [iColumn, iColumn + 2) must be a valid range
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
 *  @param iMatrix     
 *		- INPUT
 *		- Random iterator to first element of matrix A in row major order.
 *      - A must be of size 3x3.
 *		- [iMatrix, iMatrix + 9) must be a valid range
 *  @param ioColumn 
 *		- INPUT and OUTPUT
 *		- Random iterator to first element of B as input
 *		- Random iterator to first element of X as output
 *		- [iColumn, iColumn + 3) must be a valid range
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

}

}

}

#endif

// EOF
