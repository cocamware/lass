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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#include "lass_common.h"
#include "binary_o_socket.h"
#include "socket.h"
#include "../util/callback_0.h"
#include "../util/thread_fun.h"
#include "../num/num_cast.h"
#include <string.h>

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

BinaryOSocket::BinaryOSocket(size_t bufferSize, unsigned long flushPeriod):
	BinaryOStream(),
	socket_(0),
	requestedBufferSize_(bufferSize),
	current_(0),
	flushPeriod_(flushPeriod),
	stopFlushThread_(false),
	skipABeat_(false)
{
	init();
}



/**
 *	@param socket [in] BinaryISocket does _not_ take ownership of socket, and it must be alive as long as BinaryISocket needs it 
 *		(until BinaryISocket goes out of scope, or another socket is installed).
 */
BinaryOSocket::BinaryOSocket(Socket* socket, size_t bufferSize, unsigned long flushPeriod):
	BinaryOStream(),
	socket_(socket),
	requestedBufferSize_(bufferSize),
	current_(0),
	flushPeriod_(flushPeriod),
	stopFlushThread_(false),
	skipABeat_(false)
{
	init();
}



BinaryOSocket::~BinaryOSocket()
{
	stopFlushThread_ = true;
	skipABeat_ = false;
	try
	{
		LASS_LOCK(bufferLock_)
		{
			flushImpl();
		}
		flushCondition_.signal();
		flushThread_->join();
	}
	catch (std::exception& error)
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: exception thrown in ~BinaryOSocket(): " << error.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: unknown exception thrown in ~BinaryOSocket()" << std::endl;
	}
}



Socket* BinaryOSocket::socket() const
{
	return socket_;
}



/**
 *	@param socket [in] BinaryISocket does _not_ take ownership of socket, and it must be alive as long as BinaryISocket needs it 
 *		(until BinaryISocket goes out of scope, or another socket is installed).
 */
void BinaryOSocket::setSocket(Socket* socket)
{
	LASS_LOCK(bufferLock_)
	{
		flushImpl();
		socket_ = socket;
		init();
	}
}



// --- private -------------------------------------------------------------------------------------

long BinaryOSocket::doTellp() const
{
	LASS_THROW("no position in network streams");
}



void BinaryOSocket::doSeekp(long, std::ios_base::seekdir)
{
	LASS_THROW("no seeking in network streams");
}



void BinaryOSocket::doFlush()
{
	LASS_LOCK(bufferLock_)
	{
		flushImpl();
	}
}



/** write a buffer of bytes to the stream
 *  @par begin pointer to buffer.
 *  @par numberOfBytes length of buffer in bytes.
 */
void BinaryOSocket::doWrite(const void* begin, size_t numberOfBytes)
{
	LASS_LOCK(bufferLock_)
	{
		if (!socket_)
		{
			setstate(std::ios_base::badbit); // we won't be able to flush this data as there's nothing to flush it to ...
			return;
		}
		const char* first = static_cast<const char*>(begin);
		while (numberOfBytes > 0)
		{
			if (current_ == buffer_.size())
			{
				flushImpl();
			}

			if (!good())
			{
				return;
			}

			LASS_ASSERT(current_ < buffer_.size());
			const size_t freeSize = buffer_.size() - current_;
			const size_t writeSize = std::min(numberOfBytes, freeSize);

			::memcpy(&buffer_[current_], first, writeSize);
			current_ += writeSize;
			first += writeSize;
			numberOfBytes -= writeSize;
		}
		skipABeat_ = true;
	}
}



void BinaryOSocket::init()
{
	size_t size = requestedBufferSize_;
	if (socket_)
	{
		const size_t maxSize = num::numCast<size_t>(socket_->sizeSendBuffer());
		if (size)
		{
			size = std::min(size, maxSize);
		}
		else
		{
			size = maxSize;
		}
	}
	buffer_.resize(size);
	current_ = 0;

	if (!flushThread_)
	{
		flushThread_.reset(util::threadMemFun(this, &BinaryOSocket::flusher, util::threadJoinable));
		flushThread_->run();
	}
}



void BinaryOSocket::flusher()
{
	while (!stopFlushThread_)
	{
		LASS_TRY_LOCK( bufferLock_ )
		{
			if (!skipABeat_)
			{
				flushImpl();
			}
			skipABeat_ = false;
		}
		flushCondition_.wait(flushPeriod_);
	}
}


void BinaryOSocket::flushImpl()
{
	if (!good() || !socket_ || buffer_.empty())
	{
		return;
	}

	const char* begin = &buffer_[0];
	const size_t nMax = static_cast<size_t>(num::NumTraits<int>::max);

	while (current_ > 0)
	{
		const int n = static_cast<int>(current_ > nMax ? nMax : current_);
		LASS_ASSERT(n > 0 && static_cast<size_t>(n) <= current_);
		try
		{
			const int sent = socket_->send(begin, n);
			LASS_ASSERT(sent >= 0 && sent <= n);
			begin += sent;
			current_ -= static_cast<size_t>(sent);
		}
		catch (util::Exception&)
		{
			setstate(std::ios_base::badbit);
			return;
		}
	}
}


}

}

// EOF
