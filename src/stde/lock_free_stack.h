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

	pointer_t;
};

}

}

#include "lock_free_stack.inl"

#endif

// EOF
