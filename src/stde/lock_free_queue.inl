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
	util::AllocatorConcurrentFreeList<A>(sizeof(node_t))
{
	pointer_t dummy(make_node(value_type()), 0);
	head_ = dummy;
	tail_ = dummy;
}



template <typename T, typename A>
lock_free_queue<T, A>::~lock_free_queue()
{
	while (head_)
	{
		pointer_t next = head_->next;
		free_node(head_.get());
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
#pragma LASS_FIXME("what happens if next is already reclaimed? ... Exactly [Bramz]")
				x = next->value;
				pointer_t new_head(next.get(), head.tag() + 1);
				if (head_.atomicCompareAndSwap(head, new_head))
				{
					break;
				}
			}
		}
	}
	free_node(head.get());
	return true;
}

// --- private -------------------------------------------------------------------------------------

template <typename T, typename A>
typename lock_free_queue<T, A>::node_t* const
lock_free_queue<T, A>::make_node(const value_type& x)
{
	node_t* node = static_cast<node_t*>(allocate());
	try
	{
		new (&node->next) pointer_t();
	}
	catch (...)
	{
		deallocate(node);
		throw;
	}
	try
	{
		new (&node->value) value_type(x);
	}
	catch (...)
	{
		node->next.~pointer_t();
		deallocate(node);
		throw;
	}
	return node;
}
	


template <typename T, typename A>
void lock_free_queue<T, A>::free_node(node_t* node)
{
	node->value.~value_type();
	node->next.~pointer_t();
	deallocate(node);
}

}

}

// EOF
