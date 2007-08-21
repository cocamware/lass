/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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
	LASS_ENFORCE_CLIB_RC(putenv(const_cast<char*>(buffer.str().c_str())));
#endif
}

}
}
}

// EOF
