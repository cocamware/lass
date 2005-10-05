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

#ifdef LASS_THREAD_WIN32

#include "thread.h"
#include <process.h> // windows related stuff

namespace lass
{
namespace util
{

class MutexInternal
{
public:
	MutexInternal()
	{
		m_mutex = ::CreateMutex(NULL, FALSE, NULL);
		if ( !m_mutex )
		{
			LASS_LOG("Can not create mutex");
		}
	}
	~MutexInternal() { if ( m_mutex ) CloseHandle(m_mutex); }
public:
	HANDLE m_mutex;
};

Mutex::Mutex(void )
{
	m_internal = new MutexInternal;
	m_locked = 0;
}

Mutex::~Mutex(void)
{
	if ( m_locked > 0 )
		LASS_LOG("Warning: freeing a locked mutex with nonzero locks).");
	delete m_internal;
}

MutexError Mutex::lock()
{
	DWORD ret;

	ret = WaitForSingleObject(m_internal->m_mutex, INFINITE);
	switch ( ret )
	{
		case WAIT_ABANDONED:
			return MUTEX_BUSY;
		case WAIT_OBJECT_0:
			// ok
			break;
		case WAIT_FAILED:
			LASS_LOG("Couldn't acquire a mutex lock");
			return MUTEX_MISC_ERROR;
		case WAIT_TIMEOUT:
		default:
			LASS_LOG("impossible return value in mutex::lock");
	}

	m_locked++;
	return MUTEX_NO_ERROR;
}

MutexError Mutex::tryLock()
{
	DWORD ret;

	ret = WaitForSingleObject(m_internal->m_mutex, 0);
	if (ret == WAIT_TIMEOUT || ret == WAIT_ABANDONED)
		return MUTEX_BUSY;

	m_locked++;
	return MUTEX_NO_ERROR;
}

MutexError Mutex::unlock()
{
	if (m_locked > 0)
		m_locked--;

	BOOL ret = ReleaseMutex(m_internal->m_mutex);
	if ( ret == 0 )
	{
		LASS_LOG("Couldn't release a mutex");
		return MUTEX_MISC_ERROR;
	}

	return MUTEX_NO_ERROR;
}


class CriticalSectionInternal
{
public:
	CriticalSectionInternal()
	{
		InitializeCriticalSection(&criticalSection_);
	}
	~CriticalSectionInternal()
	{
		if ( criticalSection_.LockCount > 0 )
			LASS_LOG("Warning: freeing a locked critical section with nonzero locks).");
		DeleteCriticalSection(&criticalSection_);
	}
public:
	CRITICAL_SECTION criticalSection_;
};




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
	EnterCriticalSection(&internal_->criticalSection_);
}

CriticalSectionError CriticalSection::tryLock()
{
	if (TryEnterCriticalSection(&internal_->criticalSection_))
		return CRITICALSECTION_NO_ERROR;
	return CRITICALSECTION_TRYLOCK_FAILED;
}

void CriticalSection::unlock()
{
	LeaveCriticalSection(&internal_->criticalSection_);
}

class ConditionInternal
{
	// the Win32 synchronization object corresponding to this event
	HANDLE m_hEvent;
	// number of threads waiting for this condition
	LONG m_nWaiters;
public:
	ConditionInternal()
	{
		m_hEvent = ::CreateEvent(
								 NULL,   // default secutiry
								 FALSE,  // not manual reset
								 FALSE,  // nonsignaled initially
								 NULL    // nameless event
								);
		if ( !m_hEvent )
			LASS_LOG("Can not create event object.");
		// nobody waits for us yet
		m_nWaiters = 0;
	}
	bool wait(DWORD timeout)
	{
		// as m_nWaiters variable is accessed from multiple waiting threads
		// (and possibly from the broadcasting thread), we need to change its
		// value atomically
		::InterlockedIncrement(&m_nWaiters);
		// FIXME this should be MsgWaitForMultipleObjects() as we want to keep
		//       processing Windows messages while waiting (or don't we?)
		DWORD rc = ::WaitForSingleObject(m_hEvent, timeout);
		::InterlockedDecrement(&m_nWaiters);
		return rc != WAIT_TIMEOUT;
	}
	void signal()
	{
		// set the event to signaled: if a thread is already waiting on it, it
		// will be woken up, otherwise the event will remain in the signaled
		// state until someone waits on it. In any case, the system will return
		// it to a non signalled state afterwards. If multiple threads are
		// waiting, only one will be woken up.
		if ( !::SetEvent(m_hEvent) )
		{
			LASS_LOG("SetEvent failed");
		}
	}
	void broadcast()
	{
		// we need to save the original value as m_nWaiters is goign to be
		// decreased by the signalled thread resulting in the loop being
		// executed less times than needed
		LONG nWaiters = m_nWaiters;

		// this works because all these threads are already waiting and so each
		// SetEvent() inside Signal() is really a PulseEvent() because the
		// event state is immediately returned to non-signaled
		for ( LONG n = 0; n < nWaiters; n++ )
			signal();
	}
	virtual  ~ConditionInternal()
	{
		if ( m_hEvent )
		{
			if ( !::CloseHandle(m_hEvent) )
			{
				LASS_LOG("CloseHandle(event) failed");
			}
		}
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
	(void)m_internal->wait(INFINITE);
}
bool Condition::wait(unsigned long sec,unsigned long nsec)
{
	return m_internal->wait(sec*1000 + nsec/1000000);
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
		m_hThread = 0;
		m_state = STATE_NEW;
	}
	~ThreadInternal()
	{
		free();
	}
	void free()
	{
		if ( m_hThread )
		{
			if ( !::CloseHandle(m_hThread) )
				LASS_LOG("CloseHandle(thread) failed");
			m_hThread = 0;
		}
	}
	// create a new (suspended) thread (for the given thread object)
	bool create(Thread *thread);
	// suspend/resume/terminate
	bool suspend();
	bool resume();
	void cancel() { m_state = STATE_CANCELED; }
	// thread state

	// thread handle and id
	HANDLE getHandle() const { return m_hThread; }
	DWORD  getId() const { return m_tid; }
	ThreadState getState() const { return m_state; }

	// thread function
	static Thread::ExitCode winThreadStart(Thread *thread);

private:
	HANDLE          m_hThread;    // handle of the thread
	ThreadState     m_state;      // state, see ThreadState enum
	unsigned int    m_priority;   // thread priority in units
	DWORD           m_tid;        // thread id
};

Thread::ExitCode ThreadInternal::winThreadStart(Thread *thread)
{
	bool wasCancelled=false;

	Thread::ExitCode rc = thread->entry();
	thread->onExit();
	// if the thread was cancelled (from Delete()), then its handle is still
	// needed there
	if ( thread->isDetached() && !wasCancelled )
	{
		// auto delete
		delete thread;
	}
	//else: the joinable threads handle will be closed when Wait() is done
	return rc;
}

bool ThreadInternal::create(Thread *thread)
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0,
									   (unsigned int(__stdcall*)(void*))ThreadInternal::winThreadStart,
									   thread, CREATE_SUSPENDED,
									   (unsigned int *)&m_tid);

	if ( m_hThread == NULL )
	{
		LASS_LOG("Can not create thread");
		return FALSE;
	}
	return TRUE;
}

bool ThreadInternal::suspend()
{
	DWORD nSuspendCount = ::SuspendThread(m_hThread);
	if ( nSuspendCount == (DWORD)-1 )
	{
		LASS_LOG("Can not suspend thread %x");
		return FALSE;
	}
	m_state = STATE_PAUSED;
	return TRUE;
}

bool ThreadInternal::resume()
{
	DWORD nSuspendCount = ::ResumeThread(m_hThread);
	if ( nSuspendCount == (DWORD)-1 )
	{
		LASS_LOG("Can not resume thread ");
		return FALSE;
	}

	// don't change the state from STATE_EXITED because it's special and means
	// we are going to terminate without running any user code - if we did it,
	// the codei n Delete() wouldn't work
	if ( m_state != STATE_EXITED )
	{
		m_state = STATE_RUNNING;
	}

	return TRUE;
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
		WaitForSingleObject(m_internal->getHandle(),INFINITE);
		m_internal->free();
	}
	ExitCode rc = (ExitCode)-1;
	return rc;
}

ThreadError Thread::kill()
{
	if ( !isRunning() )
		return THREAD_NOT_RUNNING;

	if ( !::TerminateThread(m_internal->getHandle(), (DWORD)-1) )
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

	_endthreadex((unsigned)status);
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
