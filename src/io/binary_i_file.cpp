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
#include "binary_i_file.h"

#include "../util/scoped_array.h"

#include <cstdio>



namespace lass
{

namespace io
{

/** Construct an "empty" closed stream..
 */
BinaryIFile::BinaryIFile():
	BinaryIStream(),
	file_(0)
{
}



/** Construct stream by filename and open it.
 */
BinaryIFile::BinaryIFile( const char* iFileName ):
	BinaryIStream(),
	file_(0)
{
	open(iFileName);
}



/** Construct stream by filename and open it.
 */
BinaryIFile::BinaryIFile( const std::string& iFileName ):
	BinaryIStream(),
	file_(0)
{
	open(iFileName);
}



/** Close stream on destruction.
 */
BinaryIFile::~BinaryIFile()
{
	close();
}



void BinaryIFile::open(const char* iFileName)
{
	close();
	file_ = fopen(iFileName, "rb");
	if (!file_)
	{
		setstate(std::ios_base::failbit);
	}
}



void BinaryIFile::open(const std::string& iFileName)
{
	open(iFileName.c_str());
}



void BinaryIFile::close()
{
	if (is_open())
	{
		const int result = ::fclose( file_ );
		if (result != 0)
		{
			setstate(std::ios_base::failbit);
		}
		file_ = 0;
	}
}

bool BinaryIFile::is_open() const
{
	return file_ != 0;
}



// --- private -------------------------------------------------------------------------------------

void BinaryIFile::doRead(void* oOutput, size_t iNumberOfBytes)
{
	if (!file_)
	{
		setstate(std::ios_base::failbit);
		return;
	}
	if (!good())
	{
		return;
	}
	const size_t bytesRead = ::fread(oOutput, 1, iNumberOfBytes, file_);
	if (bytesRead != iNumberOfBytes)
	{
		setstate(std::ios_base::eofbit);
	}
}

}

}