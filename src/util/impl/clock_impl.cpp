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
#include "clock_impl.h"

// --- win32 ---------------------------------------------------------------------------------------

#if defined(LASS_UTIL_CLOCK_WIN32)
#	pragma LASS_NOTE("util::Clock: using win32 implementation")

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
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
	LASS_ENFORCE_WINAPI(QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&result)))
		("Hardware does not support a high-resolution performance counter.");
	return result;
}



const ClockImpl::TTick ClockImpl::tick()
{
	TTick result;
	LASS_ENFORCE_WINAPI(QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&result)));
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
