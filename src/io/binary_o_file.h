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



/** @class lass::io::BinaryOFile
 *  @brief BinaryOStream to file.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_FILE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_FILE_H



#include "io_common.h"
#include "binary_o_stream.h"
#include <cstdio>



namespace lass
{

namespace io
{

class LASS_DLL_EXPORT BinaryOFile: public BinaryOStream
{
public:

	BinaryOFile();
	BinaryOFile( const char* iFileName );
	BinaryOFile( const std::string& iFileName );
	~BinaryOFile();

	void open(const char* iFileName);
	void open(const std::string& iFileName);
	void close();
	bool is_open() const;
	void flush();

private:

	void doWrite(const void* iBytes, size_t iNumberOfBytes);
	void doFlush();

	FILE* file_;
};

}

}

#endif
