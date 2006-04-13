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
	numThreads_(std::min<unsigned>(iNumberOfThreads, 1)),
	maxTasksInQueue_(iMaximumNumberOfTasksInQueue),
	shutDown_(false)
{
	startThreads(iConsumerPrototype);
}



template <typename T, typename C>
ThreadPool<T, C>::~ThreadPool()
{
	try
	{
		waitUntilEmpty();
	}
	catch (...)
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: failure while waiting "
			"for tasks to complete in ~ThreadPool()." << std::endl;
	}
	stopThreads(numThreads_);
}



template <typename T, typename C>
void ThreadPool<T, C>::add(typename util::CallTraits<TTask>::TParam iTask)
{
	bool ok = false;
	while (!ok)
	{
		LASS_LOCK(mutex_)
		{
			if (tasks_.size() < maxTasksInQueue_ || maxTasksInQueue_ == 0)
			{
				tasks_.push(iTask);
				ok = true;
			}
		}
		if (ok)
		{
			conditionConsumer_.signal();
		}
		else
		{
			conditionProduction_.wait(mSecsToSleep_);
		}
	}
}



template <typename T, typename C>
void ThreadPool<T, C>::clearQueue()
{
	CriticalSectionLocker lock(mutex_);
	tasks_.clear();
}



template <typename T, typename C>
void ThreadPool<T, C>::waitUntilEmpty()
{
	while (!isEmpty())
	{
		conditionProduction_.wait(mSecsToSleep_);
	}
}



template <typename T, typename C>
const bool ThreadPool<T, C>::isEmpty() const
{
	CriticalSectionLocker lock(mutex_);
	return tasks_.empty();
}



// --- private -------------------------------------------------------------------------------------

/** Allocate a bunch of threads and run them.
 *  We can't simply use a std::vector or an C-array, because the threads are neither
 *  copy-constructable or defaut construtable.  So we need to do our own household keeping.
 *  Don't worry folks, I know what I'm doing ;)
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
		stopThreads(i);
		throw;
	}
}



/** Deallocate the threads and free memory
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
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: failure while setting "
			"flag while shutting down ThreadPool" << std::endl;
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
			std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: failed to join thread "
				"while shutting down ThreadPool" << std::endl;
		}
		threads_[i].~ConsumerThread();
	}

	free(threads_);
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
			hasTask = !pool_.tasks_.empty();
			if (hasTask)
			{
				task = pool_.tasks_.front();
				pool_.tasks_.pop();
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
