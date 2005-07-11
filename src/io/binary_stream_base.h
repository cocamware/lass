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



/** @class lass::io::BinaryStreamBase
 *  @brief Base class for LASS binary streams with byte-order househoulding
 *
 *  This class is only to provide functionality to whatever stream derives from it.
 *  You're not supposed to actually "use" it.  "use" it as in having BinaryStream pointers
 *  or references floating in your code.  For that matter, the destructor of BinaryStream
 *  is protected so life gets a bit troubled if you decide to try it anyway.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_STREAM_BASE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_STREAM_BASE_H



#include "io_common.h"
#include "stream_base.h"
#include "../num/endianness.h"

namespace lass
{

namespace io
{

class LASS_DLL BinaryStreamBase: public StreamBase
{
public:

	num::Endianness endianness() const { return streamEndianness_; }
	void setEndianness(num::Endianness iEndianness) { streamEndianness_ = iEndianness; }

protected:

	BinaryStreamBase(num::Endianness iEndianness = num::littleEndian): 
		 StreamBase(),
		 streamEndianness_(iEndianness)
	{
	}

	~BinaryStreamBase() {}

	void fixEndianness(void* ioBuffer, size_t iNumberOfBytes) 
	{
		if (streamEndianness_ != num::systemEndian)
		{
			std::reverse(static_cast<num::Tuint8*>(ioBuffer), 
				static_cast<num::Tuint8*>(ioBuffer) + iNumberOfBytes);
		}
	}

private:

	num::Endianness streamEndianness_;	
};

}

}

#endif
