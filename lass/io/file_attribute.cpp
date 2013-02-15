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
#include "file_attribute.h"
#include "../stde/extended_string.h"
#include "../util/wchar_support.h"

#include <stdio.h>

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4996) // 'fopen': This function or variable may be unsafe
#endif

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** @ingroup FileAttribute
 *  return true if file exists
 */
bool fileDoesExist(const std::string& fileName)
{
#if LASS_HAVE_WCHAR_SUPPORT && LASS_HAVE_WFOPEN
	return fileDoesExist(util::utf8ToWchar(fileName));
#else
	FILE* file = fopen(fileName.c_str(), "r");
	if (file == 0)
	{
		return false;
	}
	fclose(file);
	return true;
#endif
}


#if LASS_HAVE_WCHAR_SUPPORT

/** @ingroup FileAttribute
 *  return true if file exists
 */
bool fileDoesExist(const std::wstring& fileName)
{
#if LASS_HAVE_WFOPEN
	FILE* file = _wfopen(fileName.c_str(), L"r");
	if (file == 0)
	{
		return false;
	}
	fclose(file);
	return true;
#else
	return fileDoesExist(util::wcharToUtf8(fileName));
#endif
}

#endif


/** @ingroup FileAttribute
 *  return the part of the file name behind the last dot.
 *  return an empty string if there's no dot.
 *  e.g. @e foo returns an empty string, @e foo.bar returns @e bar,
 *  @e foo.bar.fun returns @e fun.
 */
std::string fileExtension(const std::string& fileName)
{
	std::string::size_type lastDot = fileName.rfind(extensionSeperator);
	return lastDot!= std::string::npos ? fileName.substr(lastDot + 1) : "";
}


#if LASS_HAVE_WCHAR_SUPPORT

/** @ingroup FileAttribute
 *  return the part of the file name behind the last dot.
 *  return an empty string if there's no dot.
 *  e.g. @e foo returns an empty string, @e foo.bar returns @e bar,
 *  @e foo.bar.fun returns @e fun.
 */
std::wstring fileExtension(const std::wstring& fileName)
{
	std::wstring::size_type lastDot = fileName.rfind(wextensionSeperator);
	return lastDot!= std::wstring::npos ? fileName.substr(lastDot + 1) : L"";
}

#endif


/** @ingroup FileAttribute
 *  return the part of the file name before the last dot.
 *  return an original string if there's no dot.
 *  e.g. @e foo returns @e foo, @e foo.bar returns @e foo,
 *  @e foo.bar.fun returns @e foo.bar .
 */
std::string fileWithoutExtension(const std::string& fileName)
{
	return fileName.substr(0, fileName.rfind(extensionSeperator));
}


#if LASS_HAVE_WCHAR_SUPPORT

/** @ingroup FileAttribute
 *  return the part of the file name before the last dot.
 *  return an original string if there's no dot.
 *  e.g. @e foo returns @e foo, @e foo.bar returns @e foo,
 *  @e foo.bar.fun returns @e foo.bar .
 */
std::wstring fileWithoutExtension(const std::wstring& fileName)
{
	return fileName.substr(0, fileName.rfind(wextensionSeperator));
}

#endif


/** @ingroup FileAttribute
 *  return the part of the file name before the last (back)slash.
 *  return an empty string if there's no (back)slash.
 *  e.g. @e foo returns an empty string, @e foo/bar returns @e foo,
 *  @e foo/bar/fun returns @e foo/bar.
 */
std::string filePath(const std::string& fileName)
{
	const char seperators[] = { pathSeperator, pathAlternativeSeperator, '\0' }; 
	std::string::size_type lastSlash = fileName.find_last_of(seperators);
	return lastSlash != std::string::npos ? fileName.substr(0, lastSlash) : "";
}


#if LASS_HAVE_WCHAR_SUPPORT

/** @ingroup FileAttribute
 *  return the part of the file name before the last (back)slash.
 *  return an empty string if there's no (back)slash.
 *  e.g. @e foo returns an empty string, @e foo/bar returns @e foo,
 *  @e foo/bar/fun returns @e foo/bar.
 */
std::wstring filePath(const std::wstring& fileName)
{
	const wchar_t seperators[] = { wpathSeperator, wpathAlternativeSeperator, L'\0' }; 
	std::wstring::size_type lastSlash = fileName.find_last_of(seperators);
	return lastSlash != std::wstring::npos ? fileName.substr(0, lastSlash) : L"";
}

#endif


/** @ingroup FileAttribute
 *  return the part of the file name behind the last (back)slash.
 *  return the original string if there's no (back)slash.
 *  e.g. @e foo returns @e foo, @e foo/bar returns @e bar,
 *  @e foo/bar/fun returns @e fun .
 */
std::string fileWithoutPath(const std::string& fileName)
{
	const char seperators[] = { pathSeperator, pathAlternativeSeperator, '\0' }; 
	std::string::size_type lastSlash = fileName.find_last_of(seperators);
	return lastSlash != std::string::npos ? fileName.substr(lastSlash + 1) : fileName;
}


#if LASS_HAVE_WCHAR_SUPPORT

/** @ingroup FileAttribute
 *  return the part of the file name behind the last (back)slash.
 *  return the original string if there's no (back)slash.
 *  e.g. @e foo returns @e foo, @e foo/bar returns @e bar,
 *  @e foo/bar/fun returns @e fun .
 */
std::wstring fileWithoutPath(const std::wstring& fileName)
{
	const wchar_t seperators[] = { wpathSeperator, wpathAlternativeSeperator, L'\0' }; 
	std::wstring::size_type lastSlash = fileName.find_last_of(seperators);
	return lastSlash != std::wstring::npos ? fileName.substr(lastSlash + 1) : fileName;
}

#endif


/** @ingroup FileAttribute
 */
std::string fileJoinExtension(const std::string& fileName, const std::string& extension)
{
	if (stde::begins_with(extension, extensionSeperator))
	{
		return fileName + extension;
	}
	return fileName + extensionSeperator + extension;
}


#if LASS_HAVE_WCHAR_SUPPORT

/** @ingroup FileAttribute
 */
std::wstring fileJoinExtension(const std::wstring& fileName, const std::wstring& extension)
{
	if (stde::begins_with(extension, wextensionSeperator))
	{
		return fileName + extension;
	}
	return fileName + wextensionSeperator + extension;
}

#endif


/** @ingroup FileAttribute
 */
std::string fileJoinPath(const std::string& path, const std::string& fileName)
{
	if (stde::ends_with(path, pathSeperator))
	{
		return path + fileName;
	}
	if (pathAlternativeSeperator && stde::ends_with(path, pathAlternativeSeperator))
	{
		return path + fileName;
	}
	return path + pathSeperator + fileName;
}


#if LASS_HAVE_WCHAR_SUPPORT

/** @ingroup FileAttribute
 */
std::wstring fileJoinPath(const std::wstring& path, const std::wstring& fileName)
{
	if (stde::ends_with(path, wpathSeperator))
	{
		return path + fileName;
	}
	if (wpathAlternativeSeperator && stde::ends_with(path, wpathAlternativeSeperator))
	{
		return path + fileName;
	}
	return path + wpathSeperator + fileName;
}

#endif


}

}

// EOF
