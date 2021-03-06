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
 *	Copyright (C) 2004-2011 the Initial Developer.
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
#ifdef LASS_HAVE_PTHREAD_H
#	include <pthread.h>
#endif
#ifdef LASS_HAVE_PTHREAD_NP_H
#	include <pthread_np.h>
#endif
#if LASS_HAVE_SCHED_H
#	include <sched.h>
#endif
#if LASS_HAVE_UNISTD_H
#	include <unistd.h>
#endif
#if LASS_HAVE_SYS_PROCESSOR_H
#	include <sys/processor.h>
#endif
#if LASS_HAVE_SYS_SYSCALL_H
#	include <sys/syscall.h>
#endif
#if LASS_HAVE_SYSCTL_H_CTL_HW && LASS_HAVE_SYSCTL_H_HW_NCPU
#	include <sys/sysctl.h>
#endif
#if LASS_HAVE_SYS_TIME_H
#	include <sys/time.h>
#endif
#if LASS_HAVE_SYS_CPUSET_H
#	include <sys/param.h> // also required
#	include <sys/cpuset.h>
#endif
#if LASS_HAVE_MACH_THREAD_POLICY_H
#	include <mach/thread_policy.h>
#endif

#if LASS_HAVE_SCHED_H_CPU_SET_T || LASS_HAVE_SYS_CPUSET_H
#	define LASS_HAVE_CPU_SET_T 1
#endif

namespace lass
{
namespace util
{
namespace impl
{

#if LASS_HAVE_SYS_CPUSET_H
typedef cpuset_t cpu_set_t;
#endif

#if LASS_HAVE_CPU_SET_T
cpu_set_t originalCpuSet()
{
	static cpu_set_t cpu_set;
	static bool isInitialized = false;
	if (!isInitialized)
	{
		CPU_ZERO(&cpu_set);
#if LASS_HAVE_SCHED_H_CPU_SET_T
		LASS_ENFORCE_CLIB(sched_getaffinity(0, sizeof(cpu_set_t), &cpu_set));		
#elif LASS_HAVE_SYS_CPUSET_H
		LASS_ENFORCE_CLIB(cpuset_getaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1, sizeof(cpu_set_t), &cpu_set));	
#else
#	error missing implementation
#endif
		isInitialized = true;
	}
	return cpu_set;
}
cpu_set_t forceCalculationBeforeMain = originalCpuSet();
#endif

TCpuSet availableProcessors()
{
#if LASS_HAVE_CPU_SET_T
	cpu_set_t cpu_set = originalCpuSet();
#endif

	// determine number of processors (highest set bit of systemAffinityMask)
#if LASS_HAVE_UNISTD_H_SC_NPROCESSORS_CONF
	const size_t n = static_cast<size_t>(sysconf(_SC_NPROCESSORS_CONF));
#elif LASS_HAVE_CPU_SET_T
	size_t n = 0;
	for (int i = 0; i < CPU_SETSIZE; ++i)
	{
		if (CPU_ISSET(i, &cpu_set))
		{
			n = static_cast<size_t>(i) + 1;
		}
	}
#elif LASS_HAVE_SYSCTL_H_CTL_HW && LASS_HAVE_SYSCTL_H_HW_NCPU
	int mid[2] = { CTL_HW, HW_NCPU };
	int ncpus = 1;
	size_t len = sizeof(ncpus);
	sysctl(mib, 2, &ncpus, &len, 0, 0);
	const size_t n = static_cast<size_t>(ncpus);
#else
#	error missing implementation
#endif

	// determine what processors are available to this process
	TCpuSet cpuSet(n, false);
	for (size_t i = 0; i < n; ++i)
	{
#if LASS_HAVE_CPU_SET_T
		cpuSet[i] = CPU_ISSET(static_cast<int>(i), &cpu_set);
#elif LASS_HAVE_SYS_PROCESSOR_H
		const int r = LASS_ENFORCE_CLIB(p_online(static_cast<processorid_t>(i), P_STATUS))("i=")(i);
		cpuSet[i] = r == P_ONLINE || r == P_NOINTR;
#else
#		warning [LASS BUILD MSG] do not know how to figure out what processors in the CPU set are available. Will assuming they all are. 
		cpuSet[i] = true;
#endif
	}

	return cpuSet;
}


inline pid_t lass_gettid()
{
#if LASS_HAVE_SYS_SYSCALL_H_GETTID
	return static_cast<pid_t>(syscall(__NR_gettid));
#else
	return -1;
#endif
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
	LockResult tryLock()
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
	unsigned lockCount() const 
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
	WaitResult wait(unsigned long milliSeconds)
	{
		enum 
		{
			msec_per_sec = 1000,
			nsec_per_usec = 1000,
			nsec_per_msec = 1000 * 1000,
			nsec_per_sec = nsec_per_msec * msec_per_sec
		};
		
		const unsigned long seconds = milliSeconds / msec_per_sec;
		milliSeconds %= msec_per_sec;
		
		LASS_ENFORCE_CLIB_RC(pthread_mutex_lock(&mutex_));
		
		struct timespec timeToWaitTo;
#if LASS_HAVE_CLOCK_GETTIME
		clock_gettime(CLOCK_REALTIME, &timeToWaitTo);
#else
		::timeval now; 
		::gettimeofday(&now, 0);
		timeToWaitTo.tv_sec = now.tv_sec;
		timeToWaitTo.tv_nsec = now.tv_usec * nsec_per_usec;
#endif
		timeToWaitTo.tv_nsec += milliSeconds * nsec_per_msec;
		timeToWaitTo.tv_sec += seconds;
		if (timeToWaitTo.tv_nsec >= nsec_per_sec)
		{
			timeToWaitTo.tv_sec += timeToWaitTo.tv_nsec / nsec_per_sec;
			timeToWaitTo.tv_nsec %= nsec_per_sec;
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
void bindThread(pthread_t LASS_UNUSED(handle), pid_t LASS_UNUSED(tid), size_t LASS_UNUSED(processor))
{
#if LASS_HAVE_CPU_SET_T
	cpu_set_t mask;
	if (processor == Thread::anyProcessor)
	{
		mask = originalCpuSet();
	}
	else
	{
		LASS_ASSERT(static_cast<int>(processor) >= 0);
		CPU_ZERO(&mask);	
		CPU_SET(static_cast<int>(processor), &mask);
	}
#	if LASS_HAVE_PTHREAD_SETAFFINITY_NP
	LASS_ENFORCE_CLIB(pthread_setaffinity_np(handle, sizeof(cpu_set_t), &mask))("handle=")(handle);
#	else
	LASS_ENFORCE_CLIB(sched_setaffinity(tid, sizeof(cpu_set_t), &mask))("tid=")(tid);
#	endif
#elif LASS_HAVE_SYS_PROCESSOR_H
	const processorid_t cpu_id = processor == Thread::anyProcessor ? PBIND_NONE : static_cast<processorid_t>(processor);
	LASS_ENFORCE_CLIB(processor_bind(P_LWPID, handle, cpu_id, 0))("handle=")(handle);
#else
#	warning [LASS BUILD MSG] no implementation for util::Thread::bind: setting thread affinity will be a no-op.
#endif
}



#define LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(exception_type)\
	catch (const exception_type& error)\
	{\
		pimpl->error_.reset(new RemoteExceptionWrapper<exception_type>(error));\
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

	bool isJoinable() const
	{
		return isJoinable_ && isCreated_;
	}
	
	void join()
	{			
		if (!isJoinable())
		{
			LASS_THROW("Can not wait for uncreated or detached threads");
		}

		pthread_join(handle_, 0);
		isJoinable_ = false;
		if (error_.get())
		{
			error_->throwSelf();
		}
	}

	void bind(size_t processor)
	{
		bindThread(handle_, tid_, processor);
	}

	const TCpuSet affinity() const
	{
		const size_t n = numberOfProcessors();
		TCpuSet result(n, false);
#if LASS_HAVE_CPU_SET_T
		cpu_set_t mask;
#	if LASS_HAVE_PTHREAD_SETAFFINITY_NP
		LASS_ENFORCE_CLIB(pthread_getaffinity_np(handle_, sizeof(cpu_set_t), &mask))("handle=")(handle_);
#	else
		LASS_ENFORCE_CLIB(sched_getaffinity(tid_, sizeof(cpu_set_t), &mask))("tid=")(tid_);
#	endif
		for (size_t i = 0; i < n; ++i)
		{
			result[i] = CPU_ISSET(static_cast<int>(i), &mask);
		}
#elif LASS_HAVE_SYS_PROCESSOR_H
		processorid_t cpu_id;
		LASS_ENFORCE_CLIB(processor_bind(P_LWPID, handle_, PBIND_QUERY, &cpu_id))("handle=")(handle_);
		if (cpu_id == PBIND_NONE)
		{
			return availableProcessors();
		}
		result[static_cast<size_t>(cpu_id)] = true;
#else
#	warning [LASS BUILD MSG] no implementation for util::Thread::affinity: will return set with all available processors 
		return availableProcessors();
#endif		
		return result;
	}
	
	static void sleep(unsigned long milliSeconds)
	{
		enum 
		{
			msec_per_sec = 1000,
			nsec_per_msec = 1000 * 1000,
			nsec_per_sec = nsec_per_msec * msec_per_sec
		};
		
		timespec timeOut;
		if (milliSeconds < msec_per_sec)
		{
			timeOut.tv_sec = 0;
			timeOut.tv_nsec = static_cast<long>(milliSeconds * nsec_per_msec);
		}
		else
		{
			timeOut.tv_sec = milliSeconds / msec_per_sec;
			timeOut.tv_nsec = (milliSeconds % msec_per_sec) * nsec_per_msec;
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
		bindThread(pthread_self(), lass_gettid(), processor);
	}

	// thread function
	static void* staticThreadStart(void* iPimpl)
	{
		LASS_ASSERT(iPimpl);
		ThreadInternal* pimpl = static_cast<ThreadInternal*>(iPimpl);
		pimpl->tid_ = lass_gettid();
		pimpl->isCreated_ = true;
		if (pimpl->isJoinable_)
		{
			try
			{
				pimpl->runCondition_.signal();
				pimpl->thread_.doRun();
			}
			catch (const RemoteExceptionBase& error)
			{
				pimpl->error_ = error.clone();
			}
			LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(::std::domain_error)
			LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(::std::invalid_argument)
			LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(::std::length_error)
			LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(::std::out_of_range)
			LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(::std::range_error)
			LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(::std::overflow_error)
			LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(::std::underflow_error)
			LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(::std::runtime_error)
			LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(::std::logic_error)
			LASS_UTIL_THREAD_POSIX_CATCH_AND_WRAP(::std::exception)
		}
		else
		{
			pimpl->runCondition_.signal();
			pimpl->thread_.doRun();
			delete &pimpl->thread_;
		}
		return 0;
	}

private:

	Thread& thread_;
	pthread_t handle_;	 // handle of the thread
	Condition runCondition_;
	TRemoteExceptionBasePtr error_;

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
	void* get() const
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
