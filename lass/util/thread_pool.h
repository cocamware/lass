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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

/** @class ThreadPool
 *  @ingroup Threading
 *  @brief Production-Consumer concurrency pattern
 *  @author Bramz
 *
 *  @code
 *  #include <lass/util/thread_pool.h>
 *  #include <lass/util/bind.h>
 *  using namespace lass::util;
 *
 *  void parallelWork(int begin, int end);
 *  const int n = 1000000;
 *  const int step = 1000;
 *
 *  util::ThreadPool<> pool;
 *  for (int i = 0; i < n; i += step)
 *  {
 *  	pool.addTask(bind(parallelWork, i, std::min(i + step, n)));
 *  }
 *  pool.completeAllTasks();
 *  @endcode
 * 
 *  @section Good-Practice
 *
 *  Using the policies and constructor parameters, the ThreadPool can be operated in many
 *  different configurations.  However, there are two that are particular interesting:
 *
 *  @subsection 1 fast synchronized thread pool
 *
 *  Best used when short bursts of a bounded number of tasks are followed by completeAllTasks().
 *  Example: A single task is splitted in a parallel subtasks, all feeded to the pool at once,
 *  	and completeAllTasks() is called to finish the grand tasks. 
 
 *  @arg IdlePolicy: Spinning, for a fast reaction time
 *  @arg ParticipationPolicy: SelfParticipating, so that during completeAllTasks() the cpu cycles 
 *  	of the control thread are not wasted by spinning.
 *  @arg numberOfThreads: autoNumberOfThreads
 *  @arg maxNumberOfTasksInQueue: unlimitedNumberOfTasks, because the number of tasks in the queue
 *  	can be (and should be) bounded by the application itself, and we want the control thread
 *  	to go in completeAllTasks() ASAP, so it can donate cpu cycles to completing the tasks
 *  	instead of waisting them spinning until another task can be added to the queue.
 *
 *  @subsection 2 slower continues thread pool
 *
 *  Best used when a continues (unbounded) stream of tasks reaches a server.
 *
 *  @arg IdlePolicy: Signaled, to avoid needing an extra cpu for the control thread.
 *  @arg ParticipationPolicy: NotParticipating, because you'll never call completeAllTasks()
 *  @arg numberOfThreads: autoNumberOfThreads
 *  @arg maxNumberOfTasksInQueue: <A LIMITED NUMBER> to avoid an excessive queue size
 *
 *  @section Policies
 *
 *  @subsection IdlePolicy
 *
 *  @code
 *  class IdlePolicy
 *  {
 *  protected:
 *  	void sleepProducer();
 *  	void wakeProducer();
 *  	void sleepConsumer();
 *  	void wakeConsumer();
 *  	void wakeAllConsumers();
 *  };
 *  @endcode
 *
 *  @subsection ParticipationPolicy
 *
 *  @code
 *  template <typename TaskType, typename ConsumerType, typename IdlePolicy>
 *  class ParticipationPolicy: public IdlePolicy
 *  {
 *  protected:
 *  	NotParticipating(const ConsumerType& prototype);
 *  	const unsigned numDynamicThreads(unsigned numThreads) const;
 *  	template <typename Queue> bool participate(Queue&);
 *  };
 *  @endcode
 *
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_POOL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_POOL_H

#include "util_common.h"
#include "thread.h"
#include "callback_0.h"
#include "../stde/lock_free_queue.h"
#include "future.h"
#include <cstring>

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
	void operator()(typename util::CallTraits<TaskType>::TParam task);
};

/** implementation of ThreadPool's IdlePolicy
 *  @ingroup Threading
 *  @sa ThreadPool
 *
 *  Idle threads don't go to sleep, but spin until a condition is met.
 *  This results in a faster response but it should only be used if each thread 
 *  (including the control thread) can have it's own cpu or core.
 */
class Spinning
{
protected:
	Spinning() {}
	~Spinning() {}
	void sleepProducer() {}
	void wakeProducer() {}
	void sleepConsumer() {}
	void wakeConsumer() {}
	void wakeAllConsumers() {}
};

/** implementation of ThreadPool's IdlePolicy
 *  @ingroup Threading
 *  @sa ThreadPool
 *
 *  Idle threads are put to sleep and signaled to awake when a condition is met.
 *  It results in a slower response of the threads, but it avoids wasting cpu cycles
 *  to idle threads.
 */
class Signaled
{
protected:
	Signaled() {}
	~Signaled() {}
	void sleepProducer() { producer_.wait(mSecsToSleep_); }
	void wakeProducer() { producer_.signal(); }
	void sleepConsumer() { consumer_.wait(mSecsToSleep_); }
	void wakeConsumer() { consumer_.signal(); }
	void wakeAllConsumers() { consumer_.broadcast(); }
private:
	enum { mSecsToSleep_ = 50 };
	Condition producer_;
	Condition consumer_;
};

/** implementation of ThreadPool's ParticipationPolicy
 *  @ingroup Threading
 *  @sa ThreadPool
 *
 *  The control thread participates as producer while waiting to complete all tasks.
 *  Is best used when short burst of adding a limited number of tasks to an 
 *  unlimited queue, followed by a completeAllTasks().
 */
template <typename TaskType, typename ConsumerType, typename IdlePolicy>
class SelfParticipating: public IdlePolicy
{
protected:
	SelfParticipating(const ConsumerType& prototype): IdlePolicy(), consumer_(prototype) { Thread::bindCurrent(0); }
	~SelfParticipating() { Thread::bindCurrent(Thread::anyProcessor); }
	size_t numDynamicThreads(size_t numThreads) const { return numThreads - 1; }
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

/** implementation of ThreadPool's ParticipationPolicy
 *  @ingroup Threading
 *  @sa ThreadPool
 *
 *  The control thread will not participate as producer thread at any time.
 *  Should be used when there's a continues stream of added tasks too a limited queue,
 *  and when completeAllTasks() is never called.
 */
template <typename TaskType, typename ConsumerType, typename IdlePolicy>
class NotParticipating: public IdlePolicy
{
protected:
	NotParticipating(const ConsumerType&): IdlePolicy() {}
	~NotParticipating() {}
	size_t numDynamicThreads(size_t numThreads) const { return numThreads; }
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

	enum 
	{ 
		autoNumberOfThreads = 0,
		unlimitedNumberOfTasks = 0
	};

	ThreadPool(size_t numberOfThreads = autoNumberOfThreads, 
		size_t maximumNumberOfTasksInQueue = unlimitedNumberOfTasks, 
		const TConsumer& consumerPrototype = TConsumer(),
		const char* name = 0);
	~ThreadPool();

	void addTask(typename util::CallTraits<TTask>::TParam task);
	void completeAllTasks();
	void clearQueue();
	size_t numberOfThreads() const;

private:

	typedef stde::lock_free_queue<TTask> TTaskQueue;

	friend class ConsumerThread;

	class ConsumerThread: public Thread
	{
	public:
		ConsumerThread(const TConsumer& consumer, TSelf& pool, const char* name);
		unsigned bindToNextAvailable(unsigned processor);
	private:
		void doRun();
		TConsumer consumer_;
		TSelf& pool_;
	};

	void startThreads(const TConsumer& consumerPrototype, const char* name);
	void stopThreads(size_t numAllocatedThreads);

	TTaskQueue waitingTasks_;
	std::auto_ptr<RemoteExceptionBase> error_;
	ConsumerThread* threads_;
	unsigned long mSecsToSleep_;
	size_t numThreads_;
	size_t maxWaitingTasks_;
	volatile size_t numWaitingTasks_;
	volatile size_t numRunningTasks_;
	volatile bool shutDown_;
	volatile bool abort_;
};


}

}

#include "thread_pool.inl"

#endif

// EOF
