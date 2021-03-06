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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_IMPL_MATRIX_SOLVE_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_IMPL_MATRIX_SOLVE_H

#include "../num_common.h"

#include <cstddef>

namespace lass
{
namespace num
{
namespace impl
{

template 
<
	typename T, 
	typename RandomIterator1, 
	typename RandomIterator2
>
bool ludecomp(RandomIterator1 ioMatrix,
			  RandomIterator2 oIndex,
			  std::ptrdiff_t iSize,
			  int& iD);



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
			 std::ptrdiff_t iSize);



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
			  std::ptrdiff_t iSize);



template <typename T, typename RandomIterator1,	typename RandomIterator2> 
bool cramer2(RandomIterator1 iMatrixRowMajor, 
			 RandomIterator2 ioColumnFirst, RandomIterator2 ioColumnLast);

template <typename T, typename RandomIterator1,	typename RandomIterator2> 
bool cramer3(RandomIterator1 iMatrixRowMajor,
			 RandomIterator2 ioColumnFirst, RandomIterator2 ioColumnLast);




template <typename T, typename RandomIterator1, typename RandomIterator2, typename RandomIterator3>
bool solveTridiagonal(RandomIterator1 iA_1, RandomIterator1 iB_0, RandomIterator1 iC_0,
					  RandomIterator2 ioSolution, RandomIterator3 ioTemp, std::ptrdiff_t iSize);

}

}

}

#include "matrix_solve.inl"

#endif

// EOF
