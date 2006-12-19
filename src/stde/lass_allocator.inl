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

namespace lass
{
namespace stde
{

// --- public --------------------------------------------------------------------------------------

template <typename T, typename VA>
lass_allocator<T, VA>::lass_allocator() throw()
{
}



template <typename T, typename VA>
lass_allocator<T, VA>::lass_allocator(const lass_allocator& other) throw():
	VA(static_cast<const VA&>(other))
{
}



template <typename T, typename VA>
template <typename U>
lass_allocator<T, VA>::lass_allocator(const lass_allocator<U, VA>& other) throw():
	VA(static_cast<const VA&>(other))
{
}



template <typename T, typename VA>
lass_allocator<T, VA>::~lass_allocator() throw()
{
}



template <typename T, typename VA>
typename lass_allocator<T, VA>::pointer
lass_allocator<T, VA>::address(reference x) const
{
	return &x;
}



template <typename T, typename VA>
typename lass_allocator<T, VA>::const_pointer
lass_allocator<T, VA>::address(const_reference x) const
{
	return &x;
}



template <typename T, typename VA>
typename lass_allocator<T, VA>::pointer
lass_allocator<T, VA>::allocate(size_type n, typename lass_allocator<void, VA>::pointer)
{
	const size_type size = n * sizeof(T);
	return static_cast<pointer>(VA::allocate(size));
}



template <typename T, typename VA>
void lass_allocator<T, VA>::deallocate(pointer p, size_type n)
{
	const size_type size = n * sizeof(T);
	VA::deallocate(p, size);
}



template <typename T, typename VA>
typename lass_allocator<T, VA>::size_type
lass_allocator<T, VA>::max_size() const throw()
{
	// this is only a rough estimation ...
	const size_type n = size_type(-1) / sizeof(T);
	return n > 0 ? n : 1;
}


template <typename T, typename VA>
void lass_allocator<T, VA>::construct(pointer p, const T& val)
{
	new(p) T(val);
}



template <typename T, typename VA>
void lass_allocator<T, VA>::destroy(pointer p)
{
	p->~T();
}



// --- protected functions -------------------------------------------------------------------------




// --- private functions ---------------------------------------------------------------------------



// --- free functions ------------------------------------------------------------------------------

template <typename T1, typename T2, typename VA>
bool operator==(const lass_allocator<T1, VA>&, const lass_allocator<T2, VA>&)
{
	return true;
}



template <typename T1, typename T2, typename VA>
bool operator!=(const lass_allocator<T1, VA>&, const lass_allocator<T2, VA>&)
{
	return false;
}



}

}

// EOF
