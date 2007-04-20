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



/** @class lass::io::BinaryOStream
 *  @brief base class of binary output streams.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_STREAM_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_STREAM_H



#include "io_common.h"
#include "binary_stream_base.h"

namespace lass
{

namespace io
{

class LASS_DLL BinaryOStream: public BinaryStreamBase
{
public:

	BinaryOStream();
	~BinaryOStream();

	long tellp() const;
	BinaryOStream& seekp(long position);
	BinaryOStream& seekp(long offset, std::ios_base::seekdir direction); 
	void flush();

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
	template<typename T> BinaryOStream& operator<<( const std::vector<T>& x );
	template<typename T> BinaryOStream& operator<<( const std::complex<T>& x );

	void write(const void* buffer, size_t byteLength);

private:

	void writeString(const char* string, size_t length);

	virtual long doTellp() const = 0;
	virtual void doSeekp(long offset, std::ios_base::seekdir direction) = 0;
	virtual void doFlush() = 0;
	virtual void doWrite(const void* bytes, size_t numberOfBytes) = 0;
};

}

}

#include "binary_o_stream.inl"

#endif
