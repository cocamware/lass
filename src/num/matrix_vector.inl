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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_VECTOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_VECTOR_INL

#include "num_common.h"
#include "matrix_vector.h"

namespace lass
{
namespace num
{

#define LASS_NUM_MATRIX_VECTOR_ENFORCE_ADJACENT_DIMENSION(a, b)\
	LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::EqualPredicate,\
		::lass::util::impl::DefaultRaiser,\
		(a).columns(), \
		(b).size(), \
		"Matrix '" LASS_STRINGIFY(a) "' and Vector '" LASS_STRINGIFY(b) "' have no adjacent dimensions for multiplication in '" LASS_HERE "'.")



// --- free ----------------------------------------------------------------------------------------

/** multiply matrix with column vector
 *  @relates lass::num::Matrix
 *  @relatesalso lass::num::Vector
 */
template <typename T, typename S1, typename S2>
Vector<T, impl::MVRightProd<T, S1, S2> >
operator*(const Matrix<T, S1>& iA, const Vector<T, S2>& iB)
{
	LASS_NUM_MATRIX_VECTOR_ENFORCE_ADJACENT_DIMENSION(iA, iB);
	typedef impl::MVRightProd<T, S1, S2> TExpression;
	return Vector<T, TExpression>(TExpression(iA.storage(), iB.storage()));
}



/** Create diagonal matrix from vector
 *  @relates lass::num::Matrix
 *  @relatesalso lass::num::Vector
 *  @sa lass::num::Matrix::isDiagonal
 */
template <typename T, typename S>
Matrix<T, impl::MVDiag<T, S> > diagonal(const Vector<T, S>& iB)
{
	typedef impl::MVDiag<T, S> TExpression;
	return Matrix<T, TExpression>(TExpression(iB.storage()));
}



/** Solves set of equation A * X == B
 *  @relates lass::num::Matrix
 *  @relatesalso lass::num::Vector
 *  @sa lass::num::Matrix::solve
 */
template <typename T, typename S>
bool solve(const Matrix<T, S>& iA, Vector<T>& iB)
{
	typedef impl::MVColumn<T, typename Vector<T>::TStorage > TAdaptor;
	Matrix<T, TAdaptor> adaptedB(TAdaptor(iB.storage()));
	return solve(iA, adaptedB);
}



}

}

#endif

// EOF
