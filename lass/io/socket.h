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
namespace impl
{
	class SocketImpl;
}

/** @relates lass::io::Socket
 */
class SocketError: public util::ExceptionMixin<SocketError>
{
public:
	SocketError(const std::string& msg, const std::string& loc): util::ExceptionMixin<SocketError>(msg, loc) {}
	~SocketError() throw() {}
};



class LASS_DLL Socket: public util::NonCopyable
{
public:

	typedef unsigned short TPort;

	Socket();
	~Socket();

	void open();
	void close();

	void bind(TPort port = 0);
	void bind(const std::string& address, TPort port = 0);
	TPort bindInRange(TPort begin, TPort end);
	TPort bindInRange(const std::string& address, TPort begin, TPort end);
	void listen();
	void accept(Socket& oConnection) const;
	void connect(const std::string& address, TPort port);

	std::string address() const;
	TPort port() const;

	int send(const void* begin, int length) const;
	const void* send(const void* begin, const void* end) const;
	int receive(void* begin, int length) const;
	void* receive(void* begin, void* end) const;

	int sizeSendBuffer() const;

	void swap(Socket& other);

private:
	impl::SocketImpl* pimpl_;
};

}
}

#endif

// EOF
