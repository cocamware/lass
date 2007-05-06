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

#include "io_common.h"
#include "binary_i_stream.h"
#include "../num/basic_types.h"

namespace lass
{

namespace io
{

BinaryIStream::BinaryIStream():
	BinaryStreamBase()
{
}



BinaryIStream::~BinaryIStream()
{
}



long BinaryIStream::tellg() const
{
	return doTellg();
}



BinaryIStream& BinaryIStream::seekg(long position)
{
	doSeekg(position, std::ios_base::beg);
	return *this;
}



BinaryIStream& BinaryIStream::seekg(long offset, std::ios_base::seekdir directioin)
{
	doSeekg(offset, directioin);
	return *this;
}



#define LASS_IO_BINARY_I_STREAM_EXTRACTOR( type )\
BinaryIStream& BinaryIStream::operator>>( type & x )\
{\
	if (good())\
	{\
		type temp;\
		doRead(&temp, sizeof(type));\
		if (good())\
		{\
			x = num::fixEndianness(temp, endianness());\
		}\
	}\
	return *this;\
}

LASS_IO_BINARY_I_STREAM_EXTRACTOR(char)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(num::Tint8)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(num::Tuint8)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(num::Tint16)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(num::Tuint16)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(num::Tint32)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(num::Tuint32)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(num::Tint64)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(num::Tuint64)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(num::Tfloat32)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(num::Tfloat64)

BinaryIStream& BinaryIStream::operator>>(bool& x)
{
	if (good())
	{
		num::Tuint8 temp;
		*this >> temp;
		if (good())
		{
			x = temp ? true : false;
		}
	}
	return *this;
}

BinaryIStream& BinaryIStream::operator>>(void*& x)
{
	if (good())
	{
		num::Tint64 temp;
		*this >> temp;
		if (good())
		{
#pragma LASS_FIXME("do something special here if cast causes truncation [Bramz]")
			x = reinterpret_cast<void*>(static_cast<num::TintPtr>(temp));
		}
	}
	return *this;
}



BinaryIStream& BinaryIStream::operator>>( std::string& x )
{
	if (good())
	{
		num::Tuint64 n;
		*this >> n;
		size_t length = static_cast<size_t>(n);
#pragma LASS_FIXME("do something special if there's an overflow [Bramz]")
		LASS_ASSERT(n == static_cast<num::Tuint64>(n));
		if (good())
		{
			std::vector<char> buffer(length + 2, '\0'); // [TDM] null character storage + safety :o)
			doRead(&buffer[0], length);
			if (good())
			{
				x = std::string(&buffer[0]);
			}
		}
	}
	return *this;
}



/** read a number of bytes from stream to buffer
 *
 *  @param iBytes 
 *		pointer to buffer.  Must be able to contain at least @a numBytes bytes.
 *  @param numBytes 
 *		number of bytes to be read
 */
void BinaryIStream::read(void* output, size_t numBytes)
{
	doRead(output, numBytes);
}



}

}

