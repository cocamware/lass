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
#include "wchar_support.h"
#include "../num/num_cast.h"

#if LASS_HAVE_MULTIBYTETOWIDECHAR
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	include <windows.h>
#endif

namespace lass
{
namespace util
{

#if LASS_HAVE_WCHAR_SUPPORT

std::wstring utf8ToWchar(const char* utf8) 
{ 
	return utf8ToWchar(utf8, ::strlen(utf8)); 
}

inline std::wstring utf8ToWchar(const std::string& utf8) 
{ 
	return utf8ToWchar(utf8.c_str(), utf8.length()); 
}

#	if LASS_HAVE_MULTIBYTETOWIDECHAR

LASS_META_ASSERT( sizeof(wchar_t) == sizeof(WCHAR), wchar_t_should_be_of_same_size_as_WCHAR ); 

std::wstring utf8ToWchar(const char* utf8, size_t length)
{
	const int utf8Bytes = num::numCast<int>(length + 1);
	const int wcharBytes = LASS_ENFORCE_WINAPI(MultiByteToWideChar(CP_UTF8, 0, utf8, utf8Bytes, 0, 0));
	std::vector<wchar_t> wideString(wcharBytes);
	LASS_ENFORCE_WINAPI(MultiByteToWideChar(CP_UTF8, 0, utf8, utf8Bytes, &wideString[0], wcharBytes));
	return std::wstring(&wideString[0]);
}

#	endif



inline std::string wcharToUtf8(const wchar_t* wide)
{
	return wcharToUtf8(wide, ::wcslen(wide));
}

inline std::string wcharToUtf8(const std::wstring& wide)
{
	return wcharToUtf8(wide.c_str(), wide.length());
}

#	if LASS_HAVE_MULTIBYTETOWIDECHAR

std::string wcharToUtf8(const wchar_t* wide, size_t length)
{
	const int wcharBytes = num::numCast<int>(length + 1);
	const int utf8Bytes = LASS_ENFORCE_WINAPI(WideCharToMultiByte(CP_UTF8, 0, wide, wcharBytes, 0, 0, NULL, NULL));
	std::vector<char> utf8String(utf8Bytes);
	LASS_ENFORCE_WINAPI(WideCharToMultiByte(CP_UTF8, 0, wide, wcharBytes, &utf8String[0], utf8Bytes, NULL, NULL));
	return std::string(&utf8String[0]);
}

#	endif

#endif

}
}

// EOF
