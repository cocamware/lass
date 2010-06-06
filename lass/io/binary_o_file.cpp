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
 *	Copyright (C) 2004-2010 the Initial Developer.
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
#include "binary_o_file.h"

#if LASS_HAVE_WFOPEN && LASS_HAVE_MULTIBYTETOWIDECHAR
#	include "../util/wchar_support.h"
#endif

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4996) // 'fopen': This function or variable may be unsafe
#endif

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



void BinaryOFile::open(const char* path)
{
	close();
	if (good())
	{
#if LASS_HAVE_WFOPEN && LASS_HAVE_MULTIBYTETOWIDECHAR
		std::wstring wpath = util::utf8ToWchar(path);
		file_ = ::_wfopen(wpath.c_str(), L"wb");
#else
		file_ = ::fopen(path, "wb");
#endif
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
	const int result = ::fseek(file_, iOffset, impl::seekdir2stdio(iDirection));
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
