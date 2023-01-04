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
 *	Copyright (C) 2004-2023 the Initial Developer.
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

#include "lock_free_spmc_ring_buffer.h"

namespace lass
{
namespace stde
{
namespace impl
{

// --- lock_free_spmc_ring_buffer_base -------------------------------------------------------------

inline lock_free_spmc_ring_buffer_base::lock_free_spmc_ring_buffer_base(size_t capacity):
	head_(tagged_index_t(0, 0)),
	tail_(tagged_index_t(0, 0)),
	ring_size_(static_cast<index_type>(enforce_valid_size(capacity)))
{
#if defined(__cpp_lib_atomic_is_always_lock_free)
	static_assert(std::atomic<tagged_index_t>::is_always_lock_free);
#else
	LASS_ENFORCE(head_.is_lock_free());
	LASS_ENFORCE(tail_.is_lock_free());
#endif
}


/** Return true if ring buffer is empty
 */
inline bool lock_free_spmc_ring_buffer_base::empty() const
{
	return tagged_index_t::empty(head_.load(std::memory_order_acquire), tail_.load(std::memory_order_acquire));
}



/** Return true if ring buffer is empty
 */
inline bool lock_free_spmc_ring_buffer_base::full() const
{
	return tagged_index_t::full(head_.load(std::memory_order_acquire), tail_.load(std::memory_order_acquire));
}



lock_free_spmc_ring_buffer_base::tagged_index_t
inline lock_free_spmc_ring_buffer_base::next_index(tagged_index_t index) const
{
	const index_type i = ++index.index;
	return i < ring_size_
		? tagged_index_t(i, index.tag)
		: tagged_index_t(0, index.tag + 1);
}



inline size_t lock_free_spmc_ring_buffer_base::enforce_valid_size(size_t size)
{
	if (size == 0)
	{
		throw std::length_error("ring buffer capacitance cannot be zero");
	}
	if (size > num::NumTraits<index_type>::max)
	{
		throw std::length_error("exceeded maximum ring buffer capacitance");
	}
	return size;
}



// --- lock_free_spmc_value_ring_buffer ------------------------------------------------------------

template <typename T>
lock_free_spmc_value_ring_buffer<T>::lock_free_spmc_value_ring_buffer(size_t capacity):
	lock_free_spmc_ring_buffer_base(capacity),
	ring_(capacity)
{
}



/** Try to push a value x on the front.
 *  @return false if buffer was full and x could not be pushed.
 */
template <typename T>
bool lock_free_spmc_value_ring_buffer<T>::try_push(const value_type& x)
{
	const tagged_index_t head = head_.load(std::memory_order_relaxed);
	const tagged_index_t tail = tail_.load(std::memory_order_acquire);
	if (tagged_index_t::full(head, tail))
	{
		LASS_ASSERT(head.tag == tail.tag + 1);
		return false;
	}
	ring_[head.index].store(x, std::memory_order_release);
	head_.store(next_index(head), std::memory_order_release);
	return true;
}



/** Try to push a value x on the front.
 *  @return false if buffer was full and x could not be pushed.
 */
template <typename T>
bool lock_free_spmc_value_ring_buffer<T>::try_push(value_type&& x)
{
	const tagged_index_t head = head_.load(std::memory_order_relaxed);
	const tagged_index_t tail = tail_.load(std::memory_order_acquire);
	if (tagged_index_t::full(head, tail))
	{
		LASS_ASSERT(head.tag == tail.tag + 1);
		return false;
	}
	ring_[head.index].store(std::move(x), std::memory_order_release);
	head_.store(next_index(head), std::memory_order_release);
	return true;
}



/** Try to emplace a value on the front.
 *  @return false if buffer was full and value could not be pushed.
 */
template <typename T>
template <class... Args>
bool lock_free_spmc_value_ring_buffer<T>::try_emplace(Args&&... args)
{
	const tagged_index_t head = head_.load(std::memory_order_relaxed);
	const tagged_index_t tail = tail_.load(std::memory_order_acquire);
	if (tagged_index_t::full(head, tail))
	{
		LASS_ASSERT(head.tag == tail.tag + 1);
		return false;
	}
	ring_[head.index].store(value_type(std::forward<Args...>(args...)), std::memory_order_release);
	head_.store(next_index(head), std::memory_order_release);
	return true;
}



/** Try to pop a value from the back and store it in x.
 *  @return false if buffer was empty and no element could be popped.
 */
template <typename T>
bool lock_free_spmc_value_ring_buffer<T>::try_pop(value_type& x)
{
	tagged_index_t tail = tail_.load(std::memory_order_acquire);
	while (true)
	{
		const tagged_index_t head = head_.load(std::memory_order_acquire);
		if (tagged_index_t::empty(head, tail))
		{
			return false;
		}
		x = ring_[tail.index].load(std::memory_order_acquire);
		if (tail_.compare_exchange_weak(tail, next_index(tail)))
		{
			return true;
		}
	}
}



// --- lock_free_spmc_object_ring_buffer ----------------------------------------------------------

template <typename T, typename A>
lock_free_spmc_object_ring_buffer<T, A>::lock_free_spmc_object_ring_buffer(size_t capacity):
	lock_free_spmc_ring_buffer_base(capacity),
	value_allocator_(sizeof(T)),
	ring_(capacity)
{
}



template <typename T, typename A>
lock_free_spmc_object_ring_buffer<T, A>::~lock_free_spmc_object_ring_buffer()
{
	const tagged_index_t head = head_.load(std::memory_order_acquire);
	const tagged_index_t tail = tail_.load(std::memory_order_acquire);
	for (tagged_index_t i = tail; !tagged_index_t::empty(i, head); i = next_index(i))
	{
		value_type* p = ring_[i.index].load(std::memory_order_relaxed);
		p->~value_type();
		value_allocator_.deallocate(p);
	}
}



/** Try to push a value x on the front.
 *  @return false if buffer was full and x could not be pushed.
 */
template <typename T, typename A>
bool lock_free_spmc_object_ring_buffer<T, A>::try_push(const value_type& x)
{
	const tagged_index_t head = head_.load(std::memory_order_relaxed);
	const tagged_index_t tail = tail_.load(std::memory_order_acquire);
	if (tagged_index_t::full(head, tail))
	{
		return false;
	}
	void* p = value_allocator_.allocate();
	try
	{
		ring_[head.index] = new (p) value_type(x);
	}
	catch (...)
	{
		value_allocator_.deallocate(p);
		throw;
	}
	head_.store(next_index(head), std::memory_order_release);
	return true;
}



/** Try to push a value x on the front.
 *  @return false if buffer was full and x could not be pushed.
 */
template <typename T, typename A>
bool lock_free_spmc_object_ring_buffer<T, A>::try_push(value_type&& x)
{
	const tagged_index_t head = head_.load(std::memory_order_relaxed);
	const tagged_index_t tail = tail_.load(std::memory_order_acquire);
	if (tagged_index_t::full(head, tail))
	{
		return false;
	}
	void* p = value_allocator_.allocate();
	try
	{
		ring_[head.index] = new (p) value_type(std::move(x));
	}
	catch (...)
	{
		value_allocator_.deallocate(p);
		throw;
	}
	head_.store(next_index(head), std::memory_order_release);
	return true;
}



/** Try to push a value x on the front.
 *  @return false if buffer was full and x could not be pushed.
 */
template <typename T, typename A>
template <class... Args>
bool lock_free_spmc_object_ring_buffer<T, A>::try_emplace(Args&&... args)
{
	const tagged_index_t head = head_.load(std::memory_order_relaxed);
	const tagged_index_t tail = tail_.load(std::memory_order_acquire);
	if (tagged_index_t::full(head, tail))
	{
		return false;
	}
	void* p = value_allocator_.allocate();
	try
	{
		ring_[head.index] = new (p) value_type(std::forward<Args...>(args...));
	}
	catch (...)
	{
		value_allocator_.deallocate(p);
		throw;
	}
	head_.store(next_index(head), std::memory_order_release);
	return true;
}


/** Try to pop a value from the back and store it in x.
 *  @return false if buffer was empty and no element could be popped.
 */
template <typename T, typename A>
bool lock_free_spmc_object_ring_buffer<T, A>::try_pop(value_type& x)
{
	tagged_index_t tail = tail_.load(std::memory_order_acquire);
	while (true)
	{
		const tagged_index_t head = head_.load(std::memory_order_acquire);
		if (tagged_index_t::empty(head, tail))
		{
			return false;
		}
		value_type* p = ring_[tail.index];
		if (tail_.compare_exchange_weak(tail, next_index(tail)))
		{
			x = std::move(*p);
			p->~value_type();
			value_allocator_.deallocate(p);
			return true;
		}
	}
}


}
}
}

// EOF
