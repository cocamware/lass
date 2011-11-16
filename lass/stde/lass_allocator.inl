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
