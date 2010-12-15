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

#include "lass_common.h"
#include "binary_o_socket.h"
#include "socket.h"
#include "../util/callback_0.h"
#include "../util/thread_fun.h"
#include <string.h>

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



void BinaryOSocket::doSeekp(long, std::ios_base::seekdir)
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
