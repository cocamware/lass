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



/** @defgroup FileAttribute FileAttribute
 *  @brief group of functions to test file attributes
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_FILE_ATTRIBUTE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_FILE_ATTRIBUTE_H

#include "io_common.h"
#include "../util/wchar_support.h"

namespace lass
{
namespace io
{

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
const char pathSeperator = '\\';
const char pathAlternativeSeperator = '/';
const char extensionSeperator = '.';
#else
const char pathSeperator = '/';
const char pathAlternativeSeperator = '\0';
const char extensionSeperator = '.';
#endif

LASS_DLL bool LASS_CALL fileDoesExist(const std::string& iFileName);
LASS_DLL std::string LASS_CALL fileExtension(const std::string& iFileName);
LASS_DLL std::string LASS_CALL fileWithoutExtension(const std::string& iFileName);
LASS_DLL std::string LASS_CALL filePath(const std::string& iFileName);
LASS_DLL std::string LASS_CALL fileWithoutPath(const std::string& iFileName);
LASS_DLL std::string LASS_CALL fileJoinExtension(const std::string& iFilename, const std::string& iExtension);
LASS_DLL std::string LASS_CALL fileJoinPath(const std::string& iPath, const std::string& iFilename);

#if LASS_HAVE_WCHAR_SUPPORT

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
const wchar_t wpathSeperator = L'\\';
const wchar_t wpathAlternativeSeperator = L'/';
const wchar_t wextensionSeperator = L'.';
#else
const wchar_t wpathSeperator = L'/';
const wchar_t wpathAlternativeSeperator = L'\0';
const wchar_t wextensionSeperator = L'.';
#endif

LASS_DLL bool LASS_CALL fileDoesExist(const std::wstring& iFileName);
LASS_DLL std::wstring LASS_CALL fileExtension(const std::wstring& iFileName);
LASS_DLL std::wstring LASS_CALL fileWithoutExtension(const std::wstring& iFileName);
LASS_DLL std::wstring LASS_CALL filePath(const std::wstring& iFileName);
LASS_DLL std::wstring LASS_CALL fileWithoutPath(const std::wstring& iFileName);
LASS_DLL std::wstring LASS_CALL fileJoinExtension(const std::wstring& iFilename, const std::wstring& iExtension);
LASS_DLL std::wstring LASS_CALL fileJoinPath(const std::wstring& iPath, const std::wstring& iFilename);

#endif

}

}

#endif

// EOF
