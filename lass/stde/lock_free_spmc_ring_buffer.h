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
 *	Copyright (C) 2004-2022 the Initial Developer.
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

#pragma once

#include "stde_common.h"
#include "../util/non_copyable.h"
#include "../util/atomic.h"

namespace lass
{
namespace stde
{
namespace impl
{

class lock_free_spmc_ring_buffer_base: util::NonCopyable
{
public:

	bool empty() const;
	bool full() const;

protected:

	lock_free_spmc_ring_buffer_base(size_t capacity);
	~lock_free_spmc_ring_buffer_base() = default;

	typedef num::Tuint32 index_type;

	struct alignas(8) tagged_index_t
	{
		tagged_index_t() noexcept {}
		tagged_index_t(index_type index, index_type tag) : index(index), tag(tag) {}
		index_type index = 0;
		index_type tag = 0;

		static bool empty(tagged_index_t head, tagged_index_t tail) { return head.index == tail.index && head.tag == tail.tag; }
		static bool full(tagged_index_t head, tagged_index_t tail) { return head.index == tail.index && head.tag != tail.tag; }
	};

	
	tagged_index_t next_index(tagged_index_t index) const;

	alignas(LASS_LOCK_FREE_ALIGNMENT) std::atomic<tagged_index_t> head_;
	alignas(LASS_LOCK_FREE_ALIGNMENT) std::atomic<tagged_index_t> tail_;
	const index_type ring_size_;

private:

	static_assert(sizeof(tagged_index_t) == 8, "tagged_index_t should be 8-bytes");

	static size_t enforce_valid_size(size_t size);

};



template<typename T>
class lock_free_spmc_value_ring_buffer: private lock_free_spmc_ring_buffer_base
{
public:
	typedef T value_type;

	lock_free_spmc_value_ring_buffer(size_t capacity);
	~lock_free_spmc_value_ring_buffer() = default;

	bool try_push(const value_type& x);
	bool try_push(value_type&& x);
	template <class... Args> bool try_emplace(Args&&... args);
	bool try_pop(value_type& x);

	using lock_free_spmc_ring_buffer_base::empty;
	using lock_free_spmc_ring_buffer_base::full;

private:

	std::vector< std::atomic<value_type> > ring_;
};


template
<
	typename T,
	typename FixedAllocator = util::AllocatorConcurrentFreeList<>
>
class lock_free_spmc_object_ring_buffer: private lock_free_spmc_ring_buffer_base
{
public:
	typedef T value_type;

	lock_free_spmc_object_ring_buffer(size_t capacity);
	~lock_free_spmc_object_ring_buffer();

	bool try_push(const value_type& x);
	bool try_push(value_type&& x);
	template <class... Args> bool try_emplace(Args&&... args);
	bool try_pop(value_type& x);

	using lock_free_spmc_ring_buffer_base::empty;
	using lock_free_spmc_ring_buffer_base::full;

private:

	FixedAllocator value_allocator_;
	std::vector< std::atomic<T*> > ring_;
};

}

template<typename T>
class lock_free_spmc_ring_buffer: public std::conditional <
		std::is_trivially_copy_constructible<T>::value,
		impl::lock_free_spmc_value_ring_buffer<T>,
		impl::lock_free_spmc_object_ring_buffer<T> 
	>::type
{
};

}

}

#include "lock_free_spmc_ring_buffer.inl"

// EOF
