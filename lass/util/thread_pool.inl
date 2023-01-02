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
 *	Copyright (C) 2004-2023 the Initial Developer.
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

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4267) // conversion from 'size_t' to 'lass::num::Tuint32', possible loss of data
#	pragma warning(disable: 4996) // this function or variable may be unsafe
#endif

#include <string.h>
#include <stdio.h>

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
	numThreads_(numberOfThreads == autoNumberOfThreads ? numberOfProcessors() : numberOfThreads),
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
		if (maxWaitingTasks_ == unlimitedNumberOfTasks || numWaitingTasks_ < maxWaitingTasks_)
		{
			// because a single producer is assumed, we're certain we can push the task now
			++numWaitingTasks_;
			waitingTasks_.push(task);
			this->wakeConsumer();
			return;
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
			--numWaitingTasks_;
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
		--numWaitingTasks_;
	}
}



template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
size_t ThreadPool<T, C, IP, PP>::numberOfThreads() const
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
	size_t nextProcessor = numThreads_ - dynThreads; // bind dynamic threads to "upper bits"
	try
	{
		for (i = 0; i < dynThreads; ++i)
		{

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
			::_snprintf(
#else
			::snprintf(
#endif
				indexBuffer, indexLength + 1, "%02X", int(i & 0xff));
			indexBuffer[indexLength] = '\0';

			new (&threads_[i]) ConsumerThread(consumerPrototype, *this, nameBuffer);
			try
			{
				threads_[i].run();
				nextProcessor = threads_[i].bindToNextAvailable(nextProcessor);
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



template <typename T, typename C, typename IP, template <typename, typename, typename> class PP>
size_t ThreadPool<T, C, IP, PP>::ConsumerThread::bindToNextAvailable(size_t nextProcessor)
{
	const size_t n = numberOfProcessors();
	const size_t lastBeforeError = nextProcessor + n;
	while (true)
	{
		try
		{
			this->bind(nextProcessor++ % n);
			return nextProcessor % n;
		}
		catch (...)
		{
			if (nextProcessor >= lastBeforeError)
			{
				throw;
			}
		}
	}
	LASS_ASSERT_UNREACHABLE;
}



#define LASS_UTIL_THREAD_POOL_CATCH_AND_WRAP(exception_type)\
catch (const exception_type& error)\
{\
	pool_.error_.reset(new RemoteExceptionWrapper<exception_type>(error));\
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
			++pool_.numRunningTasks_;
			--pool_.numWaitingTasks_;
			pool_.wakeProducer();
			try
			{
				consumer_(task);
			}
			catch (const RemoteExceptionBase& error)
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
			--pool_.numRunningTasks_;
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
