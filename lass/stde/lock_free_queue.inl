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

namespace lass
{
namespace stde
{

// --- public --------------------------------------------------------------------------------------

template <typename T, typename A>
lock_free_queue<T, A>::lock_free_queue():
	node_allocator_(sizeof(node_t)),
	value_allocator_(sizeof(value_type))
{
	pointer_t tail(make_node(nullptr), 0);
	head_ = tail;
	tail_ = tail;

#if defined(__cpp_lib_atomic_is_always_lock_free)
	static_assert(std::atomic<pointer_t>::is_always_lock_free);
#else
	LASS_ENFORCE(head_.is_lock_free());
	LASS_ENFORCE(tail_.is_lock_free());
#endif
}



template <typename T, typename A>
lock_free_queue<T, A>::~lock_free_queue()
{
	// head node has no value, but it always exists.
	pointer_t head = head_.load(std::memory_order_acquire);
	pointer_t node = head->next.load(std::memory_order_relaxed);
	while (node)
	{
		pointer_t next = node->next.load(std::memory_order_relaxed);
		free_value(node->value.load(std::memory_order_relaxed));
		free_node(node.get());
		node = next;
	}
	free_node(head.get());
}



/**	push a value in the back
 *	@arg exception safe: if no node of could be allocatoed, or if copy constructor of x throws, 
 *		it fails gracefully.
 */
template <typename T, typename A>
void lock_free_queue<T, A>::push(const value_type& x)
{
	value_type* const value = make_value(x);
	push_value(value);
}



/**	push a value in the back
 */
template <typename T, typename A>
void lock_free_queue<T, A>::push(value_type&& x)
{
	value_type* const value = make_value(std::move(x));
	push_value(value);
}



/**	emplace a value in the back
 */
template <typename T, typename A>
template <class... Args>
void lock_free_queue<T, A>::emplace(Args&&... args)
{
	value_type* const value = make_value(std::forward<Args>(args)...);
	push_value(value);
}



/** Try to pop a value from the front and store it in x.
 *  @return false if no element could be popped.
 */
template <typename T, typename A>
bool lock_free_queue<T, A>::pop(value_type& x)
{
	while (true)
	{
		pointer_t head = head_.load(std::memory_order_acquire);
		pointer_t tail = tail_.load(std::memory_order_acquire);
		pointer_t next = head->next.load(std::memory_order_acquire);
		if (head == head_.load(std::memory_order_acquire))
		{
			if (head.get() == tail.get())
			{
				if (!next)
				{
					return false;
				}
				pointer_t new_tail(next.get(), tail.nextTag());
				tail_.compare_exchange_weak(tail, new_tail);
			}
			else
			{
				// This is the tricky part ... does 'next' still exist?
				// In theory, it can be freed by now. But by using
				// AllocatorConcurrentFreeList, it's guaranteed that at least its memory
				// is not reclaimed by the OS.  It's either sitting unallocated in the
				// free-list and has its memory preserved, or it's already being
				// reallocated for a new node.
				// In both cases, it should be safe to read next->value.
				// And in both cases, the compare_exchange_strong that follows will
				// return false before we try to dereference value.
				//
				value_type* value = next->value.load(std::memory_order_acquire);

				pointer_t new_head(next.get(), head.nextTag());
				if (head_.compare_exchange_strong(head, new_head))
				{
					try
					{
						x = std::move(*value);
					}
					catch (...)
					{
						free_value(value);
						free_node(head.get());
						throw;
					}
					free_value(value);
					free_node(head.get());
					return true;
				}
			}
		}
	}
}

// --- private -------------------------------------------------------------------------------------

template <typename T, typename A>
template <class... Args>
typename lock_free_queue<T, A>::value_type*
lock_free_queue<T, A>::make_value(Args&&... args)
{
	void* p = value_allocator_.allocate();
	try
	{
		return new (p) value_type{ std::forward<Args>(args)... };
	}
	catch (...)
	{
		value_allocator_.deallocate(p);
		throw;
	}
}



template <typename T, typename A>
void lock_free_queue<T, A>::free_value(value_type* value)
{
	value->~value_type();
	value_allocator_.deallocate(value);
}



template <typename T, typename A>
void lock_free_queue<T, A>::push_value(value_type* value)
{
	node_t* node = 0;
	try
	{
		node = make_node(value);
	}
	catch (...)
	{
		free_value(value);
		throw;
	}
	pointer_t tail;
	while (true)
	{
		tail = tail_.load(std::memory_order_acquire);
		pointer_t next = tail->next.load(std::memory_order_acquire);
		if (tail == tail_.load(std::memory_order_acquire))
		{
			if (!next)
			{
				pointer_t new_next(node, next.nextTag());
				if (tail->next.compare_exchange_weak(next, new_next))
				{
					break;
				}
			}
			else
			{
				pointer_t new_tail(next.get(), tail.nextTag());
				tail_.compare_exchange_weak(tail, new_tail);
			}
		}
	}
	pointer_t new_tail(node, tail.nextTag());
	tail_.compare_exchange_strong(tail, new_tail);
}



template <typename T, typename A>
typename lock_free_queue<T, A>::node_t*
lock_free_queue<T, A>::make_node(value_type* value)
{
	void* p = node_allocator_.allocate();
	try
	{
		return new (p) node_t(value);
	}
	catch (...)
	{
		node_allocator_.deallocate(p);
		throw;
	}
}
	


template <typename T, typename A>
void lock_free_queue<T, A>::free_node(node_t* node)
{
	node->~node_t();
	node_allocator_.deallocate(node);
}



}

}

// EOF
