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

/** @class lass::stde::lass_allocator
 *  @brief an adaptor to use an lass::util Allocator as a replacement for std::allocator
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_LASS_ALLOCATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_LASS_ALLOCATOR_H

#include "stde_common.h"

#include <cstddef>

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
	typedef std::ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	template <typename U> struct rebind { typedef lass_allocator<U, VariableAllocator> other; };

	lass_allocator() noexcept;
	lass_allocator(const lass_allocator&) noexcept;
	template <typename U> lass_allocator(const lass_allocator<U, VariableAllocator>&) noexcept;
	~lass_allocator() noexcept;

	pointer address(reference x) const;
	const_pointer address(const_reference x) const;

	pointer allocate(size_type n, typename lass_allocator<void, VariableAllocator>::pointer hint = 0);
	void deallocate(pointer p, size_type n);
	size_type max_size() const noexcept;

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
