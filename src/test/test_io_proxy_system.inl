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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_IO_PROXY_SYSTEM_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_IO_PROXY_SYSTEM_INL

#include "test_common.h"

#include "../io/logger.h"
#include "../io/proxy_man.h"

namespace lass
{
namespace test
{

void testIoProxySystem()
{
	using namespace lass::io;

	// first, we have to hook bogus streams to the proxy manager.
	//
	//proxyMan()->cout()->remove(&std::cout);
	//proxyMan()->clog()->remove(&std::clog);
	//proxyMan()->cerr()->remove(&std::cerr);
	boost::test_toolbox::output_test_stream testOut;
	boost::test_toolbox::output_test_stream testLog;
	boost::test_toolbox::output_test_stream testErr;
	proxyMan()->cout()->add(&testOut);
	proxyMan()->clog()->add(&testLog);
	proxyMan()->cerr()->add(&testErr);

	try
	{
		const int a = 5;
		const std::string all = "all";

		Logger logger("logger.txt");
		logger.subscribeTo(proxyMan()->clog());
		logger.subscribeTo(proxyMan()->cerr());

		// test it
		LASS_COUT << "message to lass cout\n";
		BOOST_CHECK(testOut.is_equal("message to lass cout\n"));
		BOOST_CHECK(testLog.is_empty());
		BOOST_CHECK(testErr.is_empty());

		LASS_CLOG << "message to lass clog\n";
		BOOST_CHECK(testOut.is_empty());
		BOOST_CHECK(testLog.is_equal("message to lass clog\n"));
		BOOST_CHECK(testErr.is_empty());

		LASS_CERR << "message to lass cerr\n";
		BOOST_CHECK(testOut.is_empty());
		BOOST_CHECK(testLog.is_empty());
		BOOST_CHECK(testErr.is_equal("message to lass cerr\n"));

		// test subscribtions
		logger.subscribeTo(proxyMan()->cout());
		LASS_COUT << "this should go in the logger\n";
		BOOST_CHECK(testOut.is_equal("this should go in the logger\n"));
		logger.unsubscribeTo(proxyMan()->cout());
		LASS_COUT << "but this shouldn't\n";
		BOOST_CHECK(testOut.is_equal("but this shouldn't\n"));

		// test debug macros
		LASS_WARNING("this is a warning");
		LASS_EVAL(a);
		LASS_LOG("this is a log entry");

		// test filters
		LASS_EVAL(logger.filter(proxyMan()->clog()));
		logger.setFilter(proxyMan()->clog(), 0x1);
		LASS_CLOG(0x1) << "this " << "should " << all << " go " << "in " << "the " << "log." << std::flush;
		LASS_CLOG << all << " this " << "too.\n";
		LASS_CLOG(0x2) << "but " << all << " this " << "should " << "not.\n";

		// test append
		{
			Logger clearLog("append.txt");
			clearLog.subscribeTo(proxyMan()->cout());
			LASS_COUT << "first line.\n";
		}
		{
			Logger appendLog("append.txt", Logger::lmAppend);
			appendLog.subscribeTo(proxyMan()->cout());
			LASS_COUT << "second line.\n";
		}
	}
	catch(...)
	{
		BOOST_ERROR("exception is thrown");
	}

	//proxyMan()->cout()->add(&std::cout);
	//proxyMan()->clog()->add(&std::clog);
	//proxyMan()->cerr()->add(&std::cerr);
	proxyMan()->cout()->remove(&testOut);
	proxyMan()->clog()->remove(&testLog);
	proxyMan()->cerr()->remove(&testErr);
}

}

}

#endif

// EOF
