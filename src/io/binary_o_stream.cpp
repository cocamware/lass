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

/** Construct an "empty" closed stream..
 */
BinaryOStream::BinaryOStream():
	file_(0)
{
}



/** Construct stream by filename and open it.
 */
BinaryOStream::BinaryOStream( const std::string& iFileName ):
	file_(0)
{
	open(iFileName);
}

/** Close stream on destruction.
 */
BinaryOStream::~BinaryOStream()
{
	close();
}

void BinaryOStream::open(const std::string& iFileName)
{
	close();
	file_ = LASS_ENFORCE_POINTER(fopen(iFileName.c_str(), "w"));
}

void BinaryOStream::close()
{
	if (isOpen())
	{
		fclose( LASS_ENFORCE_POINTER(file_) );
		file_ = 0;
	}
}

bool BinaryOStream::isOpen() const
{
	return file_ != 0;
}

void BinaryOStream::flush()
{
	fflush( LASS_ENFORCE_POINTER(file_) );
}


#define LASS_IO_BINARY_O_STREAM_INSERTOR( type )\
BinaryOStream& BinaryOStream::operator<<( type iIn )\
{\
	fwrite( &iIn, sizeof(type), 1, LASS_ENFORCE_POINTER(file_) );\
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
	*this << length;
	fwrite( iIn, length, 1, LASS_ENFORCE_POINTER(file_) );
	return *this;
}

BinaryOStream& BinaryOStream::operator<<(const std::string& iIn)
{
	size_t length = iIn.length() + 1;
	*this << length;
	fwrite( iIn.c_str(), length, 1, LASS_ENFORCE_POINTER(file_) );
	return *this;
}

/** write a buffer of bytes to the stream
 *  @par iIn pointer to buffer.
 *  @par iBufferLength length of buffer in bytes.
 */
void BinaryOStream::write(const void* iIn, size_t iBufferLength)
{
	fwrite( iIn, iBufferLength, 1, LASS_ENFORCE_POINTER(file_) );
}



}

}

