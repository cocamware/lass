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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_POOL_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_POOL_INL

#include "util_common.h"
#include "thread_pool.h"

namespace lass
{
namespace util
{

// --- public --------------------------------------------------------------------------------------

template <typename T, typename C>
ThreadPool<T, C>::ThreadPool(
		unsigned iNumberOfThreads, 
		unsigned iMaximumNumberOfTasksInQueue,
		const TConsumer& iConsumerPrototype):
	threads_(0),
	mSecsToSleep_(100),
	numThreads_(std::max<unsigned>(iNumberOfThreads, 1)),
	maxTasksInQueue_(iMaximumNumberOfTasksInQueue),
	busyThreads_(0),
	shutDown_(false)
{
	startThreads(iConsumerPrototype);
}



template <typename T, typename C>
ThreadPool<T, C>::~ThreadPool()
{
	try
	{
		joinAll();
	}
	catch (...)
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: ~ThreadPool(): "
			"failure while waiting for tasks to complete." << std::endl;
	}
	stopThreads(numThreads_);
}



template <typename T, typename C>
void ThreadPool<T, C>::add(typename util::CallTraits<TTask>::TParam iTask)
{
	bool queueIsFull = true;
	while (queueIsFull)
	{
		LASS_LOCK(mutex_)
		{
			const unsigned tasksInQueue = 
				static_cast<unsigned>(waitingTasks_.size()) + busyThreads_;
			if (tasksInQueue < maxTasksInQueue_ || maxTasksInQueue_ == 0)
			{
				queueIsFull = false;
				waitingTasks_.push(iTask);
			}
		}
		if (queueIsFull)
		{
			conditionProduction_.wait(mSecsToSleep_);
		}
		else
		{
			conditionConsumer_.signal();
		}
	}
}



template <typename T, typename C>
void ThreadPool<T, C>::joinAll()
{
	while (!isEmpty())
	{
		conditionProduction_.wait(mSecsToSleep_);
	}
}



template <typename T, typename C>
void ThreadPool<T, C>::clearQueue()
{
	LASS_LOCK(mutex_)
	{
		while (!waitingTasks_.empty())
		{
			waitingTasks_.pop();
		}
	}
	joinAll();
}



template <typename T, typename C>
const bool ThreadPool<T, C>::isEmpty() const
{
	bool result = false;
	LASS_LOCK(mutex_)
	{
		result = waitingTasks_.empty() && busyThreads_ == 0;
	}
	return result;
}



// --- private -------------------------------------------------------------------------------------

/** Allocate a bunch of threads and run them.
 *  We can't simply use a std::vector or an C-array, because the threads are neither
 *  copy-constructable or default constructable.  So we need to do our own housekeeping.
 *  Don't worry folks, I know what I'm doing ;) [Bramz]
 */
template <typename T, typename C>
void ThreadPool<T, C>::startThreads(const TConsumer& iConsumerPrototype)
{
	LASS_ASSERT(numThreads_ > 0);
	threads_ = static_cast<ConsumerThread*>(malloc(numThreads_ * sizeof(ConsumerThread)));
	if (!threads_)
	{
		throw std::bad_alloc();
	}

	unsigned i;
	try
	{
		for (i = 0; i < numThreads_; ++i)
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
template <typename T, typename C>
void ThreadPool<T, C>::stopThreads(unsigned iNumAllocatedThreads)
{
	try
	{
		LASS_LOCK(mutex_) 
		{
			shutDown_ = true;
		}
		conditionConsumer_.broadcast();
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

template <typename T, typename C>
ThreadPool<T, C>::ConsumerThread::ConsumerThread(const TConsumer& iConsumer, TSelf& iPool):
	Thread(threadJoinable), 
	consumer_(iConsumer),
	pool_(iPool) 
{
}




template <typename T, typename C>
void ThreadPool<T, C>::ConsumerThread::doRun()
{
	TTask task;
	bool hasTask = false;
	while (true)
	{
		LASS_LOCK(pool_.mutex_)
		{
			hasTask = !pool_.waitingTasks_.empty();
			if (hasTask)
			{
				task = pool_.waitingTasks_.front();
				pool_.waitingTasks_.pop();
				++pool_.busyThreads_;
			}
			else if (pool_.shutDown_)
			{
				return;
			}
		}
		if (hasTask)
		{
			pool_.conditionProduction_.signal();
			consumer_(task);
			LASS_LOCK(pool_.mutex_)
			{
				--pool_.busyThreads_;
			}
		}
		else
		{
			pool_.conditionConsumer_.wait(pool_.mSecsToSleep_);
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

#endif

// EOF
