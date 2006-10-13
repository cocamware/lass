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

#include "prim_common.h"
#include "prim_allocator.h"
#include "../../util/allocator.h"
#include "../../util/singleton.h"

namespace lass
{
namespace prim
{
namespace impl
{

typedef util::AllocatorThrow<
	util::AllocatorPerThread<
		util::AllocatorVariableHybrid<
			util::AllocatorFreeList<>, 128
			>
		>
	>
	TMemoryAllocator;

TMemoryAllocator& memoryAllocator()
{
	return *util::Singleton<TMemoryAllocator, util::destructionPriorityInternalAllocators>::instance();
}

void* allocateMemory(size_t size)
{
	return memoryAllocator().allocate(size);
}

void deallocateMemory(void* p, size_t size)
{
	memoryAllocator().deallocate(p, size);
}

}
}
}

// EOF
