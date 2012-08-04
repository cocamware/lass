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

#include "test_common.h"
#include "../lass/util/wchar_support.h"

namespace lass
{
namespace test
{
void testUtilWCharSupport()
{
	LASS_TEST_CHECK_EQUAL(util::utf8ToWchar("").length(), size_t(0));
	LASS_TEST_CHECK_EQUAL(util::wcharToUtf8(L"").length(), size_t(0));

	const std::string utf8Simple = "a simple ascii string";
	const std::wstring wideSimple = L"a simple ascii string";
	LASS_TEST_CHECK_EQUAL(util::wcharToUtf8(wideSimple), utf8Simple);
	LASS_TEST_CHECK(util::utf8ToWchar(utf8Simple) == wideSimple);

	const std::wstring wideHelloWorld = L"\x2653\x212e\x0142\x029f\x263a \x0428\x263a\x0491\x2113\x1e13\x203c";
	const std::string utf8HelloWorld = "\xe2\x99\x93\xe2\x84\xae\xc5\x82\xca\x9f\xe2\x98\xba \xd0\xa8\xe2\x98\xba\xd2\x91\xe2\x84\x93\xe1\xb8\x93\xe2\x80\xbc";
	LASS_TEST_CHECK_EQUAL(wideHelloWorld.length(), size_t(12));
	LASS_TEST_CHECK_EQUAL(utf8HelloWorld.length(), size_t(30));
	LASS_TEST_CHECK_EQUAL(util::wcharToUtf8(wideHelloWorld), utf8HelloWorld);
	LASS_TEST_CHECK(util::utf8ToWchar(utf8HelloWorld) == wideHelloWorld);

#if LASS_HAVE_WCHAR_T == 4
	// UTF-32
	const std::wstring wideNonBmp = L"\x10337\x10334\x1033b\x1033b\x10349 \x10345\x10349\x10342\x1033b\x10333!";
	LASS_TEST_CHECK_EQUAL(wideNonBmp.length(), size_t(12));
#else
	// UTF-16
	const std::wstring wideNonBmp = L"\xd800\xdf37\xd800\xdf34\xd800\xdf3b\xd800\xdf3b\xd800\xdf49 \xd800\xdf45\xd800\xdf49\xd800\xdf42\xd800\xdf3b\xd800\xdf33!";
	LASS_TEST_CHECK_EQUAL(wideNonBmp.length(), size_t(22));
#endif
	const std::string utf8NonBmp = "\xf0\x90\x8c\xb7\xf0\x90\x8c\xb4\xf0\x90\x8c\xbb\xf0\x90\x8c\xbb\xf0\x90\x8d\x89 \xf0\x90\x8d\x85\xf0\x90\x8d\x89\xf0\x90\x8d\x82\xf0\x90\x8c\xbb\xf0\x90\x8c\xb3!";
	LASS_TEST_CHECK_EQUAL(utf8NonBmp.length(), size_t(42));
	LASS_TEST_CHECK_EQUAL(util::wcharToUtf8(wideNonBmp), utf8NonBmp);
	LASS_TEST_CHECK(util::utf8ToWchar(utf8NonBmp) == wideNonBmp);

}

TUnitTest test_util_wchar_support()
{
	return TUnitTest(1, LASS_TEST_CASE(testUtilWCharSupport));
}

}
}

// EOF
