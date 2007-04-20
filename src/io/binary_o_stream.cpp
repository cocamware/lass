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
#include "binary_o_stream.h"

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

BinaryOStream::BinaryOStream():
	BinaryStreamBase()
{
}



BinaryOStream::~BinaryOStream()
{
}



long BinaryOStream::tellp() const
{
	return doTellp();
}



BinaryOStream& BinaryOStream::seekp(long position)
{
	doSeekp(position, std::ios_base::beg);
	return *this;
}



BinaryOStream& BinaryOStream::seekp(long offset, std::ios_base::seekdir direction)
{
	doSeekp(offset, direction);
	return *this;
}



void BinaryOStream::flush()
{
	doFlush();
}



#define LASS_IO_BINARY_O_STREAM_INSERTOR( type )\
BinaryOStream& BinaryOStream::operator<<( type x )\
{\
	type temp = num::fixEndianness(x, endianness());\
	doWrite(&temp, sizeof(type));\
	return *this;\
}

LASS_IO_BINARY_O_STREAM_INSERTOR(num::Tint8)
LASS_IO_BINARY_O_STREAM_INSERTOR(num::Tuint8)
LASS_IO_BINARY_O_STREAM_INSERTOR(num::Tint16)
LASS_IO_BINARY_O_STREAM_INSERTOR(num::Tuint16)
LASS_IO_BINARY_O_STREAM_INSERTOR(num::Tint32)
LASS_IO_BINARY_O_STREAM_INSERTOR(num::Tuint32)
LASS_IO_BINARY_O_STREAM_INSERTOR(num::Tint64)
LASS_IO_BINARY_O_STREAM_INSERTOR(num::Tuint64)
LASS_IO_BINARY_O_STREAM_INSERTOR(num::Tfloat32)
LASS_IO_BINARY_O_STREAM_INSERTOR(num::Tfloat64)

BinaryOStream& BinaryOStream::operator<<(bool x)
{
	return *this << (x ? num::Tuint8(1) : num::Tuint8(0));
}

BinaryOStream& BinaryOStream::operator<<(const void* x)
{
	return *this << static_cast<num::Tint64>(reinterpret_cast<num::TintPtr>(x));
}

BinaryOStream& BinaryOStream::operator<<(const char* x)
{
	writeString(x, strlen(x));
	return *this;
}

BinaryOStream& BinaryOStream::operator<<(const std::string& x)
{
	writeString(x.data(), x.length());
	return *this;
}

/** write a buffer of bytes to the stream
 *  @param bytes pointer to buffer.
 *  @param numBytes length of buffer in bytes.
 */
void BinaryOStream::write(const void* bytes, size_t numBytes)
{
	doWrite(bytes, numBytes);
}

// --- private -------------------------------------------------------------------------------------

void BinaryOStream::writeString(const char* string, size_t length)
{
	const num::Tuint64 n = static_cast<num::Tuint64>(length);
	LASS_ASSERT(length == static_cast<size_t>(n));
	*this << n;
	doWrite(string, length);
}

}

}

