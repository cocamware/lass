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
#include "socket.h"

#if HAVE_CONFIG_H
#	include "lass_auto_config.h"
#endif

#if HAVE_SYS_SOCKET_H && !defined(LASS_IO_SOCKET_HAVE_LIBC)
#	define LASS_IO_SOCKET_HAVE_LIBC
#endif

#if defined(LASS_IO_SOCKET_HAVE_WINSOCK)
#	include "impl/socket_winsock.inl"
#elif defined(LASS_IO_SOCKET_HAVE_LIBC)
#	include "impl/socket_libc.inl"
#else
#	error "[LASS BUILD MSG] Socket not supported for this platform"
#endif

namespace lass
{
namespace io
{

Socket::Socket()
{
	pimpl_ = new impl::SocketImpl;
}




Socket::~Socket()
{
	LASS_ASSERT(pimpl_);
	impl::SocketImpl* pimpl = static_cast<impl::SocketImpl*>(pimpl_);
	delete pimpl;
	pimpl_ = 0;
}



void Socket::bind(unsigned short iPortNumber)
{
	LASS_ASSERT(pimpl_);
	impl::SocketImpl* pimpl = static_cast<impl::SocketImpl*>(pimpl_);
	pimpl->bind(iPortNumber);
}



void Socket::listen()
{
	LASS_ASSERT(pimpl_);
	impl::SocketImpl* pimpl = static_cast<impl::SocketImpl*>(pimpl_);
	pimpl->listen();
}



void Socket::accept(Socket& oConnection)
{
	LASS_ASSERT(pimpl_);
	impl::SocketImpl* pimpl = static_cast<impl::SocketImpl*>(pimpl_);
	LASS_ASSERT(oConnection.pimpl_);
	impl::SocketImpl* connection = static_cast<impl::SocketImpl*>(oConnection.pimpl_);
	pimpl->accept(connection);
}



void Socket::connect(const std::string& iIpAddress, unsigned short iPort)
{
	LASS_ASSERT(pimpl_);
	impl::SocketImpl* pimpl = static_cast<impl::SocketImpl*>(pimpl_);
	pimpl->connect(iIpAddress, iPort);
}


const int Socket::send(const void* iBegin, int iLength)
{
	LASS_ASSERT(pimpl_);
	impl::SocketImpl* pimpl = static_cast<impl::SocketImpl*>(pimpl_);
	return pimpl->send(iBegin, iLength);
}



const void* const Socket::send(const void* iBegin, const void* iEnd)
{
	LASS_ASSERT(pimpl_);
	impl::SocketImpl* pimpl = static_cast<impl::SocketImpl*>(pimpl_);
	const char* begin = static_cast<const char*>(iBegin);
	const char* end = static_cast<const char*>(iEnd);
	const int length = end - begin;
	const int sent = pimpl->send(begin, length);
	LASS_ASSERT(sent >= 0);
	return begin + sent;
}



const int Socket::receive(void* iBegin, int iLength)
{
	LASS_ASSERT(pimpl_);
	impl::SocketImpl* pimpl = static_cast<impl::SocketImpl*>(pimpl_);
	return pimpl->receive(iBegin, iLength);
}



void* const Socket::receive(void* iBegin, void* iEnd)
{
	LASS_ASSERT(pimpl_);
	impl::SocketImpl* pimpl = static_cast<impl::SocketImpl*>(pimpl_);
	char* begin = static_cast<char*>(iBegin);
	char* end = static_cast<char*>(iEnd);
	const int length = end - begin;
	const int sent = pimpl->receive(begin, length);
	LASS_ASSERT(sent >= 0);
	return begin + sent;
}



void Socket::swap(Socket& ioOther)
{
	std::swap(pimpl_, ioOther.pimpl_);
}



}
}

// EOF
