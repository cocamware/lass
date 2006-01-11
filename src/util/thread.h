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
 *
 *  @par Extra credits and license info:
 *      Most of this code (Mutex, Event and Thread) is code from wxWidgets (formerly
 *      known as wxWindows) adapted to the LASS style guide.
 */

/**
*   @defgroup   Threading Threading
*   Threading functions.  The Mutex and Thread classes for WIN32 are almost literal copy and pastes
*   of the functions of wxWindows2.4.2.  The implementation has changed a little bit to
*   use more of the LASS utility functions.  The most noticible difference is in the thread
*   class where the Delete is not implemented, due to too large dependencies.  The Critical
*   Section on WIN32 is an own implementation, just like all the UNIX Posix threading stuff.
*
*   A little word on synchronization: in windows there are quite a few synchronisation
*   primitives and synchronized objects.  Only the most common are implemented and wrapped
*   in the LASS environment:
*       Mutex : system-wide synchronisation object between processes/threads.  Mutexes set in one process can be
*               unlocked in another one.  That is the theory.  In practice with the LASS
*               wrapping, there is no possibility to reference these mutexes in other
*               processes, named mutexes are not supported.  Mutex are rather heavyweight
*               structures.  Cannot be used for intrathread synchronisation AFAIK [TDM]
*       CriticalSection : process-wide synchronisation object between threads.  Lightweight
*               construction.  Cannot be used for intra-thread synchronisation.
*       Condition: this corresponds with MS Windows Events, can be used for interprocess/
*               interthread and intrathread synchronisation. Not implemented in UNIX build.
*/

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_H

#include "util_common.h"
#include "non_copyable.h"

namespace lass
{
namespace util
{
namespace impl
{
	class MutexInternal;
	class CriticalSectionInternal;
	class ConditionInternal;
	class ThreadInternal;
}

/** Return code for lock functions
 *  @ingroup Threading
 *  @see Mutex,CriticalSection
 */
enum LockResult
{
	lockSuccess,		/**< Mutex/CriticalSection is succesfully locked by this thread */
	lockBusy,		/**< Mutex/CriticalSection is locked by another thread */
};

/** Return code for wait functions
 *  @ingroup Threading
 *  @see Condition
 */
enum WaitResult
{
	waitSuccess,	/**< Wait is successfully terminated */ 
	waitTimeout,	/**< Wait failed because of a timeout */
};

/** ThreadKind.
*   @ingroup Threading
*   @see Thread
*/
enum ThreadKind
{
	threadDetached,		/**< detached thread */
	threadJoinable,		/**< joinable thread, can be waited for */
};

/** Mutex.
*   @ingroup Threading
*   @see MutexLocker
*   A mutex is a systemwide synchronization object. It is heavier in use than a critical
*   section.  For exception-safe basic use, the MutexLocker can be used.
*/
class Mutex : NonCopyable
{
public:
	Mutex();
	~Mutex();
	// Lock the mutex.
	void lock();
	// Try to lock the mutex: if it can't, returns immediately with an error.
	const LockResult tryLock();
	// Unlock the mutex.
	void unlock();
	// Returns true if the mutex is locked.
	const bool isLocked() const;

private:
	impl::MutexInternal *pimpl_;
};


/** MutexLocker.
*   @ingroup Threading
*   @see Mutex
*   Auto-locker-release of mutexes.
*/
class MutexLocker: NonCopyable
{
public:
	MutexLocker(Mutex& iMutex);
	~MutexLocker();
private:
	Mutex& mutex_;
};

/** CriticalSection.
*   @ingroup Threading
*   @see CriticalSectionLocker
*   @author Tom De Muer [TDM]
*   @date 2004
*   Process wide synchronization object.  The recommended object for interthread synchronisation
*   within a process in Windows.
*/
class CriticalSection : NonCopyable
{
public:
	CriticalSection();
	~CriticalSection();
	
	// Lock the critical section.
	void lock();
	// Try to lock the critical section: if it can't, returns immediately with an error.
	const LockResult tryLock();
	// Unlock the mutex.
	void unlock();
	
	const bool isLocked() const;

private:
	impl::CriticalSectionInternal* pimpl_;
};

/** CriticalSectionLocker.
*   @ingroup Threading
*   @see CriticalSection
*   Auto-locker-release of critical sections
*/
class CriticalSectionLocker: NonCopyable
{
	CriticalSection& criticalSection_;
public:
	CriticalSectionLocker(CriticalSection& iSection);
	~CriticalSectionLocker();
};

/** Condition.
*   @ingroup Threading
*   A condition can be used to synchronize using messages were a condition waits for
*   a signal or broadcast.  A signal will only release one waiter, a broadcast will release
*   all current waiting wait-conditions.
*/
class Condition
{
public:
	Condition();
	~Condition();
	void wait();
	const WaitResult wait(unsigned long iMilliSeconds);
	void signal();
	void broadcast();
private:
	impl::ConditionInternal* pimpl_;
};

/** A base class for threads.
*   @ingroup Threading
*   The virtual function doRun() needs to be overriden and after creation of the 
*   thread on the heap or stack, the thread is in a state ready to be run.  
*   The run() function actually starts the thread.
*
*   JOINABLE threads can be waited for, DETACHED threads can not be waited for.
*/
class Thread: NonCopyable
{
public:

	virtual ~Thread();
	
	void run();
	void join();
	
	static void sleep(unsigned long iMilliseconds);
	static void yield();

protected:

	Thread(ThreadKind iKind = threadDetached);

private:

	friend class impl::ThreadInternal;

	virtual void doRun() = 0;

	impl::ThreadInternal* pimpl_;
};

}

}


/** @brief Locks a @a iLock and starts a scope block in which it remains locked.
 *  @author [Bramz]
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
 *      @a iLock can be of any type that implements two methods @c lock() and @c unlock(), that have
 *      to do the obvious thing.  Two excellent types to be used are lass::util::Mutex and
 *      lass::util::CriticalSection.
 *
 *  @par references:
 *      <i>Concurrent Acces Control & C++</i>, C/C++ Users Journal, January 2004
 *
 *  @note Guess what, for some reason bill gates might know,
 *      @c LASS_UNIQUENAME(lassUtilImplGenericLocker) causes troubles in the @c win32_vc7_d build.
 *      huray!  Does the concatenation fails?  It seems not, since in the preprocessor files
 *      everything seems fine.  And yet, it breaks.  Hence, we had to give up on it, there's only
 *      @c lassUtilImplGenericLocker now.  I don't think that will cause any problems, since it's
 *      inside an @c if/else scope block.  But if you get name conflicts on LASS_LOCK, this might
 *      be the reason ...
 */
#define LASS_LOCK(iLock)\
	if (const lass::util::impl::GenericLockerBase& lassUtilImplGenericLocker =\
		lass::util::impl::makeGenericLocker(iLock))\
	{\
		LASS_ASSERT_UNREACHABLE;\
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
