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
 *	@arg concept: VariableAllocator
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
		return size_t(1) << bin;
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
	size_t maxBinSize = 128,
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
		TTaggedPtr top = top_;
		AllocationNode* node = top.get();
		while (node)
		{
			AllocationNode* next = node->next;
			FixedAllocator::deallocate(node);
			node = next;
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

	volatile TTaggedPtr top_;
};



/** A simple fixed-size block allocator
 *	@ingroup Allocator
 *	@arg concept: FixedAllocator
 *	@arg thread UNSAFE.
 *	@arg not copy-constructible, not assignable
 *  @arg The blocks are only deallocated at destruction time to simplify the allocation/deallocation logic.
 *      However, nodes that are deallocate can still be reused.
 *  @arg It is NOT SAFE to deallocate memory using another instance of AllocatorSimpleBlock than the one
 *      used to allocate it.
 */
template 
<
	size_t requestedBlockSize = 8192,
	typename FixedAllocator = AllocatorFixed<AllocatorMalloc>
>
class AllocatorSimpleBlock: public FixedAllocator
{
public:
	AllocatorSimpleBlock(size_t size):
		FixedAllocator(blockSize(size, 0, 0)),
		blocks_(0),
		pool_(0)
	{
		blockSize(size, &size_, &allocationsPerBlock_);
	}
	~AllocatorSimpleBlock()
	{
		while (blocks_)
		{
			Node* const node = blocks_;
			blocks_ = node->next;
			FixedAllocator::deallocate(node);
		}
	}
	void* allocate()
	{
		if (!pool_ && !grow())
		{
			return 0;
		}
		LASS_ASSERT(pool_);
		Node* p = pool_;
		pool_ = p->next;
		return p;
	}
	void deallocate(void* pointer)
	{
		Node* p = static_cast<Node*>(pointer);
		p->next = pool_;
		pool_ = p;
	}

private:
	
	struct Node
	{
		Node* next;
	};

	const bool grow()
	{
		char* mem = static_cast<char*>(FixedAllocator::allocate());
		if (!mem)
		{
			return false;
		}
		Node* const block = reinterpret_cast<Node*>(mem);
		block->next = blocks_;
		blocks_ = block;
		mem += sizeof(Node);
		for (size_t i = 0; i < allocationsPerBlock_; ++i)
		{
			deallocate(mem); // pushes in pool
			mem += size_;
		}
		return true;
	}
	static const size_t blockSize(size_t size, size_t* pSize, size_t* pAllocationsPerBlock)
	{
		size = std::max(sizeof(Node), size);
		const size_t maxBlockSize = std::max(sizeof(Node), requestedBlockSize);
		const size_t allocationsPerBlock = std::max(size_t(1), (maxBlockSize - sizeof(Node)) / size);
		const size_t blockSize = sizeof(Node) + allocationsPerBlock * size;
		if (pSize) *pSize = size;
		if (pAllocationsPerBlock) *pAllocationsPerBlock = allocationsPerBlock;
		return blockSize;
	}

	AllocatorSimpleBlock(const AllocatorSimpleBlock&);
	AllocatorSimpleBlock& operator=(const AllocatorSimpleBlock&);

	Node* blocks_;
	Node* pool_;
	size_t size_;
	size_t allocationsPerBlock_;
};



/** @ingroup Allocator
 *	@arg concept: VariableAllocator
 *
 *	AllocatorAligned adds @a alignment bytes to the requested block size to be able to shift
 *	the pointer to a alignment boundary.
 */
template
<
	unsigned char alignment,
	typename VariableAllocator = AllocatorMalloc
>
class AllocatorAligned: public VariableAllocator
{
public:
	void* allocate(size_t size)
	{
		return align(VariableAllocator::allocate(size + alignment));
	}
	void deallocate(void* mem, size_t size)
	{
		VariableAllocator::deallocate(unalign(mem), size + alignment);
	}
private:
	void* align(void* mem)
	{
		const num::TuintPtr address = reinterpret_cast<num::TuintPtr>(mem);
		const unsigned char offset = alignment - (address % alignment);
		LASS_ASSERT(offset > 0);
		unsigned char* const aligned = static_cast<unsigned char*>(mem) + offset;
		*(aligned - 1) = offset;
		return aligned;
	}
	void* unalign(void* mem)
	{
		unsigned char* const aligned = static_cast<unsigned char*>(mem);
		const unsigned char offset = *(aligned - 1);
		return aligned - offset;
	}
};



/** @ingroup Allocator
 *	@arg concept: VariableAllocator
 *
 *	AllocatorSized adds sizeof(size_t) bytes in front of the returned pointer to store the
 *	requested block size.  Because of that, you can use the simpler form 
 *  <tt>void deallocate(void* mem)</tt> to deallocate the memory.
 *
 *  @warning the fact that this allocator supports <tt>void deallocate(void* mem)</tt> does
 *		@b NOT mean it implements the FixedAllocator concept, not even partially.  So when
 *		you use AllocatorSized as a base for an allocator that implements both the 
 *		VariableAllocator as FixedAllocator concept depending on the base allocator (such as
 *		AllocatorStats), the result will be a VariableAllocator.  You cannot generally assume 
 *		you will be able to use <tt>void deallocate(void* mem)</tt> on the resulting allocator
 *		as well.  Particularly for AllocatorStats, it is know that this will cause undefined
 *		behaviour.  However, using AllocatorStats as base AllocatorSized will achieve the 
 *		desired effect perfectly well, and will cause no problem (= using AllocatorSized
 *		on top-level)
 *
 *	<b>it is advised to use AllocatorSized as a top-level allocator.  Using AllocatorSized
 *	as a base allocator for others can cause undefined behaviour.  A notable exception is
 *	AllocatorSingleton, it is safe to use that one on top of AllocatorSized.</b>
 */
template
<
	typename VariableAllocator = AllocatorMalloc
>
class AllocatorSized: public VariableAllocator
{
public:
	void* allocate(size_t size)
	{
		size_t* p = static_cast<size_t*>(VariableAllocator::allocate(size + sizeof(size_t)));
		*p = size;
		return p + 1;
	}
	void deallocate(void* mem, size_t size)
	{
		size_t* p = static_cast<size_t*>(mem) - 1;
		LASS_ASSERT(*p == size);
		VariableAllocator::deallocate(p, size + sizeof(size_t));
	}
	void deallocate(void* mem)
	{
		size_t* p = static_cast<size_t*>(mem) - 1;
		VariableAllocator::deallocate(p, *p + sizeof(size_t));
	}
};

}
}

#endif

// EOF
