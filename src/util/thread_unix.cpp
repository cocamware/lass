/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "util_common.h"

#if (LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_UNIX)

#include <pthread.h>
#include <time.h>

#pragma error("No full support for multi threading in UNIX implemented")
namespace lass
{
namespace util
{
class MutexInternal
{
public:
    MutexInternal()
    {
		int ret = pthread_mutex_init(&mutex_,NULL);
        if ( !ret )
        {
            LASS_LOG("Can not create mutex");
        }
    }
    ~MutexInternal() {  pthread_mutex_destroy(&m_mutex); }
public:
    pthread_mutex_t mutex_;
};

Mutex::Mutex(void )
{
    m_internal = new MutexInternal;
    m_locked = 0;
}

Mutex::~Mutex(void)
{
    if ( m_locked > 0 )
        LASS_LOG("warning: freeing a locked mutex with nonzero locks).");
    delete m_internal;
}

MutexError Mutex::lock()
{
    if (!pthread_mutex_lock(&m_internal->m_mutex))
	{
		LASS_LOG("could not lock mutex");
		return MUTEX_MISC_ERROR;
	}
	else
		m_locked++;
    return MUTEX_NO_ERROR;
}

MutexError Mutex::tryLock()
{
    int ret;

    ret = pthread_mutex_trylock(&m_internal->m_mutex);
#pragma LASS_FIXME("also check for other return conditions")
    if (ret != 0)
        return MUTEX_BUSY;

    m_locked++;
    return MUTEX_NO_ERROR;
}

MutexError Mutex::unlock()
{
    if (m_locked > 0)
        m_locked--;
    pthread_mutex_unlock(&m_internal->m_mutex);
    return MUTEX_NO_ERROR;
}


typedef MutexInternal CriticalSectionInternal;

CriticalSection::CriticalSection(void )
{
	internal_ = new CriticalSectionInternal;
}

CriticalSection::~CriticalSection(void)
{
	delete internal_;
}

void CriticalSection::lock()
{    
	if (!pthread_mutex_lock(&m_internal->m_mutex))
		LASS_LOG("could not lock mutex");
}

CriticalSectionError CriticalSection::tryLock()
{
    ret = pthread_mutex_trylock(&m_internal->m_mutex);
#pragma LASS_FIXME("also check for other return conditions")
    if (ret != 0)
        return CRITICALSECTION_TRYLOCK_FAILED;

    return CRITICALSECTION_NO_ERROR;
}

void CriticalSection::unlock()
{
    pthread_mutex_unlock(&m_internal->m_mutex);
}



class ConditionInternal
{
    pthread_cond_t condition_;
	pthread_mutex_t mutex_;
public:
	ConditionInternal()
	{
		if ( !pthread_cond_init(&condition_,NULL) ||
			 !pthread_mutex_init(&mutex_,NULL))
            LASS_LOG("Can not create condition");
    }
	void wait()
	{
		pthread_cond_wait(&condition_,&mutex_);
	}

	bool wait(long iSec,long iNSec)
    {
		struct timespec timeToWaitTo;
		clock_gettime(CLOCK_REALTIME,&timeToWaitTo);
		timeToWaitTo.tv_sec+=iSec;
		timeToWaitTo.tv_nsec+=iNSec;
		int ret=pthread_cond_timedwait(&condition_,&mutex_,&timeToWaitTo);
        return !ret;
    }
	void signal()
    {
		pthread_cond_signal(&condition_);
    }
	void broadcast()
    {
		pthread_cond_broadcast(condtion_);
    }
    virtual  ~ConditionInternal()
    {
		if (!pthread_mutex_destroy(&mutex_) || 
			!pthread_cond_destroy(&condition_))
			LASS_LOG("could not destroy condition")
    }
};


Condition::Condition(void)
{
	m_internal = new ConditionInternal;
}
Condition::~Condition(void)
{
	delete m_internal;
}
void Condition::wait()
{	
	m_internal->wait();
}
bool Condition::wait(unsigned long sec,unsigned long nsec)
{
	return m_internal->wait(sec,nsec);
}
void Condition::signal()
{
	m_internal->signal();
}
void Condition::broadcast()
{
	m_internal->broadcast();
}



// ThreadInternal class
// ----------------------

class ThreadInternal
{
public:
    ThreadInternal()
    {
        thread_ = 0;
        state_ = STATE_NEW;
    }
    ~ThreadInternal()
    {
        free();
    }
    void free()
    {
		pthread_attr_destroy(tattr_);
    }
    // create a new (suspended) thread (for the given thread object)
    bool create(Thread *thread);
    // suspend/resume/terminate
    bool suspend();
    bool resume();
    void cancel() { state_ = STATE_CANCELED; }
    // thread state

    // thread handle and id
    pthread_t getHandle() const { return thread_; }
    DWORD  getId() const { return (DWORD)thread_; }
	ThreadState getState() const { return state_; }

    // thread function
    static DWORD posixThreadStart(Thread *thread);

private:
    pthread_t		thread_;    // handle of the thread
	pthread_attr_t	attr_;		// attributes of the posix thread
    ThreadState		state_;      // state, see ThreadState enum
    unsigned int	priority_;   // thread priority in units
    DWORD			tid_;        // thread id
};

void ThreadInternal::posixThreadStart(Thread *thread)
{
    DWORD rc;
    bool wasCancelled=false;

    rc = reinterpret_cast<DWORD>(thread->entry());
    thread->onExit();
    // if the thread was cancelled (from Delete()), then its handle is still
    // needed there
    if ( thread->isDetached() && !wasCancelled )
    {
        // auto delete
        delete thread;
    }
    //else: the joinable threads handle will be closed when Wait() is done
}

bool ThreadInternal::create(Thread *thread)
{
	pthread_attr_init(&tattr_);
	int ret = int pthread_create(&thread_, &tattr_,void *(*start_routine)(ThreadInternal::posixThreadStart*), thread);

	if ( ret != 0 )
    {
		LASS_LOG("Can not create thread");
        return FALSE;
    }
    return TRUE;
}

bool ThreadInternal::suspend()
{
	LASS_THROW("suspend not supported");
    return FALSE;
}

bool ThreadInternal::resume()
{
	LASS_THROW("resume not supported");
    return FALSE;
}

// ctor and dtor
// -------------

Thread::Thread(ThreadKind kind)
{
    m_internal = new ThreadInternal();
    m_isDetached = kind == THREAD_DETACHED;
}

Thread::~Thread()
{
    delete m_internal;
}

// create/start thread
// -------------------

ThreadError Thread::create()
{
    MutexLocker lock(m_mutex);
    if ( !m_internal->create(this) )
        return THREAD_NO_RESOURCE;
    return THREAD_NO_ERROR;
}

ThreadError Thread::run()
{
    MutexLocker lock(m_mutex);

    if ( m_internal->getState() != STATE_NEW )
    {
        // actually, it may be almost any state at all, not only STATE_RUNNING
        return THREAD_RUNNING;
    }

    // the thread has just been created and is still suspended - let it run
    return resume();
}

// suspend/resume thread
// ---------------------
ThreadError Thread::pause()
{
    MutexLocker lock(m_mutex);
    return m_internal->suspend() ? THREAD_NO_ERROR : THREAD_MISC_ERROR;
}

ThreadError Thread::resume()
{
    MutexLocker lock(m_mutex);
    return m_internal->resume() ? THREAD_NO_ERROR : THREAD_MISC_ERROR;
}

// stopping thread
// ---------------

Thread::ExitCode Thread::wait()
{
    // although under Windows we can wait for any thread, it's an error to
    // wait for a detached one in Win API
	if (isDetached())
	{	
		LASS_LOG("Can not wait for detached thread");
		return (ExitCode)-1;
	}
	else
	{
		pthread_join(m_internal->thread_,NULL);
	    m_internal->free();
	}
    ExitCode rc = (ExitCode)-1;
    return rc;
}

ThreadError Thread::kill()
{
    if ( !isRunning() )
        return THREAD_NOT_RUNNING;


    if ( pthread_cancel(m_internal->thread_) )
    {
        LASS_LOG("Could not terminate thread");
        return THREAD_MISC_ERROR;
    }

    m_internal->free();
    if ( isDetached() )
    {
        delete this;
    }
    return THREAD_NO_ERROR;
}

void Thread::exit(ExitCode status)
{
    m_internal->free();

    if ( isDetached() )
    {
        delete this;
    }
    LASS_LOG("Could not return from ExitThread()!");
}

unsigned long Thread::getId() const
{
    MutexLocker lock((Mutex &)m_mutex);
    return (unsigned long)m_internal->getId();
}

bool Thread::isRunning() const
{
    MutexLocker lock((Mutex &)m_mutex);
    return m_internal->getState() == STATE_RUNNING;
}

bool Thread::isAlive() const
{
    MutexLocker lock((Mutex &)m_mutex);
    return (m_internal->getState() == STATE_RUNNING) ||
           (m_internal->getState() == STATE_PAUSED);
}

bool Thread::isPaused() const
{
    MutexLocker lock((Mutex &)m_mutex);
    return m_internal->getState() == STATE_PAUSED;
}

}
}


#endif
