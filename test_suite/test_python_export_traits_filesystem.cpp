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
#include "../lass/python/export_traits_filesystem.h"

#if LASS_HAVE_STD_FILESYSTEM

namespace lass
{
namespace test
{

void testPythonExportTraitsPath()
{
	using namespace lass::python;

	initPythonEmbedding();

	LockGIL LASS_UNUSED(lock);

	TPyObjPtr pathlib{ PyImport_ImportModule("pathlib") };
	TPyObjPtr pathType{ PyObject_GetAttrString(pathlib.get(), "Path") };
	TPyObjPtr purepathType{ PyObject_GetAttrString(pathlib.get(), "PurePath") };

	// Win32 will use wide strings (std::wstring) which works well with Python's (unicode) str type
	// Others (POSIX) will use narrow strings (std::string) wich works well with Python's bytes type
	// PyExportTraits<std::filesystem::path> will use Py_FileSystemDefaultEncoding to convert between
	// both if necessary, but this test will assume that's UTF-8 (which in practice it almost certainly is).
	//
	// The test string is a funny unicode version of "hello/world", both in UTF-16 and UTF-8
	//
	using string_type = std::filesystem::path::string_type;
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
	static_assert(std::is_same_v<string_type, std::wstring>);
	const string_type native{ L"C:\\\x2653\x212e\x0142\x029f\x263a\\\x0428\x263a\x0491\x2113\x1e13" };
#else
	static_assert(std::is_same_v<string_type, std::string>);
	const string_type native{ "/\xe2\x99\x93\xe2\x84\xae\xc5\x82\xca\x9f\xe2\x98\xba/\xd0\xa8\xe2\x98\xba\xd2\x91\xe2\x84\x93\xe1\xb8\x93" };
#endif
	LASS_TEST_CHECK_EQUAL(std::string{ Py_FileSystemDefaultEncoding }, std::string{ "utf-8" });

	{
		// build path
		std::filesystem::path path{ native };
		TPyObjPtr obj{ pyBuildSimpleObject(path) };
		LASS_TEST_CHECK(PyObject_IsInstance(obj.get(), pathType.get()));
		TPyObjPtr strObj{ PyObject_Str(obj.get()) };
		string_type str;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(strObj.get(), str), 0);
		LASS_TEST_CHECK(str == native);
	}

	{
		// get path from str
		TPyObjPtr obj{ pyBuildSimpleObject(native) };
		LASS_TEST_CHECK(PyUnicode_Check(obj.get()));
		std::filesystem::path path;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), path), 0);
		LASS_TEST_CHECK(path.native() == native);
	}

	{
		// get path from bytes
		TPyObjPtr obj{ pyBuildSimpleObject(native) };
		TPyObjPtr bytes{ PyUnicode_EncodeFSDefault(obj.get()) };
		LASS_TEST_CHECK(PyBytes_Check(bytes.get()));
		std::filesystem::path path;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(bytes.get(), path), 0);
		LASS_TEST_CHECK(path.native() == native);
	}

	{
		// get path from pathlib.Path
		TPyObjPtr obj{ PyObject_Call(pathType.get(), makeTuple(native).get(), nullptr) };
		std::filesystem::path path;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), path), 0);
		LASS_TEST_CHECK(path.native() == native);
	}

	{
		// get path from another PathLike
		TPyObjPtr obj{ PyObject_Call(purepathType.get(), makeTuple(native).get(), nullptr) };
		std::filesystem::path path;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), path), 0);
		LASS_TEST_CHECK(path.native() == native);
	}
}

TUnitTest test_python_export_traits_filesystem()
{
	return TUnitTest
	{
		LASS_TEST_CASE(testPythonExportTraitsPath),
	};
}

}

}

#endif

// EOF
