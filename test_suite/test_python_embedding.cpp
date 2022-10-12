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
 *	Copyright (C) 2004-2022 the Initial Developer.
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
#include "bar.h"
#include "python_embedding.h"

#include "../lass/util/shared_ptr.h"
#include "../lass/util/environment.h"
#include "../lass/io/file_attribute.h"
#include "../lass/stde/extended_string.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4996) // 'fopen': This function or variable may be unsafe
#endif

namespace lass
{
namespace test
{

void testPythonEmbedding()
{
	initPythonEmbedding();

	LASS_TEST_CHECK(python::globals());
	
	python::TPyObjPtr a;
	int b = 0;
	LASS_TEST_CHECK_NO_THROW(a = python::evaluate("4 + 4"));
	LASS_TEST_CHECK_EQUAL(python::pyGetSimpleObject(a.get(), b), 0);
	LASS_TEST_CHECK_EQUAL(b, 8);

	LASS_TEST_CHECK_NO_THROW(python::execute("foo = 9"));
	LASS_TEST_CHECK_NO_THROW(a = python::evaluate("foo"));
	LASS_TEST_CHECK_EQUAL(python::pyGetSimpleObject(a.get(), b), 0);
	LASS_TEST_CHECK_EQUAL(b, 9);
	
	LASS_TEST_CHECK_THROW(python::execute("foo = bar"), python::PythonException);

	python::putenv("FOO", "BAR");
	//LASS_TEST_CHECK_EQUAL(util::getEnvironment<std::string>("FOO"), "BAR"); this does not work reliable, can live in different runtimes.
	LASS_TEST_CHECK_NO_THROW(python::execute("import os"));
	LASS_TEST_CHECK_NO_THROW(a = python::evaluate("os.environ.get('FOO')"));
	std::string s;
	LASS_TEST_CHECK_EQUAL(python::pyGetSimpleObject(a.get(), s), 0);
	LASS_TEST_CHECK_EQUAL(s, "BAR");

	// execfile is no longer part of python 3.0
	const std::string testFile = io::fileJoinPath(test::inputDir(), "test_python_embedding.py");
	std::string commandStr = "exec(open('" + testFile + "').read())";
	commandStr = stde::replace_all(commandStr, std::string("\\"), std::string("\\\\"));
	LASS_TEST_CHECK_EQUAL( PyRun_SimpleString( const_cast<char*>(commandStr.c_str()) ) , 0 );
	
	/*
	typedef std::vector<double> TV;
	TV vec;
	python::impl::PySequence pyseqtest(vec);
	LASS_TEST_CHECK_EQUAL( PySequence_Check(&pyseqtest) , 1);
	*/

}

TUnitTest test_python_embedding()
{
	return TUnitTest(1, LASS_TEST_CASE(testPythonEmbedding));
}

}

}

// EOF
