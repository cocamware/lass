/**	@file
 *  @internal
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#include "test_common.h"

// we must include the implementation of the test exec monitor exaclty once (see documentation
// in Boost).  This seems a nice spot to do so.  Disable a few spurious warnings ...
//
#pragma warning(push)
#	pragma warning(disable: 4267)
#	pragma warning(disable: 4535)
#	include <boost/test/included/test_exec_monitor.hpp>
#pragma warning(pop)

#include "test_io.h"
#include "test_meta.h"
#include "test_num.h"
#include "test_prim.h"
#include "test_spat.h"
#include "test_stde.h"
#include "test_util.h"

#include "../io/arg_parser.h"

#include <boost/test/detail/unit_test_parameters.hpp>

int test_main(int argc, char* argv[])
{
    using namespace boost::unit_test_framework;

	lass::io::proxyMan()->clog()->remove(&std::clog);

	lass::io::ArgParser parser;
	lass::io::ArgFlag argXml(parser, "x", "xml");
	parser.parse(argc, argv);

    test_suite testSuite("lass test suite");
    
	testSuite.add(lass::test::testIo());
    testSuite.add(lass::test::testMeta());
    testSuite.add(lass::test::testNum());
    testSuite.add(lass::test::testPrim());
	testSuite.add(lass::test::testStde());
    testSuite.add(lass::test::testUtil());
    testSuite.add(lass::test::testSpat());

	std::ofstream log;
	if (argXml)
	{
		// redirect info to files
		//
		unit_test_log::instance().set_log_format("XML");
		unit_test_log::instance().set_log_threshold_level(log_test_suites);
		log.open("test_log." LASS_TEST_VERSION ".xml");
		unit_test_log::instance().set_log_stream(log);
	}

	testSuite.run();

	if (argXml)
	{
		unit_test_result::instance().set_report_format("XML");
		std::ofstream result("test_result." LASS_TEST_VERSION ".xml");
		unit_test_result::instance().detailed_report(result);
	}

	unit_test_result::instance().set_report_format("HRF");
	unit_test_log::instance().set_log_stream(std::cout);
    return 0;
}

// EOF
