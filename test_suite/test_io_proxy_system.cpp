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

#include "../lass/io/logger.h"
#include "../lass/io/proxy_man.h"

namespace lass
{
namespace test
{

void testIoProxySystem()
{
	using namespace lass::io;

	// first, we have to hook bogus streams to the proxy manager.
	//
	proxyMan()->cout()->remove(&std::cout);
	proxyMan()->cerr()->remove(&std::cerr);
	TestStream testOut;
	TestStream testLog;
	TestStream testErr;
	proxyMan()->cout()->add(&testOut.stream());
	proxyMan()->clog()->add(&testLog.stream());
	proxyMan()->cerr()->add(&testErr.stream());

	try
	{
		const int a = 5;
		const std::string all = "all";

		Logger logger("logger.txt");
		logger.subscribeTo(proxyMan()->clog());
		logger.subscribeTo(proxyMan()->cerr());

		// test it
		LASS_COUT << "message to lass cout\n";
		LASS_TEST_CHECK(testOut.isEqual("message to lass cout\n"));
		LASS_TEST_CHECK(testLog.isEqual(""));
		LASS_TEST_CHECK(testErr.isEqual(""));

		LASS_CLOG << "message to lass clog\n";
		LASS_TEST_CHECK(testOut.isEqual(""));
		LASS_TEST_CHECK(testLog.isEqual("message to lass clog\n"));
		LASS_TEST_CHECK(testErr.isEqual(""));

		LASS_CERR << "message to lass cerr\n";
		LASS_TEST_CHECK(testOut.isEqual(""));
		LASS_TEST_CHECK(testLog.isEqual(""));
		LASS_TEST_CHECK(testErr.isEqual("message to lass cerr\n"));

		// test subscribtions
		logger.subscribeTo(proxyMan()->cout());
		LASS_COUT << "this should go in the logger\n";
		LASS_TEST_CHECK(testOut.isEqual("this should go in the logger\n"));
		logger.unsubscribeTo(proxyMan()->cout());
		LASS_COUT << "but this shouldn't\n";
		LASS_TEST_CHECK(testOut.isEqual("but this shouldn't\n"));

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
	LASS_TEST_CATCH_EXCEPTIONS("an exception is thrown")

	proxyMan()->cout()->add(&std::cout);
	proxyMan()->cerr()->add(&std::cerr);
	proxyMan()->cout()->remove(&testOut.stream());
	proxyMan()->clog()->remove(&testLog.stream());
	proxyMan()->cerr()->remove(&testErr.stream());
}

TUnitTest test_io_proxy_system()
{
	return TUnitTest(1, LASS_TEST_CASE(testIoProxySystem));
}

}

}

// EOF
