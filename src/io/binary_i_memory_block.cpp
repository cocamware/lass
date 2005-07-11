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
 */

#include "io_common.h"
#include "binary_i_memory_block.h"

namespace lass
{

namespace io
{

/** Construct an "empty" closed stream..
 */
BinaryIMemoryBlock::BinaryIMemoryBlock():
	BinaryIStream(),
	begin_(0),
	size_(0),
	position_(0)
{
}



/** Construct stream by pointer to memory block and length of block
 */
BinaryIMemoryBlock::BinaryIMemoryBlock( const void* iBegin, long iSize ):
	BinaryIStream(),
	begin_(static_cast<const TByte*>(iBegin)),
	size_(iSize),
	position_(0)
{
	LASS_ASSERT(size_ >= 0);
}



/** Construct stream by pointer to memory block and to one byte past end of block.
 */
BinaryIMemoryBlock::BinaryIMemoryBlock( const void* iBegin, const void* iEnd ):
	BinaryIStream(),
	begin_(static_cast<const TByte*>(iBegin)),
	size_(static_cast<long>(static_cast<const TByte*>(iEnd) - static_cast<const TByte*>(iBegin))),
	position_(0)
{
	LASS_ASSERT(size_ >= 0);
}



// --- private -------------------------------------------------------------------------------------

void BinaryIMemoryBlock::doRead(void* oOutput, size_t iNumberOfBytes)
{
	if (!begin_)
	{
		setstate(std::ios_base::failbit);
		return;
	}
	if (!good())
	{
		return;
	}

	LASS_ASSERT(static_cast<long>(iNumberOfBytes) >= 0);
	const long next = position_ + static_cast<long>(iNumberOfBytes);
	if (next > size_ || next < position_)
	{
		setstate(std::ios_base::eofbit | std::ios_base::failbit);
	}
	else
	{
		memcpy(oOutput, &begin_[position_], iNumberOfBytes);
		position_ = next;
	}
}



long BinaryIMemoryBlock::doTellg() const
{
	return position_;
}



void BinaryIMemoryBlock::doSeekg(long iOffset, std::ios_base::seekdir iDirection)
{
	switch (iDirection)
	{
	case std::ios_base::beg: 
		position_ = iOffset;
		break;
	case std::ios_base::cur:
		position_ += iOffset;
		break;
	case std::ios_base::end:
		position_ = size_ + iOffset;
		break;
	default:
		LASS_ASSERT(false);
	};
}

}

}
