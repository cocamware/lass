/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4244) // conversion from '__w64 int' to 'cont int', possible loss of data
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
