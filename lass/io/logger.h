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


/** @class lass::io::Logger
 *  @brief Logs proxy streams to files.
 *  @date 2003
 *
 *  Construct a logger and subscribe it to proxy streams so it can log them.  All output to all proxy
 *  streams it is subscribed to will be saved to a file.  Per proxy stream you can set a filter
 *  of what messages should be logged.  The logger will unsubscribe itself on destruction.
 *
 *  @code
 *  Logger logger("logfile.txt");       // construct logger\n
 *  logger.subscribe(proxyOut());       // subsribe to the proxy out stream.\n
 *  ...\n
 *  logger.unsubscribe(proxyOut());\n
 *  @endcode
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_LOGGER_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_LOGGER_H

#include "io_common.h"
#include "proxy_o_stream.h"
#include <fstream>

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4251) // on STL in dll interface ...
#endif


namespace lass
{
namespace io
{

class LASS_DLL Logger
{
public:

	enum LogMode
	{
		lmAppend,   // append to existing file
		lmClear     // on construction of the logger, clear the specified logfile
	};

	typedef ProxyOStream::TMask TMask;

	//Logger();
	Logger(const std::string& iLogFile, LogMode iLogMode = lmClear);
	~Logger();

	void subscribeTo(ProxyOStream* iProxy, TMask iFilter = ProxyOStream::acceptAll);
	void unsubscribeTo(ProxyOStream* iProxy);

	TMask filter(ProxyOStream* iProxy);
	void setFilter(ProxyOStream* iProxy, TMask iFilter);

private:

	typedef std::list<ProxyOStream*> TProxyList;

	void openLog(const std::string& iLogFile, LogMode iLogMode = lmClear);

	std::ofstream logFile_;
	TProxyList proxyList_;

};

}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif

// EOF
