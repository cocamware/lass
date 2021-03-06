/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#include "lass_common.h"
#include "binary_i_memory_block.h"
#include <string.h>

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

size_t BinaryIMemoryBlock::doRead(void* out, size_t numberOfBytes)
{
	if (!begin_)
	{
		setstate(std::ios_base::failbit);
		return 0;
	}
	if (!good())
	{
		return 0;
	}
	if (position_ >= size_)
	{
		setstate(std::ios_base::eofbit);
		return 0;
	}
	LASS_ASSERT(static_cast<long>(numberOfBytes) >= 0);
	long next = position_ + static_cast<long>(numberOfBytes);
	if (next > size_ || next < position_)
	{
		next = size_;
		numberOfBytes = static_cast<size_t>(size_ - position_);
	}
	memcpy(out, &begin_[position_], numberOfBytes);
	position_ = next;
	return numberOfBytes;
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
