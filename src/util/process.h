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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PROCESS_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PROCESS_H

#include "util_common.h"
#include "dictionary.h"
#include "singleton.h"

/** @defgroup Process
 *  @brief module concerning process settings
 */

namespace lass
{
namespace util
{

/** different process priority levels
 *  @ingroup Process
 */
enum ProcessPriority
{
	ppLow,
	ppBelowNormal,
	ppNormal,
	ppAboveNormal,
	ppHigh,
	numberOfProcessPriorities
};

/** type of dictionary that can translate strings to ProcessPriority values.
 *  @ingroup Process
 *  Its keys are strings, its values are ProcessPriority values.
 */
typedef util::Dictionary<std::string, ProcessPriority> TProcessPriorityDictionary;

/** singleton with dictionary that can translate strings to ProcessPriority.
 *  @ingroup Process
 *  Valid keys are "low", "belownormal", "normal", "abovenormal" and "high".
 *  @code
 *  ProcessPriority priority = processPriorityDictionary()["normal"];
 *  @endcode
 */
inline TProcessPriorityDictionary& processPriorityDictionary()
{
	return *util::Singleton<TProcessPriorityDictionary>::instance();
}

/** set process priority by ProcessPriority value
 *  @ingroup Process
 *  @param iPriority
 *		One of the values of ProcessPriority except numberOfProcessPriorities!
 *  @throw Exception
 *		An exception is thrown if setting the process priority fails.
 */
LASS_DLL void LASS_CALL setProcessPriority(ProcessPriority iPriority);

/** set process priority by string
 *  @ingroup Process
 *  @param iPriority
 *  	The key string which is looked up in processPriorityDictionary().
 *  	The return is used to set the process priority.  
 *		See TProcessPriorityDictionary for valid keys.
 *  @throw Exception
 *		An exception is thrown if @a iPriority is an invalid key or 
 *		setting the process priority fails.
 */
LASS_DLL void LASS_CALL setProcessPriority(const std::string& iPriority);

}

}

#endif

// EOF
