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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_THREAD_WIN32_INL
#pragma LASS_NOTE("util::Threading: using win32 implementation")

#include "../util_common.h"
#include "../thread.h"
#include "lass_errno.h"
#include <process.h>
#include <windows.h>

namespace lass
{
namespace util
{
namespace impl
{

/** @internal
 *  @ingroup Threading
 */
class MutexInternal
{
public:
	MutexInternal():
		lockCount_(0)
	{
		LASS_ENFORCE_HANDLE(mutex_ = ::CreateMutex(NULL, FALSE, NULL));

	}
	~MutexInternal() 
	{
		LASS_ASSERT(lockCount_ == 0);
		const BOOL ret = CloseHandle(mutex_);
		LASS_ASSERT(ret != 0);
		if (ret == 0)
		{
			std::cerr << "[LASS RUN MSG] WARNING:  CloseHandle failed in ~MutexInternal(): "
				<< GetLastError << std::endl;
		}
	}
	void lock()
	{
		const DWORD ret = WaitForSingleObject(mutex_, INFINITE);
		switch ( ret )
		{
			case WAIT_ABANDONED:
				// if you get here, you have a design flaw.  But we'll try to continue.
				LASS_ASSERT(false);
				std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
					<< "WaitForSingleObject returned with WAIT_ABANDONED. "
					<< "Going to take ownership and continue happily." << std::endl;
				break;
			case WAIT_OBJECT_0:
				// ok
				break;
			case WAIT_FAILED:
				LASS_THROW("WaitForSingleObject failed: " << GetLastError());
			case WAIT_TIMEOUT:
			default:
				LASS_THROW("impossible return value of WaitForSingleObject: " << ret);
		}
		++lockCount_;
	}
	const LockResult tryLock()
	{
		const DWORD ret = WaitForSingleObject(mutex_, INFINITE);
		switch ( ret )
		{
			case WAIT_ABANDONED:
				// if you get here, you have a design flaw.  But we'll try to continue.
				LASS_ASSERT(false);
				std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
					<< "WaitForSingleObject returned with WAIT_ABANDONED. "
					<< "Going to take ownership and continue happily." << std::endl;
				break;
			case WAIT_OBJECT_0:
				// ok
				break;
			case WAIT_FAILED:
				LASS_THROW("WaitForSingleObject failed: " << GetLastError());
			case WAIT_TIMEOUT:
				return lockBusy;
			default:
				LASS_THROW("impossible return value of WaitForSingleObject: " << ret);
		}
		++lockCount_;
		return lockSuccess;
	}
	void unlock()
	{
		LASS_ASSERT(lockCount_ > 0);
		if (lockCount_ == 0)
		{
			LASS_THROW("attempting to unlock an unlocked mutex");
		}
		--lockCount_;
		const BOOL ret = ReleaseMutex(mutex_);
		if (ret == 0)
		{
			LASS_THROW("ReleaseMutex failed: " << GetLastError());
		}
	}
	const unsigned lockCount() const 
	{
		return lockCount_;
	}
private:
	HANDLE mutex_;
	unsigned lockCount_;
};



/** @internal
 *  @ingroup Threading
 */
class CriticalSectionInternal
{
public:
	CriticalSectionInternal()
	{
		InitializeCriticalSection(&criticalSection_);
	}
	~CriticalSectionInternal()
	{
		LASS_ASSERT(criticalSection_.LockCount == -1);
		DeleteCriticalSection(&criticalSection_);
	}
	void lock()
	{
		EnterCriticalSection(&criticalSection_);
	}
	const LockResult tryLock()
	{
		return TryEnterCriticalSection(&criticalSection_) ? lockSuccess : lockBusy;
	}
	void unlock()
	{
		LASS_ASSERT(criticalSection_.RecursionCount > 0);
		LeaveCriticalSection(&criticalSection_);
	}
	const unsigned lockCount() const 
	{ 
		return criticalSection_.RecursionCount; 
	}	
private:
	CRITICAL_SECTION criticalSection_;
};



/** @internal
 *  @ingroup Threading
 */
class ConditionInternal
{
public:
	ConditionInternal():
		threadsWaiting_(0)
	{
		event_ = ::CreateEvent(
			NULL,   // default secutiry
			FALSE,  // not manual reset
			FALSE,  // nonsignaled initially
			NULL    // nameless event
		);
		LASS_ENFORCE_HANDLE(event_);
	}
	~ConditionInternal()
	{
		const BOOL ret = CloseHandle(event_);
		LASS_ASSERT(ret != 0);
		if (ret == 0)
		{
			std::cerr << "[LASS RUN MSG] WARNING:  CloseHandle failed in ~MutexInternal(): "
				<< GetLastError << std::endl;
		}
	}		
	void wait()
	{
		wait(INFINITE);
	}	
	const WaitResult wait(unsigned long iMilliSeconds)
	{
		// as m_nWaiters variable is accessed from multiple waiting threads
		// (and possibly from the broadcasting thread), we need to change its
		// value atomically
		::InterlockedIncrement(&threadsWaiting_);
		// FIXME this should be MsgWaitForMultipleObjects() as we want to keep
		//       processing Windows messages while waiting (or don't we?)
		const DWORD ret = WaitForSingleObject(event_, iMilliSeconds);
		::InterlockedDecrement(&threadsWaiting_);
		
		switch ( ret )
		{
			case WAIT_OBJECT_0:
				return waitSuccess;
			case WAIT_TIMEOUT:
				return waitTimeout;
			case WAIT_FAILED:
				LASS_THROW("WaitForSingleObject failed: " << GetLastError());
			default:
				LASS_THROW("impossible return value of WaitForSingleObject: " << ret);
		}
	}
	void signal()
	{
		// set the event to signaled: if a thread is already waiting on it, it
		// will be woken up, otherwise the event will remain in the signaled
		// state until someone waits on it. In any case, the system will return
		// it to a non signalled state afterwards. If multiple threads are
		// waiting, only one will be woken up.
		const BOOL ret = ::SetEvent(event_);
		if (ret == 0)
		{
			LASS_THROW("SetEvent failed: " << GetLastError());
		}
	}
	void broadcast()
	{
		// we need to save the original value as m_nWaiters is goign to be
		// decreased by the signalled thread resulting in the loop being
		// executed less times than needed
		LONG nWaiters = threadsWaiting_;

		// this works because all these threads are already waiting and so each
		// SetEvent() inside Signal() is really a PulseEvent() because the
		// event state is immediately returned to non-signaled
		for ( LONG n = 0; n < nWaiters; ++n )
			signal();
	}
private:
	HANDLE event_;
	LONG threadsWaiting_;
};



/** @internal
 *  @ingroup Threading
 */
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
		handle_ = (HANDLE) _beginthreadex(NULL, 0, &ThreadInternal::staticThreadStart, this, 0, &id_);
		if (handle_ == 0)
		{
			const int errnum = lass_errno();
			LASS_THROW("_beginthreadex failed: (" << errnum << ") " << lass_strerror(errnum));
		}
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
			const DWORD ret = WaitForSingleObject(handle_, INFINITE);
			switch ( ret )
			{
				case WAIT_OBJECT_0:
					// ok
					break;
				case WAIT_FAILED:
					LASS_THROW("WaitForSingleObject failed: " << GetLastError());
				default:
					LASS_THROW("impossible return value of WaitForSingleObject: " << ret);
			}
			isJoinable_ = false;
		}
	}
	
	static void sleep(unsigned long iMilliSeconds)
	{
		Sleep(iMilliSeconds);
	}
	
	static void yield()
	{
		Sleep(0);
	}

	// thread function
	static unsigned __stdcall staticThreadStart(void* iPimpl)
	{
		LASS_ASSERT(iPimpl);
		ThreadInternal* pimpl = static_cast<ThreadInternal*>(iPimpl);
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
	HANDLE handle_;	 // handle of the thread
	unsigned id_;
	Condition runCondition_;
	bool isJoinable_;
	bool isCreated_;
};

}
}
}

#endif
 
// EOF
 