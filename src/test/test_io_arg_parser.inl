/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#include "test_common.h"

#include "../io/arg_parser.h"

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
	LASS_TEST_CHECK_EQUAL(say.size(), 1);
	LASS_TEST_CHECK_EQUAL(say[0], "\"hello world!\"");

	LASS_TEST_CHECK(path);
	LASS_TEST_CHECK_EQUAL(path.size(), 2);
	LASS_TEST_CHECK_EQUAL(path[0], "c:\\foo");
	LASS_TEST_CHECK_EQUAL(path[1], "c:\\my bar");

	LASS_TEST_CHECK(diffraction);
	LASS_TEST_CHECK_EQUAL(diffraction.size(), 1); // still default value
	LASS_TEST_CHECK_EQUAL(diffraction[0], 2);

	LASS_TEST_CHECK_EQUAL(result.size(), 2);
	LASS_TEST_CHECK_EQUAL(result[0], "script.py");
	LASS_TEST_CHECK_EQUAL(result[1], "0build.py");

	proxyMan()->cout()->add(&std::cout);
	proxyMan()->cout()->remove(&testStream.stream());
}



}

}

// EOF
