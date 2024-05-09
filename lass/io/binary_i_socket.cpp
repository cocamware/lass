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
 *	Copyright (C) 2004-2024 the Initial Developer.
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
#include "binary_i_socket.h"
#include "socket.h"

namespace lass
{
namespace io
{

BinaryISocket::BinaryISocket():
	BinaryIStream(),
	socket_(0)
{
}



/**
 *	@param socket [in] BinaryISocket does _not_ take ownership of socket, and it must be alive as long as BinaryISocket needs it 
 *		(until BinaryISocket goes out of scope, or another socket is installed).
 */
BinaryISocket::BinaryISocket(Socket* socket):
	BinaryIStream(),
	socket_(socket)
{
}



Socket* BinaryISocket::socket() const
{
	return socket_;
}



/**
 *	@param socket [in] BinaryISocket does _not_ take ownership of socket, and it must be alive as long as BinaryISocket needs it 
 *		(until BinaryISocket goes out of scope, or another socket is installed).
 */
void BinaryISocket::setSocket(Socket* socket)
{
	socket_ = socket;
	setstate(std::ios_base::goodbit);
}



// --- private -------------------------------------------------------------------------------------

size_t BinaryISocket::doRead(void* oBegin, size_t iNumberOfBytes)
{
	if (!good())
	{
		return 0;
	}
	if (!socket_)
	{
		setstate(std::ios_base::failbit);
		return 0;
	}
	char* begin = static_cast<char*>(oBegin);
	int numberOfBytes = static_cast<int>(iNumberOfBytes);
	LASS_ASSERT(numberOfBytes >= 0 && static_cast<size_t>(numberOfBytes) == iNumberOfBytes);
	size_t bytesRead = 0;
	while (numberOfBytes > 0)
	{
		try
		{
			const int read = socket_->receive(begin, numberOfBytes);
			if (read == 0)
			{
				setstate(std::ios_base::eofbit);
				return 0;
			}
			LASS_ASSERT(read >= 0 && read <= numberOfBytes);
			begin += read;
			bytesRead += static_cast<size_t>(read);
			numberOfBytes -= read;
		}
		catch (const util::Exception&)
		{
			setstate(std::ios_base::badbit);
			return bytesRead;
		}
	}
	return bytesRead;
}



BinaryISocket::pos_type BinaryISocket::doTellg() const
{
	LASS_THROW("no position in network streams!");
	// return 0;
}



void BinaryISocket::doSeekg(pos_type)
{
	LASS_THROW("no seeking in network streams!");
}



void BinaryISocket::doSeekg(off_type, std::ios_base::seekdir)
{
	LASS_THROW("no seeking in network streams!");
}


}

}
