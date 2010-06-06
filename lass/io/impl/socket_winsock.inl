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

	int send(const void* iBegin, int iLength)
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

	int receive(void* iBegin, int iLength)
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
