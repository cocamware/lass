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
#include <winsock.h>

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	pragma comment(lib, "ws2_32.lib")
#endif

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
		socket_(INVALID_SOCKET)
	{
		WSADATA wsaData;
		if (WSAStartup(0x0202, &wsaData) != 0)
		{
			LASS_THROW_EX(SocketError, "Failed to startup Windows Socket API.");
		}
		if (wsaData.wVersion != 0x0202)
		{
			WSACleanup();
			LASS_THROW_EX(SocketError, "Windows Socket API is not version 2.2.");
		}
	}

	~SocketImpl()
	{
		if (socket_ != INVALID_SOCKET)
		{
			const int err = closesocket(socket_);
			LASS_ASSERT(err == 0);
		}

		const int err = WSACleanup();
		LASS_ASSERT(err == 0);
	}

	void bind(unsigned short iPortNumber)
	{
		openSocket();

		SOCKADDR_IN addr;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(iPortNumber);

		if (::bind(socket_, (LPSOCKADDR)&addr, sizeof(addr)) != 0)
		{
			LASS_THROW_EX(SocketError, "Failed to bind socket to port " << iPortNumber);
		}
	}

	void listen()
	{
		LASS_ASSERT(socket_ != INVALID_SOCKET);
		::listen(socket_, SOMAXCONN);
	}

	void accept(SocketImpl* oConnection)
	{
		LASS_ASSERT(socket_ != INVALID_SOCKET);
		SOCKET connnection = ::accept(socket_, 0, 0);
		if (connnection == INVALID_SOCKET)
		{
			LASS_THROW_EX(SocketError, "Failed to accept connection");
		}
		LASS_ASSERT(oConnection);
		oConnection->socket_ = connnection;
	}

	void connect(const std::string& iIpAddress, unsigned short iPortNumber)
	{
		openSocket();

		SOCKADDR_IN dest;
		dest.sin_addr.s_addr = inet_addr(iIpAddress.c_str());
		dest.sin_family = PF_INET;
		dest.sin_port = htons(iPortNumber);
		const int ret = ::connect(socket_, reinterpret_cast<SOCKADDR*>(&dest), sizeof(dest));
		if (ret == SOCKET_ERROR)
		{
			LASS_THROW_EX(SocketError, "could not connect " << iIpAddress << ":" << iPortNumber);
		}
	}

	const int send(const void* iBegin, int iLength)
	{
		LASS_ASSERT(socket_ != INVALID_SOCKET);
		const int ret = ::send(socket_, static_cast<const char*>(iBegin), iLength, 0);
		if (ret == SOCKET_ERROR)
		{
			LASS_THROW_EX(SocketError, "Failure to send data: " << WSAGetLastError());
		}
		LASS_ASSERT(ret >= 0);
		return ret;
	}

	const int receive(void* iBegin, int iLength)
	{
		const int ret = ::recv(socket_, static_cast<char*>(iBegin), iLength, 0);
		if (ret == SOCKET_ERROR)
		{
			LASS_THROW_EX(SocketError, "Failure to receive data: " << WSAGetLastError());
		}
		LASS_ASSERT(ret >= 0);
		return ret;
	}

private:

	void openSocket()
	{
		if (socket_ == INVALID_SOCKET)
		{
			socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (socket_ == INVALID_SOCKET)
			{
				LASS_THROW_EX(SocketError, "Failed to create socket.");
			}
		}
	}

	SOCKET socket_;
};

}
}
}

// EOF
