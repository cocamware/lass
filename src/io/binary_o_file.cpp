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
#include "binary_o_file.h"

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

/** Construct an "empty" closed file..
 */
BinaryOFile::BinaryOFile():
	BinaryOStream(),
	file_(0)
{
}



/** Construct stream by filename and open it.
 */
BinaryOFile::BinaryOFile( const char* iFileName ):
	BinaryOStream(),
	file_(0)
{
	open(iFileName);
}



/** Construct stream by filename and open it.
 */
BinaryOFile::BinaryOFile( const std::string& iFileName ):
	BinaryOStream(),
	file_(0)
{
	open(iFileName);
}



/** Close stream on destruction.
 */
BinaryOFile::~BinaryOFile()
{
	close();
}



void BinaryOFile::open(const char* iFileName)
{
	close();
	if (good())
	{
		file_ = ::fopen(iFileName, "wb");
		if (!file_)
		{
			setstate(std::ios_base::failbit);
		}
	}
}



void BinaryOFile::open(const std::string& iFileName)
{
	open(iFileName.c_str());
}



void BinaryOFile::close()
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



bool BinaryOFile::is_open() const
{
	return file_ != 0;
}

// --- private -------------------------------------------------------------------------------------

long BinaryOFile::doTellp() const
{
	return ::ftell(file_);
}



void BinaryOFile::doSeekp(long iOffset, std::ios_base::seekdir iDirection)
{
	LASS_META_ASSERT(std::ios_base::beg == SEEK_SET, ios_base_beg_is_not_equal_to_SEEK_SET);
	LASS_META_ASSERT(std::ios_base::cur == SEEK_CUR, ios_base_cur_is_not_equal_to_SEEK_CUR);
	LASS_META_ASSERT(std::ios_base::end == SEEK_END, ios_base_end_is_not_equal_to_SEEK_END);

	const int result = ::fseek(file_, iOffset, iDirection);
	if (result != 0)
	{
		setstate(std::ios_base::badbit);
	}
}



void BinaryOFile::doFlush()
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
	const int result = ::fflush(file_);
	if (result != 0)
	{
		setstate(std::ios_base::badbit);
	}
}



/** write a buffer of bytes to the stream
 *  @par iIn pointer to buffer.
 *  @par iBufferLength length of buffer in bytes.
 */
void BinaryOFile::doWrite(const void* iBytes, size_t iNumberOfBytes)
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
	const size_t bytesWritten = ::fwrite( iBytes, 1, iNumberOfBytes, file_ );
	if (bytesWritten != iNumberOfBytes)
	{
		setstate(std::ios_base::badbit);
	}
}


}

}

// EOF
