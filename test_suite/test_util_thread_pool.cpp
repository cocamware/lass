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

#include "../lass/util/thread_pool.h"
#include "../lass/util/bind.h"
#include "../lass/util/atomic.h"

namespace lass
{
namespace test
{
namespace thread_pool
{
	const size_t numberOfTasks = 40;
	bool taskIsDone[numberOfTasks];
	size_t counter = 0;
	util::CriticalSection lock;

	void task(size_t i)
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
	void test(size_t numberOfThreads, size_t maxNumberOfTasksInQueue)
	{	
		using namespace util;
		typedef DefaultConsumer<util::Callback0> TConsumer;
		typedef ThreadPool<Callback0, TConsumer, IdlePolicy, ParticipatingPolicy> 
			TThreadPool;
		LASS_COUT << typeid(TThreadPool).name() << std::endl;

		std::fill(taskIsDone, taskIsDone + numberOfTasks, false);
		counter = 0;

		TThreadPool pool(numberOfThreads, maxNumberOfTasksInQueue, TConsumer(), "pool");
		for (size_t i = 0; i < numberOfTasks; ++i)
		{
			pool.addTask(util::bind(task, i));
		}
		pool.completeAllTasks();
		
		LASS_TEST_CHECK_EQUAL(counter, numberOfTasks);
		for (size_t i = 0; i < numberOfTasks; ++i)
		{
			LASS_TEST_CHECK_EQUAL(taskIsDone[i], true);
		}
	}
}

void testUtilNumberOfProcessors()
{
	LASS_TEST_CHECK(util::numberOfProcessors() > 0);
	LASS_TEST_CHECK(util::numberOfAvailableProcessors() > 0);
	LASS_TEST_CHECK(util::numberOfAvailableProcessors() <= util::numberOfProcessors());
}

void testUtilThreadPool()
{
	using namespace util;
	thread_pool::test<Signaled, NotParticipating>(4, 20);
	thread_pool::test<Signaled, SelfParticipating>(4, 0);
	thread_pool::test<Spinning, NotParticipating>(std::max<size_t>(util::numberOfProcessors() - 1, 1), 20);
	thread_pool::test<Spinning, SelfParticipating>(0, 0);
}

TUnitTest test_util_thread_pool()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testUtilNumberOfProcessors));
	result.push_back(LASS_TEST_CASE(testUtilThreadPool));
	return result;
}

}

}

// EOF
