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
#include "environment.h"

#if HAVE_CONFIG_H
#	include "lass_auto_config.h"
#endif
#include <stdlib.h>

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC && LASS_COMPILER_VERSION >= 1400
#	define LASS_UTIL_IMPL_HAVE_ENV_S
#endif

namespace lass
{
namespace util
{
namespace impl
{

const std::string lass_getenv(const std::string& iName)
{
#ifdef LASS_UTIL_IMPL_HAVE_ENV_S
	size_t size;
	LASS_ENFORCE_CLIB_RC(getenv_s(&size, 0, 0, iName.c_str()));
	if (size == 0)
	{
		LASS_THROW("Could not find environment variable '" << iName << "'.");
	}
	std::vector<char> buffer(size);
	LASS_ENFORCE_CLIB_RC(getenv_s(&size, &buffer[0], size, iName.c_str()));
	return std::string(&buffer[0]);
#else
	const char* result = getenv(iName.c_str());
	if (!result)
	{
		LASS_THROW("Could not find environment variable '" << iName << "'.");
	}
	return std::string(result);
#endif
}

void lass_putenv(const std::string& iName, const std::string& iValue)
{
#ifdef LASS_UTIL_IMPL_HAVE_ENV_S
	LASS_ENFORCE_CLIB_RC(_putenv_s(iName.c_str(), iValue.c_str()));
#else
	std::stringstream buffer;
	buffer << iName << "=" << iValue;
	LASS_ENFORCE_CLIB_RC(putenv(buffer.str().c_str()));
#endif
}

}
}
}

// EOF
