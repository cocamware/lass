/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_CLOCK_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_CLOCK_INL

#include "test_common.h"
#include "../util/clock.h"
#include "../util/stop_watch.h"


namespace lass
{
namespace test
{

void testUtilClock()
{
	util::Clock deviceUnderTest;
	LASS_COUT << "frequency: " << deviceUnderTest.frequency() << std::endl;
	BOOST_CHECK_EQUAL(deviceUnderTest.frequency() * deviceUnderTest.resolution(), 1);

	for (unsigned warmup = 0; warmup < 16; ++warmup)
	{
		deviceUnderTest.time();
	}

	int i = 0;

	double testSeconds = 1;

	const ::clock_t stdBegin = ::clock();
	const ::clock_t stdEnd = stdBegin + static_cast<::clock_t>(testSeconds * CLK_TCK);

	while (::clock() <= stdBegin)
	{
		++i;
	}
	const util::Clock::TTime dutBegin = deviceUnderTest.time();
	LASS_EVAL(i);

	while (::clock() <= stdEnd)
	{
		++i;
	}
	const util::Clock::TTime dutEnd = deviceUnderTest.time();
	LASS_EVAL(i);

	LASS_EVAL(stdBegin);
	LASS_EVAL(stdEnd);
	LASS_EVAL(dutBegin);
	LASS_EVAL(dutEnd);

	BOOST_CHECK_CLOSE(dutEnd - dutBegin, testSeconds, 100. / CLK_TCK);


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



}

}

#endif

// EOF