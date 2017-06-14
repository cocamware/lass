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
#include "../lass/stde/extended_cstring.h"

namespace lass
{
namespace test
{

void testStdeSafeSprintf()
{
	using stde::safe_sprintf;

	char buffer[20];

	LASS_TEST_CHECK_EQUAL(safe_sprintf(buffer, "%s", ""), 0);
	LASS_TEST_CHECK_EQUAL(safe_sprintf(buffer, "%s", "hallo!"), 6);

	const char* veryLongString = "this is a very long string that should cause a lot of troubles!";
	LASS_TEST_CHECK_THROW(safe_sprintf(buffer, "%s", veryLongString), std::length_error);
	
	LASS_TEST_CHECK_EQUAL(safe_sprintf(buffer, "%s", "1234567890123456789"), 19);
	LASS_TEST_CHECK_THROW(safe_sprintf(buffer, "%s", "12345678901234567890"), std::length_error);
	LASS_TEST_CHECK_THROW(safe_sprintf(buffer, "%s", "123456789012345678901"), std::length_error);

#if !LASS_STDE_VSNPRINTF_MSVC_OLD
	const wchar_t* wide = L"\x2653\x212e\x0142\x029f\x263a \x0428\x263a\x0491\x2113\x1e13\x203c";
	LASS_TEST_CHECK_THROW(safe_sprintf(buffer, "%ls", wide), std::runtime_error);
#endif
}

void testStdeSafeFormat()
{
	using stde::safe_format;

	LASS_TEST_CHECK_EQUAL(safe_format("%s", "").length(), size_t(0));

	LASS_TEST_CHECK_EQUAL(safe_format("%s %s!", "hello", "world"), "hello world!");

	const char* a = "12345678901234567890123456789012345678901234567890";
	LASS_TEST_CHECK_EQUAL(strlen(a), size_t(50));
	LASS_TEST_CHECK_EQUAL(safe_format("%s%s%s%s%s", a, a, a, a, a).length(), size_t(250));
	LASS_TEST_CHECK_EQUAL(safe_format("%s%s%s%s%s1234", a, a, a, a, a).length(), size_t(254));
	LASS_TEST_CHECK_EQUAL(safe_format("%s%s%s%s%s12345", a, a, a, a, a).length(), size_t(255));
	LASS_TEST_CHECK_EQUAL(safe_format("%s%s%s%s%s123456", a, a, a, a, a).length(), size_t(256));
	LASS_TEST_CHECK_EQUAL(safe_format("%s%s%s%s%s1234567", a, a, a, a, a).length(), size_t(257));
	LASS_TEST_CHECK_EQUAL(safe_format("%s%s%s%s%s%s%s%s%s%s", a, a, a, a, a, a, a, a, a, a).length(), size_t(500));

#if !LASS_STDE_VSNPRINTF_MSVC_OLD
	const wchar_t* wide = L"\x2653\x212e\x0142\x029f\x263a \x0428\x263a\x0491\x2113\x1e13\x203c";
	LASS_TEST_CHECK_THROW(safe_format("%ls", wide), std::runtime_error);
#endif
}

TUnitTest test_stde_extended_cstring()
{
	TUnitTest tests;
	tests.push_back(LASS_TEST_CASE(testStdeSafeSprintf));
	tests.push_back(LASS_TEST_CASE(testStdeSafeFormat));
	return tests;
}

}

}

// EOF
