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
lock_free_stack<T, A>::lock_free_stack():
	util::AllocatorConcurrentFreeList<A>(sizeof(node_t)),
	top_() 
{
#if defined(__cpp_lib_atomic_is_always_lock_free)
	static_assert(std::atomic<pointer_t>::is_always_lock_free);
#else
	LASS_ENFORCE(top_.is_lock_free());
#endif
}



template <typename T, typename A>
lock_free_stack<T, A>::~lock_free_stack()
{
	node_t* top = top_.load(std::memory_order_acquire).get();
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
	emplace(x);
}



/** push value on stack.
 */
template <typename T, typename A>
void lock_free_stack<T, A>::push(value_type&& x)
{
	emplace(std::move(x));
}



/** push value on stack.
 */
template <typename T, typename A>
template <class... Args>
void lock_free_stack<T, A>::emplace(Args&&... args)
{
	node_t* node = make_node();
	try
	{
		new (&node->value) value_type(std::forward<Args>(args)...);
	}
	catch (...)
	{
		this->deallocate(node);
		throw;
	}
	push_node(node);
}



/** Try to pop a value and copy it in @a x .
 *  @return false if stack is empty
 *  @arg strong exception safety: if copy-constructor of x throws, 
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
		x = std::move(node->value);
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
 *  @arg exception safety: no-throw guarantee (if x.swap(y) plays by the rules)
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
typename lock_free_stack<T, A>::node_t* 
lock_free_stack<T, A>::make_node()
{
	node_t* node = static_cast<node_t*>(this->allocate());
	node->next = nullptr;
	return node;
}



template <typename T, typename A>
void lock_free_stack<T, A>::free_node(node_t* node)
{
	node->value.~value_type();
	this->deallocate(node);
}



template <typename T, typename A>
void lock_free_stack<T, A>::push_node(node_t* node)
{
	pointer_t top = top_.load(std::memory_order_acquire);
	pointer_t new_top;
	do
	{
		node->next = top.get();
		new_top = pointer_t(node, top.nextTag());
	}
	while (!top_.compare_exchange_weak(top, new_top));
}



template <typename T, typename A>
typename lock_free_stack<T, A>::node_t* 
lock_free_stack<T, A>::pop_node()
{
	pointer_t top = top_.load(std::memory_order_acquire);
	pointer_t next;
	do
	{
		if (!top)
		{
			return 0;
		}

		// This is the tricky part ... does top still exist?
		// In theory, it can be freed by now. But by using AllocatorConcurrentFreeList,
		// it's guaranteed that at least its memory is not reclaimed by the OS. It's
		// either sitting unallocated in the free-list and has its memory preserved, or
		// it's already being reallocated for a new node.
		// In both cases, it should be safe to read top->next.
		//
		next = pointer_t(top->next, top.nextTag());
	}
	while (!top_.compare_exchange_weak(top, next));
	return top.get();
}

}

}

// EOF
