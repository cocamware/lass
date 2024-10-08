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
 *	Copyright (C) 2004-2024 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_CSTRING_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_CSTRING_H

#include "stde_common.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	pragma warning(push)
#	pragma warning(disable: 4996) // 'vsnprintf': This function or variable may be unsafe.
#endif

// Forwarding va_list arguments to vsnprintf will cause Clang to complain about not being able 
// to check the arguments passed to vsnprintf.  To remedy, we need to tell the compiler that 
// the wrapper function's arguments also need to be checked to be compliant with the sprintf formats.
// If this works, we might move this to a more permanent location.
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_GCC || LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_CLANG
#	define LASS_EXPERIMENTAL_ATTRIBUTE_FORMAT(archetype, string_index, first_to_check) __attribute__ ((format (archetype, string_index, first_to_check)))
#else
#	define LASS_EXPERIMENTAL_ATTRIBUTE_FORMAT(archetype, string_index, first_to_check)
#endif

namespace lass
{
namespace stde
{

template <int N> int safe_vsprintf(char(&buffer)[N], const char* format, va_list args) LASS_EXPERIMENTAL_ATTRIBUTE_FORMAT(printf, 2, 0);
template <int N> int safe_sprintf(char(&buffer)[N], const char* format, ...) LASS_EXPERIMENTAL_ATTRIBUTE_FORMAT(printf, 2, 3);

template <int N>
int safe_vsprintf(char (&buffer)[N], const char* format, va_list args)
{
	const int numWritten = ::vsnprintf(buffer, N, format, args);
	if (numWritten < 0)
	{
		throw std::runtime_error("safe_vformat: encoding error");
	}
	if (numWritten >= N)
	{
		throw std::length_error("safe_vsprintf: buffer overflow.");
	}
	buffer[N - 1] = 0;
	return numWritten;
}

template <int N>
int safe_sprintf(char (& buffer)[N], const char* format, ...)
{
	va_list args;
	va_start(args, format);
	const int numWritten = safe_vsprintf(buffer, format, args);
	va_end(args);
	return numWritten;
}

template <size_t N>
char* safe_strcat(char (&buffer)[N], const char* source)
{
	if (strlen(buffer) + strlen(source) >= N)
	{
		throw std::length_error("safe_strcat: buffer overflow");
	}
	return strcat(buffer, source);
}

template <size_t N>
char* safe_strcpy(char (&buffer)[N], const char* source)
{
	if (strlen(source) >= N)
	{
		throw std::length_error("safe_strcpy: buffer overflow");
	}
	return strcpy(buffer, source);
}

LASS_DLL std::string LASS_CALL safe_vformat(const char* format, va_list args) LASS_EXPERIMENTAL_ATTRIBUTE_FORMAT(printf, 1, 0);
LASS_DLL std::string LASS_CALL safe_format(const char* format, ...) LASS_EXPERIMENTAL_ATTRIBUTE_FORMAT(printf, 1, 2);

}
}

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	pragma warning(pop)
#endif

#endif

// EOF
