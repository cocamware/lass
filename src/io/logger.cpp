/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "io_common.h"
#include "logger.h"

namespace lass
{
namespace io
{

/** Default constructor for the sake of static library.  We don't recommend of using it.
 *//*
Logger::Logger()
{
	openLog("lass.log", lmClear);
}
*/


/** Create a logger to a log file and specify if it must append to the file or clear it.
 */
Logger::Logger(const std::string &iLogFilename, LogMode iLogMode)
{
	openLog(iLogFilename, iLogMode);
}



/** Unsubscribe to all proxys
 */
Logger::~Logger()
{
	for (TProxyList::iterator pit = proxyList_.begin(); pit != proxyList_.end(); ++pit)
	{
		ProxyOStream* proxy = *pit;
		proxy->remove(&logFile_);
	}
}



/** Subscribe this logger to a proxy stream and set the filter on it.
 *  @param iProxy if null pointer, then an exception is thrown.
 *  @param iFilter filter between proxy and logger
 */
void Logger::subscribeTo(ProxyOStream* iProxy, TMask iFilter)
{
	proxyList_.push_back(LASS_ENFORCE_POINTER(iProxy));
	iProxy->add(&logFile_, iFilter);	
}



/** Unsubscribe this logger to a proxy stream
 *  @param iProxy if null pointer, then an exception is thrown.
 */
void Logger::unsubscribeTo(ProxyOStream* iProxy)
{
	proxyList_.remove(LASS_ENFORCE_POINTER(iProxy));
	iProxy->remove(&logFile_);
}



/** return the filter between proxy and logger
 *  @param iProxy logger must be subscribed to this proxy (and proxy should not be a NULL pointer),
 *                otherwise and exception is thrown.
 */
Logger::TMask Logger::filter(ProxyOStream* iProxy)
{
	return LASS_ENFORCE_POINTER(iProxy)->filter(&logFile_);
}



/** Set filter between proxy and logger
 *  @param iProxy logger must be subscribed to this proxy (and proxy should not be a NULL pointer),
 *                otherwise and exception is thrown.
 *  @param iFilter the filter between proxy and logger.
 */
void Logger::setFilter(ProxyOStream* iProxy, TMask iFilter)
{
	LASS_ENFORCE_POINTER(iProxy)->setFilter(&logFile_, iFilter);
}



/** open file to write logs to.
 *  You should do this in (and only in) the constructor.  The Logger doesn't like to have no
 *  filestreams open, or to change the file stream since it relies on it to subscribe to
 *  the different proxy streams.  So make sure that you have a file stream op after construction,
 *  and that you never change it!
 *
 *  QUESTION: is this true?  afterall, if you open a different file, the pointer to lofFile_
 *  will not change, but maybe the proxy streams don't like closed filestreams?
 */
void Logger::openLog(const std::string &iLogFilename, LogMode iLogMode)
{
	if (logFile_.is_open())
	{
		LASS_THROW("There's already an open log file!  Can't open new one ...");
	}

	std::ios::openmode mode = std::ios::out;
	mode |= (iLogMode == lmAppend) ? std::ios::app : std::ios::trunc;

	logFile_.open(iLogFilename.c_str(), mode);

	if (!logFile_.is_open())
	{
		LASS_THROW("Could not open log file '" << iLogFilename << "'");
	}
}

}

}

// EOF
