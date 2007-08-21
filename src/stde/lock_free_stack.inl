/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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
typename lock_free_stack<T, A>::node_t* const 
lock_free_stack<T, A>::make_node(const value_type& x)
{
	node_t* node = static_cast<node_t*>(this->allocate());
	try
	{
		new (&node->value) value_type(x);
	}
	catch (...)
	{
		this->deallocate(node);
		throw;
	}
	node->next = 0;
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
		// stack's lifecycle. [Bramz]
		//
		// Update: This is not entirely true ... if top is currently "in use", then top->next may
		// correspond with a bit pattern that isn't a valid pointer at all (and most likely,
		// if it is a valid bit pattern, it won't point to allocated memory).  Anyway, this
		// means that the behaviour is pretty much undefined.  The machine can do whatever it wants:
		// halt, reboot, make funny noises or make some coffee ... [Bramz]
		//
		pointer_t next(top->next, top.tag() + 1);
	}
	while (!top_.atomicCompareAndSwap(top, next));
	return top.get();
}

}

}

// EOF
