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
#include "binary_o_stream.h"

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

BinaryOStream::BinaryOStream():
	StreamBase()
{
}

BinaryOStream::~BinaryOStream()
{
}

void BinaryOStream::flush()
{
	doFlush();
}

#define LASS_IO_BINARY_O_STREAM_INSERTOR( type )\
BinaryOStream& BinaryOStream::operator<<( type iIn )\
{\
	doWrite(&iIn, sizeof(type));\
	return *this;\
}

LASS_IO_BINARY_O_STREAM_INSERTOR(char)
LASS_IO_BINARY_O_STREAM_INSERTOR(signed char)
LASS_IO_BINARY_O_STREAM_INSERTOR(unsigned char)
LASS_IO_BINARY_O_STREAM_INSERTOR(signed int)
LASS_IO_BINARY_O_STREAM_INSERTOR(unsigned int)
LASS_IO_BINARY_O_STREAM_INSERTOR(signed short)
LASS_IO_BINARY_O_STREAM_INSERTOR(unsigned short)
LASS_IO_BINARY_O_STREAM_INSERTOR(signed long)
LASS_IO_BINARY_O_STREAM_INSERTOR(unsigned long)
LASS_IO_BINARY_O_STREAM_INSERTOR(float)
LASS_IO_BINARY_O_STREAM_INSERTOR(double)
LASS_IO_BINARY_O_STREAM_INSERTOR(long double)
LASS_IO_BINARY_O_STREAM_INSERTOR(bool)
LASS_IO_BINARY_O_STREAM_INSERTOR(const void*)

BinaryOStream& BinaryOStream::operator<<(const char* iIn)
{
	size_t length = strlen(iIn);
	doWrite(&length, sizeof(size_t));
	doWrite(iIn, length);
	return *this;
}

BinaryOStream& BinaryOStream::operator<<(const std::string& iIn)
{
	size_t length = iIn.length();
	doWrite(&length, sizeof(size_t));
	doWrite(iIn.c_str(), length);
	return *this;
}

/** write a buffer of bytes to the stream
 *  @param iBytes pointer to buffer.
 *  @param iNumberOfBytes length of buffer in bytes.
 */
void BinaryOStream::write(const void* iBytes, size_t iNumberOfBytes)
{
	doWrite(iBytes, iNumberOfBytes);
}

}

}

