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
#include "shared_memory.h"
#include "../stde/extended_cstring.h"
#include "../num/num_cast.h"

#ifdef _WIN32
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#else
#   include "../util/impl/lass_errno.h"
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>
#   include <unistd.h>
#   include <errno.h>
#endif

namespace lass
{
namespace io
{
namespace impl
{

#ifdef _WIN32

class SharedMemoryImpl
{
public:
    SharedMemoryImpl():
        map_(0),
        view_(0),
        size_(0)
    {
    }

    ~SharedMemoryImpl()
    {
        close();
    }

    bool create(size_t size) 
    {
        close();

        const unsigned long pid = GetCurrentProcessId();
        name_ = stde::safe_format("Local\\LassIOSharedMemory_%lx_%i", pid, id_++);

        LARGE_INTEGER s;
        s.QuadPart = size;
        map_ = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, s.HighPart, s.LowPart, name_.c_str());
        if (!mapView())
        {
            return false;
        }

        size_ = size;
        return true;
    }

    bool open(const char* name)
    { 
        close();

        name_ = name;
        map_ = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
        if (!mapView())
        {
            return false;
        }

        // OK, we can't really know the size. The following is our best guess.
        MEMORY_BASIC_INFORMATION buffer;
        if ( VirtualQuery( view_, &buffer, sizeof(buffer) ) )
        {
            size_ = buffer.RegionSize;
        }

        return true;
    }

    void close()
    {
        if (view_)
        {
            UnmapViewOfFile(view_);
            view_ = 0;
        }
        if (map_)
        {
            CloseHandle(map_);
            map_ = 0;
        }
        size_ = 0;
    }

    const char* name() const
    { 
        return name_.c_str(); 
    }

    void* get() const
    {
        return view_;
    }

    size_t size() const
    {
        return size_;
    }

private:
    
    bool mapView()
    {
        if (!map_)
        {
            return false;
        }
        LASS_ASSERT(!view_);
        view_ = MapViewOfFile(map_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (!view_)
        {
            close();
            return false;
        }
        return true;
    }

    HANDLE map_;
    void* view_;
    size_t size_;
    std::string name_;
    static unsigned id_;
};

unsigned SharedMemoryImpl::id_ = 0;

#else

class SharedMemoryImpl
{
public:
    SharedMemoryImpl():
        view_(0),
        size_(0),
        fd_(-1)
    {
    }

    ~SharedMemoryImpl()
    {
        close();
    }

    bool create(size_t size) 
    {
        close();
        
        off_t ssize = num::numCast<off_t>(size);
        
        char shmname[] = "/dev/shm/lass-shm.XXXXXX";
        if (!tryCreate(shmname, ssize))
        {
            char tmpname[] = "/tmp/lass-shm.XXXXXX";
            if (!tryCreate(tmpname, ssize))
            {
                return false; // give up
            }
        }
        
        return mapView(ssize);
    }

    bool open(const char* name)
    { 
        close();
        
        fd_ = ::open(name, O_RDWR, 0);
        if (fd_ == -1)
        {
            return false;
        }
        
        struct stat st;
        if (fstat(fd_, &st) == -1)
        {
            close();
            return false;
        }

        return mapView(st.st_size);
    }

    void close()
    {
        if (view_ && view_ != MAP_FAILED)
        {
            munmap(view_, this->size());
        }
        view_ = 0;
        
        if (fd_ != -1)
        {
            ::close(fd_);
            fd_ = -1;
        }
        
        size_ = 0;
        name_.clear();
    }

    const char* name() const
    { 
        return name_.c_str(); 
    }

    void* get() const
    {
        return view_;
    }

    size_t size() const
    {
        LASS_ASSERT(size_ >= 0);
        return static_cast<size_t>(size_);
    }

private:

    bool tryCreate(char* namebuf, off_t size)
    {
        fd_ = mkstemp(namebuf);
        if (fd_ == -1)
        {
            return false;
        }
        unlink(namebuf);
        return ftruncate(fd_, size) == 0;
    }

    bool mapView(off_t size)
    {       
        const int pid = getpid();
        name_ = lass::stde::safe_format("/proc/%i/fd/%i", pid, fd_);
        
        LASS_ASSERT(size_ >= 0);
        view_ = mmap(0, static_cast<size_t>(size), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd_, 0);
        if (view_ == MAP_FAILED)
        {
            close();
            return false;
        }
        size_ = size;
        return true;
    }
    
    void* view_;
    off_t size_;
    std::string name_;
    int fd_;
};

#endif

}


// --- SharedMemory ---------------------------------------------------------------------------------

SharedMemory::SharedMemory():
    pimpl_(new impl::SharedMemoryImpl())
{
}


SharedMemory::~SharedMemory()
{
    delete pimpl_;
}


bool SharedMemory::create(size_t size)
{
    return pimpl_->create(size);
}


bool SharedMemory::open(const char* pipeName)
{
    return pimpl_->open(pipeName);
}


void SharedMemory::close()
{
    pimpl_->close();
}


const char* SharedMemory::name() const
{
    return pimpl_->name();
}


size_t SharedMemory::size() const
{
    return pimpl_->size();
}


void* SharedMemory::get() const
{
    return pimpl_->get();
}


bool SharedMemory::operator!() const
{
    return !pimpl_->get();
}


}
}

// EOF
