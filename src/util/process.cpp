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

#include "util_common.h"
#include "process.h"
#include "dictionary.h"

#if HAVE_CONFIG_H
#	include "../../lass_auto_config.h"
#endif

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	pragma LASS_NOTE("util::setProcessPriority: using win32 implementation")
#	define LASS_UTIL_PROCESS_HAVE_WIN32
#elif HAVE_SYS_RESOURCE_H
#	define LASS_UTIL_PROCESS_HAVE_SYS_RESOURCE
#else
#	error lass/util/process.h is not supported on this platform
#endif

#if defined(LASS_UTIL_PROCESS_HAVE_WIN32)
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#elif defined(LASS_UTIL_PROCESS_HAVE_SYS_RESOURCE)
#	include <sys/resource.h>
#	if HAVE_LIMITS_H
#		include <limits.h>
#	endif
#	ifndef NZERO
#		define NZERO 20
#	endif
#endif

namespace lass
{
namespace util
{

LASS_EXECUTE_BEFORE_MAIN_EX(lassImpl_processPriorityDictionary,
		processPriorityDictionary().add("low", ppLow);
		processPriorityDictionary().add("belownormal", ppBelowNormal);
		processPriorityDictionary().add("normal", ppNormal);
		processPriorityDictionary().add("abovenormal", ppAboveNormal);
		processPriorityDictionary().add("high", ppHigh);
	)



void setProcessPriority(ProcessPriority iPriority)
{
	LASS_ASSERT(iPriority >= 0 && iPriority < numberOfProcessPriorities);

#if defined(LASS_UTIL_PROCESS_HAVE_WIN32)
	static const int winPriorities[numberOfProcessPriorities] = 
	{ 
		IDLE_PRIORITY_CLASS, 
		BELOW_NORMAL_PRIORITY_CLASS,
		NORMAL_PRIORITY_CLASS,
		ABOVE_NORMAL_PRIORITY_CLASS,
		HIGH_PRIORITY_CLASS
	};
	const int priority = winPriorities[iPriority];
	LASS_ENFORCE_WINAPI(SetPriorityClass(GetCurrentProcess(), priority))
		("Failed to set process priority");

#elif defined(LASS_UTIL_PROCESS_HAVE_SYS_RESOURCE)
	static const int niceValues[numberOfProcessPriorities] = 
	{
		NZERO - 1,
		NZERO / 2 - 1,
		0,
		-NZERO / 2,
		-NZERO
	};
	const int niceValue = niceValues[iPriority];
	LASS_ENFORCE_CLIB(setpriority(PRIO_PROCESS, 0, niceValue));

#else
#	error setProcessPriority is not supported on this platform
#endif
}



void setProcessPriority(const std::string& iPriority)
{
	ProcessPriority priority = ppNormal;
	try
	{
		priority = processPriorityDictionary()[iPriority];
	}
	catch(util::Exception)
	{
		LASS_THROW("'" << iPriority << "' is an invalid priority setting.  "
			"It should be one of these: " << processPriorityDictionary().keys() << ".");
	}

	setProcessPriority(priority);
}



}

}


// EOF
