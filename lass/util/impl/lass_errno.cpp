/**     @file
 *      @author Bram de Greve (bram@cocamware.com)
 *      @author Tom De Muer (tom@cocamware.com)
 *
 *      *** BEGIN LICENSE INFORMATION ***
 *      
 *      The contents of this file are subject to the Common Public Attribution License 
 *      Version 1.0 (the "License"); you may not use this file except in compliance with 
 *      the License. You may obtain a copy of the License at 
 *      http://lass.sourceforge.net/cpal-license. The License is based on the 
 *      Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *      use of software over a computer network and provide for limited attribution for 
 *      the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *      with Exhibit B.
 *      
 *      Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *      WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *      language governing rights and limitations under the License.
 *      
 *      The Original Code is LASS - Library of Assembled Shared Sources.
 *      
 *      The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *      The Original Developer is the Initial Developer.
 *      
 *      All portions of the code written by the Initial Developer are:
 *      Copyright (C) 2004-2011 the Initial Developer.
 *      All Rights Reserved.
 *      
 *      Contributor(s):
 *
 *      Alternatively, the contents of this file may be used under the terms of the 
 *      GNU General Public License Version 2 or later (the GPL), in which case the 
 *      provisions of GPL are applicable instead of those above.  If you wish to allow use
 *      of your version of this file only under the terms of the GPL and not to allow 
 *      others to use your version of this file under the CPAL, indicate your decision by 
 *      deleting the provisions above and replace them with the notice and other 
 *      provisions required by the GPL License. If you do not delete the provisions above,
 *      a recipient may use your version of this file under either the CPAL or the GPL.
 *      
 *      *** END LICENSE INFORMATION ***
 */

#include "lass_common.h"
#include "lass_errno.h"

#include <errno.h>
#include <string.h>

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#       define NOMINMAX
#       define WIN32_LEAN_AND_MEAN
#       include <windows.h>
#       include "../../meta/meta_assert.h"
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
int lass_errno()
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
const std::string lass_strerror(int errnum)
{
	const char* msg = 0;

#if LASS_HAVE_FUNC_STRERROR_R

        const size_t bufferLength = 256;
        char buffer[bufferLength + 1];
        errno = 0;
#       if LASS_HAVE_STRERROR_R_CHAR_P
        msg = ::strerror_r(errnum, buffer, bufferLength);
        if (errno != 0)
        {
		msg = "[no error message due to strerror_r failure]";
        }
#       else
        const int rc = ::strerror_r(errnum, buffer, bufferLength);
        msg = (rc == 0) ? buffer : "[no error message due to strerror_r failure]";
#       endif
	if (!msg)
	{
                msg = "[no error message due to strerror_r failure]";
	}
        
#elif LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32

#       if LASS_COMPILER_VERSION >= 1400
        const size_t bufferLength = 256;
        char buffer[bufferLength + 1];
        const errno_t rc = strerror_s(buffer, bufferLength, errnum);
	msg = (rc == 0) ? buffer : "[no error message due to strerror_s failure]";
#       else
        msg = strerror(errnum);
#       endif

#endif

        std::ostringstream stream;
        stream << "errno " << errnum;
	if (msg)
	{
		stream << ": " << msg;
	}
        return stream.str();
}

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32

LASS_META_ASSERT(sizeof(DWORD) <= sizeof(unsigned), unsigned_should_be_big_enough_for_a_DWORD);

/** returns GetLastError()
 *  @ingroup LassErrno
 *  @internal
 *  @arg only available on Windows platforms
 */
unsigned lass_GetLastError()
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
