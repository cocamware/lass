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

/** @class lass::io::BinaryOSocket
 *  @brief BinaryOStream to socket.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_SOCKET_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_SOCKET_H

#include "io_common.h"
#include "binary_o_stream.h"
#include "../util/scoped_ptr.h"
#include "../util/thread.h"

namespace lass
{
namespace io
{

class Socket;

class LASS_DLL BinaryOSocket: public BinaryOStream
{
public:

	BinaryOSocket(Socket& iSocket, std::size_t iBufferSize = 1024, 
		unsigned long iFlushPeriod = 100);
	~BinaryOSocket();

private:

	typedef std::vector<char> TBuffer;

	long doTellp() const;
	void doSeekp(long iOffset, std::ios_base::seekdir iDirection);
	void doWrite(const void* iBytes, size_t iNumberOfBytes);
	void doFlush();

	void flusher();

	Socket& socket_;
	TBuffer buffer_;
	std::size_t bufferSize_;
	std::size_t current_;
	util::Semaphore bufferLock_;
	util::Condition flushCondition_;
	util::ScopedPtr<util::Thread> flushThread_;
	unsigned long flushPeriod_;
	volatile bool stopFlushThread_;
};

}

}

#endif
