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
#include "python_embedding.h"

#include "../lass/python/python_api.h"


namespace lass
{
namespace test
{

void testPythonTuple()
{
    using namespace lass::python;

	initPythonEmbedding();
    LockGIL LASS_UNUSED(lock);

    {
        TPyObjPtr obj = makeTuple();

        LASS_TEST_CHECK(obj.get());
        LASS_TEST_CHECK(PyTuple_Check(obj.get()));
        LASS_TEST_CHECK_EQUAL(PyTuple_Size(obj.get()), 0);
    }

    {
        TPyObjPtr obj = makeTuple(1, "2", 3.14);

        LASS_TEST_CHECK(obj.get());
        LASS_TEST_CHECK(PyTuple_Check(obj.get()));
        LASS_TEST_CHECK_EQUAL(PyTuple_Size(obj.get()), 3);

        int p1 = 0;
        char* p2 = nullptr;
        float p3 = 0.f;
        LASS_TEST_CHECK(PyArg_ParseTuple(obj.get(), "isf", &p1, &p2, &p3));
        LASS_TEST_CHECK_EQUAL(p1, 1);
        LASS_TEST_CHECK(p2 && strcmp(p2, "2") == 0);
        LASS_TEST_CHECK_EQUAL(p3, 3.14f);
    }

    {
        TPyObjPtr obj(Py_BuildValue("()"));

        LASS_TEST_CHECK_EQUAL(decodeTuple(obj), 0);

        // getting three values from an empty tuple should fail
        int p1 = 0;
        std::string p2;
        float p3 = 0.f;
        LASS_TEST_CHECK_EQUAL(decodeTuple(obj, p1, p2, p3), 1);
        PyErr_Clear();
    }

    {
        TPyObjPtr obj(Py_BuildValue("(isf)", 1, "2", 3.14f));
        LASS_TEST_CHECK_EQUAL(PyTuple_Size(obj.get()), 3);

        // getting zero values from a triple should fail
        LASS_TEST_CHECK_EQUAL(decodeTuple(obj), 1);
        PyErr_Clear();

        int p1 = 0;
        std::string p2;
        float p3 = 0.f;
        LASS_TEST_CHECK_EQUAL(decodeTuple(obj, p1, p2, p3), 0);
        LASS_TEST_CHECK_EQUAL(p1, 1);
        LASS_TEST_CHECK_EQUAL(p2, "2");
        LASS_TEST_CHECK_EQUAL(p3, 3.14f);
    }

    {
        TPyObjPtr obj(Py_BuildValue("(is)", 1, "2"));

        int p1 = 0;
        std::string p2;
        float p3 = 0.f;
        LASS_TEST_CHECK_EQUAL(decodeTupleMinimum(obj, 2, p1, p2, p3), 0);
        LASS_TEST_CHECK_EQUAL(p1, 1);
        LASS_TEST_CHECK_EQUAL(p2, "2");
        LASS_TEST_CHECK_EQUAL(p3, 0.f);
    }

    {
        std::tuple<int, std::string, float> t1{ 1, "2", 3.14f };
        TPyObjPtr obj(pyBuildSimpleObject(t1));
        LASS_TEST_CHECK(obj.get());
        LASS_TEST_CHECK(PyTuple_Check(obj.get()));
        LASS_TEST_CHECK_EQUAL(PyTuple_Size(obj.get()), 3);
        int p1 = 0;
        char* p2 = nullptr;
        float p3 = 0.f;
        LASS_TEST_CHECK(PyArg_ParseTuple(obj.get(), "isf", &p1, &p2, &p3));
        LASS_TEST_CHECK_EQUAL(p1, 1);
        LASS_TEST_CHECK(p2 && strcmp(p2, "2") == 0);
        LASS_TEST_CHECK_EQUAL(p3, 3.14f);
    }

    {
        TPyObjPtr obj(Py_BuildValue("(isf)", 1, "2", 3.14f));

        std::tuple<int, std::string, float> t1;
        LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), t1), 0);
        LASS_TEST_CHECK_EQUAL(std::get<0>(t1), 1);
        LASS_TEST_CHECK_EQUAL(std::get<1>(t1), "2");
        LASS_TEST_CHECK_EQUAL(std::get<2>(t1), 3.14f);

        // getting a tuple of the wrong size should fail
        std::tuple<int, std::string> t2;
        LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), t2), 1);
        PyErr_Clear();
    }
}

TUnitTest test_python_tuple()
{
	return TUnitTest(1, LASS_TEST_CASE(testPythonTuple));
}

}

}

// EOF
