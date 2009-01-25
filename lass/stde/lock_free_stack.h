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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_LOCK_FREE_STACK_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_LOCK_FREE_STACK_H

#include "stde_common.h"
#include "../util/allocator.h"

namespace lass
{
namespace util
{

// NOT TESTED YET!!!!

template 
<
	typename T, 
	typename FixedAllocator = AllocatorFixed<AllocatorMalloc>
>
class lock_free_stack: private util::AllocatorConcurrentFreeList<FixedAllocator>
{
public:

	typedef T value_type;

	lock_free_stack();
	~lock_free_stack();

	void push(const value_type& x);
	bool pop(value_type& x);
	bool pop_swap(value_type& x);

private:

	struct node_t
	{
		node_t* next;
		value_type value;
	};
	
	typedef util::TaggedPtr<node_t> pointer_t;
	
	lock_free_stack(const lock_free_stack&);
	lock_free_stack& operator=(const lock_free_stack&);

	node_t* const make_node(const value_type& x);
	void free_node(node_t* node);
	void push_node(node_t* node);
	node_t* const pop_node();

	volatile pointer_t top_;
};

}

}

#include "lock_free_stack.inl"

#endif

// EOF
