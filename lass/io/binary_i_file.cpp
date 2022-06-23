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

#include "lass_common.h"
#include "binary_i_file.h"
#include "../meta/meta_assert.h"
#include <stdio.h>

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4996) // 'fopen': This function or variable may be unsafe
#endif

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
BinaryIFile::BinaryIFile(const char* path):
	BinaryIStream(),
	file_(0)
{
	open(path);
}



/** Construct stream by filename and open it.
 */
BinaryIFile::BinaryIFile(const std::string& path):
	BinaryIStream(),
	file_(0)
{
	open(path);
}



#if LASS_HAVE_WCHAR_SUPPORT

/** Construct stream by filename and open it.
 */
BinaryIFile::BinaryIFile(const wchar_t* path):
	BinaryIStream(),
	file_(0)
{
	open(path);
}



/** Construct stream by filename and open it.
 */
BinaryIFile::BinaryIFile(const std::wstring& path):
	BinaryIStream(),
	file_(0)
{
	open(path);
}

#endif



#if __cpp_lib_filesystem

/** Construct stream by filename and open it.
 */
BinaryIFile::BinaryIFile(const std::filesystem::path& path):
	BinaryIStream(),
	file_(0)
{
	open(path);
}

#endif



/** Close stream on destruction.
 */
BinaryIFile::~BinaryIFile()
{
	close();
}



void BinaryIFile::open(const char* path)
{
#if LASS_HAVE_WCHAR_SUPPORT && LASS_HAVE_WFOPEN
	open(util::utf8ToWchar(path));
#else
	close();
	file_ = ::fopen(path, "rb");
	if (!file_)
	{
		setstate(std::ios_base::failbit);
	}
#endif
}



void BinaryIFile::open(const std::string& path)
{
	open(path.c_str());
}



#if LASS_HAVE_WCHAR_SUPPORT

void BinaryIFile::open(const wchar_t* path)
{
#	if LASS_HAVE_WFOPEN
	close();
	file_ = ::_wfopen(path, L"rb");
	if (!file_)
	{
		setstate(std::ios_base::failbit);
	}
#	else
	open(util::wcharToUtf8(path));
#	endif
}



void BinaryIFile::open(const std::wstring& path)
{
	open(path.c_str());
}

#endif



#if __cpp_lib_filesystem

void BinaryIFile::open(const std::filesystem::path& path)
{
	close();
	if (good())
	{
#if LASS_HAVE_WFOPEN
		file_ = ::_wfopen(path.c_str(), L"rb");
#else:
		file_ = ::fopen(path.c_str(), "rb");
#endif
		if (!file_)
		{
			setstate(std::ios_base::failbit);
		}
	}
}

#endif



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

BinaryIFile::pos_type BinaryIFile::doTellg() const
{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC && LASS_ADDRESS_SIZE == 64
	const off_type pos = ::_ftelli64(file_);
#else
	const off_type pos = ::ftell(file_);
#endif
	return static_cast<pos_type>(pos >= 0 ? pos : -1);
}



void BinaryIFile::doSeekg(pos_type position)
{
	if (position > num::NumTraits<off_type>::max)
	{
		setstate(std::ios_base::failbit);
		return;
	}
	doSeekg(static_cast<off_type>(position), std::ios_base::beg);
}



void BinaryIFile::doSeekg(off_type offset, std::ios_base::seekdir direction)
{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC && LASS_ADDRESS_SIZE == 64
	const int result = ::_fseeki64(file_, offset, impl::seekdir2stdio(direction));
#else
	const int result = ::fseek(file_, offset, impl::seekdir2stdio(direction));
#endif
	if (result != 0)
	{
		setstate(std::ios_base::failbit);
	}
}



size_t BinaryIFile::doRead(void* output, size_t numberOfBytes)
{
	if (!file_)
	{
		setstate(std::ios_base::failbit);
		return 0;
	}
	if (!good())
	{
		return 0;
	}
	const size_t bytesRead = ::fread(output, 1, numberOfBytes, file_);
	if (bytesRead != numberOfBytes)
	{
		setstate(std::ios_base::eofbit);
	}
	return bytesRead;
}



}

}

