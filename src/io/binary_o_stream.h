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



/** @class lass::io::BinaryOStream
 *  @brief base class of binary output streams.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_STREAM_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_STREAM_H



#include "io_common.h"
#include "stream_base.h"

namespace lass
{

namespace io
{

class LASS_DLL_EXPORT BinaryOStream: public StreamBase
{
public:

	BinaryOStream();
	~BinaryOStream();

	void flush();

	BinaryOStream& operator<<( char iIn );
	BinaryOStream& operator<<( signed char iIn );
	BinaryOStream& operator<<( unsigned char iIn );
	BinaryOStream& operator<<( signed short iIn );
	BinaryOStream& operator<<( unsigned short iIn );
	BinaryOStream& operator<<( signed int iIn );
	BinaryOStream& operator<<( unsigned int iIn );
	BinaryOStream& operator<<( signed long iIn );
	BinaryOStream& operator<<( unsigned long iIn );
	BinaryOStream& operator<<( float iIn );
	BinaryOStream& operator<<( double iIn );
	BinaryOStream& operator<<( long double iIn );
	BinaryOStream& operator<<( bool iIn );
	BinaryOStream& operator<<( const void* iIn );
	BinaryOStream& operator<<( const char* iIn );
	BinaryOStream& operator<<( const std::string& iIn );
	template<typename T> BinaryOStream& operator<<( const std::vector<T>& iIn );
	template<typename T> BinaryOStream& operator<<( const std::complex<T>& iIn );

	void write(const void* iBuffer, size_t iByteLength);

private:

	virtual void doWrite(const void* iBytes, size_t iNumberOfBytes) = 0;
	virtual void doFlush() = 0;
};

}

}

#include "binary_o_stream.inl"

#endif
