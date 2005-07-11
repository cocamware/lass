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
	BinaryIStream& seekg(long iOffset, std::ios_base::seekdir iDirection); 

	BinaryIStream& operator>>( char& oOut );
	BinaryIStream& operator>>( signed char& oOut );
	BinaryIStream& operator>>( unsigned char& oOut );
	BinaryIStream& operator>>( signed short& oOut );
	BinaryIStream& operator>>( unsigned short& oOut );
	BinaryIStream& operator>>( signed int& oOut );
	BinaryIStream& operator>>( unsigned int& oOut );
	BinaryIStream& operator>>( signed long& oOut );
	BinaryIStream& operator>>( unsigned long& oOut );
	BinaryIStream& operator>>( float& oOut );
	BinaryIStream& operator>>( double& oOut );
	BinaryIStream& operator>>( long double& oOut );
	BinaryIStream& operator>>( bool& oOut );
	BinaryIStream& operator>>( void*& oOut );

	BinaryIStream& operator>>( std::string& oOut );
	template <typename T> BinaryIStream& operator>>( std::vector<T>& oOut );
	template <typename T> BinaryIStream& operator>>( std::complex<T>& oOut );

	void read(void* oOutput, size_t iNumberOfBytes);

private:

	virtual long doTellg() const = 0;
	virtual void doSeekg(long iOffset, std::ios_base::seekdir iDirection) = 0;
	virtual void doRead(void* oOutput, size_t iNumberOfBytes) = 0;
};



}

}

#include "binary_i_stream.inl"

#endif
