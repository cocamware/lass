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

#include "util_common.h"
#include "process.h"

/** @fun lass::util::setProcessPriority
 *  @ingroup Process
 *  set priority of current process by a string value.
 *  @param iPriority
 *		valid values are @e low, @e belownormal, @e normal, @e abovenormal, @e high and @e realtime
 *  @warning
 *		Be carefull when setting the process priority.  Any value above @e normal might freeze your
 *		system (especially @e realtime does ;).
 */

#if defined(LASS_UTIL_PROCESS_WIN32)
#pragma LASS_NOTE("util::setProcessPriority: using win32 implementation")

#include "dictionary.h"
#include <windows.h>

namespace lass
{
namespace util
{

void setProcessPriority(const std::string& iPriority)
{
	Dictionary<std::string, int> priorities;
	priorities.add("low", IDLE_PRIORITY_CLASS);
	priorities.add("belownormal", BELOW_NORMAL_PRIORITY_CLASS);
	priorities.add("normal", NORMAL_PRIORITY_CLASS);
	priorities.add("abovenormal", ABOVE_NORMAL_PRIORITY_CLASS);
	priorities.add("high", HIGH_PRIORITY_CLASS);
	priorities.add("realtime", REALTIME_PRIORITY_CLASS);

	int priority;
	try
	{
		priority = priorities[iPriority];
	}
	catch(util::Exception)
	{
		LASS_THROW("'" << iPriority << "' is an invalid priority setting.  "
			"It should be one of these: " << priorities.keys() << ".");
	}

	if (SetPriorityClass(GetCurrentProcess(), priority) == 0)
	{
		LASS_THROW("SetPriorityClass failed with error code '" << GetLastError() << "'.");
	}
}

}

}

#endif

// EOF
