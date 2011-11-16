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



/** @defgroup extended_cstring
 *  @brief extra functions for plain old C strings
 *  @author Bram de Greve [BdG]
 */

#include "lass_common.h"
#include "extended_cstring.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4996) // 'vsnprintf': This function or variable may be unsafe.
#endif

namespace lass
{
namespace stde
{

std::string safe_vformat(const char* format, va_list args)
{
	size_t size = 256;
	std::vector<char> dynamicBuffer(size);
	while (true)
	{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
		const int numWritten = ::vsnprintf(&dynamicBuffer[0], size - 1, format, args);
#else		
		va_list ap;
		va_copy(ap, args);
		const int numWritten = ::vsnprintf(&dynamicBuffer[0], size, format, ap);
		va_end(ap);
#endif
		if (numWritten > 0 && numWritten < static_cast<int>(size))
		{
			return std::string(&dynamicBuffer[0]);
		}
		if (numWritten < 0)
		{
			size *= 2;
		}
		else
		{
			size = numWritten + 1;
		}
		if (size < dynamicBuffer.size() || static_cast<int>(size) < 0)
		{
			throw std::length_error("safe_vformat: buffer is growing to large");
		}
		dynamicBuffer.resize(size);
	}
}

std::string safe_format(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::string result = safe_vformat(format, args);
	va_end(args);
	return result;
}

}
}


// EOF
