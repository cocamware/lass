/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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

#include "util_common.h"

#if 0

#ifdef LASS_THREAD_POSIX

#include "thread.h"
#include "impl/lass_errno.h"
#include <errno.h>
#include <pthread.h>

namespace lass
{
namespace util
{

namespace impl
{
	class MutexInternal
	{
	public:
		MutexInternal():
			lockCount_(0)
		{
			pthread_mutexattr_t mutexattr;
			LASS_ENFORCE_CLIB_RC(pthread_mutexattr_init(&mutexattr));
			LASS_ENFORCE_CLIB_RC(pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP));
			LASS_ENFORCE_CLIB_RC(pthread_mutex_init(&mutex_,&mutexattr));
			LASS_ENFORCE_CLIB_RC(pthread_mutexattr_destroy(&mutexattr));
		}
		~MutexInternal() 
		{
			LASS_ASSERT(lockCount_ == 0);
			if (lockCount_ > 0)
			{
				std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
					<< "destroying a mutex that still has " << lockCount_
					<< "locks." << std::endl;
			}
			const int ret = pthread_mutex_destroy(&mutex_);
			LASS_ASSERT(ret == 0);
			if (ret != 0)
			{
				std::cerr << "[LASS RUN MSG] WARNING: pthread_mutex_destroy failed: (" 
					<< ret << ") " << impl::lass_strerror(ret) << std::endl;
			}
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
}

Mutex::Mutex(void )
{
	pimpl_ = new impl::MutexInternal;
}

Mutex::~Mutex(void)
{
	delete pimpl_;
	pimpl_ = 0;
}

void Mutex::lock()
{
	LASS_ASSERT(pimpl_);
	pimpl_->lock();
}

const LockResult Mutex::tryLock()
{
	LASS_ASSERT(pimpl_);
	return pimpl_->tryLock();
}

void Mutex::unlock()
{
	LASS_ASSERT(pimpl_);
	pimpl_->unlock();
}

const bool Mutex::isLocked() const
{
	LASS_ASSERT(pimpl_);
	return pimpl_->lockCount() > 0;
}



namespace impl
{
	class CriticalSectionInternal
	{
	public:
		void lock()
		{
			mutex_.lock();
		}
		const LockResult tryLock()
		{
			return mutex_.tryLock();
		}
		void unlock()
		{
			mutex_.unlock();
		}
		const unsigned lockCount() const 
		{ 
			return mutex_.lockCount(); 
		}	
	private:
		MutexInternal mutex_;
	};
}


CriticalSection::CriticalSection(void )
{
	pimpl_ = new impl::CriticalSectionInternal;
}

CriticalSection::~CriticalSection(void)
{
	delete pimpl_;
	pimpl_ = 0;
}

void CriticalSection::lock()
{
	LASS_ASSERT(pimpl_);
	pimpl_->lock();
}

const LockResult CriticalSection::tryLock()
{
	LASS_ASSERT(pimpl_);
	return pimpl_->tryLock();
}

void CriticalSection::unlock()
{
	LASS_ASSERT(pimpl_);
	pimpl_->unlock();
}

const bool CriticalSection::isLocked() const
{
	LASS_ASSERT(pimpl_);
	return pimpl_->lockCount() > 0;
}



namespace impl
{
	class ConditionInternal
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
			int ret = pthread_mutex_destroy(&mutex_);
			LASS_ASSERT(ret == 0);
			if (ret != 0)
			{
				std::cerr << "[LASS RUN MSG] WARNING: pthread_mutex_destroy failed: (" 
					<< ret << ") " << impl::lass_strerror(ret) << std::endl;
			}
			ret = pthread_cond_destroy(&condition_);
			LASS_ASSERT(ret == 0);
			if (ret != 0)
			{
				std::cerr << "[LASS RUN MSG] WARNING: pthread_cond_destroy failed: (" 
					<< ret << ") " << impl::lass_strerror(ret) << std::endl;
			}
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
		const WaitResult wait(long iSec,long iNSec)
		{
			LASS_ENFORCE_CLIB_RC(pthread_mutex_lock(&mutex_));
			
			struct timespec timeToWaitTo;
			clock_gettime(CLOCK_REALTIME,&timeToWaitTo);
			timeToWaitTo.tv_sec+=iSec;
			timeToWaitTo.tv_nsec+=iNSec;
			
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
}

Condition::Condition(void)
{
	pimpl_ = new impl::ConditionInternal;
}
Condition::~Condition(void)
{
	delete pimpl_;
	pimpl_ = 0;
}
void Condition::wait()
{
	LASS_ASSERT(pimpl_);
	pimpl_->wait();
}
const WaitResult Condition::wait(unsigned long sec,unsigned long nsec)
{
	LASS_ASSERT(pimpl_);
	return pimpl_->wait(sec,nsec);
}
void Condition::signal()
{
	LASS_ASSERT(pimpl_);
	pimpl_->signal();
}
void Condition::broadcast()
{
	LASS_ASSERT(pimpl_);
	pimpl_->broadcast();
}



namespace impl
{
	class ThreadInternal
	{
	public:
	
		ThreadInternal(Thread& iThread, ThreadKind iKind):
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
			runCondition_.wait();
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
			// that should be handled by the calling thread.  I don't really
			// expect that to happen.  But we'll deal with it with a nice
			// message. [Bramz]
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
				std::cerr << "[LASS RUN MSG] nanosleep in Thread::sleep "
				 	<< "interrupted by signal.  Going back to sleep ...\n";
				timeOut.tv_sec = timeRemaining.tv_sec;
				timeOut.tv_nsec = timeRemaining.tv_nsec;
			}
		}
		
		static void yield()
		{
			LASS_ENFORCE_CLIB_RC(pthread_yield());
		}
	
		// thread function
		static void* staticThreadStart(void* iPimpl)
		{
			LASS_ASSERT(iPimpl);
			std::cout << "in staticThreadStart\n";
			ThreadInternal* pimpl = static_cast<ThreadInternal*>(iPimpl);
			pimpl->isCreated_ = true;
			pimpl->runCondition_.signal();
			std::cout << "about to run\n";
			pimpl->thread_.doRun();
			std::cout << "done\n";
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
		bool isJoinable_;
		bool isCreated_;
	};
}



Thread::Thread(ThreadKind iKind)
{
	pimpl_ = new impl::ThreadInternal(*this, iKind);
}

Thread::~Thread()
{
	delete pimpl_;
	pimpl_ = 0;
}

void Thread::run()
{
	LASS_ASSERT(pimpl_);
	pimpl_->run();
}

void Thread::join()
{
	LASS_ASSERT(pimpl_);
	pimpl_->join();
}

void Thread::sleep(unsigned long iMilliSeconds)
{
	impl::ThreadInternal::sleep(iMilliSeconds);
}

void Thread::yield()
{
	impl::ThreadInternal::yield();
}

}
}

#endif

#endif

// EOF
