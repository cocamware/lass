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
#include "binary_i_stream.h"

namespace lass
{

namespace io
{

BinaryIStream::BinaryIStream():
	StreamBase()
{
}

BinaryIStream::~BinaryIStream()
{
}

#define LASS_IO_BINARY_I_STREAM_EXTRACTOR( type )\
BinaryIStream& BinaryIStream::operator>>( type & oOut )\
{\
	doRead(&oOut, sizeof(type));\
	return *this;\
}

LASS_IO_BINARY_I_STREAM_EXTRACTOR(char)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(signed char)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(unsigned char)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(signed int)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(unsigned int)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(signed short)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(unsigned short)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(signed long)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(unsigned long)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(float)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(double)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(long double)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(bool)
LASS_IO_BINARY_I_STREAM_EXTRACTOR(void*)

BinaryIStream& BinaryIStream::operator>>( std::string& oOut )
{
	size_t length;
	*this >> length;
	if (good())
	{
		std::vector<char> buffer(length + 2, '\0'); // [TDM] null character storage + safety :o)
		doRead(&buffer[0], length);
		if (good())
		{
			oOut = std::string(&buffer[0]);
		}
	}
	return *this;
}



}

}

