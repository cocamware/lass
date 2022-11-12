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
 *	Copyright (C) 2022 the Initial Developer.
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
#include "python_embedding.h"
#include "../lass/python/python_api.h"


#define TEST_PYTHON_STRING_ROUNDTRIP(in, out, kind) \
	{\
		TPyObjPtr obj(pyBuildSimpleObject(in));\
		LASS_TEST_CHECK(PyUnicode_Check(obj.get()));\
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyUnicode_KIND(obj.get())), kind);\
		std::remove_const<decltype(out)>::type t;\
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), t), 0);\
		LASS_TEST_CHECK(t == out);\
	}

namespace lass
{
namespace test
{

void testPythonExportTraitsString()
{
	using namespace lass::python;
	
	initPythonEmbedding();
		
	{
		TPyObjPtr obj(PyUnicode_FromString(""));
		LASS_TEST_CHECK(obj.get());
		LASS_TEST_CHECK(PyUnicode_Check(obj.get()));
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyUnicode_KIND(obj.get())), PyUnicode_1BYTE_KIND);

		std::string s;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), s), 0);
		LASS_TEST_CHECK(s.empty());
		std::wstring ws;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), ws), 0);
		LASS_TEST_CHECK(ws.empty());
#if LASS_HAVE_STD_U8STRING
		std::u8string u8s;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), u8s), 0);
		LASS_TEST_CHECK(u8s.empty());
#endif
		std::u16string u16s;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), u16s), 0);
		LASS_TEST_CHECK(u16s.empty());
		std::u32string u32s;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), u32s), 0);
		LASS_TEST_CHECK(u32s.empty());
	}

	{
		TPyObjPtr obj(PyUnicode_FromStringAndSize("\0", 1));
		LASS_TEST_CHECK(obj.get());
		LASS_TEST_CHECK(PyUnicode_Check(obj.get()));
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyUnicode_KIND(obj.get())), PyUnicode_1BYTE_KIND);

		std::string s;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), s), 0);
		LASS_TEST_CHECK_EQUAL(s.size(), size_t(1));
		LASS_TEST_CHECK(s[0] == '\0');
		std::wstring ws;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), ws), 0);
		LASS_TEST_CHECK_EQUAL(ws.size(), size_t(1));
		LASS_TEST_CHECK(ws[0] == L'\0');
#if LASS_HAVE_STD_U8STRING
		std::u8string u8s;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), u8s), 0);
		LASS_TEST_CHECK_EQUAL(u8s.size(), size_t(1));
		LASS_TEST_CHECK(u8s[0] == u8'\0');
#endif
		std::u16string u16s;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), u16s), 0);
		LASS_TEST_CHECK_EQUAL(u16s.size(), size_t(1));
		LASS_TEST_CHECK(u16s[0] == u'\0');
		std::u32string u32s;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), u32s), 0);
		LASS_TEST_CHECK_EQUAL(u32s.size(), size_t(1));
		LASS_TEST_CHECK(u32s[0] == U'\0');
	}

	{
		// ASCII
		const std::string s = "hello world!";
		const std::wstring ws = L"hello world!";
		const std::u16string u16s = u"hello world!";
		const std::u32string u32s = U"hello world!";

		TEST_PYTHON_STRING_ROUNDTRIP(s, s, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(s, ws, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(s, u16s, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(s, u32s, PyUnicode_1BYTE_KIND)
			
		TEST_PYTHON_STRING_ROUNDTRIP(ws, s, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u16s, s, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u32s, s, PyUnicode_1BYTE_KIND)

#if LASS_HAVE_STD_U8STRING
		const std::u8string u8s = u8"hello world!";
		TEST_PYTHON_STRING_ROUNDTRIP(s, u8s, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u8s, s, PyUnicode_1BYTE_KIND)
#endif
	}

	{
		// Latin-1, basically the first 256 charaeters of Unicode.
		// It can still fit in one byte, but it's not UTF-8!
		const std::string s = "h\xc3\xa8ll\xc3\xb4\xc2\xa0w\xc3\xb5\xc2\xaeld\xc2\xa1";
		const std::wstring ws = L"h\xe8ll\xf4\xa0w\xf5\xaeld\xa1";
		const std::u16string u16s = u"h\xe8ll\xf4\xa0w\xf5\xaeld\xa1";
		const std::u32string u32s = U"h\xe8ll\xf4\xa0w\xf5\xaeld\xa1";

		TEST_PYTHON_STRING_ROUNDTRIP(s, s, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(s, ws, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(s, u16s, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(s, u32s, PyUnicode_1BYTE_KIND)
			
		TEST_PYTHON_STRING_ROUNDTRIP(ws, s, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u16s, s, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u32s, s, PyUnicode_1BYTE_KIND)

#if LASS_HAVE_STD_U8STRING
		const std::u8string u8s = u8"h\xe8ll\xf4\xa0w\xf5\xaeld\xa1";
		TEST_PYTHON_STRING_ROUNDTRIP(s, u8s, PyUnicode_1BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u8s, s, PyUnicode_1BYTE_KIND)
#endif
	}

	{
		// UCS-2
		const std::string s = "\xe2\x99\x93\xe2\x84\xae\xc5\x82\xca\x9f\xe2\x98\xba \xd0\xa8\xe2\x98\xba\xd2\x91\xe2\x84\x93\xe1\xb8\x93\xe2\x80\xbc";
		const std::wstring ws = L"\x2653\x212e\x0142\x029f\x263a \x0428\x263a\x0491\x2113\x1e13\x203c";
		const std::u16string u16s = u"\x2653\x212e\x0142\x029f\x263a \x0428\x263a\x0491\x2113\x1e13\x203c";
		const std::u32string u32s = U"\x2653\x212e\x0142\x029f\x263a \x0428\x263a\x0491\x2113\x1e13\x203c";

		TEST_PYTHON_STRING_ROUNDTRIP(s, s, PyUnicode_2BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(s, ws, PyUnicode_2BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(s, u16s, PyUnicode_2BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(s, u32s, PyUnicode_2BYTE_KIND)

		TEST_PYTHON_STRING_ROUNDTRIP(s, u16s, PyUnicode_2BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(ws, u16s, PyUnicode_2BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u32s, u16s, PyUnicode_2BYTE_KIND)

#if LASS_HAVE_STD_U8STRING
		const std::u8string u8s = u8"\x2653\x212e\x0142\x029f\x263a \x0428\x263a\x0491\x2113\x1e13\x203c";
		TEST_PYTHON_STRING_ROUNDTRIP(s, u8s, PyUnicode_2BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u8s, u16s, PyUnicode_2BYTE_KIND)
#endif
	}

	{
		// UCS-4
		const std::string s = "\xf0\x90\x8c\xb7\xf0\x90\x8c\xb4\xf0\x90\x8c\xbb\xf0\x90\x8c\xbb\xf0\x90\x8d\x89 \xf0\x90\x8d\x85\xf0\x90\x8d\x89\xf0\x90\x8d\x82\xf0\x90\x8c\xbb\xf0\x90\x8c\xb3!";
#if LASS_HAVE_WCHAR_T == 4
		const std::wstring ws = L"\x10337\x10334\x1033b\x1033b\x10349 \x10345\x10349\x10342\x1033b\x10333!";
#else
		const std::wstring ws = L"\xd800\xdf37\xd800\xdf34\xd800\xdf3b\xd800\xdf3b\xd800\xdf49 \xd800\xdf45\xd800\xdf49\xd800\xdf42\xd800\xdf3b\xd800\xdf33!";
#endif
		const std::u16string u16s = u"\xd800\xdf37\xd800\xdf34\xd800\xdf3b\xd800\xdf3b\xd800\xdf49 \xd800\xdf45\xd800\xdf49\xd800\xdf42\xd800\xdf3b\xd800\xdf33!";
		const std::u32string u32s = U"\x10337\x10334\x1033b\x1033b\x10349 \x10345\x10349\x10342\x1033b\x10333!";

		TEST_PYTHON_STRING_ROUNDTRIP(u32s, s, PyUnicode_4BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u32s, ws, PyUnicode_4BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u32s, u16s, PyUnicode_4BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u32s, u32s, PyUnicode_4BYTE_KIND)

		TEST_PYTHON_STRING_ROUNDTRIP(s, u32s, PyUnicode_4BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(ws, u32s, PyUnicode_4BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u16s, u32s, PyUnicode_4BYTE_KIND)

#if LASS_HAVE_STD_U8STRING
		const std::u8string u8s = u8"\x10337\x10334\x1033b\x1033b\x10349 \x10345\x10349\x10342\x1033b\x10333!";
		TEST_PYTHON_STRING_ROUNDTRIP(u32s, u8s, PyUnicode_4BYTE_KIND)
		TEST_PYTHON_STRING_ROUNDTRIP(u8s, u32s, PyUnicode_4BYTE_KIND)
#endif

	}
}



TUnitTest test_python_export_traits()
{
	return TUnitTest
	{
		LASS_TEST_CASE(testPythonExportTraitsString),
	};
}

}

}

// EOF
