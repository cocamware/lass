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
#include "../lass/util/clock.h"
#include "../lass/util/stop_watch.h"

#include <ctime>
//#ifndef CLK_TCK
//#	include <unistd.h>
//#	define CLOCKS_PER_SEC sysconf(_SC_CLK_TCK)
//#endif

namespace lass
{
namespace test
{

void testUtilClock()
{
	typedef util::Clock::TTime TTime;
	
	util::Clock deviceUnderTest;
	LASS_COUT << "frequency: " << deviceUnderTest.frequency() << std::endl;
	LASS_TEST_CHECK_CLOSE(static_cast<TTime>(deviceUnderTest.frequency()) * deviceUnderTest.resolution(), 1.0, 1e-6);

	for (size_t warmup = 0; warmup < 16; ++warmup)
	{
		deviceUnderTest.time();
	}

	int i = 0;

	double testSeconds = 1;


	const std::clock_t stdBegin = std::clock();
	while (std::clock() <= stdBegin)
	{
		++i;
	}
	const util::Clock::TTime dutBegin = deviceUnderTest.time();
	LASS_EVAL(i);
	LASS_EVAL(std::clock());
	const std::clock_t stdEnd = stdBegin + static_cast< std::clock_t >(testSeconds * CLOCKS_PER_SEC);
	while (std::clock() <= stdEnd)
	{
		++i;
	}
	const util::Clock::TTime dutEnd = deviceUnderTest.time();
	LASS_EVAL(i);
	LASS_EVAL(std::clock());

	LASS_EVAL(stdBegin);
	LASS_EVAL(stdEnd);
	LASS_EVAL(dutBegin);
	LASS_EVAL(dutEnd);

	LASS_TEST_CHECK_CLOSE(dutEnd - dutBegin, testSeconds, 0.10);


	const util::Clock::TTick tick1 = deviceUnderTest.tick();
	const util::Clock::TTick tick2 = deviceUnderTest.tick();
	LASS_COUT << "delta tick: " << (tick2 - tick1) << std::endl;

	const util::Clock::TTime time1 = deviceUnderTest.time();
	const util::Clock::TTime time2 = deviceUnderTest.time();
	LASS_COUT << "delta time: " << (time2 - time1) << std::endl;

	util::StopWatch stopWatch(deviceUnderTest);
	stopWatch.start();
	stopWatch.stop();
	LASS_COUT << "delta stop watch: " << stopWatch.time() << std::endl;
}

TUnitTest test_util_clock()
{
	return TUnitTest(1, LASS_TEST_CASE(testUtilClock));
}

}

}

// EOF
