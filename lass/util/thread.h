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
#include "atomic.h"

namespace lass
{
namespace util
{
namespace impl
{
	class MutexInternal;
	class ConditionInternal;
	class ThreadInternal;
	class ThreadLocalStorageInternal;
}

/** Return code for lock functions
 *  @ingroup Threading
 *  @see Mutex,CriticalSection
 */
enum LockResult
{
	lockSuccess,		/**< Mutex/CriticalSection is succesfully locked by this thread */
	lockBusy		/**< Mutex/CriticalSection is locked by another thread */
};

/** Return code for wait functions
 *  @ingroup Threading
 *  @see Condition
 */
enum WaitResult
{
	waitSuccess,	/**< Wait is successfully terminated */ 
	waitTimeout	/**< Wait failed because of a timeout */
};

/** ThreadKind.
*   @ingroup Threading
*   @see Thread
*/
enum ThreadKind
{
	threadDetached,		/**< detached thread */
	threadJoinable		/**< joinable thread, can be waited for */
};

typedef std::vector<bool> TCpuSet;

LASS_DLL const TCpuSet availableProcessors();

/** Return highest id of processor + 1, in this machine
 *  @ingroup Threading  
 */
LASS_DLL size_t numberOfProcessors();

/** Return total number of processors in machine that are online
 *  @ingroup Threading  
 */
LASS_DLL size_t numberOfAvailableProcessors();

/** Check whether a processor is avaialable.
 *  @ingroup Threading  
 */
LASS_DLL bool isAvailableProcessor(size_t processor);

/** Mutex.
*   @ingroup Threading
*   @see MutexLocker
*   A mutex is a systemwide synchronization object. It is heavier in use than a critical
*   section.  For exception-safe basic use, the MutexLocker can be used.
*/
class LASS_DLL Mutex : NonCopyable
{
public:
	Mutex();
	~Mutex();
	void lock();
	LockResult tryLock();
	void unlock();
	bool isLocked() const;

private:
	impl::MutexInternal* pimpl_;
};

/** CriticalSection.
*   @ingroup Threading
*   @see CriticalSectionLocker
*   @author Tom De Muer [TDM]
*   @date 2004
*   Process wide synchronization object.  The recommended object for interthread synchronisation
*   within a process in Windows.
*/
class LASS_DLL CriticalSection : NonCopyable
{
public:
	CriticalSection();
	~CriticalSection();
	
	void lock();
	LockResult tryLock();
	void unlock();
	
	bool isLocked() const;

private:
	void* pimpl_;
};



/** Condition.
*   @ingroup Threading
*   A condition can be used to synchronize using messages were a condition waits for
*   a signal or broadcast.  A signal will only release one waiter, a broadcast will release
*   all current waiting wait-conditions.
*/
class LASS_DLL Condition: NonCopyable
{
public:
	Condition();
	~Condition();
	void wait();
	WaitResult wait(unsigned long iMilliSeconds);
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
class LASS_DLL Thread: NonCopyable
{
public:

	enum 
	{ 
		anyProcessor = size_t(-1) /**< argument for Thread::bind to unbind the thread so it runs on any processor */
	};

	virtual ~Thread();
	
	void run();
	void join();
	void bind(size_t processor);
	const TCpuSet affinity() const;

	
	static void sleep(unsigned long milliseconds);
	static void yield();
	static void bindCurrent(size_t processor);

protected:

	Thread(ThreadKind kind = threadDetached, const char* name = 0);

private:

	friend class impl::ThreadInternal;

	virtual void doRun() = 0;

	impl::ThreadInternal* pimpl_;
};



/** A primitive to provide Thread Local Storage functionality
 *  @ingroup Threading
 */
class LASS_DLL ThreadLocalStorage: NonCopyable
{
public:
	ThreadLocalStorage(void (*destructor)(void*) = 0);
	~ThreadLocalStorage();
	void* get() const;
	void set(void* value);
private:
	impl::ThreadLocalStorageInternal* pimpl_;
};




/** A primitive to provide Thread Local Storage functionality for a first-citizen class.
 *  @ingroup Threading
 *
 *	@arg requirements: T must be copyconstructible.
 *
 *	@warning 
 *		On Win32 platform, you should _access_ a ThreadLocalVariable inside code running
 *		from a util::Thread (or util::threadFun for that matter).  This is because the destructor
 *		of the variable will _only_ be called on exit of a util::Thread.  If you access it from
 *		any other thread (even the main thread), the variable will correctly constructed for that
 *		thread, but it will _never_ be destructed!
 *		On a POSIX platform, this is not a problem though!  The destructor will always be called.
 */
template <typename T>
class ThreadLocalVariable: NonCopyable
{
public:
	typedef typename CallTraits<T>::TValue TValue;
	typedef typename CallTraits<T>::TParam TParam;
	typedef typename CallTraits<T>::TConstReference TConstReference;
	typedef typename CallTraits<T>::TReference TReference;

	explicit ThreadLocalVariable(TParam prototype = T()):
		prototype_(prototype),
		storage_(&ThreadLocalVariable<T>::destructor)
	{
	}

	TValue* get()
	{
		TValue* ptr = static_cast<TValue*>(storage_.get());
		if (!ptr)
		{
			std::auto_ptr<T> newCopy(new TValue(prototype_));
			storage_.set(newCopy.get());
			ptr = newCopy.release();
		}
		return ptr;
	}

	const TValue* get() const
	{
		const TValue* ptr = static_cast<const TValue*>(storage_.get());
		if (!ptr)
		{
			std::auto_ptr<T> newCopy(new TValue(prototype_));
			storage_.set(newCopy.get());
			ptr = newCopy.release();
		}
		return ptr;
	}

	TValue* operator->()
	{
		return get();
	}

	const TValue* operator->() const
	{
		return get();
	}

	TReference operator*()
	{
		return *get();
	}

	TConstReference operator*() const
	{
		return *get();
	}

private:

	static void destructor(void* p)
	{
		delete static_cast<T*>(p);
	}

	TValue prototype_;
	mutable ThreadLocalStorage storage_;
};



/** Lean and mean synchronisation object, without OS support.
*   @ingroup Threading
*   @see SemaphoreLocker
*   @author Bram de Greve
*   @date 2004
*   
*	This semaphore is built upon atomic operations that are programmed in assembly.
*/
class Semaphore: NonCopyable
{
public:
	Semaphore(int numberOfSlots = 1): freeSlots_(numberOfSlots) {}	
	void lock() { atomicLock(freeSlots_); }
	LockResult tryLock() { return atomicTryLock(freeSlots_) ? lockSuccess : lockBusy; }
	void unlock() { atomicUnlock(freeSlots_); }
	bool isLocked() const { return freeSlots_ == 0; }

private:
	volatile int freeSlots_;
};




/** Common base class for lockers
 *  @ingroup Threading
 */
class LockerBase
{
public:
	bool hasLock() const { return hasLock_; }
	operator bool() const { return hasLock_; }
protected:
	LockerBase(bool hasLock = true): hasLock_(hasLock) {}
private:
	bool hasLock_;
};



template <typename LockType>
struct DefaultLockTraits
{
	static bool lock(LockType& x) { x.lock(); return true; }
	static void unlock(LockType& x) { x.unlock(); }
};

template <typename LockType>
struct TryLockTraits
{
	static bool lock(LockType& x) { return x.tryLock() == lockSuccess; }
	static void unlock(LockType& x) { x.unlock(); }
};

template <typename LockType>
struct IntegralLockTraits
{
	static bool lock(LockType& x) { atomicLock(x); return true; }
	static void unlock(LockType& x) { atomicUnlock(x); }
};


/** @ingroup Threading
 */
template 
<	
	typename LockType, 
	typename LockTraits = DefaultLockTraits<LockType>
>
class Locker: public LockerBase
{
public:
	typedef LockType TLock;
	typedef LockTraits TLockTraits;
	Locker(TLock& iLock):
		LockerBase(TLockTraits::lock(iLock)),
		lock_(&iLock)
	{ 
	}
	~Locker() 
	{
		if (!hasLock())
		{
			return;
		}
		LASS_ASSERT(lock_);
		try
		{
			TLockTraits::unlock(*lock_);
		}
		catch (const std::exception& error)
		{
			std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
				<< "exception thrown in ~Locker(): " << error.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
				<< "unknown exception thrown in ~Locker()" << std::endl;
		}
	}
	const TLock& mutex() const
	{ 
		LASS_ASSERT(lock_);
		return *lock_;
	}
	void swap(Locker& iOther)
	{
		std::swap(lock_, iOther.lock_);
	}
private:
	TLock* lock_;
};



/** @ingroup Threading
 *  @relates Locker
 */
template <typename T>
inline Locker<T, DefaultLockTraits<T> > makeLocker(T& iLock)
{
	return Locker<T, DefaultLockTraits<T> >(iLock);
}



/** @ingroup Threading
 *  @relates Locker
 */
template <typename T>
inline Locker<T, TryLockTraits<T> > makeTryLocker(T& iLock)
{
	return Locker<T, TryLockTraits<T> >(iLock);
}



/** @ingroup Threading
 *  @relates Locker
 */
template <typename T>
inline Locker<T, IntegralLockTraits<T> > makeIntegralLocker(T& islots)
{
	return Locker<T, IntegralLockTraits<T> >(islots);
}



/** typedef of Locker for Mutex
 *	@ingroup Threading
 *	@sa Mutex
 *  @sa Locker
*/
typedef Locker<Mutex> MutexLocker;

/** typedef of Locker for CriticalSection
 *	@ingroup Threading
 *	@sa CriticalSection
 *  @sa Locker
*/
typedef Locker<CriticalSection> CriticalSectionLocker;

/** typedef of Locker for Semaphore
 *	@ingroup Threading
 *	@sa Semaphore
 *  @sa Locker
*/
typedef Locker<Semaphore> SemaphoreLocker;

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
	if (const ::lass::util::LockerBase& LASS_UNUSED(lassUtilImplLocker) = ::lass::util::makeLocker(iLock))

#define LASS_TRY_LOCK(iLock)\
	if (const ::lass::util::LockerBase& LASS_UNUSED(lassUtilImplLocker) = ::lass::util::makeTryLocker(iLock))

#define LASS_LOCK_INTEGRAL(iSlots)\
	if (const ::lass::util::LockerBase& LASS_UNUSED(lassUtilImplLocker) = ::lass::util::makeIntegralLocker(iSlots))

#endif
