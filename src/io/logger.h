/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2003
 *
 *  Construct a logger and subscribe it to proxy streams so it can log them.  All output to all proxy 
 *  streams it is subscribed to will be saved to a file.  Per proxy stream you can set a filter
 *  of what messages should be logged.  The logger will unsubscribe itself on destruction.
 *
 *  Logger logger("logfile.txt");		// construct logger\n
 *  logger.subscribe(proxyOut());		// subsribe to the proxy out stream.\n
 *  ...\n
 *  logger.unsubscribe(proxyOut());\n
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_LOGGER_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_LOGGER_H

#include "io_common.h"
#include "proxy_o_stream.h"
#include <fstream>

#pragma warning(push)
#pragma warning(disable: 4251) // on STL in dll interface ...



namespace lass
{
namespace io
{

class LASS_DLL Logger
{
public:

	enum LogMode
	{
		lmAppend,	// append to existing file
		lmClear		// on construction of the logger, clear the specified logfile
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

#pragma warning(pop)

#endif

// EOF