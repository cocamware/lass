/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 *
 *  original code by Andrei Alexandrescu:\n
 *  <i>
 *  The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *  This code (Loki) accompanies the book:\n
 *  Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied". 
 *  Copyright (c) 2001. Addison-Wesley.\n
 *  Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is 
 *  hereby granted without fee, provided that the above copyright notice appear in all copies and 
 *  that both that copyright notice and this permission notice appear in supporting documentation.\n
 *  The author or Addison-Wesley Longman make no representations about the suitability of this 
 *  software (Loki) for any purpose. It is provided "as is" without express or implied warranty.\n
 *  </i>
 */



#include "util_common.h"
#include "small_object_impl.h"
#include "../../num/num_traits.h"

namespace lass
{
namespace util
{
namespace impl
{

// --- FixedAllocator ------------------------------------------------------------------------------

FixedAllocator::FixedAllocator(std::size_t iBlockSize, std::size_t iChunkSize):
	blockSize_(iBlockSize),
	allocateChunk_(0),
	deallocateChunk_(0)
{
	LASS_ASSERT(blockSize_ > 0);

	std::size_t numBlocks = iChunkSize / blockSize_;
	if (numBlocks > num::NumTraits<TByte>::max)
	{
		numBlocks = num::NumTraits<TByte>::max;
	}
	else if (numBlocks == 0)
	{
		numBlocks = 8; // was 8 * iBlockSize [BdG]
	}

	numBlocks_ = static_cast<TByte>(numBlocks);
	LASS_ASSERT(numBlocks_ == numBlocks);
}



FixedAllocator::~FixedAllocator()
{
	for (TChunks::iterator cit = chunks_.begin(); cit != chunks_.end(); ++cit)
	{
		LASS_ASSERT(cit->blocksAvailable_ == numBlocks_);
		cit->release();
	}
}



/** allocates a block of fixed size
 */
void* FixedAllocator::allocate()
{
	if (allocateChunk_ == 0 || allocateChunk_->blocksAvailable_ == 0)
	{
		// no available memory in this chunk, try to find one
		//
		allocateChunk_ = 0;
		for (TChunks::iterator cit = chunks_.begin(); cit != chunks_.end(); ++cit)
		{
			if (cit->blocksAvailable_ > 0)
			{
				// found a chunk
				//
				allocateChunk_ = &*cit;
				break;
			}
		}
		if (!allocateChunk_)
		{
			// all filled up, add a new chunk
			//
			chunks_.reserve(chunks_.size() + 1);
			Chunk newChunk;
			newChunk.init(blockSize_, numBlocks_);
			chunks_.push_back(newChunk);
			allocateChunk_ = &chunks_.back();
			deallocateChunk_ = &chunks_.front();
		}
	}

	LASS_ASSERT(allocateChunk_ != 0);
	LASS_ASSERT(allocateChunk_->blocksAvailable_ > 0);
	return allocateChunk_->allocate(blockSize_);
}



/** deallocates a block previously allocated with @a allocate
 *  @warning undefined behavious if called with the wrong pointer
 */
void FixedAllocator::deallocate(void* iPointer)
{
	LASS_ASSERT(!chunks_.empty());
	LASS_ASSERT(&chunks_.front() <= deallocateChunk_);
	LASS_ASSERT(&chunks_.back() >= deallocateChunk_);

	deallocateChunk_ = vicinityFind(iPointer);
	LASS_ASSERT(deallocateChunk_);

	doDeallocate(iPointer);
}



/** finds the chunk corresponding to a pointer, using an efficient search
 */
FixedAllocator::Chunk* FixedAllocator::vicinityFind(void* iPointer)
{
	LASS_ASSERT(!chunks_.empty());
	LASS_ASSERT(deallocateChunk_);

	const std::size_t chunkLength = numBlocks_ * blockSize_;

	Chunk* low = deallocateChunk_;
	Chunk* high = deallocateChunk_ + 1;
	Chunk* lowBound = &chunks_.front();
	Chunk* highBound = &chunks_.back() + 1;

	// special case: deallocateChunk_ is the last in the array
	if (high == highBound)
	{
		high = 0;
	}

	while (true)
	{
		if (low)
		{
			if (iPointer >= low->data_ && iPointer < low->data_ + chunkLength)
			{
				return low;
			}
			if (low == lowBound)
			{
				low = 0;
			}
			else
			{
				--low;
			}
		}

		if (high)
		{
			if (iPointer >= high->data_ && iPointer < high->data_ + chunkLength)
			{
				return high;
			}
			++high;
			if (high == highBound)
			{
				high = 0;
			}
		}
	}
	LASS_ASSERT(false);
	return 0;
}



/** performs deallocation.
 *  @pre assumes @a deallocChunk_ points to the correct chunk
 */
void FixedAllocator::doDeallocate(void* iPointer)
{
	LASS_ASSERT(deallocateChunk_->data_ <= iPointer);
	LASS_ASSERT(deallocateChunk_->data_ + numBlocks_ * blockSize_ > iPointer);

	// call into the chunk, will adjust the inner list but won't release memory
	deallocateChunk_->deallocate(iPointer, blockSize_);

	if (deallocateChunk_->blocksAvailable_ == numBlocks_)
	{
		// deallocateChunk_ is completely free, should we release it?
		//
		Chunk& lastChunk = chunks_.back();
		if (&lastChunk == deallocateChunk_)
		{
			// check if we have two last chunks empty
			//
			if (chunks_.size() > 1 && deallocateChunk_[-1].blocksAvailable_ == numBlocks_)
			{
				// two free chunks, discard the last one
				//
				lastChunk.release();
				chunks_.pop_back();
				allocateChunk_ = deallocateChunk_ = &chunks_.front();
			}
			return;
		}

		if (lastChunk.blocksAvailable_ == numBlocks_)
		{
			// two free blocks, discard one
			//
			lastChunk.release();
			chunks_.pop_back();
			allocateChunk_ = deallocateChunk_;
		}
		else
		{
			// move the empty chunk to the end
			//
			std::swap(*deallocateChunk_, lastChunk);
			allocateChunk_ = &chunks_.back();
		}
	}
}



// --- FixedAllocator::Chunk -----------------------------------------------------------------------

/** initializes a chunk object
 */
void FixedAllocator::Chunk::init(std::size_t iBlockSize, unsigned iBlocks)
{
	LASS_ASSERT(iBlockSize > 0);
	LASS_ASSERT(iBlocks > 0);
	LASS_ASSERT((iBlockSize * iBlocks) / iBlockSize == iBlocks); // Overflow check

	firstAvailableBlock_ = 0;
	blocksAvailable_ = iBlocks;

	data_ = new TByte[iBlockSize * iBlocks];
	TByte* p = data_;
	for (TByte i = 0; i != iBlocks; p += iBlockSize)
	{
		*p = ++i;
	}
}



/** releases the data managed by a chunk
 */
void FixedAllocator::Chunk::release()
{
	delete [] data_;
}



/** allocated a block from a chunk
 */
void* FixedAllocator::Chunk::allocate(std::size_t iBlockSize)
{
	if (!blocksAvailable_)
	{
		return 0;
	}

	TByte* result = data_ + (firstAvailableBlock_ * iBlockSize);

	// update firstAvailableBlock_ to point to the next block
	//
	firstAvailableBlock_ = *result;
	--blocksAvailable_;

	return result;
}



/** deallocates a block from a chunk 
 */
void FixedAllocator::Chunk::deallocate(void* iPointer, std::size_t iBlockSize)
{
	LASS_ASSERT(iPointer >= data_);
	TByte* toRelease = static_cast<TByte*>(iPointer);
	LASS_ASSERT((toRelease - data_) % iBlockSize == 0); // alignment check
		
	*toRelease = firstAvailableBlock_;
	firstAvailableBlock_ = static_cast<TByte>((toRelease - data_) / iBlockSize);
	LASS_ASSERT(firstAvailableBlock_ == (toRelease - data_) / iBlockSize);

	++blocksAvailable_;
}



// --- SmallObjectAllocator ------------------------------------------------------------------------

SmallObjectAllocator::SmallObjectAllocator():
	lastAllocate_(0),
	lastDeallocate_(0)
{
}



void* SmallObjectAllocator::allocate(std::size_t iSize)
{
	if (iSize > maxObjectSize_) 
	{
		return operator new(iSize);
	}

	if (lastAllocate_ && lastAllocate_->blockSize() == iSize)
	{
		return lastAllocate_->allocate();
	}

	TPool::iterator i = 
		std::lower_bound(pool_.begin(), pool_.end(), iSize, CompareFixedAllocatorSize());

	if (i == pool_.end() || (*i)->blockSize() != iSize)
	{
		i = pool_.insert(i, TFixedAllocatorPtr(new FixedAllocator(iSize, chunkSize_)));
		lastDeallocate_ = pool_.begin()->get();
	}

	lastAllocate_ = i->get();
	return lastAllocate_->allocate();
}



void SmallObjectAllocator::deallocate(void* iPointer, std::size_t iSize)
{
    if (iSize > maxObjectSize_) 
	{
		return operator delete(iPointer);
	}

    if (lastDeallocate_ && lastDeallocate_->blockSize() == iSize)
    {
        lastDeallocate_->deallocate(iPointer);
        return;
    }

    TPool::iterator pit = 
		std::lower_bound(pool_.begin(), pool_.end(), iSize, CompareFixedAllocatorSize());
    LASS_ASSERT(pit != pool_.end());
    LASS_ASSERT((*pit)->blockSize() == iSize);
    lastDeallocate_ = pit->get();
    lastDeallocate_->deallocate(iPointer);
}



// --- free ----------------------------------------------------------------------------------------

}

}

}

// EOF
