/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

