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

/** @class lass::stde::small_object_allocator
 *  @brief a replacement for std::allocator that uses same allocator as lass::util::SmallObject
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_SMALL_OBJECT_ALLOCATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_SMALL_OBJECT_ALLOCATOR_H

#include "stde_common.h"
#include "../util/small_object.h"

namespace lass
{
namespace stde
{

template <typename T> class small_object_allocator;

template <>
class small_object_allocator<void>
{
public:
	typedef void* pointer;
	typedef const void* const_pointer;
	typedef void value_type;
	template <typename U> struct rebind { typedef small_object_allocator<U> other; };
};

template <typename T>
class small_object_allocator
{
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	template <typename U> struct rebind { typedef small_object_allocator<U> other; };

	small_object_allocator() throw();
	small_object_allocator(const small_object_allocator&) throw();
	template <typename U> small_object_allocator(const small_object_allocator<U>&) throw();
	~small_object_allocator() throw();

	pointer address(reference x) const;
	const_pointer address(const_reference x) const;

	pointer allocate(size_type n, small_object_allocator<void>::pointer hint = 0);
	void deallocate(pointer p, size_type n);
	size_type max_size() const throw();

	void construct(pointer p, const T& val);
	void destroy(pointer p);

private:

	enum
	{
		chunkSize_ = 4096,
		maxObjectSize_ = 64
	};

	typedef unsigned char atom_type;
};

template <typename T1, typename T2>
bool operator==(const small_object_allocator<T1>&, const small_object_allocator<T2>&);

template <typename T1, typename T2>
bool operator!=(const small_object_allocator<T1>&, const small_object_allocator<T2>&);

}

}

#include "small_object_allocator.inl"

#endif

// EOF
