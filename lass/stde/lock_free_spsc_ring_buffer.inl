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
 *	Copyright (C) 2004-2021 the Initial Developer.
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

#include "lock_free_spsc_ring_buffer.h"

namespace lass
{
namespace stde
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
lock_free_spsc_ring_buffer<T>::lock_free_spsc_ring_buffer(size_t capacity):
	head_(0),
	tail_(0),
	ring_(capacity + 1),
	ring_size_(capacity + 1)
{
#if defined(__cpp_lib_atomic_is_always_lock_free)
	static_assert(std::atomic<size_t>::is_always_lock_free);
#else
	LASS_ENFORCE(head_.is_lock_free());
	LASS_ENFORCE(tail_.is_lock_free());
#endif
}



template <typename T>
lock_free_spsc_ring_buffer<T>::~lock_free_spsc_ring_buffer()
{
}



/** Try to push a value x on the front.
 *  @return false if buffer was full and x could not be pushed.
 */
template <typename T>
bool lock_free_spsc_ring_buffer<T>::try_push(const value_type& x)
{
	const size_t head = head_.load(std::memory_order_relaxed);
	const size_t tail = tail_.load(std::memory_order_acquire);
	const size_t new_head = next_index(head);
	if (new_head == tail)
	{
		return false;
	}
	ring_[head] = x;
	head_.store(new_head, std::memory_order_release);
	return true;
}



/** Try to push a value x on the front.
 *  @return false if buffer was full and x could not be pushed.
 */
template <typename T>
bool lock_free_spsc_ring_buffer<T>::try_push(value_type&& x)
{
	const size_t head = head_.load(std::memory_order_relaxed);
	const size_t tail = tail_.load(std::memory_order_acquire);
	const size_t new_head = next_index(head);
	if (new_head == tail)
	{
		return false;
	}
	ring_[head] = std::move(x);
	head_.store(new_head, std::memory_order_release);
	return true;
}



/** Try to push a value x on the front.
 *  @return false if buffer was full and x could not be pushed.
 */
template <typename T>
template <typename... Args>
bool lock_free_spsc_ring_buffer<T>::try_emplace(Args&&... args)
{
	const size_t head = head_.load(std::memory_order_relaxed);
	const size_t tail = tail_.load(std::memory_order_acquire);
	const size_t new_head = next_index(head);
	if (new_head == tail)
	{
		return false;
	}
	ring_[head] = value_type(std::forward<Args...>(args...));
	head_.store(new_head, std::memory_order_release);
	return true;
}



/** Try to pop a value from the back and store it in x.
 *  @return false if buffer was empty and no element could be popped.
 */
template <typename T>
bool lock_free_spsc_ring_buffer<T>::try_pop(value_type& x)
{
	const size_t head = head_.load(std::memory_order_acquire);
	const size_t tail = tail_.load(std::memory_order_relaxed);
	if (tail == head)
	{
		return false;
	}
	x = std::move(ring_[tail]);
	tail_.store(next_index(tail), std::memory_order_release);
	return true;
}



/** Return true if ring buffer is empty
 */
template <typename T>
bool lock_free_spsc_ring_buffer<T>::empty() const
{
	const size_t head = head_.load(std::memory_order_acquire);
	const size_t tail = tail_.load(std::memory_order_acquire);
	return head == tail;
}



/** Return true if ring buffer is empty
 */
template <typename T>
bool lock_free_spsc_ring_buffer<T>::full() const
{
	const size_t head = head_.load(std::memory_order_acquire);
	const size_t tail = tail_.load(std::memory_order_acquire);
	return next_index(head) == tail;
}


// --- private -------------------------------------------------------------------------------------

template <typename T>
size_t lock_free_spsc_ring_buffer<T>::next_index(size_t index) const
{
	++index;
	return index < ring_size_ ? index : 0;
}


}

}

// EOF
