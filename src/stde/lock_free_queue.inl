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

template <typename T, typename A>
lock_free_queue<T, A>::lock_free_queue():
	util::AllocatorConcurrentFreeList<A>(sizeof node_t)
{
	pointer_t dummy = { make_node(value_type()), 0 };
	head_ = dummy;
	tail_ = dummy;
}



template <typename T, typename A>
lock_free_queue<T, A>::~lock_free_queue()
{
	while (head_.ptr)
	{
		pointer_t next = head_.ptr->next;
		free_node(head_.ptr);
		head_ = next;
	}
}



/**	push a value in the back
 *	@arg exception safe: if no node of could be allocatoed, or if copy constructor of x throws, 
 *		it fails gracefully.
 */
template <typename T, typename A>
void lock_free_queue<T, A>::push(const value_type& x)
{
	node_t* const node = make_node(x);
	pointer_t tail;
	while (true)
	{
		tail = tail_;
		pointer_t next = tail.ptr->next;
		if (tail.ptr == tail_.ptr && tail.tag == tail_.tag)
		{
			if (next.ptr == 0)
			{
				pointer_t new_next = { node, next.tag + 1 };
				if (compare_and_swap(tail.ptr->next, next, new_next))
				{
					break;
				}
			}
			else
			{
				pointer_t new_tail = { next.ptr, tail.tag + 1 };
				compare_and_swap(tail_, tail, new_tail);
			}
		}
	}
	pointer_t new_tail = { node, tail.tag + 1 };
	compare_and_swap(tail_, tail, new_tail);
}



/** Try to pop a value from the front and store it in x.
 *  @return false if no element could be popped.
 *  @arg EXCEPTION UNSAFE: if the copy constructor of x can throw, things can go miserably wrong!
 */
template <typename T, typename A>
bool lock_free_queue<T, A>::pop(value_type& x)
{
	pointer_t head;
	while (true)
	{
		head = head_;
		pointer_t tail = tail_;
		pointer_t next = head.ptr->next;
		if (head.ptr == head_.ptr && head.tag == head_.tag)
		{
			if (head.ptr == tail.ptr)
			{
				if (next.ptr == 0)
				{
					return false;
				}
				pointer_t new_tail = { next.ptr, tail.tag + 1 };
				compare_and_swap(tail_, tail, new_tail);
			}
			else
			{
				x = next.ptr->value;
				pointer_t new_head = { next.ptr, head.tag + 1 };
				if (compare_and_swap(head_, head, new_head))
				{
					break;
				}
			}
		}
	}
	free_node(head.ptr);
}

// --- private -------------------------------------------------------------------------------------

template <typename T, typename A>
typename lock_free_queue<T, A>::node_t* const
lock_free_queue<T, A>::make_node(const value_type& x)
{
	node_t* node = static_cast<node_t*>(allocate());
	try
	{
		new (&node->value) value_type(x);
	}
	catch (...)
	{
		deallocate(node);
		throw;
	}
	node->next.ptr = 0;
	node->next.tag = 0;
	return node;
}
	


template <typename T, typename A>
void lock_free_queue<T, A>::free_node(node_t* node)
{
	node->value.~value_type();
	deallocate(node);
}

	

template <typename T, typename A> inline 
bool lock_free_queue<T, A>::compare_and_swap(
		pointer_t& dest, const pointer_t& expected, const pointer_t& fresh)
{
	return util::atomicCompareAndSwap(dest.ptr, expected.ptr, expected.tag, fresh.ptr, fresh.tag);
}

}

}

// EOF
