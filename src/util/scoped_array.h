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


 */



/** @class lass::util::ScopedArray
 *  @ingroup SmartPtr
 *  @brief Smart pointer with simple sole ownership of arrays, non-copyable.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  @deprecated Use @c util::lass::ScopedPtr<T,ArrayStorage> now.
 *
 *  To be honest, this is nothing but a copy of boost::scoped_array, see http://www.boost.org.
 *  So, let's quote boost for they have the best explanation of what ScopedArray is:
 *
 *  <i>"The scoped_array class template stores a pointer to a dynamically allocated array. 
 *  (Dynamically allocated arrays are allocated with the C++ new[] expression.) The array 
 *  pointed to is guaranteed to be deleted, either on destruction of the scoped_array, or 
 *  via an explicit reset.
 *
 *  The scoped_array template is a simple solution for simple needs. It supplies a basic 
 *  "resource acquisition is initialization" facility, without shared-ownership or 
 *  transfer-of-ownership semantics. Both its name and enforcement of semantics (by being 
 *  noncopyable) signal its intent to retain ownership solely within the current scope. 
 *  Because it is noncopyable, it is safer than shared_array for pointers which should not 
 *  be copied.
 *
 *  Because scoped_array is so simple, in its usual implementation every operation is as 
 *  fast as a built-in array pointer and it has no more space overhead that a built-in array
 *  pointer.
 *
 *  It cannot be used in C++ standard library containers. See shared_array if scoped_array 
 *  does not meet your needs.
 *
 *  It cannot correctly hold a pointer to a single object. See scoped_ptr for that usage.
 *
 *  A std::vector is an alternative to a scoped_array that is a bit heavier duty but far 
 *  more flexible. A boost::array is an alternative that does not use dynamic allocation."</i>
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SCOPED_ARRAY_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SCOPED_ARRAY_H



#include "util_common.h"
#include "non_copyable.h"



namespace lass
{

namespace util
{

template <typename T>
class ScopedArray: NonCopyable
{
public:

	typedef T TPointee;

	explicit ScopedArray( T* iPointee = 0 );
	~ScopedArray();

	void reset( T* iPointee = 0 );

	T& operator[]( unsigned iIndex ) const;
	T* const get() const;
	const bool isEmpty() const;

	void swap( ScopedArray<T>& iOther );

private:

	T* pointee_;
};

template <typename T> void swap( ScopedArray<T>& iA, ScopedArray<T>& iB );



}

}

#include "scoped_array.inl"



#endif




