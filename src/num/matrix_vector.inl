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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_VECTOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_VECTOR_INL

#include "num_common.h"
#include "matrix_vector.h"

namespace lass
{
namespace num
{

#define LASS_NUM_MATRIX_VECTOR_ENFORCE_ADJACENT_DIMENSION(iA, iB)\
	(*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
									 lass::util::impl::DefaultRaiser>\
	((iA).cols() == (iB).dimension(), "Matrix '" LASS_STRINGIFY(iA) "' and Vector '"\
	 LASS_STRINGIFY(iB) "' have no adjacent dimensions for multiplication in '" LASS_HERE "'."))

	
	
// --- free ----------------------------------------------------------------------------------------

/** multiply matrix with column vector
 *  @relates lass::num::Matrix
 *  @relatesalso lass::num::Vector
 */
template <typename T> Vector<T> operator*(const Matrix<T>& iA, const Vector<T>& iB)
{
	typedef typename Matrix<T>::TNumTraits TNumTraits;

	LASS_NUM_MATRIX_VECTOR_ENFORCE_ADJACENT_DIMENSION(iA, iB);
	typedef typename Matrix<T>::TSize TSize;
	const TSize m = iA.rows();
	const TSize n = iA.cols();

	Vector<T> result(m, true);
	for (TSize i = 0; i < m; ++i)
	{
		result[i] = TNumTraits::zero;
		for (TSize j = 0; j < n; ++j)
		{
			result[i] += iA(i, j) * iB[j];
		}
	}
	return result;
}



/** Create diagonal matrix from vector
 *  @relates lass::num::Matrix
 *  @relatesalso lass::num::Vector
 *  @sa lass::num::Matrix::isDiagonal
 */
template <typename T>
Matrix<T> diagonal(const Vector<T>& iB)
{
	typedef typename Vector<T>::TSize TSize;
	const TSize size = iB.dimension();

	Matrix<T> result(size, size);
	for (TSize i = 0; i < size; ++i)
	{
		result(i, i) = iB[i];
	}
	return result;
}



/** Solves set of equation A * X == B
 *  @relates lass::num::Matrix
 *  @relatesalso lass::num::Vector
 *  @sa lass::num::Matrix::solve
 */
template <typename T>
bool solve(const Matrix<T>& iA, Vector<T>& iB)
{
	typedef typename Matrix<T>::TSize TSize;
	const TSize size = iB.dimension();
	
	Matrix<T> temp(size, 1, false);
	TSize i;
	for (i = 0; i < size; ++i)
	{
		temp(i, 0) = iB[i];
	}

	if (!iA.solve(temp))
	{
		return false;
	}

	for (i = 0; i < size; ++i)
	{
		iB[i] = temp(i, 0);
	}
	return true;
}



}

}

#endif

// EOF
