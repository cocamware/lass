/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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
#include "clock_impl.h"

// --- win32 ---------------------------------------------------------------------------------------

#if defined(LASS_UTIL_CLOCK_WIN32)
#	pragma LASS_NOTE("util::Clock: using win32 implementation")

#include <windows.h>

namespace lass
{
namespace util
{
namespace impl
{

const ClockImpl::TTick ClockImpl::frequency()
{
	TTick result;
	if (!QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&result)))
	{
		LASS_THROW("Hardware does not support a high-resolution performance counter.");
	}
	return result;
}



const ClockImpl::TTick ClockImpl::tick()
{
	TTick result;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&result));
	return result;
}

}

}

}

#endif

// --- standard c ----------------------------------------------------------------------------------

#if defined(LASS_UTIL_CLOCK_STANDARD_C)
//#	if !defined(CLK_TCK)
//#		include <unistd.h>
//#		define CLK_TCK sysconf(_SC_CLK_TCK)
//#	endif
//#	pragma LASS_NOTE("util::Clock: using standard C implementation")

namespace lass
{
namespace util
{
namespace impl
{

const ClockImpl::TTick ClockImpl::frequency()
{
	return CLOCKS_PER_SEC;
}



const ClockImpl::TTick ClockImpl::tick()
{
	return ::clock();
}

}

}

}

#endif

// EOF
