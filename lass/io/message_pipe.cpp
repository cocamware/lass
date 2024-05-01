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
 *	Copyright (C) 2004-2024 the Initial Developer.
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
#include "message_pipe.h"
#include "../stde/extended_cstring.h"
#include "../num/num_cast.h"

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#else
#   if LASS_HAVE_SYS_SOCKET_H
#       include <sys/socket.h>
#   endif
#   if LASS_HAVE_LINUX_UN_H
#       include <linux/un.h>
#   elif LASS_HAVE_SYS_UN_H
#       include <sys/un.h>
#   endif
#   if LASS_HAVE_FCNTL_H
#       include <fcntl.h>
#   endif
#   if LASS_HAVE_POLL_H
#       include <poll.h>
#   endif
#   if LASS_HAVE_UNISTD_H
#       include <unistd.h>
#   endif
#   include <errno.h>
#endif

#include <assert.h>
#include <string.h>

namespace lass
{
namespace io
{

namespace impl
{

#ifdef _WIN32

class MessagePipeImpl
{
public:
    static constexpr size_t infinite = MessagePipe::infinite;

    MessagePipeImpl(size_t bufferSize):
        pipe_(INVALID_HANDLE_VALUE),
        bufferSize_(static_cast<DWORD>(bufferSize))
    {
        name_[0] = 0;
    }

    ~MessagePipeImpl()
    {
        close();
    }

    bool create()
    {
        close();

        const unsigned long pid = GetCurrentProcessId();
        stde::safe_sprintf(name_, "\\\\.\\pipe\\LassIOMessagePipe_%lx_%i", pid, pipeId_++);
        name_[maxNameLength] = 0;

        if (!initOverlapped())
        {
            return false;
        }

        pipe_ = CreateNamedPipe(
            name_,
            PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,
            bufferSize_,
            bufferSize_,
            0,
            0);

        return pipe_ != INVALID_HANDLE_VALUE;
    }

    bool connect(const char* pipeName, size_t msecTimeout)
    {
        close();

        stde::safe_strcpy(name_, pipeName);
        name_[maxNameLength] = 0;

        if (!initOverlapped())
        {
            return false;
        }

        // attempt to connect
        const size_t attempts = msecTimeout == infinite ? 1 : std::max<size_t>(msecTimeout / 1000, 1);
        const DWORD msecTimeoutPerAttemt = dwordTimeout(msecTimeout / attempts);
        for (size_t k = 0; k <= attempts; ++k) // always try once more
        {
            pipe_ = CreateFile(
                pipeName,
                FILE_READ_DATA | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES,
                0,
                0,
                OPEN_EXISTING,
                SECURITY_IDENTIFICATION | SECURITY_SQOS_PRESENT | FILE_FLAG_OVERLAPPED,
                0);
            if (pipe_ != INVALID_HANDLE_VALUE)
            {
                DWORD mode = PIPE_READMODE_MESSAGE;
                if (!SetNamedPipeHandleState(pipe_, &mode, 0, 0))
                {
                    close();
                    return false;
                }
                return true;
            }
            if (GetLastError() != ERROR_PIPE_BUSY || k == attempts)
            {
                return false;
            }
            if (!WaitNamedPipe(pipeName, msecTimeoutPerAttemt))
            {
                return false;
            }
        }
        return false;
    }

    bool accept(size_t msecTimeout)
    {
        if (ConnectNamedPipe(pipe_, &overlapped_ ))
        {
            return true; // w00t! connected.
        }
        switch ( GetLastError() )
        {
        case ERROR_PIPE_CONNECTED:
            return true; // was connected before the call. that's fine too.
        case ERROR_IO_PENDING:
            {
                if (WaitForSingleObject( overlapped_.hEvent, dwordTimeout(msecTimeout)) != WAIT_OBJECT_0)
                {
                    CancelIo( pipe_ );
                    return false;
                }
                DWORD dummy;
                return GetOverlappedResult(pipe_, &overlapped_, &dummy, FALSE) != 0;
            }
        default:
            return false;
        }
    }

    void close()
    {
        if (pipe_ != INVALID_HANDLE_VALUE)
        {
            CloseHandle(pipe_);
            pipe_ = INVALID_HANDLE_VALUE;
        }
    }

    const char* name() const
    {
        return name_;
    }

    bool operator!() const
    {
        return pipe_ == INVALID_HANDLE_VALUE;
    }

    bool send(const void* out, size_t size, size_t msecTimeout) const
    {
        assert(size <= bufferSize_);
        if (!WriteFile(pipe_, out, static_cast<DWORD>(size), 0, &overlapped_))
        {
            if ( GetLastError() != ERROR_IO_PENDING )
            {
                return false;
            }
            if ( WaitForSingleObject( overlapped_.hEvent, dwordTimeout(msecTimeout)) != WAIT_OBJECT_0 )
            {
                CancelIo( pipe_ );
                return false;
            }
        }
        DWORD bytesWritten = 0;
        if ( !GetOverlappedResult( pipe_, &overlapped_, &bytesWritten, FALSE ) )
        {
            return false;
        }
        return bytesWritten == size;
    }

    bool receive(void* in, size_t size, size_t msecTimeout) const
    {
        assert(size <= bufferSize_);
        if ( !ReadFile(pipe_, in, static_cast<DWORD>(size),0, &overlapped_) )
        {
            if ( GetLastError() != ERROR_IO_PENDING )
            {
                return false;
            }
            if ( WaitForSingleObject( overlapped_.hEvent, dwordTimeout(msecTimeout)) != WAIT_OBJECT_0 )
            {
                CancelIo( pipe_ );
                return false;
            }
        }
        DWORD bytesRead = 0;
        if ( !GetOverlappedResult( pipe_, &overlapped_, &bytesRead, FALSE ) )
        {
            return false;
        }
        return bytesRead == size;
    }

    bool transact(const void* out, size_t sizeOut, void* in, size_t sizeIn, size_t msecTimeout=INFINITE) const
    {
        assert(sizeOut <= bufferSize_ && sizeIn <= bufferSize_);
        if ( !TransactNamedPipe(pipe_, const_cast<void*>(out), static_cast<DWORD>(sizeOut), in, static_cast<DWORD>(sizeIn), 0, &overlapped_) )
        {
            if ( GetLastError() != ERROR_IO_PENDING )
            {
                return false;
            }
            if ( WaitForSingleObject( overlapped_.hEvent, dwordTimeout(msecTimeout)) != WAIT_OBJECT_0 )
            {
                CancelIo( pipe_ );
                return false;
            }
        }
        DWORD bytesRead = 0;
        if ( !GetOverlappedResult( pipe_, &overlapped_, &bytesRead, FALSE ) )
        {
            return false;
        }
        return bytesRead == sizeIn;
    }

private:
    static DWORD dwordTimeout(size_t msecTimeout)
    {
        return msecTimeout == MessagePipe::infinite ? INFINITE : num::numCast<DWORD>(msecTimeout);
    }

    bool initOverlapped()
    {
        memset(&overlapped_, 0, sizeof(OVERLAPPED));
        overlapped_.hEvent = CreateEvent(0, TRUE, FALSE, 0);
        return overlapped_.hEvent != 0;
    }

    enum
    {
        maxNameLength = 256 // according to MSDN docs on named pipes.
    };

    HANDLE pipe_;
    mutable OVERLAPPED overlapped_;
    char name_[maxNameLength + 1];
    DWORD bufferSize_;
    static unsigned pipeId_;
};

unsigned MessagePipeImpl::pipeId_ = 0;

#else

#ifndef UNIX_PATH_MAX
    // FreeBSD's sys/un.h doesn't have UNIX_PATH_MAX
    // Actually, Linux's has neither, but we use linux/un.h instead.
#   define UNIX_PATH_MAX sizeof(((sockaddr_un*)0)->sun_path)
#endif

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_LINUX
#   define LASS_HAVE_ABSTRACT_NAMES 1
#endif

class MessagePipeImpl
{
public:

    static constexpr size_t infinite = MessagePipe::infinite;

    MessagePipeImpl(size_t /*bufferSize*/):
        socket_(-1),
        pipe_(-1),
        fd_(-1),
        isServer_(false)
    {
        name_[0] = 0;
    }

    bool create()
    {
        close();

        // open a socket
        socket_ = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (socket_ < 0)
        {
            return false;
        }

        // Not sure if this is actually necessary? Autobind should work without.
        // In any case, it's only available on Linux
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_LINUX
        const int passcred = 1;
        if (setsockopt(socket_, SOL_SOCKET, SO_PASSCRED, &passcred, sizeof(passcred)) != 0)
        {
            close();
            return false;
        }
#endif

        // make it non-blocking
        if (fcntl(socket_, F_SETFL, O_NONBLOCK) != 0)
        {
            close();
            return false;
        }

        sockaddr_un addr;
        memset(&addr, 0, sizeof(sockaddr_un));
        addr.sun_family = AF_UNIX;
#if LASS_HAVE_ABSTRACT_NAMES
        // autobind: don't set the name, the length just includes the family
        socklen_t addr_len = sizeof(sa_family_t);
#else
        // BSD doesn't have the concept of abstract names,
        // so create a temporary file in /tmp.
        stde::safe_strcpy(name_, "/tmp/lass-sock-XXXXXX");
        fd_ = mkstemp(name_);
        if (fd_ == -1)
        {
            close();
            return false;
        }
        stde::safe_strcpy(addr.sun_path, name_);
        socklen_t addr_len = sizeof(sa_family_t) + static_cast<socklen_t>(::strlen(addr.sun_path));
#endif
        if (bind(socket_, reinterpret_cast<sockaddr*>(&addr), addr_len) != 0)
        {
            close();
            return false;
        }

#if LASS_HAVE_ABSTRACT_NAMES
        // let's retrieve the actual socket name.
        addr_len = sizeof(sockaddr_un);
        if(getsockname(socket_, reinterpret_cast<sockaddr*>(&addr), &addr_len) != 0)
        {
            close();
            return false;
        }
        if (addr_len <= sizeof(sa_family_t))
        {
            return false; // err, no autobind?
        }
        assert(addr.sun_path[0] == 0); // abstract names start with null, and don't have a terminating null
        size_t name_len = addr_len - sizeof(sa_family_t) - 1; // without leading null
        assert(name_len < sizeof(name_));
        memcpy(name_, &addr.sun_path[1], name_len); // don't copy leading null
        name_[name_len] = 0; // set a terminating null
#endif

        // already start listening, so that child can already connect,
        // even if we didnt' connect yet.
        if (listen(socket_, 1) != 0)
        {
            close();
            return false;
        }

        isServer_ = true;
        return true;
    }

    bool connect(const char* pipeName, size_t msecTimeout)
    {
        close();

        // open a socket
        socket_ = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (socket_ < 0)
        {
            return false;
        }

        // make it non-blocking
        if (fcntl(socket_, F_SETFL, O_NONBLOCK) != 0)
        {
            close();
            return false;
        }

        // set its name
        assert(strlen(pipeName) < UNIX_PATH_MAX);
        const size_t name_len = std::min<size_t>(strlen(pipeName), UNIX_PATH_MAX - 1);
        memcpy(name_, pipeName, name_len);
        name_[name_len] = 0;
        sockaddr_un addr;
        addr.sun_family = AF_UNIX;
#if LASS_HAVE_ABSTRACT_NAMES
        memset(addr.sun_path, 0, UNIX_PATH_MAX); // fill with zeroes.
        memcpy(&addr.sun_path[1], pipeName, name_len);
        socklen_t addr_len = static_cast<socklen_t>(sizeof(sa_family_t) + name_len + 1);
#else
        memcpy(&addr.sun_path[0], pipeName, name_len);
        socklen_t addr_len = static_cast<socklen_t>(sizeof(sa_family_t) + name_len);
#endif

        if ( ::connect(socket_, reinterpret_cast<sockaddr*>(&addr), addr_len) == -1 )
        {
            switch (errno)
            {
            case EINPROGRESS:
            case EALREADY:
            case EINTR:
                break;
            default:
                close();
                return false;
            }

            if ( !poll(socket_, POLLOUT, msecTimeout) )
            {
                close();
                return false;
            }

            int so_error;
            socklen_t length = sizeof(so_error);
            if ( ::getsockopt(socket_, SOL_SOCKET, SO_ERROR, &so_error, &length) == -1 )
            {
                close();
                return false;
            }
            if ( so_error != 0 )
            {
                close();
                return false;
            }
        }

        // we're good!
        pipe_ = socket_; // is same thing
        isServer_ = false;
        return true;
    }

    bool accept(size_t msecTimeout)
    {
        assert(isServer_);
        if (socket_ < 0)
        {
            return false;
        }
        if (pipe_ >= 0)
        {
            ::close(pipe_);
        }

        if (!poll(socket_, POLLIN, msecTimeout))
        {
            return false;
        }

        pipe_ = ::accept(socket_, 0, 0);

        if (fcntl(pipe_, F_SETFL, O_NONBLOCK) != 0)
        {
            close();
            return false;
        }

        return pipe_ >= 0;
    }

    void close()
    {
        if (isServer_ && pipe_ >= 0)
        {
            ::close(pipe_);
            pipe_ = -1;
        }
        if (socket_ >= 0)
        {
            ::close(socket_);
            socket_ = -1;
        }
        if (fd_ >= 0)
        {
            ::close(fd_);
            fd_ = -1;
            ::unlink(name_);
        }
    }

    const char* name() const
    {
        return name_;
    }

    bool operator!() const
    {
        return socket_ < 0;
    }

    bool send(const void* out, size_t size, size_t msecTimeout) const
    {
        if (!poll(pipe_, POLLOUT, msecTimeout))
        {
            return false;
        }

        msghdr hdr;
        memset(&hdr, 0, sizeof(msghdr));

        iovec msg;
        msg.iov_base = const_cast<void*>(out);
        msg.iov_len = size;
        hdr.msg_iov = &msg;
        hdr.msg_iovlen = 1;
        hdr.msg_flags = MSG_EOR;

        return sendmsg(pipe_, &hdr, 0) >= 0;
    }
    bool receive(void* in, size_t size, size_t msecTimeout) const
    {
        if (!poll(pipe_, POLLIN, msecTimeout))
        {
            return false;
        }

        msghdr hdr;
        memset(&hdr, 0, sizeof(msghdr));

        iovec msg;
        msg.iov_base = in;
        msg.iov_len = size;
        hdr.msg_iov = &msg;
        hdr.msg_iovlen = 1;

        if (recvmsg(pipe_, &hdr, 0) <= 0) // -1 is error, 0 is orderly shutdown
        {
            return false;
        }

        // normally, msg_flags should have MSG_EOR set, but linux doesn't care.
        return !(hdr.msg_flags & MSG_TRUNC);
    }
    bool transact(const void* out, size_t sizeOut, void* in, size_t sizeIn, size_t msecTimeout) const
    {
        return send(out, sizeOut, msecTimeout) && receive(in, sizeIn, msecTimeout);
    }

private:

    bool poll(int fd, short events, size_t msecTimeout) const
    {
        pollfd pfd;
        pfd.fd = fd;
        pfd.events = events;
        pfd.revents = 0;

        int timeout = -1;
        num::Tint64 msecDeadline = 0;
        if (msecTimeout != infinite)
        {
            timeout = num::numCast<int>(msecTimeout);
            msecDeadline = msecTime() + timeout;
        }

        int rc = ::poll(&pfd, 1, timeout);
        while (true)
        {
            switch(rc)
            {
            case -1:
                if (errno != EINTR)
                {
                    return false;
                }
                break;
            case 0:
                return false;
            default:
                LASS_ASSERT( rc == 1 );
                return pfd.revents & events;
            }

            if (msecTimeout != infinite)
            {
                const num::Tint64 timeleft = msecDeadline - msecTime();
                if (timeleft < 0)
                    return false;
                timeout = static_cast<int>(timeleft);
            }

            rc = ::poll(&pfd, 1, timeout);
        }
    }

    num::Tint64 msecTime() const
    {
        struct timespec tp;
        LASS_ENFORCE_CLIB(clock_gettime(CLOCK_MONOTONIC, &tp));
        return static_cast<num::Tint64>(tp.tv_sec) * 1000 + static_cast<num::Tint64>(tp.tv_nsec) / 1000000;
    }

    int socket_;
    int pipe_;
    int fd_;
    bool isServer_;
    char name_[UNIX_PATH_MAX];
};

#endif

}

// --- MessagePipe ---------------------------------------------------------------------------------

MessagePipe::MessagePipe(size_t bufferSize):
    pimpl_(new impl::MessagePipeImpl(bufferSize))
{
}


MessagePipe::~MessagePipe()
{
    delete pimpl_;
}


bool MessagePipe::create()
{
    return pimpl_->create();
}


bool MessagePipe::connect(const char* pipeName, size_t msecTimeout)
{
    return pimpl_->connect(pipeName, msecTimeout);
}


bool MessagePipe::accept(size_t msecTimeout)
{
    return pimpl_->accept(msecTimeout);
}


void MessagePipe::close()
{
    pimpl_->close();
}


const char* MessagePipe::name() const
{
    return pimpl_->name();
}


bool MessagePipe::operator!() const
{
    return !(*pimpl_);
}


bool MessagePipe::send(const void* out, size_t size, size_t msecTimeout) const
{
    return pimpl_->send(out, size, msecTimeout);
}


bool MessagePipe::receive(void* in, size_t size, size_t msecTimeout) const
{
    return pimpl_->receive(in, size, msecTimeout);
}


bool MessagePipe::transact(const void* out, size_t outSize, void* in, size_t inSize, size_t msecTimeout) const
{
    return pimpl_->transact(out, outSize, in, inSize, msecTimeout);
}


}
}

// EOF
