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

	BinaryOSocket(size_t bufferSize = 0, unsigned long flushPeriod = 100);
	BinaryOSocket(Socket* socket, size_t bufferSize = 0, unsigned long flushPeriod = 100);
	~BinaryOSocket();

	Socket* socket() const;
	void setSocket(Socket* socket);

private:

	typedef std::vector<char> TBuffer;

	long doTellp() const;
	void doSeekp(long iOffset, std::ios_base::seekdir iDirection);
	void doWrite(const void* iBytes, size_t iNumberOfBytes);
	void doFlush();

	void init();
	void flushImpl();
	void flusher();

	Socket* socket_;
	TBuffer buffer_;
	size_t requestedBufferSize_;
	size_t current_;
	util::Semaphore bufferLock_;
	util::Condition flushCondition_;
	util::ScopedPtr<util::Thread> flushThread_;
	unsigned long flushPeriod_;
	volatile bool stopFlushThread_;
	volatile bool skipABeat_;
};

}

}

#endif
