/** @file
 *  @internal
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_IO_SOCKET_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_IO_SOCKET_INL

#include "test_common.h"

#include "../io/socket.h"
#include "../io/binary_i_socket.h"
#include "../io/binary_o_socket.h"
#include "../util/thread_fun.h"

namespace lass
{
namespace test
{
namespace socket
{

const short int port = 7331;

const int numberSentServerToClient = 01234;	
const int numberSentClientToServer = 56789;
const std::string messageSentClientToServer = "Hi there, how are you?";
const std::string messageSentServerToClient = "I'm fine.";

int numberReceivedServerToClient = 0;
int numberReceivedClientToServer = 0;
std::string messageReceivedClientToServer;
std::string messageReceivedServerToClient;

void serverThread()
{
	LASS_COUT << "starting server\n";
	io::Socket server;
	server.bind(port);
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

	TThreadPtr server(util::threadFun(serverThread, util::threadJoinable, "server"));
	TThreadPtr client(util::threadFun(clientThread, util::threadJoinable, "client"));

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

}

}

#endif

// EOF
