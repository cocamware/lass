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

#include "test_common.h"

#include "../lass/io/socket.h"
#include "../lass/io/binary_i_socket.h"
#include "../lass/io/binary_o_socket.h"
#include "../lass/util/thread_fun.h"

namespace lass
{
namespace test
{
namespace socket
{

typedef short int TPort;

const TPort portBegin = 7331;
const TPort portEnd = 7431;
volatile TPort port = 0;

const int numberSentServerToClient = 01234;	
const int numberSentClientToServer = 56789;
const std::string messageSentClientToServer = "Hi there, how are you?";
const std::string messageSentServerToClient = "I'm fine.";

int numberReceivedServerToClient = 0;
int numberReceivedClientToServer = 0;
std::string messageReceivedClientToServer;
std::string messageReceivedServerToClient;

TPort bindRange(io::Socket& socket, TPort begin, TPort end)
{
	while (true)
	{
		try
		{
			socket.bind(begin);
			return begin;
		}
		catch (io::SocketError&)
		{
			++begin;
			if (begin == end)
			{
				throw;
			}
		}
	}
}

void serverThread()
{
	LASS_COUT << "starting server\n";
	io::Socket server;
	try
	{
		port = bindRange(server, portBegin, portEnd);
	}
	catch (io::SocketError&)
	{
		port = -1;
		throw;
	}
	LASS_COUT << "binded to port " << port << std::endl;
	server.listen();
	
	io::Socket connection;
	server.accept(connection);

	io::BinaryISocket incoming(connection);
	io::BinaryOSocket outgoing(connection);

	outgoing << numberSentServerToClient;
	incoming >> numberReceivedClientToServer;

	incoming >> messageReceivedClientToServer;
	util::Thread::sleep(500);
	outgoing << messageSentServerToClient;
}

void clientThread()
{
	LASS_COUT << "starting client\n";
	io::Socket client;
	while (port == 0)
	{
		util::Thread::sleep(10);
	}
	if (port == -1)
	{
		return;
	}
	client.connect("127.0.0.1", port);

	io::BinaryOSocket outgoing(client);
	io::BinaryISocket incoming(client);

	incoming >> numberReceivedServerToClient;
	outgoing << numberSentClientToServer;
	
	util::Thread::sleep(500);
	outgoing << messageSentClientToServer;
	incoming >> messageReceivedServerToClient;
}

}

void testIoSocket()
{
	using namespace socket;

	typedef util::ScopedPtr<util::Thread> TThreadPtr;

	TThreadPtr server(util::threadFun(serverThread, util::threadJoinable));
	TThreadPtr client(util::threadFun(clientThread, util::threadJoinable));

	server->run();
	util::Thread::sleep(500);
	client->run();

	server->join();
	client->join();

	LASS_TEST_CHECK_EQUAL(numberSentServerToClient, numberReceivedServerToClient);
	LASS_TEST_CHECK_EQUAL(numberSentClientToServer, numberReceivedClientToServer);
	LASS_TEST_CHECK_EQUAL(messageSentServerToClient, messageReceivedServerToClient);
	LASS_TEST_CHECK_EQUAL(messageSentClientToServer, messageReceivedClientToServer);
}

TUnitTest test_io_socket()
{
	return TUnitTest(1, LASS_TEST_CASE(testIoSocket));
}

}

}

// EOF
