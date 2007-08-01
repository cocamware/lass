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

#include "io_common.h"
#include "binary_i_memory_map.h"
#include "../util/impl/lass_errno.h"

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	define LASS_IO_MEMORY_MAP_WIN
#	include <windows.h>
#else
#	if HAVE_CONFIG_H
#		include "lass_auto_config.h"
#	endif
#	if HAVE_SYS_MMAN_H && HAVE_SYS_STAT_H && HAVE_FCNTL_H && HAVE_UNISTD_H
#		define LASS_IO_MEMORY_MAP_MMAP
#		include <unistd.h>
#		include <fcntl.h>
#		include <sys/stat.h>
#		include <sys/mman.h>
#		if !defined(_STAT_VER_KERNEL)
#			error "blah"
#		endif
#	endif
#endif

namespace lass
{
namespace io
{

namespace impl
{
	class BinaryIMemoryMapImpl
	{
	public:
		BinaryIMemoryMapImpl(const char* filename):
			view_(0)
		{
#if defined(LASS_IO_MEMORY_MAP_WIN)
			pageSize_ = 4096;
			map_ = 0;
#	ifdef UNICODE
			const int bufferLength = MultiByteToWideChar(CP_UTF8, 0, filename, -1, 0, 0);
			std::vector<WCHAR> buffer(bufferLength);
			MultiByteToWideChar(CP_UTF8, 0, filename, -1, &buffer[0], bufferLength);
			LPCWSTR szFile = &buffer[0];
#	else
			LPCSTR szFile = filename;
#	endif
			file_ = LASS_ENFORCE_WINAPI(::CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ,
				0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0));
			const DWORD fsize = GetFileSize(file_, 0);
			if (fsize == INVALID_FILE_SIZE)
			{
				const unsigned lastError = util::impl::lass_GetLastError();
				LASS_THROW("Failed to get file size: (" << lastError << ") "
					<< util::impl::lass_FormatMessage(lastError));
			}
			fileSize_ = static_cast<long>(fsize);
			if (fileSize_ < 0 || static_cast<DWORD>(fileSize_) != fsize)
			{
				LASS_THROW("Filesize overflow");
			}
			map_ = LASS_ENFORCE_WINAPI(::CreateFileMapping(file_, 0, PAGE_READONLY, 0, 0, 0));
#elif defined(LASS_IO_MEMORY_MAP_MMAP)
			pageSize_ = LASS_ENFORCE_CLIB(::sysconf(_SC_PAGE_SIZE));
			file_ = LASS_ENFORCE_CLIB(::open(filename, O_RDONLY));
			struct stat buf;
			LASS_ENFORCE_CLIB(::fstat(file_, &buf));
			fileSize_ = static_cast<long>(buf.st_size);
			if (fileSize_ < 0 || static_cast<off_t>(fileSize_) != buf.st_size)
			{
				LASS_THROW("Filesize overflow");
			}			
#else
#	error "[LASS BUILD MSG] no implementation for BinaryIMemoryMap"
#endif
		}
		
		
		~BinaryIMemoryMapImpl()
		{
			unmap();
#if defined(LASS_IO_MEMORY_MAP_WIN)
			LASS_WARN_WINAPI(::CloseHandle(map_));
			map_ = 0;
			LASS_WARN_WINAPI(::CloseHandle(file_));
			file_ = 0;
#elif defined(LASS_IO_MEMORY_MAP_MMAP)
			LASS_WARN_CLIB(::close(file_));
			file_ = 0;
#endif
		}
		
		
		const long fileSize() const
		{
			return fileSize_;
		}
		
		
		char* const remap(long newBegin, long& begin, long& end)
		{			
			newBegin = pageSize_ * (newBegin / pageSize_); // down to nearest boundary
			long newEnd = newBegin + pageSize_;
			if (newEnd > fileSize_ || newEnd < 0)
			{
				newEnd = fileSize_;
			}
			LASS_ASSERT(newEnd >= newBegin);
			const long mapSize = newEnd - newBegin;
#if defined(LASS_IO_MEMORY_MAP_WIN)
			char* view = static_cast<char*>(LASS_ENFORCE_WINAPI(
				::MapViewOfFile(map_, FILE_MAP_READ, 0, newBegin, mapSize)));
#elif defined(LASS_IO_MEMORY_MAP_MMAP)
			char* view = static_cast<char*>(LASS_ENFORCE_CLIB_EX(
				::mmap(0, mapSize, PROT_READ, MAP_SHARED, file_, newBegin),
				(char*)MAP_FAILED));
#else
			return 0;
#endif
			unmap();
			mapSize_ = mapSize;
			view_ = view;
			begin = newBegin;
			end = newEnd;
			return view_;
		}
		
	private:
		
		void unmap()
		{
			if (!view_)
			{
				return;
			}
#if defined(LASS_IO_MEMORY_MAP_WIN)
			LASS_WARN_WINAPI(::UnmapViewOfFile(view_));
#elif defined(LASS_IO_MEMORY_MAP_MMAP)
			LASS_WARN_CLIB(::munmap(view_, mapSize_));
#endif
			view_ = 0;
		}
		
		long pageSize_;
		long fileSize_;
		long mapSize_;
		char* view_;
#if defined(LASS_IO_MEMORY_MAP_WIN)
		HANDLE file_;
		HANDLE map_;
#elif defined(LASS_IO_MEMORY_MAP_MMAP)
		int file_;
#endif
	};
}

// --- public --------------------------------------------------------------------------------------

BinaryIMemoryMap::BinaryIMemoryMap():
	BinaryIStream(),
	pimpl_(0)
{
}

BinaryIMemoryMap::BinaryIMemoryMap(const char* filename):
	BinaryIStream(),
	pimpl_(0)
{
	open(filename);
}

BinaryIMemoryMap::BinaryIMemoryMap(const std::string& filename):
	BinaryIStream(),
	pimpl_(0)
{
	open(filename);
}



BinaryIMemoryMap::~BinaryIMemoryMap()
{
	close();
}



void BinaryIMemoryMap::open(const char* filename)
{
	std::auto_ptr<impl::BinaryIMemoryMapImpl> pimpl;
	long begin, end, size;
	char* data;

	try
	{
		pimpl.reset(new impl::BinaryIMemoryMapImpl(filename));
		size = pimpl->fileSize();
		data = pimpl->remap(0, begin, end);
	}
	catch (std::exception& error)
	{
		LASS_LOG("Error: " << error.what());
		setstate(std::ios_base::failbit);
		return;
	}

	close();
	pimpl_ = pimpl.release();
	data_ = data;
	size_ = size;
	begin_ = begin_;
	end_ = end_;
	position_ = 0;
}



void BinaryIMemoryMap::open(const std::string& filename)
{
	open(filename.c_str());
}



void BinaryIMemoryMap::close()
{
	delete pimpl_;
	pimpl_ = 0;
}



bool BinaryIMemoryMap::is_open() const
{
	return pimpl_ != 0;
}



// --- private -------------------------------------------------------------------------------------

long BinaryIMemoryMap::doTellg() const
{
	return position_;
}



void BinaryIMemoryMap::doSeekg(long offset, std::ios_base::seekdir direction)
{
	if (!good())
	{
		return;
	}
	if (!pimpl_)
	{
		setstate(std::ios_base::badbit);
		return;
	}

	switch (direction)
	{
	case std::ios_base::beg: 
		position_ = offset;
		break;
	case std::ios_base::cur:
		position_ += offset;
		break;
	case std::ios_base::end:
		position_ = size_ + offset;
		break;
	default:
		LASS_ASSERT_UNREACHABLE;
		setstate(std::ios_base::badbit);
		return;
	};
	if (position_ < begin_ || position_ >= end_)
	{
		try
		{
			data_ = pimpl_->remap(position_, begin_, end_);
		}
		catch (std::exception& error)
		{
			LASS_LOG("Error: " << error.what());
			setstate(std::ios_base::badbit);
		}
	}
}



void BinaryIMemoryMap::doRead(void* output, size_t numberOfBytes)
{
	if (!pimpl_ || !data_)
	{
		setstate(std::ios_base::failbit);
		return;
	}
	if (!good())
	{
		return;
	}

	const long last = position_ + static_cast<long>(numberOfBytes);
	if (last > size_ || last < position_)
	{
		setstate(std::ios_base::eofbit | std::ios_base::failbit);
	}	

	char* dest = static_cast<char*>(output);
	while (last >= end_)
	{
		::memcpy(dest, &data_[position_ - begin_], end_ - position_);
		dest += end_ - position_;
		try
		{
			data_ = pimpl_->remap(end_, begin_, end_);
		}
		catch (std::exception& error)
		{
			LASS_LOG("Error: " << error.what());
			setstate(std::ios_base::badbit);
			return;
		}
		position_ = begin_;
	}
	::memcpy(dest, &data_[position_ - begin_], last - position_);
	position_ = last;
}

}

}