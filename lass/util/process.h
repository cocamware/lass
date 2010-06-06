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
