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

	volatile pointer_t head_;
	volatile pointer_t tail_;
	allocator_t node_allocator_;
	allocator_t value_allocator_;
};

}

}

#include "lock_free_queue.inl"

#endif

// EOF
