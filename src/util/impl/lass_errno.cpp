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
#include "lass_errno.h"

#ifdef HAVE_CONFIG_H
#	include "lass_auto_config.h"
#endif
#include <errno.h>
#include <string.h>

namespace lass
{
namespace util
{
namespace impl
{

const int lass_errno()
{
	return errno;
}

void lass_reset_errno()
{
	errno = 0;
}

const std::string lass_strerror(int iErrnum)
{
#if HAVE_DECL_STRERROR_R

	const size_t bufferLength = 256;
	char buffer[bufferLength + 1];
	errno = 0;
#	if STRERROR_R_CHAR_P
	const char* result = strerror_r(iErrnum, buffer, bufferLength);
	if (errno != 0)
	{
		result = 0;
	}
#	else
	const int rc = strerror_r(iErrnum, buffer, bufLen);
	const char* result = (rc != 0 && errno == EINVAL) ? buffer : 0;
#	endif
	return result ? std::string(buffer) : std::string("unknown error code");
	
#elif LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32

	return strerror(iErrnum);
	
#else

	std::ostringstream buffer;
	buffer << "errnum " << iErrnum;
	return buffer.str();
	
#endif
}

}
}
}

// EOF
