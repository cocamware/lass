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

namespace lass
{
namespace util
{

// --- public --------------------------------------------------------------------------------------

/** @param iNumberOfThreads specify number of producer threads.  Specify @a autoNumberOfThreads
 *  	to automatically use as many threads as processors.
 *  @param mNumberOfTasksInQueue specifiy the maximum number of tasks that may be waiting in the
 *  	queue.  Specify @a unlimitedNumberOfTasks to have an unlimited queue.
 */
template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
ThreadPool<T, C, IP, PP>::ThreadPool(
		unsigned iNumberOfThreads, 
		unsigned iMaximumNumberOfTasksInQueue,
		const TConsumer& iConsumerPrototype):
	TParticipationPolicy(iConsumerPrototype),
	threads_(0),
	numThreads_(iNumberOfThreads == autoNumberOfThreads ? numberOfProcessors() : iNumberOfThreads),
	maxWaitingTasks_(iMaximumNumberOfTasksInQueue),
	numWaitingTasks_(0),
	numRunningTasks_(0),
	shutDown_(false)
{
	LASS_ENFORCE(numThreads_ > 0);
	startThreads(iConsumerPrototype);
}



template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
ThreadPool<T, C, IP, PP>::~ThreadPool()
{
	try
	{
		completeAllTasks();
	}
	catch (...)
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: ~ThreadPool(): "
			"failure while waiting for tasks to complete." << std::endl;
	}
	stopThreads(numDynamicThreads(numThreads_));
}



/** submit a task to the pool, and block if queue is full.
 *  Function waits until tasks can be added to queue without participating as producer 
 *  	(in case of SelfParticipating).
 */
template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
void ThreadPool<T, C, IP, PP>::addTask(typename util::CallTraits<TTask>::TParam iTask)
{
	while (true)
	{
		unsigned numWaitingTasks = numWaitingTasks_;
		if (maxWaitingTasks_ == unlimitedNumberOfTasks || numWaitingTasks < maxWaitingTasks_)
		{
			if (util::atomicCompareAndSwap(
				numWaitingTasks_, numWaitingTasks, numWaitingTasks + 1))
			{
				waitingTasks_.push(iTask);
				wakeConsumer();
				return;
			}
			else
			{
				sleepProducer();
			}
		}
	}
}



/** blocks until all tasks in the queue are completed
 *  control thread participates as producer if policy allows for it.
 */
template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
void ThreadPool<T, C, IP, PP>::completeAllTasks()
{
	while (numWaitingTasks_ > 0 || numRunningTasks_ > 0)
	{
		if (participate(waitingTasks_))
		{
			atomicDecrement(numWaitingTasks_);
		}
		sleepProducer();
	}
}



/** clear queue without completing tasks.
 *  All waiting tasks in the queue are simply thrown away without ever being completed.
 */
template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
void ThreadPool<T, C, IP, PP>::clearQueue()
{
	TTask dummy;
	while (waitingTasks_.pop(dummy))
	{
	}
}



template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
const unsigned ThreadPool<T, C, IP, PP>::numberOfThreads() const
{
	return numThreads_;
}



// --- private -------------------------------------------------------------------------------------

/** Allocate a bunch of threads and run them.
 *  We can't simply use a std::vector or an C-array, because the threads are neither
 *  copy-constructable or default constructable.  So we need to do our own housekeeping.
 *  Don't worry folks, I know what I'm doing ;) [Bramz]
 */
template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
void ThreadPool<T, C, IP, PP>::startThreads(const TConsumer& iConsumerPrototype)
{
	LASS_ASSERT(numThreads_ > 0);
	const unsigned dynThreads = numDynamicThreads(numThreads_);
	if (dynThreads == 0)
	{
		threads_ = 0;
		return;
	}

	threads_ = static_cast<ConsumerThread*>(malloc(dynThreads * sizeof(ConsumerThread)));
	if (!threads_)
	{
		throw std::bad_alloc();
	}

	unsigned i;
	try
	{
		for (i = 0; i < dynThreads; ++i)
		{
			new (&threads_[i]) ConsumerThread(iConsumerPrototype, *this);
			try
			{
				threads_[i].run();
			}
			catch (...)
			{
				threads_[i].~ConsumerThread();
				throw;
			}
		}
	}
	catch (...)
	{
		stopThreads(i); // i == number of threads already started.
		throw;
	}
}



/** Deallocate the threads and free memory.
 *  @throw no exceptions should be throw, nor bubble up from this function ... ever!
 */
template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
void ThreadPool<T, C, IP, PP>::stopThreads(unsigned iNumAllocatedThreads)
{
	try
	{
		shutDown_ = true;
		wakeAllConsumers();
	}
	catch (...)
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: ThreadPool: "
			"failure while setting shutdown flag" << std::endl;
		shutDown_ = true; // try it anyway, it's rather important.
	}

	LASS_ASSERT(static_cast<int>(iNumAllocatedThreads) >= 0);
	for (int i = static_cast<int>(iNumAllocatedThreads) - 1; i >= 0; --i)
	{
		try
		{
			threads_[i].join();
		}
		catch (...)
		{
			std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: ThreadPool: "
				"failure while joining thread" << std::endl;
		}
		threads_[i].~ConsumerThread(); // shouldn't throw anyway ...
	}

	free(threads_); // shouldn't throw
}



// --- ConsumerThread ------------------------------------------------------------------------------

template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
ThreadPool<T, C, IP, PP>::ConsumerThread::ConsumerThread(const TConsumer& iConsumer, TSelf& iPool):
	Thread(threadJoinable), 
	consumer_(iConsumer),
	pool_(iPool) 
{
}




template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
void ThreadPool<T, C, IP, PP>::ConsumerThread::doRun()
{
	TTask task;
	while (true)
	{
		if (pool_.waitingTasks_.pop(task))
		{
			util::atomicIncrement(pool_.numRunningTasks_);
			util::atomicDecrement(pool_.numWaitingTasks_);
			pool_.wakeProducer();
			consumer_(task);
			util::atomicDecrement(pool_.numRunningTasks_);
		}
		else if (pool_.shutDown_)
		{
			return;
		}
		else
		{
			pool_.sleepConsumer();
		}
	}
}

// --- DefaultConsumer -----------------------------------------------------------------------------

template <typename T> inline
void DefaultConsumer<T>::operator()(typename util::CallTraits<T>::TParam iTask)
{
	iTask();
}



}

}

// EOF
