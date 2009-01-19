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

#include "lass_common.h"
#include "binary_o_stream.h"

// static_cast from pointer to TintPtr gives warning on MSVC, yet both are identical in size [Bramz]
// 
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4311) // 'reinterpret_cast' : 'reinterpret_cast' : pointer truncation from 'const void *' to 'lass::num::TintPtr'
#endif

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



BinaryOStream& BinaryOStream::operator<<( char x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<( num::Tint8 x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<( num::Tuint8 x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<( num::Tint16 x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<( num::Tuint16 x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<( num::Tint32 x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<( num::Tuint32 x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<( num::Tint64 x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<( num::Tuint64 x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<( num::Tfloat32 x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<( num::Tfloat64 x )
{
	return writeValue(x);
}



BinaryOStream& BinaryOStream::operator<<(bool x)
{
	return *this << (x ? num::Tuint8(1) : num::Tuint8(0));
}



BinaryOStream& BinaryOStream::operator<<(const void* x)
{
	LASS_META_ASSERT(sizeof(num::TuintPtr) == sizeof(const void*), TuintPtr_should_be_of_pointer_size);
	return *this << static_cast<num::Tuint64>(reinterpret_cast<num::TuintPtr>(x));
}



BinaryOStream& BinaryOStream::operator<<(const char* x)
{
	return writeString(x, strlen(x));
}



BinaryOStream& BinaryOStream::operator<<(const std::string& x)
{
	return writeString(x.data(), x.length());
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

template <typename T>
BinaryOStream& BinaryOStream::writeValue(T x)
{
	const T temp = num::fixEndianness(x, endianness());
	doWrite(&temp, sizeof(T));
	return *this;
}



BinaryOStream& BinaryOStream::writeString(const char* string, size_t length)
{
	const num::Tuint64 n = static_cast<num::Tuint64>(length);
	LASS_ASSERT(length == static_cast<size_t>(n));
	*this << n;
	doWrite(string, length);
	return *this;
}

}

}

