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
#include "unit_test.h"

#include "test_io.h"
#include "test_meta.h"
#include "test_num.h"
#include "test_prim.h"
#include "test_spat.h"
#include "test_stde.h"
#include "test_util.h"

#include "../stde/range_algorithm.h"
#include "../io/logger.h"
#include "../io/file_attribute.h"

int main(int argc, char* argv[])
{
	using namespace lass;

	const std::string logFile = 
		io::fileJoinPath(test::workPath(), "test_" LASS_TEST_VERSION ".log");
	io::Logger logger(logFile);
	logger.subscribeTo(io::proxyMan()->cout());
	logger.subscribeTo(io::proxyMan()->clog());
	logger.subscribeTo(io::proxyMan()->cerr());

	LASS_EVAL(logFile);
	LASS_COUT << "LASS_TEST_VERSION: " << LASS_TEST_VERSION << std::endl;
	LASS_COUT << "LASS_PLATFORM: " << LASS_PLATFORM << std::endl;
	LASS_COUT << "LASS_COMPILER: " << LASS_COMPILER << std::endl;
	LASS_COUT << "LASS_COMPILER_VERSION: " << LASS_COMPILER_VERSION << std::endl;

	test::TUnitTests unitTests;
	stde::copy_r(test::testSpat(), std::back_inserter(unitTests));
	stde::copy_r(test::testUtil(), std::back_inserter(unitTests));
	stde::copy_r(test::testNum(), std::back_inserter(unitTests));
	stde::copy_r(test::testIo(), std::back_inserter(unitTests));
	stde::copy_r(test::testPrim(), std::back_inserter(unitTests));
	stde::copy_r(test::testMeta(), std::back_inserter(unitTests));
	stde::copy_r(test::testStde(), std::back_inserter(unitTests));
	unsigned errors = 0;
	unsigned fatalErrors = 0;
	const bool success = test::runTests(unitTests, argc, argv, &errors, &fatalErrors);

	if (success)
	{
		return 0;
	}
	const unsigned totalErrors = errors + fatalErrors;
	return totalErrors < 255 ? static_cast<int>(totalErrors) : 255;
}

// EOF
