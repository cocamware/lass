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
#include "../lass/python/export_traits_variant.h"

#if LASS_HAVE_STD_VARIANT

namespace lass
{
namespace test
{

void testPythonExportTraitsVariant()
{
	using namespace lass::python;

	initPythonEmbedding();
	LockGIL LASS_UNUSED(lock);

	using string_list = std::vector<std::string>;
	using string_list_list = std::vector<string_list>;
	using string_map = std::map<std::string, std::string>;
	using complex = std::complex<double>;
	using variant = std::variant<std::monostate, bool, int, float, complex, std::string, string_list, string_list_list, string_map>;

	// Building PyObject instances from std::variant is straight forward:
	{
		variant v;
		LASS_TEST_CHECK(std::holds_alternative<std::monostate>(v));
		TPyObjPtr obj{ pyBuildSimpleObject(v) };
		LASS_TEST_CHECK(obj);
		LASS_TEST_CHECK_EQUAL(obj.get(), Py_None);
	}
	{
		variant v = true;
		LASS_TEST_CHECK(std::holds_alternative<bool>(v));
		TPyObjPtr obj{ pyBuildSimpleObject(v) };
		LASS_TEST_CHECK(obj);
		LASS_TEST_CHECK(PyBool_Check(obj.get()));
		LASS_TEST_CHECK_EQUAL(obj.get(), Py_True);
	}
	{
		variant v = 42;
		LASS_TEST_CHECK(std::holds_alternative<int>(v));
		TPyObjPtr obj{ pyBuildSimpleObject(v) };
		LASS_TEST_CHECK(obj);
		LASS_TEST_CHECK(PyLong_CheckExact(obj.get()));
		LASS_TEST_CHECK_EQUAL(PyLong_AsLong(obj.get()), 42);
	}
	{
		variant v = 3.14f;
		LASS_TEST_CHECK(std::holds_alternative<float>(v));
		TPyObjPtr obj{ pyBuildSimpleObject(v) };
		LASS_TEST_CHECK(obj);
		LASS_TEST_CHECK(PyFloat_CheckExact(obj.get()));
		LASS_TEST_CHECK_EQUAL(PyFloat_AsDouble(obj.get()), 3.14f);
	}
	{
		variant v = complex(3, 4);
		LASS_TEST_CHECK(std::holds_alternative<complex>(v));
		TPyObjPtr obj{ pyBuildSimpleObject(v) };
		LASS_TEST_CHECK(obj);
		LASS_TEST_CHECK(PyComplex_CheckExact(obj.get()));
	}
	{
		variant v = std::string{ "foobar" };
		LASS_TEST_CHECK(std::holds_alternative<std::string>(v));
		TPyObjPtr obj{ pyBuildSimpleObject(v) };
		LASS_TEST_CHECK(obj);
		LASS_TEST_CHECK(PyUnicode_CheckExact(obj.get()));
	}
	{
		variant v = string_list{ "foo", "bar" };
		LASS_TEST_CHECK(std::holds_alternative<string_list>(v));
		TPyObjPtr obj{ pyBuildSimpleObject(v) };
		LASS_TEST_CHECK(obj);
		LASS_TEST_CHECK(PySequence_Check(obj.get()));
	}
	{
		variant v = string_map{ {"foo", "FOO"}, {"bar", "BAR"} };
		LASS_TEST_CHECK(std::holds_alternative<string_map>(v));
		TPyObjPtr obj{ pyBuildSimpleObject(v) };
		LASS_TEST_CHECK(obj);
		LASS_TEST_CHECK(PyMapping_Check(obj.get()));
	}

	// Getting them is order dependendent!
	{
		variant v = 42;
		LASS_TEST_CHECK(!std::holds_alternative<std::monostate>(v));
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(Py_None, v), 0);
		LASS_TEST_CHECK(std::holds_alternative<std::monostate>(v));
	}
	{
		variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(Py_True, v), 0);
		LASS_TEST_CHECK(std::holds_alternative<bool>(v));
		LASS_TEST_CHECK_EQUAL(std::get<bool>(v), true);
	}
	{
		TPyObjPtr obj{ pyBuildSimpleObject(42) };
		variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v), 0);
		LASS_TEST_CHECK(std::holds_alternative<int>(v));
		LASS_TEST_CHECK_EQUAL(std::get<int>(v), 42);
	}
	{
		TPyObjPtr obj{ pyBuildSimpleObject(3.14f) };
		variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v), 0);
		LASS_TEST_CHECK(std::holds_alternative<float>(v));
		LASS_TEST_CHECK_EQUAL(std::get<float>(v), 3.14f);
	}
	{
		TPyObjPtr obj{ pyBuildSimpleObject(complex(3, 4)) };
		variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v), 0);
		LASS_TEST_CHECK(std::holds_alternative<complex>(v));
		LASS_TEST_CHECK_EQUAL(std::get<complex>(v), complex(3, 4));
	}
	{
		TPyObjPtr obj{ pyBuildSimpleObject("foobar")};
		variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v), 0);
		LASS_TEST_CHECK(std::holds_alternative<std::string>(v));
		LASS_TEST_CHECK_EQUAL(std::get<std::string>(v), std::string{ "foobar" });
	}
	{
		TPyObjPtr obj{ makeTuple("foo", "bar")};
		variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v), 0);
		LASS_TEST_CHECK(std::holds_alternative<string_list>(v));
		LASS_TEST_CHECK_EQUAL(std::get<string_list>(v), (string_list{ "foo", "bar" }));
	}
	{
		TPyObjPtr obj{ PyDict_New()};
		PyDict_SetItemString(obj.get(), "foo", pyBuildSimpleObject("FOO"));
		PyDict_SetItemString(obj.get(), "bar", pyBuildSimpleObject("BAR"));
		variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v), 0); 
		LASS_TEST_CHECK(std::holds_alternative<string_map>(v));
		LASS_TEST_CHECK_EQUAL(std::get<string_map>(v), (string_map{ {"foo", "FOO"}, {"bar", "BAR"} }));
	}

	// Putting the types in the wrong order, will get unexpected results!
	using bad_variant = std::variant<std::monostate, string_map, string_list_list, string_list, std::string, complex, float, int, bool>;
	{
		bad_variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(Py_True, v), 0);
		LASS_TEST_CHECK(std::holds_alternative<complex>(v)); // oops, expected bool!
		LASS_TEST_CHECK_EQUAL(std::get<complex>(v), complex(1, 0));

		std::variant<float, int, bool> v2;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(Py_True, v2), 0);
		LASS_TEST_CHECK(std::holds_alternative<float>(v2)); // oops, expected bool!
		LASS_TEST_CHECK_EQUAL(std::get<float>(v2), 1.f);

		std::variant<int, bool> v3;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(Py_True, v3), 0);
		LASS_TEST_CHECK(std::holds_alternative<int>(v3)); // oops, expected bool!
		LASS_TEST_CHECK_EQUAL(std::get<int>(v3), 1);
	}
	{
		TPyObjPtr obj{ pyBuildSimpleObject(42) };
		bad_variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v), 0);
		LASS_TEST_CHECK(std::holds_alternative<complex>(v)); // oops, expected int!
		LASS_TEST_CHECK_EQUAL(std::get<complex>(v), complex(42, 0));

		std::variant<float, int> v2;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v2), 0);
		LASS_TEST_CHECK(std::holds_alternative<float>(v2)); // oops, expected int!
		LASS_TEST_CHECK_EQUAL(std::get<float>(v2), 42.f);
	}
	{
		TPyObjPtr obj{ pyBuildSimpleObject(3.14f) };
		bad_variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v), 0);
		LASS_TEST_CHECK(std::holds_alternative<complex>(v)); // oops, expected float!
		LASS_TEST_CHECK_EQUAL(std::get<complex>(v), complex(3.14f, 0));
	}
	{
		TPyObjPtr obj{ pyBuildSimpleObject("foobar") };
		bad_variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v), 0);
		LASS_TEST_CHECK(std::holds_alternative<string_list_list>(v)); // oops, expected std::string!
		LASS_TEST_CHECK_EQUAL(std::get<string_list_list>(v), (string_list_list{ {"f"}, {"o"}, {"o"}, {"b"}, {"a"}, {"r"} }));

		std::variant<string_list, std::string> v2;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v2), 0);
		LASS_TEST_CHECK(std::holds_alternative<string_list>(v2)); // oops, expected std::string!
		LASS_TEST_CHECK_EQUAL(std::get<string_list>(v2), (string_list{ "f", "o", "o", "b", "a", "r" }));
	}
	{
		TPyObjPtr obj{ pyBuildSimpleObject(string_list{"foo", "bar"}) };
		bad_variant v;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), v), 0);
		LASS_TEST_CHECK(std::holds_alternative<string_list_list>(v)); // oops, expected string_list!
		LASS_TEST_CHECK_EQUAL(std::get<string_list_list>(v), (string_list_list{ {"f", "o", "o"}, {"b", "a", "r"} }));
	}
}


TUnitTest test_python_export_traits_variant()
{
	return TUnitTest
	{
		LASS_TEST_CASE(testPythonExportTraitsVariant),
	};
}

}

}

#endif

// EOF
