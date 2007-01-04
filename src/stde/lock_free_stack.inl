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
namespace util
{

// --- public --------------------------------------------------------------------------------------

template <typename T, typename A>
lock_free_stack<T, A>::lock_free_stack():
	util::AllocatorConcurrentFreeList<A>(sizeof(node_t)),
	top_() 
{
}



template <typename T, typename A>
lock_free_stack<T, A>::~lock_free_stack()
{
	node_t top = top_.get();
	while (top)
	{
		node_t* const next = top->next;
		free_node(top);
		top = next;
	}
}



/** push value on stack.
 *  @arg exception safe: if no node of could be allocatoed, or if copy constructor of x throws, 
 *  	it fails gracefully.
 */
template <typename T, typename A>
void lock_free_stack<T, A>::push(const value_type& x)
{
	node_t* node = make_node(x);
	push_node(node);
}



/** Try to pop a value and copy it in @a x .
 *  @return false if stack is empty
 *  @arg strongly exception safe: it copy-constructor of x throws, 
 *		node is put back on top and exception is propagated
 */
template <typename T, typename A>
bool lock_free_stack<T, A>::pop(value_type& x)
{
	node_t* const node = pop_node();
	if (!node)
	{
		return false;
	}
	try
	{
		x = node->value;
	}
	catch (...)
	{
		// put it back!
		push_node(node);
		throw;
	}
	free_node(node);
	return true;
}



/** Try to pop a value and swap it in @a x .
 *  @return false if stack is empty
 *	@arg condition on @a value_type: x.swap(y) must be a valid, non throwing operation.
 *  @arg strongly exception safe: shoudln't throw.
 */
template <typename T, typename A>
bool lock_free_stack<T, A>::pop_swap(value_type& x)
{
	node_t* const node = pop_node();
	if (!node)
	{
		return false;
	}
	x.swap(node->value);
	free_node(node);
	return true;
}



// --- private -------------------------------------------------------------------------------------

template <typename T, typename A>
typename lock_free_stack<T, A>::node_t* const 
lock_free_stack<T, A>::make_node(const value_type& x)
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
	node->next = 0;
	return node;
}



template <typename T, typename A>
void lock_free_stack<T, A>::free_node(node_t* node)
{
	node->value.~value_type();
	deallocate(node);
}



template <typename T, typename A>
void lock_free_stack<T, A>::push_node(node_t* node)
{
	do
	{
		pointer_t top = top_;
		node->next = top.get();
		pointer_t new_top(node, top.tag() + 1);
	}
	while (!top_.atomicCompareAndSwap(top, new_top));
}



template <typename T, typename A>
typename lock_free_stack<T, A>::node_t* const 
lock_free_stack<T, A>::pop_node()
{
	pointer_t top;
	do
	{
		pointer_t top = top_;
		if (!top)
		{
			return 0;
		}

		// This is the tricky part ... does top still exist? In theory, it can be freed by now.
		// But here's the cunning part: the use of AllocatorLockFreeFreeList ensures that at least
		// the memory is not reclaimed (it merely is in purgatory instead).  
		// So, it's somehow safe to access its next member (it may give a totally wacked up 
		// result, but that doesn't really matter). 
		// It comes at a sacrifice though: memory will only be reclaimed at the end of the 
		// stack's lifecycle.
		//
		pointer_t next(top->next, top.tag() + 1);
	}
	while (!top_.atomicCompareAndSwap(top, next));
	return top.get();
}

}

}

// EOF
