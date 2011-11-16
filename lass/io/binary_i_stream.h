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



/** @class lass::io::BinaryIStream
 *  @brief base class of binary input streams.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_I_STREAM_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_I_STREAM_H

#include "io_common.h"
#include "binary_stream_base.h"

namespace lass
{

namespace io
{

class LASS_DLL BinaryIStream: public BinaryStreamBase
{
public:

	BinaryIStream();
	~BinaryIStream();

	long tellg() const;
	BinaryIStream& seekg(long iPosition);
	BinaryIStream& seekg(long offset, std::ios_base::seekdir direction); 

#if !defined(LASS_HAVE_STDINT_H_INT8_T_IS_CHAR)
	BinaryIStream& operator>>( char& x );
#endif
	BinaryIStream& operator>>( num::Tint8& x );
	BinaryIStream& operator>>( num::Tuint8& x );
	BinaryIStream& operator>>( num::Tint16& x );
	BinaryIStream& operator>>( num::Tuint16& x );
	BinaryIStream& operator>>( num::Tint32& x );
	BinaryIStream& operator>>( num::Tuint32& x );
	BinaryIStream& operator>>( num::Tint64& x );
	BinaryIStream& operator>>( num::Tuint64& x );
	BinaryIStream& operator>>( num::Tfloat32& x );
	BinaryIStream& operator>>( num::Tfloat64& x );
	BinaryIStream& operator>>( bool& x );
	BinaryIStream& operator>>( void*& x );

	BinaryIStream& operator>>( std::string& out );
	template <typename T> BinaryIStream& operator>>( std::vector<T>& out );
	template <typename T> BinaryIStream& operator>>( std::complex<T>& out );

	size_t read(void* out, size_t numberOfBytes);

private:

	template <typename T> BinaryIStream& readValue(T& x);

	virtual long doTellg() const = 0;
	virtual void doSeekg(long offset, std::ios_base::seekdir direction) = 0;
	virtual size_t doRead(void* out, size_t numberOfBytes) = 0;
};



}

}

#include "binary_i_stream.inl"

#endif
