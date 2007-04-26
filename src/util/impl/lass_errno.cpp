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
#include "lass_errno.h"

#if HAVE_CONFIG_H
#	include "lass_auto_config.h"
#endif
#include <errno.h>
#include <string.h>

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include "../../meta/meta_assert.h"
#endif

namespace lass
{
namespace util
{
namespace impl
{

/** returns CLIB errno
 *  @ingroup LassErrno
 *  @internal
 */
const int lass_errno()
{
	return errno;
}

/** sets CLIB errno to zero.
 *  @ingroup LassErrno
 *  @internal
 */
void lass_reset_errno()
{
	errno = 0;
}

/** returns message associated to an CLIB error code
 *  @ingroup LassErrno
 *  @internal
 */
const std::string lass_strerror(int iErrnum)
{
#if HAVE_DECL_STRERROR_R

	const size_t bufferLength = 256;
	char buffer[bufferLength + 1];
	errno = 0;
#	if STRERROR_R_CHAR_P
	const char* result = ::strerror_r(iErrnum, buffer, bufferLength);
	if (errno != 0)
	{
		result = 0;
	}
#	else
	const int rc = ::strerror_r(iErrnum, buffer, bufferLength);
	const char* result = (rc != 0 && errno == EINVAL) ? buffer : 0;
#	endif
	return result ? std::string(buffer) : std::string("[no error message due to strerror_r failure]");
	
#elif LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32

#	if LASS_COMPILER_VERSION >= 1400
	const size_t bufferLength = 256;
	char buffer[bufferLength + 1];
	const errno_t rc = strerror_s(buffer, bufferLength, iErrnum);
	if (rc != 0)
	{
		return "[no error message due to strerror_s failure]";
	}
	return std::string(buffer);
#	else
	return strerror(iErrnum);
#	endif

#else

	std::ostringstream buffer;
	buffer << "errnum " << iErrnum;
	return buffer.str();
	
#endif
}

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32

LASS_META_ASSERT(sizeof(DWORD) <= sizeof(unsigned), unsigned_should_be_big_enough_for_a_DWORD);

/** returns GetLastError()
 *  @ingroup LassErrno
 *  @internal
 *  @arg only available on Windows platforms
 */
const unsigned lass_GetLastError()
{
	return GetLastError();
}

/** returns message associated to an Win API error code.
 *  @ingroup LassErrno
 *  @internal
 *  @arg only available on Windows platforms
 */
const std::string lass_FormatMessage(unsigned error)
{
	LASS_ASSERT(static_cast<DWORD>(error) == error);
	LASS_ASSERT(error != 0);

	LPSTR buffer;
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
			0, static_cast<DWORD>(error), 0, (LPTSTR) &buffer, 0, 0))
	{
		return "[no error message due to FormatMessage failure]";
	}
	std::string result = buffer;
	LocalFree(buffer);
	return result;
}

#endif

}
}
}

// EOF
