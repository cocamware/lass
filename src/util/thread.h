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
 *
 *	@par Extra credits and license info:
 *		Most of this code (Mutex, Event and Thread) is code from wxWidgets (formerly
 *		known as wxWindows) adapted to the LASS style guide.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_H

#include "util_common.h"
#include "non_copyable.h"
#include <windows.h>


namespace lass
{
namespace util
{

/**
*	@defgroup	Threading Threading
*	Threading functions.  The Mutex and Thread classes are almost literal copy and pastes
*	of the functions of wxWindows2.4.2.  The implementation has changed a little bit to
*	use more of the LASS utility functions.  The most noticible difference is in the thread
*	class where the Delete is not implemented, due to too large dependencies.  The Critical
*	Section is an own implementation.
*
*	A little word on synchronization: in windows there are quite a few synchronisation
*	primitives and synchronized objects.  Only the most common are implemented and wrapped
*	in the LASS environment:
*		Mutex : system-wide synchronisation object between processes/threads.  Mutexes set in one process can be
*				unlocked in another one.  That is the theory.  In practice with the LASS
*				wrapping, there is no possibility to reference these mutexes in other
*				processes, named mutexes are not supported.  Mutex are rather heavyweight
*				structures.  Cannot be used for intrathread synchronisation AFAIK [TDM]
*		CriticalSection : process-wide synchronisation object between threads.  Lightweight
*				construction.  Cannot be used for intra-thread synchronisation.
*		Condition: this corresponds with MS Windows Events, can be used for interprocess/
*				interthread and intrathread synchronisation.  
*/


/** MutexError.
*	@ingroup Threading
*	@see	 Mutex,MutexLocker
*/
enum MutexError
{
    MUTEX_NO_ERROR = 0,
    MUTEX_DEAD_LOCK,      /**< Mutex has been already locked by THE CALLING thread */
    MUTEX_BUSY,           /**< Mutex has been already locked by ONE thread */
    MUTEX_UNLOCKED,
    MUTEX_MISC_ERROR
};


/** CriticalSectionError.
*	@ingroup Threading
*	@see	 CriticalSection,CriticalSectionLocker
*/
enum CriticalSectionError
{
    CRITICALSECTION_NO_ERROR = 0,		 /**< No error or the lock succeeded */
    CRITICALSECTION_TRYLOCK_FAILED,      /**< Try lock failed */
};


/** ThreadError.
*	@ingroup Threading
*	@see	 Thread,ThreadKind
*/
enum ThreadError
{
    THREAD_NO_ERROR = 0,      /**< No error */
    THREAD_NO_RESOURCE,       /**< No resource left to create a new thread */
    THREAD_RUNNING,           /**< The thread is already running */
    THREAD_NOT_RUNNING,       /**< The thread isn't running */
    THREAD_KILLED,            /**< Thread we waited for had to be killed */
    THREAD_MISC_ERROR         /**< Some other error */
};

/** ThreadKind.
*	@ingroup Threading
*	@see Thread, ThreadError
*/
enum ThreadKind
{
    THREAD_DETACHED,			/**< detached thread */
    THREAD_JOINABLE				/**< joinable thread, can be waited for */
};

class ConditionInternal;

/** Condition.
*	@ingroup Threading
*	A condition can be used to synchronize using messages were a condition waits for
*	a signal or broadcast.  A signal will only release one waiter, a broadcast will release
*	all current waiting wait-conditions.
*/
class Condition
{
	ConditionInternal * m_internal;
public:
	Condition(void);
	virtual ~Condition(void);
	void wait();
	bool wait(unsigned long sec,unsigned long nsec);
	void signal();
	void broadcast();
};


class MutexInternal;
class MutexLocker;

/** Mutex.
*	@ingroup Threading
*	@see MutexLocker
*	A mutex is a systemwide synchronization object. It is heavier in use than a critical
*	section.  For exception-safe basic use, the MutexLocker can be used.
*/
class Mutex : NonCopyable
{
public:
    // constructor & destructor
    Mutex(void);
    ~Mutex(void);
    // Lock the mutex.
    MutexError lock();
    // Try to lock the mutex: if it can't, returns immediately with an error.
    MutexError tryLock();
    // Unlock the mutex.
    MutexError unlock();
    // Returns true if the mutex is locked.
    bool isLocked() const { return (m_locked > 0); }

protected:
    friend class Condition;
	friend class MutexLocker;

	int m_locked;
    MutexInternal *m_internal;
};


/** MutexLocker.
*	@ingroup Threading
*	@see Mutex
*	Auto-locker-release of mutexes.
*/
class MutexLocker: NonCopyable
{
	Mutex&	m_mutex;
public:
	MutexLocker(  Mutex& iMutex );
	operator bool() const { return false; }
	virtual ~MutexLocker();
};

/** CriticalSection.
*	@ingroup Threading
*	@see CriticalSectionLocker
*	@author Tom De Muer [TDM]
*	@date 2004
*	Process wide synchronization object.  The recommended object for interthread synchronisation
*	within a process in Windows.
*/
class CriticalSection : NonCopyable
{
public:
    // constructor & destructor
    CriticalSection(void);
    ~CriticalSection(void);
    // Lock the critical section.
    void lock();
    // Try to lock the critical section: if it can't, returns immediately with an error.
    CriticalSectionError tryLock();
    // Unlock the mutex.
    void unlock();

protected:
    CRITICAL_SECTION m_internal;
};


/** CriticalSectionLocker.
*	@ingroup Threading
*	@see CriticalSection
*	Auto-locker-release of critical sections
*/
class CriticalSectionLocker: NonCopyable
{
	CriticalSection&	m_criticalsection;
public:
	CriticalSectionLocker(  CriticalSection& iMutex );
	operator bool() const { return false; }
	virtual ~CriticalSectionLocker();
};


class ThreadInternal;
/** Thread.
*	@ingroup Threading
*	A base class for threads.  The virtual function entry() needs to be overriden and
*	after creation of the thread on the heap and the call of the create() function, the
*	thread is in a state ready to be run.  The run() function actually starts the thread.
*	JOINABLE threads can be waited for, DETACHED threads can not be waited for.
*/
class Thread
{
public:
    // the return type for the thread function
    typedef void *ExitCode;

    // static functions
        // Returns the wxThread object for the calling thread. NULL is returned
        // if the caller is the main thread (but it's recommended to use
        // IsMain() and only call This() for threads other than the main one
        // because NULL is also returned on error). If the thread wasn't
        // created with wxThread class, the returned value is undefined.
    static Thread *This();

        // Returns true if current thread is the main thread.
    static bool isMain();

        // Release the rest of our time slice leting the other threads run
    static void yield();

        // Sleep during the specified period of time in milliseconds
        //
        // NB: at least under MSW worker threads can not call ::wxSleep()!
    static void sleep(unsigned long milliseconds);

        // get the number of system CPUs - useful with SetConcurrency()
        // (the "best" value for it is usually number of CPUs + 1)
        //
        // Returns -1 if unknown, number of CPUs otherwise
    static int getCPUCount();

    // constructor only creates the C++ thread object and doesn't create (or
    // start) the real thread
    Thread(ThreadKind kind = THREAD_DETACHED);

    // functions that change the thread state: all these can only be called
    // from _another_ thread (typically the thread that created this one, e.g.
    // the main thread), not from the thread itself

        // create a new thread - call Run() to start it
    ThreadError create();

        // starts execution of the thread - from the moment Run() is called
        // the execution of wxThread::Entry() may start at any moment, caller
        // shouldn't suppose that it starts after (or before) Run() returns.
    ThreadError run();

        // waits for a joinable thread to finish and returns its exit code
        //
        // Returns (ExitCode)-1 on error (for example, if the thread is not
        // joinable)
    ExitCode wait();

        // kills the thread without giving it any chance to clean up - should
        // not be used in normal circumstances, use Delete() instead. It is a
        // dangerous function that should only be used in the most extreme
        // cases!
        //
        // The wxThread object is deleted by Kill() if the thread is
        // detachable, but you still have to delete it manually for joinable
        // threads.
    ThreadError kill();

        // pause a running thread: as Delete(), this only works if the thread
        // calls TestDestroy() regularly
    ThreadError pause();

        // resume a paused thread
    ThreadError resume();

    // thread status inquiries
        // Returns true if the thread is alive: i.e. running or suspended
    bool isAlive() const;
        // Returns true if the thread is running (not paused, not killed).
    bool isRunning() const;
        // Returns true if the thread is suspended
    bool isPaused() const;

        // is the thread of detached kind?
    bool isDetached() const { return m_isDetached; }

    // Get the thread ID - a platform dependent number which uniquely
    // identifies a thread inside a process
    unsigned long getId() const;

    // called when the thread exits - in the context of this thread
    //
    // NB: this function will not be called if the thread is Kill()ed
    virtual void onExit() { }

    // dtor is public, but the detached threads should never be deleted - use
    // Delete() instead (or leave the thread terminate by itself)
    virtual ~Thread();

protected:
   // exits from the current thread - can be called only from this thread
    void exit(ExitCode exitcode = 0);

    // entry point for the thread - called by Run() and executes in the context
    // of this thread.
    virtual void *entry() = 0;

private:
    // no copy ctor/assignment operator
    Thread(const Thread&);
    Thread& operator=(const Thread&);

    friend class ThreadInternal;

    // the (platform-dependent) thread class implementation
    ThreadInternal *m_internal;

    // protects access to any methods of wxThreadInternal object
    Mutex m_mutex;

    // true if the thread is detached, false if it is joinable
    bool m_isDetached;
};

}

}



/** @brief Locks a @a iLock and starts a scope block in which it remains locked.
 *  @author [BdG]
 *  @ingroup Threading
 *
 *  This macro starts a scope block through a @c if/else construct where the lock will be locked
 *  on entering the scope, and will be unlocked on leaving the scope.  You can use this scope in two
 *  ways: by using the braces:
 *
 *  @code
 *  LASS_LOCK(lock)
 *  {
 *      // locked
 *      doSomethingWhileLocked();
 *  }
 *  // unlocked
 *  @endcode
 *
 *  Because the macro uses an @c if/else construct, you can ommit the braces if you only need one
 *  statement in the locked scope:
 *
 *  @code
 *  LASS_LOCK(lock) doSomethingWhileLocked();
 *  @endcode
 *
 *  @par requirements for @a iLock:
 *		@a iLock can be of any type that implements two methods @c lock() and @c unlock(), that have
 *		to do the obvious thing.  Two excellent types to be used are lass::util::Mutex and 
 *		lass::util::CriticalSection.
 *
 *  @par references:
 *		<i>Concurrent Acces Control & C++</i>, C/C++ Users Journal, January 2004
 *
 *  @note Guess what, for some reason bill gates might know, 
 *		@c LASS_UNIQUENAME(lassUtilImplGenericLocker) causes troubles in the @c win32_vc7_d build.
 *		huray!  Does the concatenation fails?  It seems not, since in the preprocessor files
 *		everything seems fine.  And yet, it breaks.  Hence, we had to give up on it, there's only
 *		@c lassUtilImplGenericLocker now.  I don't think that will cause any problems, since it's
 *		inside an @c if/else scope block.  But if you get name conflicts on LASS_LOCK, this might 
 *		be the reason ...
 */
#define LASS_LOCK(iLock)\
	if (const lass::util::impl::GenericLockerBase& lassUtilImplGenericLocker =\
		lass::util::impl::makeGenericLocker(iLock))\
	{\
		LASS_ASSERT(false);\
	}\
	else



// --- implementation details ----------------------------------------------------------------------

namespace lass
{
namespace util
{
namespace impl
{

class GenericLockerBase
{
public:
	operator bool() const { return false; }
};

template <typename LockType> 
class GenericLocker: public GenericLockerBase
{
public:
	GenericLocker(LockType& iLock): lock_(&iLock) { lock_->lock(); }
	~GenericLocker() { lock_->unlock(); }
private:
	LockType* lock_;
};

template <typename LockType> 
inline GenericLocker<LockType> makeGenericLocker(LockType& iLock)
{
	return GenericLocker<LockType>(iLock);
}

}

}

}

#endif