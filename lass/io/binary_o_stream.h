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



/** @class lass::io::BinaryOStream
 *  @brief base class of binary output streams.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_STREAM_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_STREAM_H



#include "io_common.h"
#include "binary_stream_base.h"
#include "../util/wchar_support.h"

namespace lass
{

namespace io
{

class LASS_DLL BinaryOStream: public BinaryStreamBase
{
public:

	BinaryOStream();
	~BinaryOStream();

	pos_type tellp() const;
	BinaryOStream& seekp(pos_type position);
	BinaryOStream& seekp(off_type offset, std::ios_base::seekdir direction); 
	void flush();

#if !defined(LASS_HAVE_STDINT_H_INT8_T_IS_CHAR)
	BinaryOStream& operator<<( char x );
#endif
	BinaryOStream& operator<<( num::Tint8 x );
	BinaryOStream& operator<<( num::Tuint8 x );
	BinaryOStream& operator<<( num::Tint16 x );
	BinaryOStream& operator<<( num::Tuint16 x );
	BinaryOStream& operator<<( num::Tint32 x );
	BinaryOStream& operator<<( num::Tuint32 x );
	BinaryOStream& operator<<( num::Tint64 x );
	BinaryOStream& operator<<( num::Tuint64 x );
	BinaryOStream& operator<<( num::Tfloat32 x );
	BinaryOStream& operator<<( num::Tfloat64 x );
	BinaryOStream& operator<<( bool x );
	BinaryOStream& operator<<( const void* x );
	BinaryOStream& operator<<( const char* x );
	BinaryOStream& operator<<( const std::string& x );
#if LASS_HAVE_WCHAR_SUPPORT
	BinaryOStream& operator<<( const wchar_t* x );
	BinaryOStream& operator<<( const std::wstring& x );
#endif
	template<typename T> BinaryOStream& operator<<( const std::vector<T>& x );
	template<typename T> BinaryOStream& operator<<( const std::complex<T>& x );

	size_t write(const void* buffer, size_t byteLength);

private:

	template <typename T> BinaryOStream& writeValue(T x);
	BinaryOStream& writeString(const char* string, size_t length);

	virtual pos_type doTellp() const = 0;
	virtual void doSeekp(pos_type position) = 0;
	virtual void doSeekp(off_type offset, std::ios_base::seekdir direction) = 0;
	virtual void doFlush() = 0;
	virtual size_t doWrite(const void* bytes, size_t numberOfBytes) = 0;
};

}

}

#include "binary_o_stream.inl"

#endif
