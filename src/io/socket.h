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



/** @class lass::io::Socket
 *  @brief TCP/IP socket
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_SOCKET_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_SOCKET_H

#include "io_common.h"
#include "../util/non_copyable.h"

namespace lass
{
namespace io
{

class Socket: public util::NonCopyable
{
public:

	Socket();
	~Socket();

	void bind(unsigned short iPort);
	void listen();
	void accept(Socket& oConnection);
	void connect(const std::string& iIpAddress, unsigned short iPort);

	const int send(const void* iBegin, int iLength);
	const void* const send(const void* iBegin, const void* iEnd);
	const int receive(void* iBegin, int iLength);
	void* const receive(void* iBegin, void* iEnd);

	void swap(Socket& ioOther);

private:
	void* pimpl_;
};

}
}

#endif

// EOF
