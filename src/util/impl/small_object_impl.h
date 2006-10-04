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
 *
 *  @par original code by Andrei Alexandrescu:
 *  The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *  This code (Loki) accompanies the book:\n
 *  Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied".
 *  Copyright (c) 2001. Addison-Wesley.\n
 *  <i>Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is
 *  hereby granted without fee, provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear in supporting documentation.\n
 *  The author or Addison-Wesley Longman make no representations about the suitability of this
 *  software (Loki) for any purpose. It is provided "as is" without express or implied warranty.</i>
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_SMALL_OBJECT_IMPL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_SMALL_OBJECT_IMPL_H

#include "../util_common.h"
#include "../non_copyable.h"
#include "../shared_ptr.h"
#include "../singleton.h"
#include "../thread.h"

//#define LASS_UTIL_SMALL_OBJECT_TRACK_ALLOCATIONS

namespace lass
{
namespace util
{
namespace impl
{

/** @internal
 *
 */
template
<
	typename IndexType
>
class FixedAllocator: public NonCopyable
{
public:

	explicit FixedAllocator(std::size_t iBlockSize, std::size_t iChunkSize);
	~FixedAllocator();

	void* allocate();
	void deallocate(void* iPointer);
	std::size_t blockSize() const { return blockSize_; }

	/** we have to use IntrusiveCounter because DefaultCounter needs
	 *	small object allocator (leading to an infinite loop).  However
	 *	this leads to an ugly referenceCount_ in the public interface of
	 *	FixedAllocator.  Luckely for us, FixedAllocator is only an implementation
	 *  detail, so it doesn't really pollute on the client side.  However,
	 *	a nicer solution might by preferable (maybe a custom CounterPolicy?)
	 */
	int uglyReferenceCount_; 

private:

	typedef IndexType TIndex;

	struct Chunk
	{
		void init(std::size_t iBlockSize, TIndex iBlocks);
		void release();
		void* allocate(std::size_t iBlockSize);
		void deallocate(void* iPointer, std::size_t iBlockSize);

		char* data_;
		TIndex firstAvailableBlock_;
		TIndex blocksAvailable_;
	};

	typedef std::vector<Chunk> TChunks;

	void doDeallocate(void* iPointer);
	Chunk* vicinityFind(void* iPointer);

	std::size_t blockSize_;
	TIndex numBlocks_;
	TChunks chunks_;
	Chunk* allocateChunk_;
	Chunk* deallocateChunk_;
};



/** @internal
 */
template
<
	std::size_t iChunkSize,
	std::size_t iMaxObjectSize,
	typename IndexType
>
class SmallObjectAllocator: public NonCopyable
{
public:

	SmallObjectAllocator();

#ifdef LASS_UTIL_SMALL_OBJECT_TRACK_ALLOCATIONS
	~SmallObjectAllocator()
	{
		if (!allocatedObjects_.empty())
		{
			std::cout << "[LASS DEBUG MSG]: Following small objects are not deallocated successfully: " 
				<< allocatedObjects_ << std::endl;
		}
	}
#endif

	void* allocate(std::size_t iSize)
	{		
		void* result = 0;
		LASS_LOCK(lock_)
		{
			result = doAllocate(iSize);
#ifdef LASS_UTIL_SMALL_OBJECT_TRACK_ALLOCATIONS
		allocatedObjects_.insert(result);
#endif
		}
		if (result == 0)
		{
			throw std::bad_alloc();
		}
		return result;
	}

	void deallocate(void* iPointer, std::size_t iSize)
	{
		LASS_LOCK(lock_)
		{
#ifdef LASS_UTIL_SMALL_OBJECT_TRACK_ALLOCATIONS
			allocatedObjects_.erase(iPointer);
#endif
			doDeallocate(iPointer, iSize);
		}
	}

private:

	typedef IndexType TIndex;
	typedef FixedAllocator<IndexType> TFixedAllocator;

	typedef util::SharedPtr<TFixedAllocator, util::ObjectStorage,
		util::IntrusiveCounter<TFixedAllocator, int, &TFixedAllocator::uglyReferenceCount_> >
		TFixedAllocatorPtr;

	class FixedAllocatorHolder
	{
	public:
		enum Dummy { dummy };

		explicit FixedAllocatorHolder(size_t blockSize):
			allocator_(new TFixedAllocator(blockSize, chunkSize_)),
			blockSize_(blockSize)
		{
		};
		FixedAllocatorHolder(size_t blockSize, Dummy):
			allocator_(0),
			blockSize_(blockSize)
		{
		};

		TFixedAllocator* operator->() const { LASS_ASSERT(allocator_); return allocator_.get(); }
		TFixedAllocator& operator*() const { LASS_ASSERT(allocator_); return *allocator_; }
		TFixedAllocator* get() { return allocator_.get(); }

		const size_t blockSize() const { return blockSize_; }

		const bool operator<(const FixedAllocatorHolder& other) const { return blockSize_ < other.blockSize_; }

	private:

		TFixedAllocatorPtr allocator_;
		size_t blockSize_;
	};

	typedef std::vector<FixedAllocatorHolder> TPool;

	enum
	{
		chunkSize_ = iChunkSize,
		maxObjectSize_ = iMaxObjectSize
	};

	void* doAllocate(std::size_t iSize);
	void doDeallocate(void* iPointer, std::size_t iSize);

	TPool pool_;
	TFixedAllocator* lastAllocate_;
	TFixedAllocator* lastDeallocate_;

	Semaphore lock_;	

#ifdef LASS_UTIL_SMALL_OBJECT_TRACK_ALLOCATIONS
	std::set<void*> allocatedObjects_;
#endif
};


/** @internal
 */
template
<
	std::size_t iChunkSize,
	std::size_t iMaxObjectSize,
	typename IndexType
>
inline SmallObjectAllocator<iChunkSize, iMaxObjectSize, IndexType>* smallObjectAllocator()
{
	typedef SmallObjectAllocator<iChunkSize, iMaxObjectSize, IndexType> TAllocator;
	return Singleton<TAllocator, 1>::instance();
}



}

}

}

#include "small_object_impl.inl"

#endif

// EOF
