/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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

namespace lass
{
namespace util
{
namespace impl
{

/** @internal
 */
template
<
	typename AtomType
>
class FixedAllocator: public NonCopyable
{
public:

	explicit FixedAllocator(std::size_t iBlockSize, std::size_t iChunkSize);
	~FixedAllocator();

	void* allocate();
	void deallocate(void* iPointer);
	std::size_t blockSize() const { return blockSize_; }

private:

	typedef AtomType TAtom;

	struct Chunk
	{
		void init(std::size_t iBlockSize, TAtom iBlocks);
		void release();
		void* allocate(std::size_t iBlockSize);
		void deallocate(void* iPointer, std::size_t iBlockSize);

		TAtom* data_;
		TAtom firstAvailableBlock_;
		TAtom blocksAvailable_;
	};

	typedef std::vector<Chunk> TChunks;

	void doDeallocate(void* iPointer);
	Chunk* vicinityFind(void* iPointer);

	std::size_t blockSize_;
	TAtom numBlocks_;
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
	typename AtomType
>
class SmallObjectAllocator: public NonCopyable
{
public:

	SmallObjectAllocator();

	void* allocate(std::size_t iSize);
	void deallocate(void* iPointer, std::size_t iSize);

private:

	typedef FixedAllocator<AtomType> TFixedAllocator;
	typedef SharedPtr<TFixedAllocator> TFixedAllocatorPtr;
	typedef std::vector<TFixedAllocatorPtr> TPool;

	struct CompareFixedAllocatorSize:
		public std::binary_function<const TFixedAllocatorPtr&, std::size_t, bool>
	{
		bool operator()(const TFixedAllocatorPtr& iAllocator, std::size_t iNumBytes) const
		{
			return iAllocator->blockSize() < iNumBytes;
		}
	};

	enum
	{
		chunkSize_ = iChunkSize,
		maxObjectSize_ = iMaxObjectSize
	};

	TPool pool_;
	TFixedAllocator* lastAllocate_;
	TFixedAllocator* lastDeallocate_;
};


/** @internal
 */
template
<
	std::size_t iChunkSize,
	std::size_t iMaxObjectSize,
	typename AtomType
>
inline SmallObjectAllocator<iChunkSize, iMaxObjectSize, AtomType>* smallObjectAllocator()
{
	typedef SmallObjectAllocator<iChunkSize, iMaxObjectSize, AtomType> TAllocator;
	return Singleton<TAllocator, 1>::instance();
}



}

}

}

#include "small_object_impl.inl"

#endif

// EOF
