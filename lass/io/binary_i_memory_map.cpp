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
 *      Copyright (C) 2004-2011 the Initial Developer.
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
        class BinaryIMemoryMapImpl
        {
        public:
                BinaryIMemoryMapImpl(const char* filename):
                        view_(0)
                {
#if defined(LASS_IO_MEMORY_MAP_WIN)
                        pageSize_ = 4096;
                        map_ = 0;
#       ifdef UNICODE
                        const int bufferLength = MultiByteToWideChar(CP_UTF8, 0, filename, -1, 0, 0);
                        std::vector<WCHAR> buffer(bufferLength);
                        MultiByteToWideChar(CP_UTF8, 0, filename, -1, &buffer[0], bufferLength);
                        LPCWSTR szFile = &buffer[0];
#       else
                        LPCSTR szFile = filename;
#       endif
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
#       error "[LASS BUILD MSG] no implementation for BinaryIMemoryMap"
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
                
                
                long fileSize() const
                {
                        return fileSize_;
                }
                
                
                char* remap(long newBegin, long& begin, long& end)
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
                                ::mmap(0, static_cast<size_t>(mapSize), PROT_READ, MAP_SHARED, file_, newBegin),
                                (char*)MAP_FAILED));
#else
                        char* view = 0;
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
                        LASS_WARN_CLIB(::munmap(view_, static_cast<size_t>(mapSize_)));
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
        begin_ = begin;
        end_ = end;
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



size_t BinaryIMemoryMap::doRead(void* output, size_t numberOfBytes)
{
        if (!pimpl_ || !data_)
        {
                setstate(std::ios_base::failbit);
                return 0;
        }
        if (!good())
        {
                return 0;
        }
		if (position_ >= size_)
		{
			setstate(std::ios_base::eofbit);
			return 0;
		}
        long last = position_ + static_cast<long>(numberOfBytes);
        if (last > size_ || last < position_)
        {
                last = size_;
				numberOfBytes = static_cast<size_t>(size_ - position_);
        }
        char* dest = static_cast<char*>(output);
		size_t bytesRead = 0;
        while (last >= end_)
        {
				const size_t n = static_cast<size_t>(end_ - position_);
                ::memcpy(dest, &data_[position_ - begin_], n);
                dest += n;
				bytesRead += n;
                try
                {
                        data_ = pimpl_->remap(end_, begin_, end_);
                }
                catch (std::exception& error)
                {
                        LASS_LOG("Error: " << error.what());
                        setstate(std::ios_base::badbit);
                        return bytesRead;
                }
                position_ = begin_;
        }
        ::memcpy(dest, &data_[position_ - begin_], static_cast<size_t>(last - position_));
        position_ = last;
		return numberOfBytes;
}

}

}
