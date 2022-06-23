/**     @file
 *      @author Bram de Greve (bram@cocamware.com)
 *      @author Tom De Muer (tom@cocamware.com)
 *
 *      *** BEGIN LICENSE INFORMATION ***
 *      
 *      The contents of this file are subject to the Common Public Attribution License 
 *      Version 1.0 (the "License"); you may not use this file except in compliance with 
 *      the License. You may obtain a copy of the License at 
 *      http://lass.sourceforge.net/cpal-license. The License is based on the 
 *      Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *      use of software over a computer network and provide for limited attribution for 
 *      the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *      with Exhibit B.
 *      
 *      Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *      WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *      language governing rights and limitations under the License.
 *      
 *      The Original Code is LASS - Library of Assembled Shared Sources.
 *      
 *      The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *      The Original Developer is the Initial Developer.
 *      
 *      All portions of the code written by the Initial Developer are:
 *      Copyright (C) 2004-2022 the Initial Developer.
 *      All Rights Reserved.
 *      
 *      Contributor(s):
 *
 *      Alternatively, the contents of this file may be used under the terms of the 
 *      GNU General Public License Version 2 or later (the GPL), in which case the 
 *      provisions of GPL are applicable instead of those above.  If you wish to allow use
 *      of your version of this file only under the terms of the GPL and not to allow 
 *      others to use your version of this file under the CPAL, indicate your decision by 
 *      deleting the provisions above and replace them with the notice and other 
 *      provisions required by the GPL License. If you do not delete the provisions above,
 *      a recipient may use your version of this file under either the CPAL or the GPL.
 *      
 *      *** END LICENSE INFORMATION ***
 */

#include "lass_common.h"
#include "binary_i_memory_map.h"
#include "../util/impl/lass_errno.h"
#include <string.h>

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#       define LASS_IO_MEMORY_MAP_WIN
#       include <windows.h>
#else
#       if LASS_HAVE_SYS_MMAN_H && LASS_HAVE_SYS_STAT_H && LASS_HAVE_FCNTL_H && LASS_HAVE_UNISTD_H
#               define LASS_IO_MEMORY_MAP_MMAP
#				include "../util/wchar_support.h"
#               include <unistd.h>
#               include <fcntl.h>
#               include <sys/stat.h>
#               include <sys/mman.h>
#       endif
#endif

namespace lass
{
namespace io
{

namespace impl
{

#if defined(LASS_IO_MEMORY_MAP_WIN)

class BinaryIMemoryMapImpl
{
public:
	BinaryIMemoryMapImpl(const wchar_t* filename)
	{
		file_ = LASS_ENFORCE_WINAPI(::CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ,
			0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0));
		LARGE_INTEGER size;
		if (!GetFileSizeEx(file_, &size))
		{
			const unsigned lastError = util::impl::lass_GetLastError();
			LASS_THROW("Failed to get file size: (" << lastError << ") "
				<< util::impl::lass_FormatMessage(lastError));
		}
#if LASS_ADDRESS_SIZE == 64
		static_assert(sizeof(size.QuadPart) == sizeof(size_t), "LARGE_INTEGER::QuadPart must have same size as size_t");
		fileSize_ = static_cast<size_t>(size.QuadPart);
#else
		if (size.HighPart != 0)
		{
			LASS_THROW("File too large");
		}
		fileSize_ = size.LowPart;
#endif
		map_ = LASS_ENFORCE_WINAPI(::CreateFileMapping(file_, 0, PAGE_READONLY, 0, 0, 0));
		data_ = static_cast<char*>(LASS_ENFORCE_WINAPI(
			::MapViewOfFile(map_, FILE_MAP_READ, 0, 0, 0)));
	}

	BinaryIMemoryMapImpl(const char* filename) :
		BinaryIMemoryMapImpl(util::utf8ToWchar(filename).c_str())
	{
	}

	~BinaryIMemoryMapImpl()
	{
		LASS_WARN_WINAPI(::UnmapViewOfFile(data_));
		data_ = nullptr;
		LASS_WARN_WINAPI(::CloseHandle(map_));
		map_ = 0;
		LASS_WARN_WINAPI(::CloseHandle(file_));
		file_ = 0;
	}

	size_t fileSize() const noexcept
	{
		return fileSize_;
	}

	const char* data() const
	{
		return data_;
	}

private:
	const char* data_;
	HANDLE file_{ 0 };
	HANDLE map_{ 0 };
	size_t fileSize_;
};

#elif defined(LASS_IO_MEMORY_MAP_MMAP)

class BinaryIMemoryMapImpl
{
public:
	BinaryIMemoryMapImpl(const char* filename)
	{
		file_ = LASS_ENFORCE_CLIB(::open(filename, O_RDONLY));
		struct stat buf;
		LASS_ENFORCE_CLIB(::fstat(file_, &buf));
		fileSize_ = static_cast<size_t>(buf.st_size);
		view_ = static_cast<char*>(LASS_ENFORCE_CLIB_EX(
			::mmap(0, static_cast<size_t>(fileSize_), PROT_READ, MAP_SHARED, file_, 0),
			(char*)MAP_FAILED));
	}

	BinaryIMemoryMapImpl(const wchar_t* filename) :
		BinaryIMemoryMapImpl(util::wcharToUtf8(filename).c_str())
	{
	}

	~BinaryIMemoryMapImpl()
	{
		LASS_WARN_CLIB(::munmap(view_, static_cast<size_t>(fileSize_)));
		view_ = nullptr;
		LASS_WARN_CLIB(::close(file_));
		file_ = 0;
	}

	const char* data() const
	{
		return view_;
	}

	size_t fileSize() const noexcept
	{
		return fileSize_;
	}

private:
	char* view_;
	size_t fileSize_;
	int file_;
};

#else

#	error "[LASS BUILD MSG] no implementation for BinaryIMemoryMap"

#endif

}

// --- public --------------------------------------------------------------------------------------

BinaryIMemoryMap::BinaryIMemoryMap():
	BinaryIStream()
{
}

BinaryIMemoryMap::BinaryIMemoryMap(const char* filename):
	BinaryIStream()
{
	open(filename);
}

BinaryIMemoryMap::BinaryIMemoryMap(const std::string& filename):
	BinaryIStream()
{
	open(filename);
}

#if LASS_HAVE_WCHAR_SUPPORT

BinaryIMemoryMap::BinaryIMemoryMap(const wchar_t* filename):
	BinaryIStream()
{
	open(filename);
}

BinaryIMemoryMap::BinaryIMemoryMap(const std::wstring& filename):
	BinaryIStream()
{
	open(filename);
}

#endif

#if __cpp_lib_filesystem

BinaryIMemoryMap::BinaryIMemoryMap(const std::filesystem::path& filename):
	BinaryIStream()
{
	open(filename);
}

#endif

BinaryIMemoryMap::~BinaryIMemoryMap()
{
	close();
}



void BinaryIMemoryMap::open(const char* filename)
{
	close();
	try
	{
		pimpl_.reset(new impl::BinaryIMemoryMapImpl(filename));
	}
	catch (const std::exception& error)
	{
		LASS_LOG("Error: " << error.what());
		pimpl_.reset();
		setstate(std::ios_base::badbit);
	}
}



void BinaryIMemoryMap::open(const std::string& filename)
{
	open(filename.c_str());
}

#if LASS_HAVE_WCHAR_SUPPORT

void BinaryIMemoryMap::open(const wchar_t* filename)
{
	close();
	try
	{
		pimpl_.reset(new impl::BinaryIMemoryMapImpl(filename));
	}
	catch (const std::exception& error)
	{
		LASS_LOG("Error: " << error.what());
		pimpl_.reset();
		setstate(std::ios_base::badbit);
	}
}



void BinaryIMemoryMap::open(const std::wstring& filename)
{
	open(filename.c_str());
}


#endif


#if __cpp_lib_filesystem

void BinaryIMemoryMap::open(const std::filesystem::path& filename)
{
	open(filename.native());
}

#endif


void BinaryIMemoryMap::close()
{
	pimpl_.reset();
}



bool BinaryIMemoryMap::is_open() const
{
	return pimpl_.get() != nullptr;
}



// --- private -------------------------------------------------------------------------------------

BinaryIMemoryMap::pos_type BinaryIMemoryMap::doTellg() const
{
	return position_;
}



void BinaryIMemoryMap::doSeekg(pos_type position)
{
	LASS_ASSERT(good());
	if (!pimpl_)
	{
		setstate(std::ios_base::badbit);
		return;
	}
	position_ = position;
}



void BinaryIMemoryMap::doSeekg(off_type offset, std::ios_base::seekdir direction)
{
	LASS_ASSERT(good());

	if (!pimpl_)
	{
		setstate(std::ios_base::badbit);
		return;
	}

	auto seek = [this](pos_type current, off_type offset)
	{
		if (offset < 0)
		{
			const pos_type negoffset = static_cast<pos_type>(-offset);
			if (negoffset > current)
			{
				setstate(std::ios_base::failbit);
				return;
			}
			this->position_ = current - negoffset;
		}
		else
		{
			const pos_type posoffset = static_cast<pos_type>(offset);
			if (current > num::NumTraits<pos_type>::max - posoffset)
			{
				setstate(std::ios_base::failbit);
				return;
			}
			this->position_ = current + posoffset;
		}
	};

	switch (direction)
	{
	case std::ios_base::beg:
		if (offset < 0)
		{
			setstate(std::ios_base::failbit);
			return;
		}
		position_ = static_cast<pos_type>(offset);
		break;
	case std::ios_base::cur:
		seek(position_, offset);
		break;
	case std::ios_base::end:
		seek(pimpl_->fileSize(), offset);
		break;
	default:
		LASS_ASSERT_UNREACHABLE;
		setstate(std::ios_base::badbit);
		return;
	};
}



size_t BinaryIMemoryMap::doRead(void* output, size_t numberOfBytes)
{
	if (!good())
	{
		return 0;
	}

	if (!pimpl_ )
	{
		setstate(std::ios_base::badbit);
		return 0;
	}

	const size_t size = pimpl_->fileSize();
	const char* data = pimpl_->data();
	if (position_ >= size)
	{
		setstate(std::ios_base::eofbit);
		return 0;
	}
	pos_type next = position_ + numberOfBytes;
	if (next > size || next < position_)
	{
		next = size;
		numberOfBytes = size - position_;
	}
	memcpy(output, &data[position_], numberOfBytes);
	position_ = next;
	return numberOfBytes;
}



}

}
