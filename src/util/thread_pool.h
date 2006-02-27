/** @file
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

/** @class ThreadPool
 *  @brief Production-Consumer concurrency pattern
 *  @author Bramz
 *
 *  @code
 *  #include <lass/util/thread_pool.h>
 *  using namespace lass::util;
 *
 *	const unsigned numberOfThreads = 10;
 *	const unsigned maxNumberOfTasksInQueue = 20;
 *	const unsigned numberOfTasks = 40;
 *
 *  unsigned counter = 0;
 *	CriticalSection counterMutex;
 *
 *	void task()
 *	{
 *		Thread::sleep(50); 
 *		LASS_LOCK(counterMutex)
 *		{
 *			++counter;
 *		}
 *	}
 *
 *  int main()
 *	{
 *		{
 *			util::ThreadPool<> pool(16, 20);
 *			for (unsigned i = 0; i < numberOfTasks; ++i)
 *			{
 *				pool.add(util::makeCallback(thread_pool::task));
 *			}
 *			// ~ThreadPool will wait for completion ...
 *		}
 *		LASS_COUT << "counter: " << counter; // == numberOfTasks
 *	}
 *  @endcode
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_POOL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_POOL_H

#include "util_common.h"
#include "thread.h"
#include "callback_0.h"

namespace lass
{
namespace util
{

/** Default consumer calls operator() of task.
 *  @sa ThreadPool
 */
template <typename TaskType>
class DefaultConsumer
{
public:
	void operator()(typename util::CallTraits<TaskType>::TParam iTask);
};



template 
<
	typename TaskType = Callback0, 
	typename ConsumerType = DefaultConsumer<TaskType>
>
class ThreadPool
{
public:
	
	typedef TaskType TTask;
	typedef ConsumerType TConsumer;
	typedef ThreadPool<TaskType, ConsumerType> TSelf;

	ThreadPool(unsigned iNumberOfThreads = 16, unsigned iMaximumNumberOfTasksInQueue = 0,
			const TConsumer& iConsumerPrototype = TConsumer());
	~ThreadPool();

	void add(typename util::CallTraits<TTask>::TParam iTask);
	void clearQueue();
	void waitUntilEmpty();
	const bool isEmpty() const;

private:

	typedef std::queue<TTask> TTaskQueue;

	friend class ConsumerThread;

	class ConsumerThread: public Thread
	{
	public:
		ConsumerThread(const TConsumer& iConsumer, TSelf& iPool);
	private:
		void doRun();
		TConsumer consumer_;
		TSelf& pool_;
	};

	void startThreads(const TConsumer& iConsumerPrototype);
	void stopThreads(unsigned iNumAllocatedThreads);

	TTaskQueue tasks_;
	mutable CriticalSection mutex_;
	Condition conditionProduction_;
	Condition conditionConsumer_;
	ConsumerThread* threads_;
	unsigned long mSecsToSleep_;
	unsigned numThreads_;
	unsigned maxTasksInQueue_;
	bool shutDown_;
};


}

}

#include "thread_pool.inl"

#endif

// EOF
