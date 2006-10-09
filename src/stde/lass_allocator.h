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

/** @class lass::stde::lass_allocator
 *  @brief an adaptor to use an lass::util Allocator as a replacement for std::allocator
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_LASS_ALLOCATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_LASS_ALLOCATOR_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

template <typename T, typename VariableAllocator> class lass_allocator;

template <typename VariableAllocator>
class lass_allocator<void, VariableAllocator>
{
public:
	typedef void* pointer;
	typedef const void* const_pointer;
	typedef void value_type;
	template <typename U> struct rebind { typedef lass_allocator<U, VariableAllocator> other; };
};

template <typename T, typename VariableAllocator>
class lass_allocator: public VariableAllocator
{
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	template <typename U> struct rebind { typedef lass_allocator<U, VariableAllocator> other; };

	lass_allocator() throw();
	lass_allocator(const lass_allocator&) throw();
	template <typename U> lass_allocator(const lass_allocator<U, VariableAllocator>&) throw();
	~lass_allocator() throw();

	pointer address(reference x) const;
	const_pointer address(const_reference x) const;

	pointer allocate(size_type n, typename lass_allocator<void, VariableAllocator>::pointer hint = 0);
	void deallocate(pointer p, size_type n);
	size_type max_size() const throw();

	void construct(pointer p, const T& val);
	void destroy(pointer p);
};

template <typename T1, typename T2, typename VariableAllocator>
bool operator==(const lass_allocator<T1, VariableAllocator>&, const lass_allocator<T2, VariableAllocator>&);

template <typename T1, typename T2, typename VariableAllocator>
bool operator!=(const lass_allocator<T1, VariableAllocator>&, const lass_allocator<T2, VariableAllocator>&);

}

}

#include "lass_allocator.inl"

#endif

// EOF
