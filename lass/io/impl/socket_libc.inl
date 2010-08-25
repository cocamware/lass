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

#include "../io_common.h"
#include "../socket.h"
#include "../../util/impl/lass_errno.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wconversion" // htons, can't avoid conversion within macro.

namespace lass
{
namespace io
{
namespace impl
{

class SocketImpl: public util::NonCopyable
{
public:
	typedef Socket::TPort TPort;

	SocketImpl():
		socket_(invalidSocket)
	{
	}

	~SocketImpl()
	{
		try
		{
			closeSocket();
		}
		catch (std::exception& error)
		{
			std::cerr << "[LASS RUN MSG] WARNING: closeSocket() failed: " << error.what();
		}
	}

	void bind(TPort portNumber)
	{
		openSocket();

		::sockaddr_in addr;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(portNumber);

		if (::bind(socket_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
		{
			const int err = util::impl::lass_errno();
			LASS_THROW_EX(SocketError, "Failed to bind socket to port " << portNumber 
				<< ": " << util::impl::lass_strerror(err));
		}
	}

	void listen()
	{
		LASS_ASSERT(socket_ != invalidSocket);
		if (::listen(socket_, SOMAXCONN) != 0)
		{
			const int err = util::impl::lass_errno();
			LASS_THROW_EX(SocketError, "Failed to listen: " << util::impl::lass_strerror(err));
		}
	}

	void accept(SocketImpl* connection)
	{
		LASS_ASSERT(socket_ != invalidSocket);
		int socket = ::accept(socket_, 0, 0);
		if (socket == invalidSocket)
		{
			const int err = util::impl::lass_errno();
			LASS_THROW_EX(SocketError, "Failed to accept connection: " << util::impl::lass_strerror(err));
		}
		LASS_ASSERT(connection);
		connection->socket_ = socket;
	}

	void connect(const std::string& ipAddress, unsigned short portNumber)
	{
		openSocket();
		
		::hostent* other = gethostbyname(ipAddress.c_str());
		if (!other)
		{
			LASS_THROW_EX(SocketError, "could not connect " << ipAddress << ":" << portNumber
				<< " : failed to lookup hostname.");
		}		

		sockaddr_in dest;
		dest.sin_addr = *(in_addr*) other->h_addr;
		dest.sin_family = PF_INET;
		dest.sin_port = htons(portNumber);
		const int ret = ::connect(socket_, reinterpret_cast<sockaddr*>(&dest), sizeof(dest));
		if (ret != 0)
		{
			const int err = util::impl::lass_errno();
			LASS_THROW_EX(SocketError, "Could not connect " << ipAddress << ":" << portNumber 
				<< " : " << util::impl::lass_strerror(err));
		}
	}

	int send(const void* begin, int length)
	{
		LASS_ASSERT(socket_ != invalidSocket);
		const int ret = ::send(socket_, static_cast<const char*>(begin), length, 0);
		if (ret == -1)
		{
			const int err = util::impl::lass_errno();
			LASS_THROW_EX(SocketError, "Failed to send data: " << util::impl::lass_strerror(err));
		}
		LASS_ASSERT(ret >= 0);
		return ret;
	}

	int receive(void* begin, int length)
	{
		const int ret = ::recv(socket_, static_cast<char*>(begin), length, 0);
		if (ret == -1)
		{
			const int err = util::impl::lass_errno();
			LASS_THROW_EX(SocketError, "Failed to receive data: " << util::impl::lass_strerror(err));
		}
		LASS_ASSERT(ret >= 0);
		return ret;
	}

private:

	enum { invalidSocket = -1 };

	void openSocket()
	{
		if (socket_ == invalidSocket)
		{
			socket_ = socket(AF_INET, SOCK_STREAM, 0);
			if (socket_ == invalidSocket)
			{
				const int err = util::impl::lass_errno();
				LASS_THROW_EX(SocketError, "Failed to create socket: " << util::impl::lass_strerror(err));
			}
		}
	}
	
	void closeSocket()
	{
		if (socket_ != invalidSocket)
		{
			const int ret = close(socket_);
			socket_ = invalidSocket;
			if (ret != 0)
			{
				const int err = util::impl::lass_errno();
				LASS_THROW_EX(SocketError, "Failed to close socket: " << util::impl::lass_strerror(err));
			} 
		}
	}

	int socket_;
};

}
}
}

// EOF
