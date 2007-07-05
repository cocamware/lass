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

#include "../io_common.h"
#include "../socket.h"
#include "../../util/impl/lass_errno.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

namespace lass
{
namespace io
{
namespace impl
{

class SocketImpl: public util::NonCopyable
{
public:

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

	void bind(unsigned short portNumber)
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
			LASS_THROW(SocketError, "Failed to listen: " << util::impl::lass_strerror(err));
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

	const int send(const void* begin, int length)
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

	const int receive(void* begin, int length)
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
