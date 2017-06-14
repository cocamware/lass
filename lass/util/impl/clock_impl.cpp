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



#include "lass_common.h"
#include "clock_impl.h"


#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32

// --- win32 ---------------------------------------------------------------------------------------

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace lass
{
namespace util
{
namespace impl
{

ClockImpl::TTick ClockImpl::frequency()
{
	TTick result;
	LASS_ENFORCE_WINAPI(QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&result)))
		("Hardware does not support a high-resolution performance counter.");
	return result;
}



ClockImpl::TTick ClockImpl::tick()
{
	TTick result;
	LASS_ENFORCE_WINAPI(QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&result)));
	return result;
}

}

}

}

#else

// --- POSIX ---------------------------------------------------------------------------------------

#include <sys/time.h>

namespace 
{

const lass::util::impl::ClockImpl::TTick nanoSecondsPerSecond = 1000000000;

// No longer using CLOCK_MONOTONIC_RAW for following reasons:
// - Not all kernels support it, even if it's defined, resulting in a runtime error.
//   We discovered this behavior on the Windows Subsystem for Linux (WSL)
// - It turns out to be more expensive, see section "Overhead of Clock Queries"
//   of http://btorpey.github.io/blog/2014/02/18/clock-sources-in-linux/
// - It's actually not what you want, because this clock isn't slewed by
//   time adjusting code. So it probably has a systematic error of running
//   either too fast or too slow.

// There's also question on FreeBSD whether you actually want CLOCK_MONOTONIC_PRECISE.
// It is more precise for sure, but it has a greater overhead ...

#if defined(CLOCK_MONOTONIC_PRECISE)
const clockid_t clkId = CLOCK_MONOTONIC_PRECISE;
#else
const clockid_t clkId = CLOCK_MONOTONIC;
#endif

}

namespace lass
{
namespace util
{
namespace impl
{

ClockImpl::TTick ClockImpl::frequency()
{
	return nanoSecondsPerSecond;
}



ClockImpl::TTick ClockImpl::tick()
{
    timespec tp;
    LASS_ENFORCE_CLIB(clock_gettime(clkId, &tp));
    return tp.tv_sec * nanoSecondsPerSecond + tp.tv_nsec;
}

}

}

}

#endif

// EOF
