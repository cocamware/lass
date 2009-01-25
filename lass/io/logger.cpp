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
 *	Copyright (C) 2004-2009 the Initial Developer.
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
	return (LASS_ENFORCE_POINTER(iProxy))->filter(&logFile_);
}



/** Set filter between proxy and logger
 *  @param iProxy logger must be subscribed to this proxy (and proxy should not be a NULL pointer),
 *                otherwise and exception is thrown.
 *  @param iFilter the filter between proxy and logger.
 */
void Logger::setFilter(ProxyOStream* iProxy, TMask iFilter)
{
	(LASS_ENFORCE_POINTER(iProxy))->setFilter(&logFile_, iFilter);
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
