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
 *	Copyright (C) 2004-2009 the Initial Developer.
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
#include "binary_i_stream.h"
#include "../num/basic_types.h"

// static_cast from TintPtr to pointer gives warning on MSVC, yet both are identical in size [Bramz]
// 
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4312) // 'reinterpret_cast' : conversion from 'lass::num::TintPtr' to 'void *' of greater size
#endif

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


#if !LASS_HAVE_STDINT_H_INT8_T_IS_CHAR

BinaryIStream& BinaryIStream::operator>>( char& x )
{
	return readValue(x);
}

#endif


BinaryIStream& BinaryIStream::operator>>( num::Tint8& x )
{
	return readValue(x);
}



BinaryIStream& BinaryIStream::operator>>( num::Tuint8& x )
{
	return readValue(x);
}



BinaryIStream& BinaryIStream::operator>>( num::Tint16& x )
{
	return readValue(x);
}



BinaryIStream& BinaryIStream::operator>>( num::Tuint16& x )
{
	return readValue(x);
}



BinaryIStream& BinaryIStream::operator>>( num::Tint32& x )
{
	return readValue(x);
}



BinaryIStream& BinaryIStream::operator>>( num::Tuint32& x )
{
	return readValue(x);
}



BinaryIStream& BinaryIStream::operator>>( num::Tint64& x )
{
	return readValue(x);
}



BinaryIStream& BinaryIStream::operator>>( num::Tuint64& x )
{
	return readValue(x);
}



BinaryIStream& BinaryIStream::operator>>( num::Tfloat32& x )
{
	return readValue(x);
}



BinaryIStream& BinaryIStream::operator>>( num::Tfloat64& x )
{
	return readValue(x);
}



BinaryIStream& BinaryIStream::operator>>(bool& x)
{
	num::Tuint8 temp;
	*this >> temp;
	if (good())
	{
		x = temp ? true : false;
	}
	return *this;
}



BinaryIStream& BinaryIStream::operator>>(void*& x)
{
	LASS_META_ASSERT(sizeof(num::TintPtr) == sizeof(const void*), TintPtr_should_be_of_pointer_size);
	num::Tuint64 temp;
	*this >> temp;
	if (good())
	{
		num::TuintPtr address = static_cast<num::TuintPtr>(temp);
		if (temp != static_cast<num::Tuint64>(address))
		{
			LASS_THROW("address overflow" << std::hex << temp);
		}
		x = reinterpret_cast<void*>(address);
	}
	return *this;
}



BinaryIStream& BinaryIStream::operator>>( std::string& x )
{
	num::Tuint64 n;
	*this >> n;
	if (good())
	{
		size_t length = static_cast<size_t>(n);
		if (n != static_cast<num::Tuint64>(length) || (length + 1 < length))
		{
			LASS_THROW("length overflow" << n);
		}
		std::vector<char> buffer(length + 1, '\0'); // [TDM] null character storage :o)
		doRead(&buffer[0], length);
		if (good())
		{
			x = std::string(&buffer[0]);
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



// --- private -------------------------------------------------------------------------------------

template <typename T>
BinaryIStream& BinaryIStream::readValue(T& x)
{
	if (good())
	{
		T temp;
		doRead(&temp, sizeof(T));
		if (good())
		{
			x = num::fixEndianness(temp, endianness());
		}
	}
	return *this;
}


}

}

