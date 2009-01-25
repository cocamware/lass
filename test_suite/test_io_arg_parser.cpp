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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

#include "../lass/io/arg_parser.h"

namespace lass
{
namespace test
{

void testIoArgParser()
{
	using namespace io;

	const std::string commandLine = "--path=c:\\foo -h --path=\"c:\\my bar\" -d -s\"\\\"hello world!\\\"\" script.py -- 0build.py";

	TestStream testStream;
	proxyMan()->cout()->add(&testStream.stream());
	proxyMan()->cout()->remove(&std::cout);

	ArgParser parser("testIoArgParser", "1.0.0", "script ...");

	// flag, short name only
	ArgFlag hello(parser, "H", "");

	// required single value
	ArgValue<std::string> say(parser, "s", "say", "text");

	// required multiple string value, long name only
	ArgValue<std::string> path(parser, "", "path", "", amMultiple | amRequired);

	// flag with optional (single) integer value, and with 2 as default.
	ArgValue<int> diffraction(parser, "d", "diffraction", "max. number of diffractions", amOptional, 2);

	ArgParser::TArguments result;
	parser.parse(commandLine, &result);

	LASS_TEST_CHECK(testStream.isEqual("testIoArgParser version 1.0.0\n"
		"usage: testIoArgParser [-v|--version] [-h|--help] [-H] [-s|--say <text>] [--path "
		"<value> ...] [-d|--diffraction [<max. number of diffractions>]] script ...\n"));

	LASS_TEST_CHECK(!hello);

	LASS_TEST_CHECK(say);
	LASS_TEST_CHECK_EQUAL(say.size(), size_t(1));
	LASS_TEST_CHECK_EQUAL(say[0], "\"hello world!\"");

	LASS_TEST_CHECK(path);
	LASS_TEST_CHECK_EQUAL(path.size(), size_t(2));
	LASS_TEST_CHECK_EQUAL(path[0], "c:\\foo");
	LASS_TEST_CHECK_EQUAL(path[1], "c:\\my bar");

	LASS_TEST_CHECK(diffraction);
	LASS_TEST_CHECK_EQUAL(diffraction.size(), size_t(1)); // still default value
	LASS_TEST_CHECK_EQUAL(diffraction[0], 2);

	LASS_TEST_CHECK_EQUAL(result.size(), size_t(2));
	LASS_TEST_CHECK_EQUAL(result[0], "script.py");
	LASS_TEST_CHECK_EQUAL(result[1], "0build.py");

	proxyMan()->cout()->add(&std::cout);
	proxyMan()->cout()->remove(&testStream.stream());
}

TUnitTest test_io_arg_parser()
{
	return TUnitTest(1, LASS_TEST_CASE(testIoArgParser));
}

}

}

// EOF
