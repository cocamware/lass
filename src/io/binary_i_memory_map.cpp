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
		BinaryIMemoryMapImpl(const char* filename)
		{
#if defined(LASS_IO_MEMORY_MAP_WIN)
			map_ = 0;
			view_ = 0;
#ifdef UNICODE
			const int bufferLength = MultiByteToWideChar(CP_ACP, 0, filename, -1, 0, 0);
			std::vector<WCHAR> buffer(bufferLength);
			MultiByteToWideChar( CP_ACP, 0, filename, -1, &buffer[0], bufferLength);
			LPCWSTR szFile = &buffer[0];
#else
			LPCSTR szFile = filename;
#endif
			file_ = LASS_ENFORCE_WINAPI(::CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, 0, 
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0));
			fileSize_ = GetFileSize(file_, 0);
			if (fileSize_ == INVALID_FILE_SIZE)
			{
				const unsigned lastError = util::impl::lass_GetLastError();
				LASS_THROW("Failed to get file size: (" << lastError << ") "
					<< util::impl::lass_FormatMessage(lastError));
			}
			map_ = LASS_ENFORCE_WINAPI(::CreateFileMapping(file_, 0, PAGE_READONLY, 0, 0, 0));
#else
			LASS_THROW("BinaryIMemoryMap not supported for this platform");
#endif
		}
		~BinaryIMemoryMapImpl()
		{
#if defined(LASS_IO_MEMORY_MAP_WIN)
			UnmapViewOfFile(view_);
			view_ = 0;
			CloseHandle(map_);
			map_ = 0;
			CloseHandle(file_);
			file_ = 0;
#endif
		}
		const long fileSize() const
		{
			return fileSize_;
		}
		char* const remap(long newBegin, long& begin, long& end)
		{
#if defined(LASS_IO_MEMORY_MAP_WIN)
			const long newEnd = std::min<long>(newBegin + maxMapSize_, fileSize_);
			LASS_ASSERT(newEnd >= newBegin);
			const long n = newEnd - newBegin;
			if (view_)
			{
				LASS_ENFORCE_WINAPI(::UnmapViewOfFile(view_));
			}
			view_ = static_cast<char*>(
				LASS_ENFORCE_WINAPI(MapViewOfFile(map_, FILE_MAP_READ, 0, newBegin, n)));
			begin = newBegin;
			end = newEnd;
			return view_;	
#else
			return 0;
#endif
		}
	private:
#if defined(LASS_IO_MEMORY_MAP_WIN)
		enum { maxMapSize_ = 0x01400000 };
		HANDLE file_;
		HANDLE map_;
		DWORD fileSize_;
		char* view_;
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
