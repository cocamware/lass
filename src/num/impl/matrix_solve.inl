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
 *  @param ioA    - INPUT and OUTPUT.
 *                - iA is replaced by its LU decomposition.
 *                - must be square.
 *  @param oIndex - OUTPUT.
 *                - records the row permutations effected by the partial pivoting.
 *  @param oD     - OUTPUT.
 *                - indicates the number of row interchanges was even (+1) or odd (-1).
 *  @return - true: LU decomposition completed
 *          - false: matrix iA is singular
 *
 *  This routine is used in combination with clusolve() to solve linear
 *  equations and clumprove() to improve the solution.
 *
 *  Method: Crout's algorithm with partial pivoting.
 */
template <typename T>
bool ludecomp(Matrix<T>& ioA,
			  std::vector<size_t>& oIndex,
			  int& iD)
{
	LASS_ASSERT(ioA.isSquare());

	typedef typename Matrix<T>::TSize TSize;
	typedef typename Matrix<T>::TValue TValue;
	typedef typename Matrix<T>::TNumTraits TNumTraits;
	typedef typename TNumTraits::baseType TBase;
	typedef NumTraits<TBase> TBaseTraits;

	const TBase epsilon = static_cast<TBase>(1e-20);
	const TSize n = ioA.rows();

	oIndex.resize(n);
	std::vector<T> vv(n);
	iD = 1;

	for (TSize i = 0; i < n; ++i)
	{
		TBase normMax = TBaseTraits::zero;
		TSize jMax = n;
		for (TSize j = 0; j < n; ++j)
		{
			const TBase temp = num::norm(ioA(i, j));
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
		LASS_ASSERT(jMax != n);

		vv[i] = num::conj(ioA(i, jMax)) / normMax;
	}

	for (TSize j = 0; j < n; ++j)
	{
		TSize i;
		for (i = 0; i < j; ++i)
		{
			TValue sum = ioA(i, j);
			for (TSize k = 0; k < i; ++k)
			{
				sum -= ioA(i, k) * ioA(k, j);
			}
			ioA(i, j) = sum;
		}

		TBase normMax = TBaseTraits::zero;
		TSize iMax = n;
		for (i = j; i < n; ++i)
		{
			TValue sum = ioA(i, j);
			for (TSize k = 0; k < j; ++k)
			{
				sum -= ioA(i, k) * ioA(k, j);
			}
			ioA(i, j) = sum;

			const TValue dum = vv[i] * sum;
			const TBase temp = num::norm(dum);
			if (temp >= normMax)
			{
			   normMax = temp;
			   iMax = i;
			}
		}
		LASS_ASSERT(iMax != n);

		if (j != iMax)
		{
		   for (TSize k = 0; k < n; ++k)
		   {
			   std::swap(ioA(iMax, k), ioA(j, k));
		   }
		   iD = -iD;
		   vv[iMax] = vv[j];
		}
		oIndex[j] = iMax;

		TBase temp = num::norm(ioA(j, j));
		if (temp == TBaseTraits::zero)
		{
			ioA(j, j) = TNumTraits::one;//CT(T(1), T(1));
			temp = epsilon;
		}

		if (j != n - 1)
		{
			const TValue dum = num::conj(ioA(j, j)) / temp;
			for (i = j + 1; i < n; ++i)
			{
				ioA(i, j) *= dum;
			}
		}
	}

	return true;
}



/** Solves the set of linear eqautions A.X = B.
 *  @param iA     - INPUT.
 *                - A as LU decomposition.�
 *                - must be square.
 *  @param iIndex - INPUT permutation vector returned by cludecomp().
 *                - must be of same dimension as iA
 *  @param ioB    - INPUT and OUTPUT
 *                - as input, it is the righthand side vector B.
 *                - as output it is the solution vector X.
 *                - must be of same dimension as iA
 *
 *  This routine can be used for successive calls with different right-hand
 *  sides B.
 *
 *  Method: backward and forward substitution, taking into account the leading
 *          zero's in B.
 */
template <typename T, typename S1, typename S2>
void lusolve(const Matrix<T, S1>& iA,
			 const std::vector<size_t>& iIndex,
			 Matrix<T, S2>& ioB)
{
	LASS_ASSERT(iA.isSquare());
	LASS_ASSERT(iIndex.size() == iA.rows());
	LASS_ASSERT(ioB.rows() == iA.rows());

	typedef typename Matrix<T, S1>::TSize TSize;
	typedef typename Matrix<T, S1>::TValue TValue;
	typedef typename Matrix<T, S1>::TNumTraits TNumTraits;

	const TSize n = iA.rows();
	const TSize m = ioB.columns();

	for (TSize k = 0; k < m; ++k)
	{
		TSize ii = n;
		for (TSize i = 0; i < n; ++i)
		{
			const TSize ip = iIndex[i];
			TValue sum = ioB(ip, k);
			ioB(ip, k) = ioB(i, k);

			if (ii != n)
			{
				for (TSize j = ii; j < i; ++j)
				{
					sum -= iA(i, j) * ioB(j, k);
				}
			}
			else
			{
				if (!(sum == TNumTraits::zero)) // BUG in STL???
				{
					ii = i;
				}
			}
			ioB(i, k) = sum;
		}

		for (TSize ic = n; ic > 0; --ic)
		{
			const TSize i = ic - 1;
			TValue sum = ioB(i, k);
			for (TSize j = ic; j < n; ++j)
			{
				sum -= iA(i, j) * ioB(j, k);
			}
			ioB(i, k) = sum / iA(i, i);
		}
	}
}



/** Improves a solution vector X of the linear set of equations A.X = B.
 *  @param iA     - INPUT
 *                - must be square.
 *  @param iAlu   - INPUT
 *                - LU decompostion of iA
 *                - must be square.
 *  @param iIndex - INPUT
 *                - must be of same dimension as iA
 *  @param iB     - INPUT
 *                - must be of same dimension as iA
 *  @param ioX    - INPUT and OUTPUT
 *                - modified to an improved set of values.
 *                - must be of same dimension as iA
 */
template <typename T>
void lumprove(const Matrix<T>& iA,
			  const Matrix<T>& iAlu,
			  const std::vector<size_t>& iIndex,
			  const Matrix<T>& iB,
			  Matrix<T>& ioX)
{
	LASS_ASSERT(iA.isSquare());
	LASS_ASSERT(iA.rows() == iAlu.rows() && iA.cols() == iAlu.cols());
	LASS_ASSERT(iIndex.size() == iA.rows())
	LASS_ASSERT(iB.rows() == iA.rows());
	LASS_ASSERT(ioX.rows() == iA.rows() && ioX.cols() == iB.cols());

	typedef typename Matrix<T>::TSize TSize;
	typedef typename Matrix<T>::TValue TValue;
	typedef typename Matrix<T>::TNumTraits TNumTraits;

	const TSize n = iA.rows();
	const TSize m = iB.cols();

	Matrix<T> r(n, m, false);
	TSize i;
	for (i = 0; i < n; ++i)
	{
		for (TSize k = 0; k < m; ++k)
		{
			r(i, k) = -iB(i, k);
			for (TSize j = 0; j < n; ++j)
			{
				r(i, k) += iA(i, j) * ioX(j, k);
			}
		}
	}

	clusolve(iAlu, iIndex, r);

	ioX -= r;
}



}

}

}

#endif

// EOF
