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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_THREAD_WIN32_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_THREAD_WIN32_INL

#include "../util_common.h"
#include "../thread.h"
#include "../singleton.h"
#include "../environment.h"
#include "../bit_manip.h"
#include "lass_errno.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <process.h>
#include <windows.h>

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4239) //  nonstandard extension used : 'argument' : conversion from 'T' to 'T&'
#	pragma warning(disable: 4267) // conversion from 'size_t' to 'unsigned int'
#	pragma warning(disable: 4996) // This function or variable may be unsafe ...
#endif

namespace lass
{
namespace util
{
namespace impl
{

TCpuSet availableProcessors()
{
	DWORD_PTR processAffinityMask, systemAffinityMask;
	LASS_ENFORCE_WINAPI(GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask));

	// determine number of processors (highest set bit of systemAffinityMask)
	size_t n = 0;
	while (systemAffinityMask)
	{
		++n;
		systemAffinityMask >>= 1;
	}

	// determine what processors are available to this process
	TCpuSet cpuSet(n, false);
	for (size_t i = 0; i < n; ++i)
	{
		cpuSet[i] = util::checkBit(processAffinityMask, i);
	}

	return cpuSet;
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
		mutex_ = LASS_ENFORCE_WINAPI(::CreateMutex(NULL, FALSE, NULL));

	}
	~MutexInternal() 
	{
		LASS_ASSERT(lockCount_ == 0);
		const BOOL ret = CloseHandle(mutex_);
		LASS_ASSERT(ret != 0);
		if (ret == 0)
		{
			const unsigned lastError = impl::lass_GetLastError();
			std::cerr << "[LASS RUN MSG] WARNING:  CloseHandle failed in ~MutexInternal(): ("
				<< lastError << ") " << impl::lass_FormatMessage(lastError) << std::endl;
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
				{
					const unsigned lastError = impl::lass_GetLastError();
					LASS_THROW("WaitForSingleObject failed: (" << lastError << ") "
						<< impl::lass_FormatMessage(lastError));
				}
			case WAIT_TIMEOUT:
			default:
				LASS_THROW("impossible return value of WaitForSingleObject: " << ret);
		}
		++lockCount_;
	}
	LockResult tryLock()
	{
		const DWORD ret = WaitForSingleObject(mutex_, 0);
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
				{
					const unsigned lastError = impl::lass_GetLastError();
					LASS_THROW("WaitForSingleObject failed: (" << lastError << ") "
						<< impl::lass_FormatMessage(lastError));
				}
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
		LASS_ENFORCE_WINAPI(ReleaseMutex(mutex_));
	}
	unsigned lockCount() const 
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
class CriticalSectionInternal: NonCopyable
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
	LockResult tryLock()
	{
		return TryEnterCriticalSection(&criticalSection_) ? lockSuccess : lockBusy;
	}
	void unlock()
	{
		LASS_ASSERT(criticalSection_.RecursionCount > 0);
		LeaveCriticalSection(&criticalSection_);
	}
	unsigned lockCount() const 
	{ 
		return criticalSection_.RecursionCount; 
	}	
private:
	CRITICAL_SECTION criticalSection_;
};



/** @internal
 *  @ingroup Threading
 */
class ConditionInternal: NonCopyable
{
public:
	ConditionInternal():
		threadsWaiting_(0)
	{
		event_ = LASS_ENFORCE_WINAPI(::CreateEvent(
			NULL,   // default secutiry
			FALSE,  // not manual reset
			FALSE,  // nonsignaled initially
			NULL    // nameless event
		));
	}
	~ConditionInternal()
	{
		const BOOL ret = CloseHandle(event_);
		LASS_ASSERT(ret != 0);
		if (ret == 0)
		{
			const unsigned lastError = impl::lass_GetLastError();
			std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
				<< " CloseHandle failed in ~MutexInternal(): (" 
				<< lastError << ") " << impl::lass_FormatMessage(lastError) << std::endl;
		}
	}		
	void wait()
	{
		wait(INFINITE);
	}	
	WaitResult wait(unsigned long iMilliSeconds)
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
				{
					const unsigned lastError = impl::lass_GetLastError();
					LASS_THROW("WaitForSingleObject failed: (" << lastError << ") "
						<< impl::lass_FormatMessage(lastError));
				}
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
		LASS_ENFORCE_WINAPI(::SetEvent(event_));
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
class ThreadLocalStorageInternal: NonCopyable
{
public:
	typedef void (*TDestructor)(void*);

	ThreadLocalStorageInternal(void (*destructor)(void*))
	{
		index_ = TlsAlloc();
		if (index_ == TLS_OUT_OF_INDEXES)
		{
			const unsigned lastError = impl::lass_GetLastError();
			LASS_THROW("Failed to allocate thread local storage: TlsAlloc failed: ("
				<< lastError << ") " << impl::lass_FormatMessage(lastError));
		}
		if (TDestructors* destrs = destructors())
		{
			try
			{
				(*destrs)[index_] = destructor;
			}
			catch (...)
			{
				freeSlot(index_);
				throw;
			}
		}
		else
		{
			freeSlot(index_);
			LASS_THROW("Failed to allocate thread local storage: dead destructor singleton");
		}
	}
	~ThreadLocalStorageInternal()
	{
		if (TDestructors* destrs = destructors())
		{
			destrs->erase(index_);
		}
		freeSlot(index_);
	}
	void* get() const
	{
		void* result = TlsGetValue(index_); // intel700 doesn't like void* const here [Bramz]
		if (result == 0)
		{
			// getting the value must be pretty fast at times, so only do this enforcer if 
			// result indicates that something _may_ be wrong [Bramz]
			//
			return LASS_ENFORCE_WINAPI(result)("Failed to get thread local storage value");
		}
		return result;
	}
	void set(void* value)
	{
		LASS_ENFORCE_WINAPI(TlsSetValue(index_, value))("Failed to set thread local storage value");
	}

	/** destruct the local copies of the variables in a thread.
	 *  to be called at end of thread's life time ...
	 */
	static void destructLocals()
	{
		if (TDestructors* destrs = destructors())
		{
			for (TDestructors::iterator i = destrs->begin(); i != destrs->end(); ++i)
			{
				DWORD index = i->first;
				TDestructor destructor = i->second;
				if (destructor)
				{
					if (void* p = TlsGetValue(index))
					{
						destructor(p);
						TlsSetValue(index, 0);
					}
				}
			}
		}
	}

private:

	typedef std::map<DWORD, TDestructor> TDestructors;

	void freeSlot(DWORD index)
	{
		if (!TlsFree(index))
		{
			const unsigned lastError = impl::lass_GetLastError();
			std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: TlsFree failed: ("
				<< lastError << ") " << impl::lass_FormatMessage(lastError) << std::endl;
		}
	}
	
	static TDestructors* destructors()
	{
		return Singleton<TDestructors, destructionPriorityInternalTlsDestructors>::instance();
	}
	
	static TDestructors* forceIntoExistance;
	
	DWORD index_;
};

// MainLocalStorageDestroyer is going to need this singleton at exit-time. Make sure it exists [Bramz]
ThreadLocalStorageInternal::TDestructors* ThreadLocalStorageInternal::forceIntoExistance = 
	ThreadLocalStorageInternal::destructors();



/** @internal
 *	@ingroup Threading
 *  Abuse singleton mechanism to destroy the local thread storage instances of the main thread
 *  before the destructor map (which is a singleton as well) goes out of business.  We do that
 *	by creating a helper singleton that has higher destruction priority
 */
class MainLocalStorageDestroyer: NonCopyable
{
public:
	~MainLocalStorageDestroyer()
	{
		ThreadLocalStorageInternal::destructLocals();
	}
private:
	static MainLocalStorageDestroyer* forceIntoExistance;
};

MainLocalStorageDestroyer* MainLocalStorageDestroyer::forceIntoExistance =
	Singleton<MainLocalStorageDestroyer, destructionPriorityInternalTlsLocalsMain>::instance();



/** @internal
 *  @ingroup Threading
 */
DWORD_PTR bindThread(HANDLE thread, size_t processor)
{
	DWORD_PTR affinityMask = 0;
	if (processor == Thread::anyProcessor)
	{
		DWORD_PTR processAffinityMask, systemAffinityMask;
		LASS_ENFORCE_WINAPI(GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask));
		affinityMask = systemAffinityMask;
	}
	else
	{
		util::setBit(affinityMask, processor);
	}
	LASS_ENFORCE_WINAPI(SetThreadAffinityMask(thread, affinityMask))
		("Failed to bind thread to processor ")(processor);
	// do it again, to confirm the set affinity
	const DWORD_PTR threadAffinity = LASS_ENFORCE_WINAPI(SetThreadAffinityMask(thread, affinityMask));
	LASS_ENFORCE(threadAffinity == affinityMask);
	return threadAffinity;
}

void setThreadName(DWORD threadId, const char* threadName)
{
	char buffer[10];
	strncpy(buffer, threadName, 10);
	buffer[9] = '\0';

	struct THREADNAME_INFO
	{
		DWORD dwType; // must be 0x1000
		LPCSTR szName; // pointer to name (in user addr space)
		DWORD dwThreadID; // thread ID (-1=caller thread)
		DWORD dwFlags; // reserved for future use, must be zero
	};

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = threadId;
	info.dwFlags = 0;

	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}



#define LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(exception_type)\
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

	ThreadInternal(Thread& iThread, ThreadKind iKind, const char* name):
		thread_(iThread),
		name_(name),
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
		handle_ = (HANDLE) _beginthreadex(NULL, 0, &ThreadInternal::startThread, this, 0, &id_);
		if (handle_ == 0)
		{
			const int errnum = lass_errno();
			LASS_THROW("_beginthreadex failed: (" << errnum << ") " << lass_strerror(errnum));
		}
		if (name_)
		{
			setThreadName(id_, name_);
		}
		runCondition_.wait();
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
		const DWORD ret = WaitForSingleObject(handle_, INFINITE);
		switch ( ret )
		{
			case WAIT_OBJECT_0:
				// ok
				break;
			case WAIT_FAILED:
				{
					const unsigned lastError = impl::lass_GetLastError();
					LASS_THROW("WaitForSingleObject failed: (" << lastError << ") "
						<< impl::lass_FormatMessage(lastError));
				}
			default:
				LASS_THROW("impossible return value of WaitForSingleObject: " << ret);
		}
		isJoinable_ = false;
		if (error_.get())
		{
			error_->throwSelf();
		}
	}

	void bind(size_t processor)
	{
		affinity_ = bindThread(handle_, processor);
	}

	const TCpuSet affinity() const
	{
		const size_t n = numberOfProcessors();
		TCpuSet result(numberOfProcessors(), false);
		for (size_t i = 0; i < n; ++i)
		{
			result[i] = util::checkBit(affinity_, i);
		}
		return result;
	};
	
	static void sleep(unsigned long iMilliSeconds)
	{
		Sleep(iMilliSeconds);
	}
	
	static void yield()
	{
		Sleep(0);
	}

	static void bindCurrent(size_t processor)
	{
		bindThread(GetCurrentThread(), processor);
	}

	// thread function
	static unsigned __stdcall startThread(void* iPimpl)
	{
		LASS_ASSERT(iPimpl);
		ThreadInternal* pimpl = static_cast<ThreadInternal*>(iPimpl);
		DWORD_PTR dummy;
		GetProcessAffinityMask(GetCurrentProcess(), &pimpl->affinity_, &dummy);
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
			LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(::std::domain_error)
			LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(::std::invalid_argument)
			LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(::std::length_error)
			LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(::std::out_of_range)
			LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(::std::range_error)
			LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(::std::overflow_error)
			LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(::std::underflow_error)
			LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(::std::runtime_error)
			LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(::std::logic_error)
			LASS_UTIL_THREAD_WIN32_CATCH_AND_WRAP(::std::exception)
		}
		else
		{
			pimpl->runCondition_.signal();
			pimpl->thread_.doRun();
			delete &pimpl->thread_;
		}
		return 0;
	}

	static void onThreadDetach()
	{
		ThreadLocalStorageInternal::destructLocals();
	}

private:

	Thread& thread_;
	HANDLE handle_;	 // handle of the thread
	DWORD_PTR affinity_;
	std::auto_ptr<RemoteExceptionBase> error_;
	Condition runCondition_;
	const char* name_;
	unsigned id_;
	volatile bool isJoinable_;
	volatile bool isCreated_;
};


}
}
}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#ifdef LASS_BUILD_DLL

#include "../dll/dll_main.h"

namespace lass
{
namespace util
{
namespace impl
{

BOOL threadDllMain(HINSTANCE, DWORD dwReason, LPVOID)
{
	switch (dwReason)
	{
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		lass::util::impl::ThreadInternal::onThreadDetach();
		break;
	}
	return TRUE;
}
}
}
}

LASS_EXECUTE_BEFORE_MAIN_EX(
	lassUtilImplThreadWin32RegisterDllMain,
	::lass::dll::registerDllMain(::lass::util::impl::threadDllMain);
)

#else

namespace
{

void NTAPI lassOnThreadCallback(PVOID, DWORD dwReason, PVOID)
{
	if(dwReason == DLL_THREAD_DETACH)
	{
		lass::util::impl::ThreadInternal::onThreadDetach();
	}
}

#if _MSC_VER >= 1400 // VC8.0
#	pragma section(".CRT$XLB", read)
#else
#	pragma section(".CRT$XLB", read, write)
#endif

extern "C" __declspec(allocate(".CRT$XLB")) PIMAGE_TLS_CALLBACK lassThreadCallback = lassOnThreadCallback;

}

#ifdef _WIN64
#	pragma comment(linker, "/INCLUDE:_tls_used")
#else
#	pragma comment(linker, "/INCLUDE:__tls_used")
#endif

#endif



#endif
 
// EOF
 
