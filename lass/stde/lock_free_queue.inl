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
 *	Copyright (C) 2004-2010 the Initial Developer.
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
	pointer_t tail(make_node(0), 0);
	
	// Mentally equivalent to: head_ = tail_ = tail;
	//
	// the funny way to assign head_ and tail_ is because we don't support
	// operator= on volatile TaggedPtrs 
	// [Bramz]
	//
	pointer_t expected = head_;
	head_.atomicCompareAndSwap(expected, tail);
	expected = tail_;
	tail_.atomicCompareAndSwap(expected, tail);
	LASS_ASSERT(tail == head_ && tail == tail_);	
}



template <typename T, typename A>
lock_free_queue<T, A>::~lock_free_queue()
{
	pointer_t head = head_;
	while (head)
	{
		pointer_t next = head->next;
		free_node(head.get());
		head = next;
	}
}



/**	push a value in the back
 *	@arg exception safe: if no node of could be allocatoed, or if copy constructor of x throws, 
 *		it fails gracefully.
 */
template <typename T, typename A>
void lock_free_queue<T, A>::push(const value_type& x)
{
	value_type* const value = make_value(x);
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
		tail = tail_;
		pointer_t next = tail->next;
		if (tail == tail_)
		{
			if (!next)
			{
				pointer_t new_next(node, next.tag() + 1);
				if (tail->next.atomicCompareAndSwap(next, new_next))
				{
					break;
				}
			}
			else
			{
				pointer_t new_tail(next.get(), tail.tag() + 1);
				tail_.atomicCompareAndSwap(tail, new_tail);
			}
		}
	}
	pointer_t new_tail(node, tail.tag() + 1);
	tail_.atomicCompareAndSwap(tail, new_tail);
}



/** Try to pop a value from the front and store it in x.
 *  @return false if no element could be popped.
 *  @arg EXCEPTION UNSAFE: if the copy constructor of x can throw, things can go miserably wrong!
 */
template <typename T, typename A>
bool lock_free_queue<T, A>::pop(value_type& x)
{
	pointer_t head;
	value_type* value;
	while (true)
	{
		head = head_;
		pointer_t tail = tail_;
		pointer_t next = head->next;
		if (head == head_)
		{
			if (head.get() == tail.get())
			{
				if (!next)
				{
					return false;
				}
				pointer_t new_tail(next.get(), tail.tag() + 1);
				tail_.atomicCompareAndSwap(tail, new_tail);
			}
			else
			{
				// 'next' might be reclaimed here, but that's not really a problem:
				// a) its memory will still exist, so at most value will contain rubbish but that's
				//		ok to copy because it's just used as a pointer
				// b) if it is reclaimed, the following CAS will fail so that value will never be
				//		used so we don't care if it contains rubbish.
				// SEE ALSO: lock_free_stack::pop_node, and why the above is not entirely true ...				
				// [Bramz]
				//
				value = next->value; 

				pointer_t new_head(next.get(), head.tag() + 1);
				if (head_.atomicCompareAndSwap(head, new_head))
				{
					x = *value;
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
typename lock_free_queue<T, A>::value_type*
lock_free_queue<T, A>::make_value(const value_type& x)
{
	value_type* value = static_cast<value_type*>(value_allocator_.allocate());
	try
	{
		new (value) value_type(x);
	}
	catch (...)
	{
		value_allocator_.deallocate(value);
		throw;
	}
	return value;
}



template <typename T, typename A>
void lock_free_queue<T, A>::free_value(value_type* value)
{
	value->~value_type();
	value_allocator_.deallocate(value);
}



template <typename T, typename A>
typename lock_free_queue<T, A>::node_t*
lock_free_queue<T, A>::make_node(value_type* value)
{
	node_t* node = static_cast<node_t*>(node_allocator_.allocate());
	try
	{
		new (&node->next) pointer_t();
	}
	catch (...)
	{
		node_allocator_.deallocate(node);
		throw;
	}
	node->value = value;
	return node;
}
	


template <typename T, typename A>
void lock_free_queue<T, A>::free_node(node_t* node)
{
	node->next.~pointer_t();
	node_allocator_.deallocate(node);
}

}

}

// EOF
