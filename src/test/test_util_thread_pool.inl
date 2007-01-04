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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_THREAD_POOL_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_THREAD_POOL_INL

#include "test_common.h"

#include "../util/thread_pool.h"
#include "../util/bind.h"
#include "../util/atomic.h"

namespace lass
{
namespace test
{
namespace thread_pool
{
	const unsigned numberOfTasks = 40;
	bool taskIsDone[numberOfTasks];
	unsigned counter = 0;
	util::CriticalSection lock;

	void task(unsigned i)
	{
		util::Thread::sleep(50); 
		LASS_LOCK(lock)
		{
			LASS_TEST_CHECK(!taskIsDone[i]);
		}
		taskIsDone[i] = true;
		util::atomicIncrement(counter);
	}

	template <typename IdlePolicy, template <typename, typename, typename> class ParticipatingPolicy>
	void test(unsigned numberOfThreads, unsigned maxNumberOfTasksInQueue)
	{	
		using namespace util;
		typedef ThreadPool<Callback0, DefaultConsumer<util::Callback0>, IdlePolicy, ParticipatingPolicy> 
			TThreadPool;
		LASS_COUT << typeid(TThreadPool).name() << std::endl;

		std::fill(taskIsDone, taskIsDone + numberOfTasks, false);
		counter = 0;

		TThreadPool pool(numberOfThreads, maxNumberOfTasksInQueue);
		for (unsigned i = 0; i < numberOfTasks; ++i)
		{
			pool.add(util::bind(task, i));
		}
		pool.joinAll();
		
		LASS_TEST_CHECK_EQUAL(counter, numberOfTasks);
		for (unsigned i = 0; i < numberOfTasks; ++i)
		{
			LASS_TEST_CHECK_EQUAL(taskIsDone[i], true);
		}
	}
}


void testUtilThreadPool()
{
	using namespace util;
	thread_pool::test<Signaled, NotParticipating>(4, 20);
	thread_pool::test<Signaled, SelfParticipating>(4, 0);
	thread_pool::test<Spinning, NotParticipating>(
		std::max<unsigned>(util::numberOfProcessors() - 1, 1), 20);
	thread_pool::test<Spinning, SelfParticipating>(0, 0);
}

}

}

#endif

// EOF
