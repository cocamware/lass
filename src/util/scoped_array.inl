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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SCOPED_ARRAY_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SCOPED_ARRAY_INL



#include "scoped_array.h"



namespace lass
{

namespace util
{

/** Constructor.
 *  Construct a scoped pointer, taking a copy of the iPointee pointer which must be
 */
template <typename T>
ScopedArray<T>::ScopedArray( T* iPointee ):
	pointee_(iPointee)
{
}



/** Destructor.
 *  Destroys the object pointed to by the stored pointer, if any, as if by using
 *  delete this->pointee().
 */
template <typename T>
ScopedArray<T>::~ScopedArray()
{
	delete [] pointee_;
	pointee_ = 0;
}



/** Reset the scoped pointer.
 *  Deletes the object pointed to by the stored pointer and then stores a copy of p,
 *  which must have been allocated via a C++ new expression or be 0.
 */
template <typename T>
void ScopedArray<T>::reset( T* iPointee )
{
	LASS_ASSERT(pointee_ == 0 || pointee_ != iPointee); // catch self-reset
	ScopedArray<T> temp(iPointee);
	swap(temp);
}



template <typename T>
T& ScopedArray<T>::operator[]( unsigned iIndex ) const
{
	LASS_ASSERT(pointee_ != 0);
	return pointee_[iIndex];
}



template <typename T>
T* const ScopedArray<T>::get() const
{
	return pointee_;
}



template <typename T>
const bool ScopedArray<T>::isEmpty() const
{
	return pointee_ == 0;
}



template <typename T>
void ScopedArray<T>::swap( ScopedArray<T>& iOther )
{
	T* temp = pointee_;
	pointee_ = iOther.pointee_;
	iOther.pointee_ = temp;
}



template <typename T>
void swap( ScopedArray<T>& iA, ScopedArray<T>& iB )
{
	iA.swap(iB);
}



}

}

#endif
