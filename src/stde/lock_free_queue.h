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

/** @class lass::stde::lock_free_queue
 *  @brief Non-blocking, lock-free FIFO data structure
 *
 *  M. M. Michael, M. L. Scott, "Simple, Fast, and Practical Non-Blocking and Blocking Concurrent
 *  Queue Algorithms", Proc. of the 15th Annual ACM Symposium on Principles of Distributed
 *  Computing (PODC' 96), New York, USA, pp. 267-275 (1996).
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_LOCK_FREE_QUEUE_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_LOCK_FREE_QUEUE_H

#include "stde_common.h"
#include "../util/allocator.h"

namespace lass
{
namespace stde
{

template 
<
	typename T, 
	typename FixedAllocator = util::AllocatorFixed<util::AllocatorMalloc>
>
class lock_free_queue
{
public:

	typedef T value_type;

	lock_free_queue();
	~lock_free_queue();
	void push(const value_type& x);
	bool pop(value_type& x);

private:
	
	struct node_t;
	typedef util::TaggedPtr<node_t> pointer_t;	
	
	struct node_t
	{
		pointer_t next;
		value_type* value;
	};

	typedef util::AllocatorThrow< util::AllocatorConcurrentFreeList<FixedAllocator> > allocator_t;

	lock_free_queue(const lock_free_queue&);
	lock_free_queue& operator=(const lock_free_queue&);

	value_type* const make_value(const value_type& x);
	void free_value(value_type* value);
	node_t* const make_node(value_type* x);
	void free_node(node_t* node);

	pointer_t head_;
	pointer_t tail_;
	allocator_t node_allocator_;
	allocator_t value_allocator_;
};

}

}

#include "lock_free_queue.inl"

#endif

// EOF
