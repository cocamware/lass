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
 *	There are in general two kind of allocator concepts: fixed-size and variable-size 
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
 *	(like AllocatorLocked) implement both the fixed-size and variable-size concept.
 *	Others (like AllocatorVariableSize) convert a fixed-size interface to variable-size.
 *
 *	On top of all that, there's AllocatorClassAdaptor you can use to overload class-specific
 *	new and delete using a variable-size allocator.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_ALLOCATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_ALLOCATOR_H

#include "util_common.h"
#include "atomic.h"
#include "singleton.h"
#include "thread.h"
#include "../meta/bool.h"
#include "../num/safe_bool.h"

namespace lass
{
namespace util
{
namespace impl
{
	/** @ingroup Allocator
	 *  @internal
	 */
	template <typename Allocator, typename AllocatorFun>
	struct IsCompatibleAllocator
	{
		static meta::True test(AllocatorFun);
		static meta::False test(...);
		enum { value = (sizeof(test(&Allocator::allocate)) == sizeof(meta::True)) };
		typedef typename meta::Bool<value>::Type Type;
	};

	/** @ingroup Allocator
	 *  @internal
	 */
	template <typename Allocator>
	struct IsFixedAllocator: public impl::IsCompatibleAllocator<Allocator, void*(*)()> {};
	
	/** @ingroup Allocator
	 *  @internal
	 */
	template <typename Allocator>
	struct IsVariableAllocator: public impl::IsCompatibleAllocator<Allocator, void*(*)(size_t)> {}; 
}



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
	typename VariableAllocator,
	int destructionPriority = destructionPriorityInternalAllocators
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

	static VariableAllocator* allocator()
	{
		return util::Singleton<VariableAllocator, destructionPriority>::instance();
	}
};



/** @ingroup Allocator
 *	@arg concept: FixedAllocator
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



/** Guard a MT unsafe allocator with some lock.
 *	@ingroup Allocator
 *  @arg concept: VariableAllocator or FixedAllocator, depending on template argument	
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
 *  @arg concept: VariableAllocator or FixedAllocator, depending on template argument
 *	@arg requirements: Allocator must be copyconstructible
 *	@warning it is potentially unsafe to allocate and deallocate memory in two different threads 
 *		if you're using this one!  This depends on underlying Allocator
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
 *  @arg concept: VariableAllocator
 *  @warning CURRENTLY THREAD UNSAFE!
 */
template 
<
	typename FixedAllocator
>
class AllocatorVariable
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
			allocator = fixedAllocators_.insert(
				std::make_pair(size, FixedAllocator(size))).first;
		}
		return allocator->second;
	}
	
	TFixedAllocators fixedAllocators_;
};



/** fixes a variable-size allocator to one size.
 */
template
<
	typename VariableAllocator = AllocatorMalloc
>
class AllocatorFixed: private VariableAllocator
{
public:
	AllocatorFixed(size_t size): 
		size_(size)
	{
	}
	void* allocate()
	{
		return VariableAllocator::allocate(size_);
	}
	void deallocate(void* mem)
	{
		VariableAllocator::deallocate(mem, size_);
	}
private:
	size_t size_;
};



/** @ingroup Allocator
 */
struct BinnerOne
{
	static size_t bin(size_t size)
	{
		return size > 0 ? size - 1 : 0;
	}
	static size_t size(size_t bin)
	{
		return bin + 1;
	}
};

/** @ingroup Allocator
 */
struct BinnerPower2
{
	static size_t bin(size_t size)
	{
		size_t bin = 0;
		while (size > BinnerPower2::size(bin))
		{
			++bin;
		}
		return bin;
	}
	static size_t size(size_t bin)
	{
		return 1 << bin;
	}
};

/** @ingroup Allocator
 */
template <size_t multiple>
struct BinnerPadded
{
	static size_t bin(size_t size)
	{
		return size > 0 ? (size - 1) / multiple : 0;
	}
	static size_t size(size_t bin)
	{
		return (bin + 1) * multiple;
	}
};

/** @ingroup Allocator
 */
template
<
	typename FixedAllocator,
	size_t maxBinSize = 64,
	typename Binner = BinnerOne,
	typename VariableAllocator = AllocatorMalloc
>
class AllocatorBinned: public VariableAllocator
{
public:
	AllocatorBinned()
	{
		initFixed();
	}
	AllocatorBinned(const AllocatorBinned& other)
	{
		copyInitFixed(other);
	}
	~AllocatorBinned()
	{
		destroyFixed(numBins());
	}
	void* allocate(size_t size)
	{
		if (size > maxBinSize)
		{
			return VariableAllocator::allocate(size);
		}
		return fixedAllocators_[Binner::bin(size)].allocate();
	}
	void deallocate(void* mem, size_t size)
	{
		if (size > maxBinSize)
		{
			VariableAllocator::deallocate(mem, size);
		}
		else
		{
			fixedAllocators_[Binner::bin(size)].deallocate(mem);
		}
	}
private:
	AllocatorBinned& operator=(const AllocatorBinned&);
	
	const size_t numBins() const
	{
		return Binner::bin(maxBinSize) + 1;
	}
	void allocFixed()
	{
		fixedAllocators_ = static_cast<FixedAllocator*>(
			VariableAllocator::allocate(numBins() * sizeof(FixedAllocator)));
		if (!fixedAllocators_)
		{
			throw std::bad_alloc();
		}
	}
	void initFixed()
	{
		// Kids, don't try this at home.  We're trained professionals here! ;)
		//
		allocFixed();
		const size_t n = numBins();
		for (size_t i = 0; i < n; ++i)
		{
			try
			{
				new(&fixedAllocators_[i]) FixedAllocator(Binner::size(i));
			}
			catch (...)
			{
				destroyFixed(i);
				throw;
			}
		}
	}
	void copyInitFixed(const AllocatorBinned& other)
	{
		// Kids, don't try this either.
		//
		allocFixed();
		const size_t n = numBins();
		for (size_t i = 0; i < n; ++i)
		{
			try
			{
				new(&fixedAllocators_[i]) FixedAllocator(other.fixedAllocators_[i]);
			}
			catch (...)
			{
				destroyFixed(i);
				throw;
			}
		}
	}
	void destroyFixed(size_t i)
	{
		// and neither this.
		//
		while (i > 0)
		{
			fixedAllocators_[--i].~FixedAllocator();
		}
		VariableAllocator::deallocate(fixedAllocators_, numBins() * sizeof(FixedAllocator));
	}

	FixedAllocator* fixedAllocators_;
};



/** @ingroup Allocator
 *	@arg concept: FixedAllocator except for the constructor.
 */
template
<
	typename FixedAllocator,
	size_t size
>
class AllocatorStaticFixed: public FixedAllocator
{
public:
	AllocatorStaticFixed():
		FixedAllocator(size)
	{
	}
};



/** @ingroup Allocator
 *	@arg concept: same as Allocator
 */
template
<
	typename Allocator
>
class AllocatorThrow: public Allocator
{
public:
	AllocatorThrow():
		Allocator()
	{
	}
	AllocatorThrow(size_t size):
		Allocator(size)
	{
	}
	void* allocate()
	{
		if (void* p = Allocator::allocate())
		{
			return p;
		}
		throw std::bad_alloc();
	}
	void* allocate(size_t size)
	{
		if (void* p = Allocator::allocate(size))
		{
			return p;
		}
		throw std::bad_alloc();
	}
};



/** @ingroup Allocator
 *	@arg concept: same as Allocator
 */
template
<
	typename Allocator
>
class AllocatorNoThrow: public Allocator
{
public:
	AllocatorNoThrow():
		Allocator()
	{
	}
	AllocatorNoThrow(size_t size):
		Allocator(size)
	{
	}
	void* allocate()
	{
		try
		{
			return Allocator::allocate();
		}
		catch (...)
		{
			return 0;
		}
	}
	void* allocate(size_t size)
	{
		try
		{
			return Allocator::allocate(size);
		}
		catch (...)
		{
			return 0;
		}
	}
};



/** @ingroup Allocator
 *	@arg concept: same as Allocator, except for the constructor if Allocator == FixedAllocator.
 */
template
<
	typename VariableAllocator,
	int destructionPriority = destructionPriorityInternalAllocators
>
class AllocatorSingleton
{
public:
	void* allocate()
	{
		return allocator()->allocate();
	}
	void* allocate(size_t size)
	{
		return allocator()->allocate(size);
	}
	void deallocate(void* mem)
	{
		return allocator()->deallocate(mem);
	}
	void deallocate(void* mem, size_t size)
	{
		return allocator()->deallocate(mem, size);
	}
	VariableAllocator* allocator()
	{
		return util::Singleton<VariableAllocator, destructionPriority>::instance();
	}
};



/** @ingroup Allocator
 *	@warning THREAD UNSAFE!
 */
template
<
	typename Allocator
>
class AllocatorStats: public Allocator
{
public:
	AllocatorStats():
		Allocator()
	{
	}
	AllocatorStats(size_t size):
		Allocator(size)
	{
		stats_.insert(std::make_pair(size, Stat()));
	}
	~AllocatorStats()
	{
		LASS_CLOG << "~" << typeid(AllocatorStats).name() << "()" << std::endl;
		LASS_CLOG << "size: allocations/deallocations" << std::endl;
		for (typename TStats::const_iterator i = stats_.begin(); i != stats_.end(); ++i)
		{
			Stat stat = i->second;
			LASS_CLOG << i->first << ": " << stat.allocations << "/" << stat.deallocations << std::endl;
			if (stat.allocations != stat.deallocations)
			{
				LASS_CERR << "[LASS RUN MSG] Allocation unbalance detected in AllocatorStats" << std::endl;
			}
		}
	}
	void* allocate(size_t size)
	{
		++stats_[size].allocations;
		return Allocator::allocate(size);
	}
	void* allocate()
	{
		++stats_.begin()->second.allocations;
		return Allocator::allocate();
	}
	void deallocate(void* mem, size_t size)
	{
		++stats_[size].deallocations;
		Allocator::deallocate(mem, size);
	}
	void deallocate(void* mem)
	{
		++stats_.begin()->second.deallocations;
		Allocator::deallocate(mem);
	}
private:
	struct Stat
	{
		unsigned allocations;
		unsigned deallocations;
	};
	typedef std::map<size_t, Stat> TStats;
	TStats stats_;
};





/** A fixed-size free-list allocator
 *	@ingroup Allocator
 *	@arg concept: FixedAllocator
 *	@arg thread UNSAFE
 *	@arg copy-constructible, not assignable
 */
template 
<
	typename FixedAllocator = AllocatorFixed<AllocatorMalloc>
>
class AllocatorFreeList: public FixedAllocator
{
public:
	AllocatorFreeList(size_t iSize):
		FixedAllocator(std::max<size_t>(sizeof(AllocationNode),iSize)),
		top_(0)
	{
	}
	~AllocatorFreeList()
	{
		while (top_)
		{
			AllocationNode* node = top_;
			top_ = node->next;
			FixedAllocator::deallocate(node);
		}
	}
	AllocatorFreeList(const AllocatorFreeList& iOther):
		FixedAllocator(static_cast<const FixedAllocator&>(iOther)),
		top_(0)
	{
	}
	void* allocate()
	{
		if (!top_)
		{
			return FixedAllocator::allocate();
		}
		AllocationNode* topNode = top_;
		top_ = topNode->next;
		return topNode;
	}
	void deallocate(void* iPointer)
	{
		if (!iPointer)
			return;
		AllocationNode* temp = static_cast<AllocationNode*>(iPointer);
		temp->next = top_;
		top_ = temp;
	}
private:
	AllocatorFreeList& operator=(AllocatorFreeList&);

	struct AllocationNode
	{
		AllocationNode* next;
	};
	AllocationNode* top_;
};

template <typename T>
class TaggedPtr
{
public:
#if LASS_ACTUAL_ADDRESS_SIZE == 48
	// We're in 64 bit space, but we don't have 128 bit CAS to do tagging ... Help!
	// Luckely, only the least significant 48 bits of a pointer are really used.
	// So, we have 16 bits we can fiddle with.  Should be enough for a counting tag.
	// We store the the pointer in the most significant part for two reasons:
	//	- order of the pointers is somewhat preserved
	//	- we can use SAR to restore the sign bit.
	//
	typedef num::Tuint16 TTag;
	TaggedPtr(): bits_(0) {}
	TaggedPtr(T* ptr, TTag tag): bits_((reinterpret_cast<num::Tuint64>(ptr) << 16) | tag) {}
	T* const get() const 
	{
#	if defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		T* ptr;
		__asm__ __volatile__("sarq $16, %0;" : "=q"(ptr) : "0"(bits_) : "cc");
		return ptr;
#	elif defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC)
#		error Not implemented yet ...
#	else
		return (bits_ & 0xa000000000000000 == 0) ?
			reinterpret_cast<T*>(bits_ >> 16) :
			reinterpret_cast<T*>((bits_ >> 16) | 0xffff000000000000);
#	endif
	}
	const TTag tag() const { return static_cast<TTag>(bits_ & 0xffff); }
	const bool operator==(const TaggedPtr& other) const { return bits_ == other.bits_; }
	bool atomicCompareAndSwap(const TaggedPtr& expected, const TaggedPtr& fresh)
	{
		return util::atomicCompareAndSwap(bits_, expected.bits_, fresh.bits_);
	}
private:
	num::Tuint64 bits_;
#else
	typedef num::TuintPtr TTag;
	TaggedPtr(): ptr_(0), tag_(0) {}
	TaggedPtr(T* ptr, TTag tag): ptr_(ptr), tag_(tag) {}
	T* const get() const { return ptr_; }
	const TTag tag() const { return tag_; }
	bool operator==(const TaggedPtr& other) const {	return ptr_ == other.ptr_ && tag_ == other.tag_; }
	bool atomicCompareAndSwap(const TaggedPtr& expected, const TaggedPtr& fresh)
	{
		return util::atomicCompareAndSwap(
			ptr_, expected.ptr_, expected.tag_, fresh.ptr_, fresh.tag_);
	}
private:
	T* ptr_;
	TTag tag_;
#endif
public:
	T* const operator->() const { LASS_ASSERT(get()); return get(); }
	const bool operator!() const { return get() == 0; }
	operator num::SafeBool() const { return get() ? num::safeTrue : num::safeFalse; }
};




/** A fixed-size lock-free free-list allocator
 *	@ingroup Allocator
 *	@arg concept: FixedAllocator
 *	@arg thread safe, lock-free
 *	@arg copy-constructible, not assignable
 */
template 
<
	typename FixedAllocator = AllocatorFixed<AllocatorMalloc>
>
class AllocatorConcurrentFreeList: public FixedAllocator
{
public:
	AllocatorConcurrentFreeList(size_t iSize):
		FixedAllocator(std::max<size_t>(sizeof(AllocationNode),iSize)),
		top_()
	{
	}
	~AllocatorConcurrentFreeList()
	{
		AllocationNode* top = top_.get();
		while (top)
		{
			AllocationNode* node = top;
			top = node->next;
			FixedAllocator::deallocate(node);
		}
	}
	AllocatorConcurrentFreeList(const AllocatorConcurrentFreeList& iOther):
		FixedAllocator(static_cast<const FixedAllocator&>(iOther)),
		top_()
	{
	}
	void* allocate()
	{
		TTaggedPtr topNode;
		TTaggedPtr next;
		do
		{
			topNode = top_;
			if (!topNode)
			{
				return FixedAllocator::allocate();
			}
			next = TTaggedPtr(topNode->next, topNode.tag() + 1);
		}
		while (!top_.atomicCompareAndSwap(topNode, next));
		return topNode.get();
	}
	void deallocate(void* iPointer)
	{
		if (!iPointer)
			return;
		AllocationNode* temp = static_cast<AllocationNode*>(iPointer);
		TTaggedPtr topNode;
		TTaggedPtr newTop;
		do
		{
			topNode = top_;
			temp->next = topNode.get();
			newTop = TTaggedPtr(temp, topNode.tag() + 1);
		}
		while (!top_.atomicCompareAndSwap(topNode, newTop));
	}
private:
	struct AllocationNode
	{
		AllocationNode* next;
	};
	
	typedef TaggedPtr<AllocationNode> TTaggedPtr;

	AllocatorConcurrentFreeList& operator=(const AllocatorConcurrentFreeList&);

	TTaggedPtr top_;
};

}
}

#endif

// EOF
