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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_VECTOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_VECTOR_INL

#include "num_common.h"
#include "matrix_vector.h"

namespace lass
{
namespace num
{

#define LASS_NUM_MATRIX_VECTOR_ENFORCE_ADJACENT_DIMENSION(a, b)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
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
void solve(const Matrix<T, S>& iA, Vector<T>& iB)
{
	typedef impl::MVColumn<T, typename Vector<T>::TStorage > TAdaptor;
	Matrix<T, TAdaptor> adaptedB(TAdaptor(iB.storage()));
	solve(iA, adaptedB);
}



}

}

#endif

// EOF
