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
#include "binary_i_socket.h"
#include "socket.h"

namespace lass
{
namespace io
{

BinaryISocket::BinaryISocket(Socket& iSocket):
	BinaryIStream(),
	socket_(iSocket)
{
}



// --- private -------------------------------------------------------------------------------------

void BinaryISocket::doRead(void* oBegin, size_t iNumberOfBytes)
{
	if (!good())
	{
		return;
	}
	char* begin = static_cast<char*>(oBegin);
	while (iNumberOfBytes > 0)
	{
		try
		{
			const int read = socket_.receive(begin, iNumberOfBytes);
			LASS_ASSERT(read >= 0 && read <= iNumberOfBytes);
			begin += read;
			iNumberOfBytes -= read;
		}
		catch (util::Exception&)
		{
			setstate(std::ios_base::badbit);
			return;
		}
	}
}



long BinaryISocket::doTellg() const
{
	LASS_THROW("no position in network streams!");
	return 0;
}



void BinaryISocket::doSeekg(long iOffset, std::ios_base::seekdir iDirection)
{
	LASS_THROW("no seeking in network streams!");
}


}

}
