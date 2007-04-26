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

/** @defgroup LassErrno
 *  @internal
 *	@brief shields some last-error and error messages functions.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_LASS_ERRNO_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_LASS_ERRNO_H

#include "../util_common.h"

namespace lass
{
namespace util
{
namespace impl
{

const int lass_errno();
void lass_reset_errno();
const std::string lass_strerror(int iErrorNumber);

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32

#define LASS_HAS_GETLASTERROR
const unsigned lass_GetLastError();
const std::string lass_FormatMessage(unsigned error);

#endif

}
}
}

#endif

// EOF
