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



#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_SMALL_OBJECT_ALLOCATOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_STDE_SMALL_OBJECT_ALLOCATOR_INL

#include "stde_common.h"
#include "small_object_allocator.h"
#include "../util/impl/small_object_impl.h"

namespace lass
{
namespace stde
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
small_object_allocator<T>::small_object_allocator()
{
}



template <typename T>
small_object_allocator<T>::small_object_allocator(const small_object_allocator&)
{
}



template <typename T>
template <typename U>
small_object_allocator<T>::small_object_allocator(const small_object_allocator<U>&)
{
}



template <typename T>
small_object_allocator<T>::~small_object_allocator()
{
}



template <typename T>
typename small_object_allocator<T>::pointer
small_object_allocator<T>::address(reference x) const
{
	return &x;
}



template <typename T>
typename small_object_allocator<T>::const_pointer
small_object_allocator<T>::address(const_reference x) const
{
	return &x;
}



template <typename T>
typename small_object_allocator<T>::pointer
small_object_allocator<T>::allocate(size_type n, small_object_allocator<void>::pointer)
{
	const size_type size = n * sizeof(T);
#ifdef LASS_BYPASS_SMALL_OBJECT_ALLOCATOR
	return ::operator new(size);
#else
	return static_cast<pointer>(
		lass::util::impl::smallObjectAllocator<chunkSize_, maxObjectSize_, atom_type>()->allocate(size));
#endif
}



template <typename T>
void small_object_allocator<T>::deallocate(pointer p, size_type n)
{
	const size_type size = n * sizeof(T);
#ifdef LASS_BYPASS_SMALL_OBJECT_ALLOCATOR
	::operator delete(p);
#else
	lass::util::impl::smallObjectAllocator<chunkSize_, maxObjectSize_, atom_type>()->deallocate(p, size);
#endif
}



template <typename T>
typename small_object_allocator<T>::size_type
small_object_allocator<T>::max_size() const
{
	// this is only a rough estimation ...
	const size_type n = size_type(-1) / sizeof(T);
	return n > 0 ? n : 1;
}


template <typename T>
void small_object_allocator<T>::construct(pointer p, const T& val)
{
	new(p) T(val);
}



template <typename T>
void small_object_allocator<T>::destroy(pointer p)
{
	p->~T();
}



// --- protected functions -------------------------------------------------------------------------




// --- private functions ---------------------------------------------------------------------------



// --- free functions ------------------------------------------------------------------------------

template <typename T1, typename T2>
bool operator==(const small_object_allocator<T1>&, const small_object_allocator<T2>&)
{
	return true;
}



template <typename T1, typename T2>
bool operator!=(const small_object_allocator<T1>&, const small_object_allocator<T2>&)
{
	return false;
}



}

}

#endif

// EOF
