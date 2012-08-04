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
#include <cstring>

#if LASS_HAVE_MULTIBYTETOWIDECHAR
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	include <windows.h>
#	include "../num/num_cast.h"
#endif

#if LASS_HAVE_ICONV
#	include <iconv.h>
#	include <errno.h>
#	include "thread.h"
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

std::wstring utf8ToWchar(const std::string& utf8) 
{ 
	return utf8ToWchar(utf8.c_str(), utf8.length()); 
}

std::string wcharToUtf8(const wchar_t* wide)
{
	return wcharToUtf8(wide, ::wcslen(wide));
}

std::string wcharToUtf8(const std::wstring& wide)
{
	return wcharToUtf8(wide.c_str(), wide.length());
}

#	if LASS_HAVE_MULTIBYTETOWIDECHAR

std::wstring utf8ToWchar(const char* utf8, size_t length)
{
	const int utf8Bytes = num::numCast<int>(length + 1);
	const int wcharBytes = LASS_ENFORCE_WINAPI(MultiByteToWideChar(CP_UTF8, 0, utf8, utf8Bytes, 0, 0));
	std::vector<wchar_t> wideString(wcharBytes);
	LASS_ENFORCE_WINAPI(MultiByteToWideChar(CP_UTF8, 0, utf8, utf8Bytes, &wideString[0], wcharBytes));
	return std::wstring(&wideString[0]);
}

std::string wcharToUtf8(const wchar_t* wide, size_t length)
{
	const int wcharBytes = num::numCast<int>(length + 1);
	const int utf8Bytes = LASS_ENFORCE_WINAPI(WideCharToMultiByte(CP_UTF8, 0, wide, wcharBytes, 0, 0, NULL, NULL));
	std::vector<char> utf8String(utf8Bytes);
	LASS_ENFORCE_WINAPI(WideCharToMultiByte(CP_UTF8, 0, wide, wcharBytes, &utf8String[0], utf8Bytes, NULL, NULL));
	return std::string(&utf8String[0]);
}

#	elif LASS_HAVE_ICONV

namespace
{
	LASS_META_ASSERT(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4, expects_wchar_t_to_be_16_or_32_bit);

	class Converter
	{
	public:
		Converter(const char* tocode, const char* fromcode)
		{
			handle_ = LASS_ENFORCE_CLIB_EX( iconv_open(tocode, fromcode), (iconv_t)-1 );
		}
		~Converter()
		{
			iconv_close(handle_);
		}
		template <typename char_type>
		std::basic_string<char_type> convert(char* in, size_t inBytes)
		{
			std::basic_string<char_type> result;
			iconv(handle_, 0, 0, 0, 0); // reset
			while (inBytes)
			{
				char* out = buffer_;
				size_t outLeft = bufsize_;
				if (iconv(handle_, &in, &inBytes, &out, &outLeft) == (size_t)-1)
				{
					if (errno == E2BIG)
					{
						const size_t bytes = bufsize_ - outLeft;
						LASS_ASSERT(bytes % sizeof(char_type) == 0);
						result.append( reinterpret_cast<char_type*>( buffer_ ), bytes / sizeof(char_type) );
					}
					else
					{
						LASS_THROW("BLABLA");
					}
				}
				else
				{
					const size_t bytes = bufsize_ - outLeft;
					LASS_ASSERT(bytes % sizeof(char_type) == 0 && (bytes / sizeof(char_type)) > 0);
					result.append( reinterpret_cast<char_type*>( buffer_ ), bytes / sizeof(char_type) - 1 );
				}
			}
			return result;
		}
	private:
		enum { bufsize_ = 1024 };
		iconv_t handle_;
		char buffer_[bufsize_];
	};

	typedef ThreadLocalPtr<Converter> TConverterPtr;

	const char* wchar_encoding =
#		if LASS_HAVE_WCHAR_T == 2
#			if LASS_HAVE_BIG_ENDIAN
				"UTF-16BE";
#			else
				"UTF-16LE";
#			endif
#		elif LASS_HAVE_WCHAR_T == 4
#			if LASS_HAVE_BIG_ENDIAN
				"UTF-32BE";
#			else
				"UTF-32LE";
#			endif
#		else
#			error "[LASS BUILD MSG] unsupported wchar_t size"
#		endif
}

std::wstring utf8ToWchar(const char* utf8, size_t length)
{
	static TConverterPtr converter;
	if ( !converter )
	{
		std::auto_ptr<Converter> p(new Converter(wchar_encoding, "UTF-8"));
		converter.reset(p);
	}
	return converter->convert< wchar_t >( const_cast<char*>(utf8), length + 1 );
}

std::string wcharToUtf8(const wchar_t* wide, size_t length)
{
	static TConverterPtr converter;
	if ( !converter )
	{
		std::auto_ptr<Converter> p(new Converter("UTF-8", wchar_encoding));
		converter.reset(p);
	}
	return converter->convert< char >( (char*) wide, sizeof(wchar_t) * (length + 1) );
}

#	else

#		error "[LASS BUILD MSG] no implementation for this platform"

#	endif

#endif

}
}

// EOF
