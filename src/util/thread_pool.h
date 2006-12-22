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
 *  @ingroup Threading
 *  @brief Production-Consumer concurrency pattern
 *  @author Bramz
 *
 *  @code
 *  #include <lass/util/thread_pool.h>
 *	#include <lass/util/atomic.h>
 *  using namespace lass;
 *
 *	const unsigned numberOfThreads = 4;
 *	const unsigned maxNumberOfTasksInQueue = 20;
 *	const unsigned numberOfTasks = 40;
 *
 *  unsigned counter = 0;
 *
 *	void task()
 *	{
 *		util::Thread::sleep(50);
 *		util::atomicIncrement(counter);
 *	}
 *
 *  int main()
 *	{
 *		{
 *			util::ThreadPool<> pool(numberOfThreads, maxNumberOfTasksInQueue);
 *			for (unsigned i = 0; i < numberOfTasks; ++i)
 *			{
 *				pool.add(util::makeCallback(thread_pool::task)); // blocks if queue is full.
 *			}
 *			// ~ThreadPool will wait for completion ...  You could also use pool.joinAll()
 *		}
 *		LASS_COUT << "counter: " << counter; // == numberOfTasks
 *	}
 *  @endcode
 *
 *	util::bind simplifies the use of ThreadPool for tasks with arguments
 *
 *  @code
 *  #include <lass/util/thread_pool.h>
 *	#include <lass/util/bind.h>
 *	using namespace lass::util;
 *
 *	void parallelWork(int begin, int end);
 *	const int n = 1000000;
 *	const int step = 1000;
 *
 *	util::ThreadPool<> pool;
 *	for (int i = 0; i < n; i += step)
 *	{
 *		pool.add(bind(parallelWork, i, std::min(i + step, n)));
 *	}
 *	pool.joinAll();
 *	@endcode
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_POOL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_POOL_H

#include "util_common.h"
#include "thread.h"
#include "callback_0.h"
#include "../stde/lock_free_queue.h"

namespace lass
{
namespace util
{

/** Default consumer calls operator() of task.
 *  @ingroup Threading
 *  @sa ThreadPool
 */
template <typename TaskType>
class DefaultConsumer
{
public:
	void operator()(typename util::CallTraits<TaskType>::TParam iTask);
};

class Spinning
{
protected:
	Spinning() {}
	~Spinning() {}
	void sleepProducer() {}
	void sleepConsumer() {}
	void signalProducer() {}
	void signalConsumer() {}
	void broadcastConsumers() {}
};

class Signaled
{
protected:
	Signaled() {}
	~Signaled() {}
	void sleepProducer() { producer_.wait(mSecsToSleep_); }
	void sleepConsumer() { consumer_.wait(mSecsToSleep_); }
	void signalProducer() { producer_.signal(); }
	void signalConsumer() { consumer_.signal(); }
	void broadcastConsumers() { consumer_.broadcast(); }
private:
	enum { mSecsToSleep_ = 50 };
	Condition producer_;
	Condition consumer_;
};

template <typename TaskType, typename ConsumerType, typename IdlePolicy>
class SelfParticipating: public IdlePolicy
{
protected:
	SelfParticipating(const ConsumerType& prototype): IdlePolicy(), consumer_(prototype) {}
	~SelfParticipating() {}
	const unsigned numDynamicThreads(unsigned numThreads) const { return numThreads - 1; }
	template <typename Queue> bool participate(Queue& queue)
	{
		TaskType task;
		if (queue.pop(task))
		{
			consumer_(task);
			return true;
		}
		return false;
	}		
private:
	ConsumerType consumer_;
};

template <typename TaskType, typename ConsumerType, typename IdlePolicy>
class NotParticipating: public IdlePolicy
{
protected:
	NotParticipating(const ConsumerType& prototype): IdlePolicy() {}
	~NotParticipating() {}
	const unsigned numDynamicThreads(unsigned numThreads) const { return numThreads; }
	template <typename Queue> bool participate(Queue&) { return false; }
};




template 
<
	typename TaskType = Callback0, 
	typename ConsumerType = DefaultConsumer<TaskType>,
	typename IdlePolicy = Signaled,
	template <typename, typename, typename> class ParticipationPolicy = NotParticipating
>
class ThreadPool: public ParticipationPolicy<TaskType, ConsumerType, IdlePolicy>
{
public:
	
	typedef TaskType TTask;
	typedef ConsumerType TConsumer;
	typedef IdlePolicy TIdlePolicy;
	typedef ParticipationPolicy<TaskType, ConsumerType, IdlePolicy> TParticipationPolicy;
	typedef ThreadPool<TaskType, ConsumerType, IdlePolicy, ParticipationPolicy> TSelf;

	enum { autoNumberOfThreads = 0 };

	ThreadPool(unsigned iNumberOfThreads = autoNumberOfThreads, 
		unsigned iMaximumNumberOfTasksInQueue = 16, 
		const TConsumer& iConsumerPrototype = TConsumer());
	~ThreadPool();

	void add(typename util::CallTraits<TTask>::TParam iTask);
	void joinAll();
	const unsigned numberOfThreads() const;

private:

	typedef stde::lock_free_queue<TTask> TTaskQueue;

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

	TTaskQueue waitingTasks_;
	ConsumerThread* threads_;
	unsigned long mSecsToSleep_;
	unsigned numThreads_;
	unsigned maxWaitingTasks_;
	unsigned numWaitingTasks_;
	unsigned numRunningTasks_;
	bool shutDown_;
};


}

}

#include "thread_pool.inl"

#endif

// EOF
