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

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_VECTOR_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_VECTOR_H

#include "num_common.h"
#include "matrix.h"
#include "vector.h"

namespace lass
{
namespace num
{

template <typename T> Vector<T> operator*(const Matrix<T>& iA, const Vector<T>& iB);
template <typename T> Matrix<T> diagonal(const Vector<T>& iB);

}

}

#include "matrix_vector.inl"

#endif

// EOF
