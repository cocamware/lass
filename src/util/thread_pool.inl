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

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4267) // conversion from 'size_t' to 'lass::num::Tuint32', possible loss of data
#	pragma warning(disable: 4996) // this function or variable may be unsafe
#endif

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
		size_t numberOfThreads, 
		size_t maximumNumberOfTasksInQueue,
		const TConsumer& consumerPrototype,
		const char* name):
	TParticipationPolicy(consumerPrototype),
	threads_(0),
	numThreads_(numberOfThreads == autoNumberOfThreads ? numberOfProcessors : numberOfThreads),
	maxWaitingTasks_(maximumNumberOfTasksInQueue),
	numWaitingTasks_(0),
	numRunningTasks_(0),
	shutDown_(false),
	abort_(false)
{
	LASS_ENFORCE(numThreads_ > 0);
	startThreads(consumerPrototype, name);
}



template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
ThreadPool<T, C, IP, PP>::~ThreadPool()
{
	try
	{
		completeAllTasks();
	}
	LASS_CATCH_TO_WARNING
	stopThreads(this->numDynamicThreads(numThreads_));
}



/** submit a task to the pool, and block if queue is full.
 *  Function waits until tasks can be added to queue without participating as producer 
 *  	(in case of SelfParticipating).
 */
template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
void ThreadPool<T, C, IP, PP>::addTask(typename util::CallTraits<TTask>::TParam task)
{
	while (true)
	{
		size_t numWaitingTasks = numWaitingTasks_;
		if (maxWaitingTasks_ == unlimitedNumberOfTasks || numWaitingTasks < maxWaitingTasks_)
		{
			if (util::atomicCompareAndSwap(
				numWaitingTasks_, numWaitingTasks, numWaitingTasks + 1))
			{
				waitingTasks_.push(task);
				this->wakeConsumer();
				return;
			}
		}
		this->sleepProducer();
		if (error_.get())
		{
			abort_ = true;
			error_->throwSelf();
		}
	}
}



/** blocks until all tasks in the queue are completed
 *  control thread participates as consumer if policy allows for it.
 */
template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
void ThreadPool<T, C, IP, PP>::completeAllTasks()
{
	while (numWaitingTasks_ > 0 || numRunningTasks_ > 0)
	{
		if (this->participate(waitingTasks_))
		{
			atomicDecrement(numWaitingTasks_);
		}
		this->sleepProducer();
		if (error_.get())
		{
			abort_ = true;
			error_->throwSelf();
		}
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
const size_t ThreadPool<T, C, IP, PP>::numberOfThreads() const
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
void ThreadPool<T, C, IP, PP>::startThreads(const TConsumer& consumerPrototype, const char* name)
{
	LASS_ASSERT(numThreads_ > 0);
	const size_t dynThreads = this->numDynamicThreads(numThreads_);
	const size_t bindOffset = numThreads_ - dynThreads; // bind dynamic threads to "upper bits"
	if (dynThreads == 0)
	{
		threads_ = 0;
		return;
	}

	const size_t bufferSize = 10;
	char nameBuffer[bufferSize] = "consumers";
	if (name)
	{
		strncpy(nameBuffer, name, bufferSize);
	}
	nameBuffer[bufferSize - 1] = '\0';
	const size_t length = strlen(nameBuffer);
	const size_t indexLength = 2;
	char* indexBuffer = nameBuffer + std::min(length, bufferSize - indexLength - 1);	

	threads_ = static_cast<ConsumerThread*>(malloc(dynThreads * sizeof(ConsumerThread)));
	if (!threads_)
	{
		throw std::bad_alloc();
	}

	size_t i;
	try
	{
		for (i = 0; i < dynThreads; ++i)
		{

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
			_snprintf(
#else
			snprintf(
#endif
				indexBuffer, indexLength + 1, "%02X", int(i & 0xff));
			indexBuffer[indexLength] = '\0';

			new (&threads_[i]) ConsumerThread(consumerPrototype, *this, nameBuffer);
			try
			{
				threads_[i].run();
				threads_[i].bind((bindOffset + i) % numberOfProcessors);
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
void ThreadPool<T, C, IP, PP>::stopThreads(size_t numAllocatedThreads)
{
	shutDown_ = true;
	try
	{
		this->wakeAllConsumers();
	}
	LASS_CATCH_TO_WARNING

	LASS_ASSERT(static_cast<int>(numAllocatedThreads) >= 0);
	for (size_t n = numAllocatedThreads; n > 0; --n)
	{
		try
		{
			threads_[n - 1].join();
		}
		LASS_CATCH_TO_WARNING
		threads_[n - 1].~ConsumerThread(); // shouldn't throw anyway ...
	}

	free(threads_); // shouldn't throw
}



// --- ConsumerThread ------------------------------------------------------------------------------

template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
ThreadPool<T, C, IP, PP>::ConsumerThread::ConsumerThread(
		const TConsumer& consumer, TSelf& pool, const char* name):
	Thread(threadJoinable, name), 
	consumer_(consumer),
	pool_(pool) 
{
}



#define LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(exception_type)\
catch (const exception_type& error)\
{\
	pool_.error_.reset(new experimental::RemoteExceptionWrapper<exception_type>(error));\
	return;\
}

template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
void ThreadPool<T, C, IP, PP>::ConsumerThread::doRun()
{
	TTask task;
	while (!pool_.abort_)
	{
		if (pool_.waitingTasks_.pop(task))
		{
			util::atomicIncrement(pool_.numRunningTasks_);
			util::atomicDecrement(pool_.numWaitingTasks_);
			pool_.wakeProducer();
			try
			{
				consumer_(task);
			}
			catch (const experimental::RemoteExceptionBase& error)
			{
				pool_.error_ = error.clone();
				return;
			}
			LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(::std::domain_error)
			LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(::std::invalid_argument)
			LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(::std::length_error)
			LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(::std::out_of_range)
			LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(::std::range_error)
			LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(::std::overflow_error)
			LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(::std::underflow_error)
			LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(::std::runtime_error)
			LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(::std::logic_error)
			LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(::std::exception)
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
void DefaultConsumer<T>::operator()(typename util::CallTraits<T>::TParam task)
{
	task();
}



}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

// EOF
