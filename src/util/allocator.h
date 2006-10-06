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

/** @defgroup Allocator
 *  @brief Custom allocator library
 *
 *	@section Introduction
 *
 *	This library provides a nice flexible set of custom allocators you can combine in
 *	anyway you like, as long as it makes sense.
 *
 *	There are in general two kind of allocator interfaces: fixed-size and variable-size 
 *	allocators, with the obvious meanings.  The former is constructed with the requested 
 *	block size and (de)allocates blocks of at least that size.  The latter has a default 
 *	constructor and the requested block size is passed at _both_ allocation and deallocation.
 *	Some variable-size allocators (like AllocatorMalloc) also provide deallocate(void* mem) 
 *  without the block size, but that's not a requirement.
 *
 *	@code
 *	class FixedSizeAllocator
 *	{
 *	public:
 *		FixedSizeAllocator(size_t size);
 *		void* allocate();
 *		void deallocate(void* mem);
 *	};
 *
 *	class VariableSizeAllocator
 *	{
 *		VariableSizeAllocator();
 *		void* allocate(size_t size);
 *		void deallocate(void* mem, size_t size);
 *	};
 *	@endcode
 *
 *	There are bottom-level allocators (like AllocatorMalloc) that do real allocation work
 *	and exist on there own.  And there are adaptor allocators (like AllocatorLocked) that
 *	extend/adapt another allocator to provide some extra functionality.  Some of them
 *	(like AllocatorLocked) implement both the fixed-size and variable-size interface.
 *	Others (like AllocatorVariableSize) convert a fixed-size interface to variable-size.
 *
 *	On top of all that, there's AllocatorClassAdaptor you can use to overload class-specific
 *	new and delete using a variable-size allocator.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_ALLOCATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_ALLOCATOR_H

#include "util_common.h"
#include "thread.h"

namespace lass
{
namespace util
{
namespace experimental
{

/** Use an Allocator to implement a class' new and delete
 *  @ingroup Allocator
 *
 *	Derive from this class to implement new and delete based on Allocator.
 *	It uses a singleton of Allocator that is shared per 
 *	<Allocator type, destruction priority> pair.  So if you have another
 *	class using the same allocator and destruction priority, the allocator
 *	singleton will be shared.
 */
template 
<
	typename Allocator,
	int destructionPriority = LASS_UTIL_SINGLETON_DEFAULT_DESTRUCTION_PRIORITY
>
class AllocatorClassAdaptor
{
public:
	static void* operator new(std::size_t size)
	{
		void* result = allocator()->allocate(size);
		if (!result)
		{
			throw std::bad_alloc();
		}
		return result;
	}

	static void* operator new(std::size_t size, std::nothrow_t) throw()
	{
		try
		{
			return allocator()->allocate(size);
		}
		catch (...)
		{
			return 0;
		}
	}

	static void* operator new(std::size_t, void* mem)
	{
		return mem;
	}

	static void* operator new[](std::size_t size)
	{
		void* result = allocator()->allocate(size);
		if (!result)
		{
			throw std::bad_alloc();
		}
		return result;
	}

	static void* operator new[](std::size_t size, std::nothrow_t) throw()
	{
		try
		{
			return allocator()->allocate(size);
		}
		catch (...)
		{
			return 0;
		}
	}

	static void* operator new[](std::size_t, void* mem)
	{
		return mem;
	}

	static void operator delete(void* mem, std::size_t size)
	{
		allocator()->deallocate(mem, size);
	}

	static void operator delete(void* mem, std::size_t size, std::nothrow_t)
	{
		allocator()->deallocate(mem, size);
	}

	static void operator delete(void*, std::size_t, std::nothrow_t, void*)
	{
	}

	static void operator delete[](void* mem, std::size_t size)
	{
		allocator()->deallocate(mem, size);
	}

	static void operator delete[](void* mem, std::size_t size, std::nothrow_t)
	{
		allocator()->deallocate(mem, size);
	}

	static void operator delete[](void*, std::size_t, std::nothrow_t, void*)
	{
	}

private:

	static Allocator* allocator()
	{
		return util::Singleton<Allocator, destructionPriority>::instance();
	}
};



/** @ingroup Allocator
 */
class AllocatorMalloc
{
public:
	void* allocate(size_t size)
	{
		return ::malloc(size);
	}
	void deallocate(void *mem)
	{
		::free(mem);
	}
	void deallocate(void* mem, size_t)
	{
		::free(mem);
	}
};



/** @ingroup Allocator
 *  Guard a MT unsafe allocator with some lock
 */
template
<
	typename Allocator,
	typename Lock,
	typename Locker = util::Locker<Lock>
>
class AllocatorLocked: public Allocator
{
public:
	AllocatorLocked():
		Allocator()
	{
	}
	AllocatorLocked(size_t size):
		Allocator(size)
	{
	}
	void* allocate()
	{
		Locker locker(lock_);
		return Allocator::allocate();
	}
	void* allocate(size_t size)
	{
		Locker locker(lock_);
		return Allocator::allocate(size);
	}
	void deallocate(void* mem)
	{
		Locker locker(lock_);
		Allocator::deallocate(mem);
	}
	void deallocate(void* mem, size_t size)
	{
		Locker locker(lock_);
		Allocator::deallocate(mem, size);
	}
private:
	Lock lock_;
};



/** Instantiates an Allocator per thread.
 *  @ingroup Allocator
 *	@warning it is potentially unsafe to allocate and deallocate memory in two different threads 
 *		if you're using this one!
 */
template
<
	typename Allocator
>
class AllocatorPerThread
{
public:
	AllocatorPerThread():
		allocator_()
	{
	}
	AllocatorPerThread(size_t size):
		allocator_(size)
	{
	}
	void* allocate()
	{
		return allocator_->allocate();
	}
	void* allocate(size_t size)
	{
		return allocator_->allocate(size);
	}
	void deallocate(void* mem)
	{
		return allocator_->deallocate(mem);
	}
	void deallocate(void* mem, size_t size)
	{
		return allocator_->deallocate(mem, size);
	}
private:
	util::ThreadLocalVariable<Allocator> allocator_;
};



/** A variable-size allocator built on top of a fixed-size allocator
 *  @ingroup Allocator
 *  @warning CURRENTLY THREAD UNSAFE!
 */
template <typename FixedAllocator>
class AllocatorVariableSize
{
public:
	void* allocate(size_t size)
	{
		return fixedAllocator(size).allocate();
	}
	void deallocate(void* mem, size_t size)
	{
		fixedAllocator(size).deallocate(mem);
	}
private:
	typedef std::map<size_t, FixedAllocator> TFixedAllocators;
	
	FixedAllocator& fixedAllocator(size_t size)
	{
		typename TFixedAllocators::iterator allocator = fixedAllocators_.find(size);
		if (allocator == fixedAllocators_.end())
		{
			allocator = fixedAllocators_.insert(std::make_pair(size, FixedAllocator(size))).first;
		}
		return allocator->second;
	}
	
	TFixedAllocators fixedAllocators_;
};

}
}
}

#endif

// EOF
