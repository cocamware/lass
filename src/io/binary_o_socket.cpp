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
#include "binary_o_socket.h"
#include "socket.h"
#include "../util/callback_0.h"
#include "../util/thread_fun.h"

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

BinaryOSocket::BinaryOSocket(Socket& iSocket, size_t iBufferSize, unsigned long iFlushPeriod):
	BinaryOStream(),
	socket_(iSocket),
	buffer_(iBufferSize),
	bufferSize_(iBufferSize),
	current_(0),
	flushPeriod_(iFlushPeriod),
	stopFlushThread_(false)
{
	flushThread_.reset(util::threadFun(
		util::makeCallback(this, &BinaryOSocket::flusher), util::threadJoinable));
	flushThread_->run();
}



BinaryOSocket::~BinaryOSocket()
{
	stopFlushThread_ = true;
	flushCondition_.signal();
	flushThread_->join();
}



// --- private -------------------------------------------------------------------------------------

long BinaryOSocket::doTellp() const
{
	LASS_THROW("no position in network streams");
}



void BinaryOSocket::doSeekp(long iOffset, std::ios_base::seekdir iDirection)
{
	LASS_THROW("no seeking in network streams");
}



void BinaryOSocket::doFlush()
{
	flushCondition_.signal();
}



/** write a buffer of bytes to the stream
 *  @par iIn pointer to buffer.
 *  @par iBufferLength length of buffer in bytes.
 */
void BinaryOSocket::doWrite(const void* iBegin, size_t iNumberOfBytes)
{
	const char* begin = static_cast<const char*>(iBegin);
	while (iNumberOfBytes > 0)
	{
		LASS_LOCK(bufferLock_)
		{
			if (!good())
			{
				return;
			}

			if (current_ < bufferSize_)
			{
				const size_t freeSize = bufferSize_ - current_;
				const size_t writeSize = std::min(iNumberOfBytes, freeSize);

				::memcpy(&buffer_[current_], begin, writeSize);
				
				current_ += writeSize;
				if (current_ < bufferSize_)
				{
					LASS_ASSERT(writeSize == iNumberOfBytes);
					return;
				}

				LASS_ASSERT(writeSize < iNumberOfBytes);
				begin += writeSize;
				iNumberOfBytes -= writeSize;
			}
		}
		flushCondition_.signal();		
	}
}



void BinaryOSocket::flusher()
{
	while (true)
	{
		LASS_LOCK(bufferLock_)
		{
			if (current_ > 0)
			{
				const char* begin = &buffer_[0];
				int n = static_cast<int>(current_);
				LASS_ASSERT(n >= 0);
				while (n > 0)
				{
					try
					{
						const int sent = socket_.send(begin, static_cast<int>(current_));
						LASS_ASSERT(sent >= 0 && sent <= n);
						begin += sent;
						n -= sent;
					}
					catch (util::Exception&)
					{
						setstate(std::ios_base::badbit);
					}
				}
				current_ = 0;
			}
			if (stopFlushThread_)
			{
				return;
			}
		}
		flushCondition_.wait(flushPeriod_);
	}
}

}

}

// EOF
