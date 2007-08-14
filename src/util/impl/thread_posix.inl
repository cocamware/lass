/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_THREAD_POSIX_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_THREAD_POSIX_INL

#include "../util_common.h"
#include "../thread.h"
#include "../../stde/extended_string.h"
#include "lass_errno.h"
#include <errno.h>
#include <pthread.h>
#include <sched.h>

namespace lass
{
namespace util
{
namespace impl
{

size_t numberOfProcessors()
{
	// we'll need to cache this if we want this to ever work ...
	
	cpu_set_t mask;
	CPU_ZERO(&mask);	
	LASS_ENFORCE_CLIB(sched_getaffinity(0, sizeof(cpu_set_t), &mask));
	
	size_t count = 0;
	int i = 0;
	while (CPU_ISSET(i++, &mask))
	{
		++count;
	}
	
	// we're doing an assumption here ... We think, we hope, that the mask
	// is a continuous series of bits starting from the LSB.  We'll test for this
	// until we are sure that our assumption is correct. [Bramz]
	//
	while (i < CPU_SETSIZE)
	{
		if (CPU_ISSET(i++, &mask))
		{
			std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR: "
				"numberOfProcessors' assumption is wrong!\n" << std::endl;
		}
	}
	
	return count;
}



/** @internal
 *  @ingroup Threading
 */
class MutexInternal: NonCopyable
{
public:
	MutexInternal():
		lockCount_(0)
	{
		pthread_mutexattr_t mutexattr;
		LASS_ENFORCE_CLIB_RC(pthread_mutexattr_init(&mutexattr));
		LASS_ENFORCE_CLIB_RC(pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE));
		LASS_ENFORCE_CLIB_RC(pthread_mutex_init(&mutex_,&mutexattr));
		LASS_ENFORCE_CLIB_RC(pthread_mutexattr_destroy(&mutexattr));
	}
	~MutexInternal() 
	{
		LASS_ASSERT(lockCount_ == 0);
		LASS_WARN_CLIB_RC(pthread_mutex_destroy(&mutex_));
	}
	void lock()
	{
		LASS_ENFORCE_CLIB_RC(pthread_mutex_lock(&mutex_));
		++lockCount_;
	}
	const LockResult tryLock()
	{
		const int ret = pthread_mutex_trylock(&mutex_);
		if (ret == 0)
		{
			++lockCount_;
			return lockSuccess;
		}
		else if (ret == EBUSY)
		{
			return lockBusy;
		}
		LASS_THROW("pthread_mutex_trylock failed: (" 
			<< ret << ") " << impl::lass_strerror(ret));
	}
	void unlock()
	{
		LASS_ASSERT(lockCount_ > 0);
		if (lockCount_ == 0)
		{
			LASS_THROW("attempting to unlock an unlocked mutex");
		}
		--lockCount_;
		LASS_ENFORCE_CLIB_RC(pthread_mutex_unlock(&mutex_));
	}
	const unsigned lockCount() const 
	{
		return lockCount_;
	}
public:
	pthread_mutex_t mutex_;
	unsigned lockCount_;
};

/** @internal
 *  @ingroup Threading
 */
typedef MutexInternal CriticalSectionInternal;



/** @internal
 *  @ingroup Threading
 */
class ConditionInternal: NonCopyable
{
public:
	ConditionInternal():
		threadsWaiting_(0),
		signalFlag_(false),
		broadcastFlag_(false)
	{
		LASS_ENFORCE_CLIB_RC(pthread_cond_init(&condition_, NULL));
		LASS_ENFORCE_CLIB_RC(pthread_mutex_init(&mutex_, NULL));
	}
	~ConditionInternal()
	{
		LASS_WARN_CLIB_RC(pthread_mutex_destroy(&mutex_));
		LASS_WARN_CLIB_RC(pthread_cond_destroy(&condition_));
	}		
	void wait()
	{
		LASS_ENFORCE_CLIB_RC(pthread_mutex_lock(&mutex_));
		
		int retWait = 0;
		++threadsWaiting_;
		while (retWait == 0 && !(signalFlag_ || broadcastFlag_))
		{
			retWait = pthread_cond_wait(&condition_, &mutex_);
			LASS_ASSERT(retWait == 0);
		}
		--threadsWaiting_;
		signalFlag_ = false;
		broadcastFlag_ &= (threadsWaiting_ > 0);		
		
		const int retUnlock = pthread_mutex_unlock(&mutex_);
		LASS_ASSERT(retUnlock == 0);
		if (retWait != 0)
		{
			LASS_THROW("pthread_cond_wait failed: (" 
				<< retWait << ") " << impl::lass_strerror(retWait));
		}
		if (retUnlock != 0)
		{
			LASS_THROW("pthread_mutex_unlock failed: (" 
				<< retUnlock << ") " << impl::lass_strerror(retUnlock));
		}
	}	
	const WaitResult wait(unsigned long iMilliSeconds)
	{
		const unsigned million =  1000000;
		const unsigned trillion = 1000000000;
		
		LASS_ENFORCE_CLIB_RC(pthread_mutex_lock(&mutex_));
		
		struct timespec timeToWaitTo;
		clock_gettime(CLOCK_REALTIME,&timeToWaitTo);
		timeToWaitTo.tv_nsec += iMilliSeconds * million;
		if (timeToWaitTo.tv_nsec >= trillion)
		{
			timeToWaitTo.tv_sec += timeToWaitTo.tv_nsec / trillion;
			timeToWaitTo.tv_nsec %= trillion;
		}
		
		++threadsWaiting_;
		int retWait = 0;
		while (retWait == 0 && !(signalFlag_ || broadcastFlag_))
		{
			retWait = pthread_cond_timedwait(&condition_,&mutex_,&timeToWaitTo);
			LASS_ASSERT(retWait == 0 || retWait == ETIMEDOUT);
		}
		
		--threadsWaiting_;
		signalFlag_ = false;
		broadcastFlag_ &= (threadsWaiting_ > 0);		

		const int retUnlock = pthread_mutex_unlock(&mutex_);
		LASS_ASSERT(retUnlock == 0);
		if (retWait != 0 && retWait != ETIMEDOUT)
		{
			LASS_THROW("pthread_cond_timedwait failed: (" 
				<< retWait << ") " << impl::lass_strerror(retWait));
		}
		if (retUnlock != 0)
		{
			LASS_THROW("pthread_mutex_unlock failed: (" 
				<< retUnlock << ") " << impl::lass_strerror(retUnlock));
		}

		LASS_ASSERT(retWait == 0 || retWait == ETIMEDOUT);
		return retWait == 0 ? waitSuccess : waitTimeout;
	}
	void signal()
	{
		LASS_ENFORCE_CLIB_RC(pthread_mutex_lock(&mutex_));
		signalFlag_ = true;
		pthread_cond_signal(&condition_);
		LASS_ENFORCE_CLIB_RC(pthread_mutex_unlock(&mutex_));
	}
	void broadcast()
	{
		LASS_ENFORCE_CLIB_RC(pthread_mutex_lock(&mutex_));
		signalFlag_ = true;
		pthread_cond_broadcast(&condition_);
		LASS_ENFORCE_CLIB_RC(pthread_mutex_unlock(&mutex_));
	}
private:
	pthread_mutex_t mutex_;
	pthread_cond_t condition_;
	unsigned threadsWaiting_;
	bool signalFlag_;
	bool broadcastFlag_;
};



/** @internal
 *  @ingroup Threading
 */
void bindThread(pid_t pid, size_t processor)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	
	if (processor == Thread::anyProcessor)
	{
		const int n = static_cast<int>(util::numberOfProcessors);
		LASS_ASSERT(n > 0);
		for (int i = 0; i < n; ++i)
		{
			CPU_SET(i, &mask);
		}
	}
	else
	{
		if (processor >= util::numberOfProcessors)
		{
			LASS_THROW("'" << processor << "' is an invalid processor index. "
				<< "Valid range is [0, " << util::numberOfProcessors << ").");
		}
		LASS_ASSERT(static_cast<int>(processor) >= 0);
		CPU_SET(static_cast<int>(processor), &mask);
	}
	
	LASS_ENFORCE_CLIB(sched_setaffinity(pid, sizeof(cpu_set_t), &mask));	
}



/** @internal
 *  @ingroup Threading
 */
class ThreadInternal: NonCopyable
{
public:

	ThreadInternal(Thread& iThread, ThreadKind iKind, const char* /*name*/):
		thread_(iThread),
		isJoinable_(iKind == threadJoinable),
		isCreated_(false)
	{

	}
	
	~ThreadInternal()
	{
	}
	
	/** run thread.
		*/
	void run()
	{
		if (isCreated_)
		{
			LASS_THROW("You can run a thread only once");
		}
		LASS_ENFORCE_CLIB_RC(pthread_create(
			&handle_, 0, &ThreadInternal::staticThreadStart, this));
		while (!isCreated_)
		{
			runCondition_.wait(100);
		}
	}
	
	void join()
	{			
		if (!(isJoinable_ && isCreated_))
		{
			LASS_THROW("Can not wait for uncreated or detached threads");
		}
		else
		{
			pthread_join(handle_, 0);
			isJoinable_ = false;
		}
	}

	void bind(size_t processor)
	{
		bindThread(tid_, processor);
	}
	
	static void sleep(unsigned long iMilliSeconds)
	{
		timespec timeOut;
		if (iMilliSeconds < 1000)
		{
			timeOut.tv_sec = 0;
			timeOut.tv_nsec = iMilliSeconds * 1000000;
		}
		else
		{
			timeOut.tv_sec = iMilliSeconds / 1000;
			timeOut.tv_nsec = (iMilliSeconds % 1000) * 1000000;
		}
		
		// nanosleep may return earlier than expected if there's a signal
		// that should be handled by the calling thread.  If it happens,
		// sleep again. [Bramz]
		//
		timespec timeRemaining;
		while (true)
		{
			const int ret = nanosleep(&timeOut, &timeRemaining);
			if (ret == 0)
			{
				// we're done =)
				return; 
			}
			const int errnum = impl::lass_errno();
			if (errnum != EINTR)
			{
				// there was an error :(
				LASS_THROW("nanosleep failed: (" << errnum
					<< ") " << impl::lass_strerror(errnum));
			}
			// if we're here, there was only an sleep interruption
			// go back to sleep.
			timeOut.tv_sec = timeRemaining.tv_sec;
			timeOut.tv_nsec = timeRemaining.tv_nsec;
		}
		LASS_ASSERT(timeRemaining.tv_sec == 0 && timeRemaining.tv_nsec == 0);
	}
	
	static void yield()
	{
		LASS_ENFORCE_CLIB(sched_yield());
	}

	static void bindCurrent(size_t processor)
	{
		bindThread(0, processor);
	}

	// thread function
	static void* staticThreadStart(void* iPimpl)
	{
		LASS_ASSERT(iPimpl);
		ThreadInternal* pimpl = static_cast<ThreadInternal*>(iPimpl);
		pimpl->tid_ = getpid();
		pimpl->isCreated_ = true;
		pimpl->runCondition_.signal();
		pimpl->thread_.doRun();
		if (!pimpl->isJoinable_)
		{
			delete &pimpl->thread_;
		}
		return 0;
	}

private:

	Thread& thread_;
	pthread_t handle_;	 // handle of the thread
	Condition runCondition_;
	pid_t tid_;
	volatile bool isJoinable_;
	volatile bool isCreated_;
};



/** @internal
 *  @ingroup Threading
 */
class ThreadLocalStorageInternal: NonCopyable
{
public:
	ThreadLocalStorageInternal(void (*destructor)(void*))
	{
		LASS_ENFORCE_CLIB_RC(pthread_key_create(&key_, destructor));
	}
	~ThreadLocalStorageInternal()
	{
		LASS_WARN_CLIB_RC(pthread_key_delete(key_));
	}
	void* const get() const
	{
		return pthread_getspecific(key_);
	}
	void set(const void* value)
	{
		LASS_ENFORCE_CLIB_RC(pthread_setspecific(key_, value));
	}
private:
	pthread_key_t key_;
};

}
}
}

#endif
 
// EOF
