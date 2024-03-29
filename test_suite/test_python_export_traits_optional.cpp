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
#include "../lass/python/export_traits_optional.h"

#if LASS_HAVE_STD_OPTIONAL

namespace lass
{
namespace test
{

void testPythonExportTraitsOptional()
{
	using namespace lass::python;

	initPythonEmbedding();

	LockGIL LASS_UNUSED(lock);

	{
		// build empty optional
		std::optional<std::string> val;
		TPyObjPtr obj{ pyBuildSimpleObject(val) };
		LASS_TEST_CHECK(obj.get() == Py_None);
	}

	{
		// build optional with value
		std::optional<std::string> val{ "a string" };
		TPyObjPtr obj{ pyBuildSimpleObject(val) };
		LASS_TEST_CHECK(obj.get() != Py_None);
		LASS_TEST_CHECK(PyUnicode_Check(obj.get()));
		std::string str;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), str), 0);
		LASS_TEST_CHECK_EQUAL(str, val.value());
	}

	{
		// get empty optional
		std::optional<std::string> val;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(Py_None, val), 0);
		LASS_TEST_CHECK(!val.has_value());
	}

	{
		// get optional with value
		const std::string str{ "a string" };
		TPyObjPtr obj{ pyBuildSimpleObject(str) };
		std::optional<std::string> val;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), val), 0);
		LASS_TEST_CHECK(val.has_value());
		LASS_TEST_CHECK_EQUAL(val.value(), str);
	}
}

TUnitTest test_python_export_traits_optional()
{
	return TUnitTest
	{
		LASS_TEST_CASE(testPythonExportTraitsOptional),
	};
}

}

}

#endif

// EOF
