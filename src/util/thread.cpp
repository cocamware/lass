/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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
#include "thread.h"
#include "atomic.h"

#if defined(LASS_UTIL_THREAD_HAVE_POSIX)
#	include "impl/thread_posix.inl"
#elif defined(LASS_UTIL_THREAD_HAVE_WIN32)
#	include "impl/thread_win32.inl"
#else
#	error "[LASS BUILD MSG] Threading not supported for this platform"
#endif

namespace lass
{
namespace util
{

size_t numberOfProcessors = impl::numberOfProcessors();

// --- Mutex ---------------------------------------------------------------------------------------

Mutex::Mutex(void )
{
	pimpl_ = new impl::MutexInternal;
}

Mutex::~Mutex(void)
{
	LASS_ASSERT(pimpl_);
	LASS_ASSERT(pimpl_->lockCount() == 0);
	if (pimpl_->lockCount() > 0)
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
			<< "destroying a CriticalSection that still has " 
			<< pimpl_->lockCount() << " locks." << std::endl;
	}
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




// --- CriticalSection -----------------------------------------------------------------------------

CriticalSection::CriticalSection(void )
{
	pimpl_ = new impl::CriticalSectionInternal;
}

CriticalSection::~CriticalSection(void)
{
	LASS_ASSERT(pimpl_);
	impl::CriticalSectionInternal* pimpl = static_cast<impl::CriticalSectionInternal*>(pimpl_);
	LASS_ASSERT(pimpl->lockCount() == 0);
	if (pimpl->lockCount() > 0)
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
			<< "destroying a CriticalSection that still has " 
			<< pimpl->lockCount() << " locks." << std::endl;
	}
	delete pimpl;
	pimpl_ = 0;
}

void CriticalSection::lock()
{
	LASS_ASSERT(pimpl_);
	impl::CriticalSectionInternal* pimpl = static_cast<impl::CriticalSectionInternal*>(pimpl_);
	pimpl->lock();
}

const LockResult CriticalSection::tryLock()
{
	LASS_ASSERT(pimpl_);
	impl::CriticalSectionInternal* pimpl = static_cast<impl::CriticalSectionInternal*>(pimpl_);
	return pimpl->tryLock();
}

void CriticalSection::unlock()
{
	LASS_ASSERT(pimpl_);
	impl::CriticalSectionInternal* pimpl = static_cast<impl::CriticalSectionInternal*>(pimpl_);
	pimpl->unlock();
}

const bool CriticalSection::isLocked() const
{
	LASS_ASSERT(pimpl_);
	impl::CriticalSectionInternal* pimpl = static_cast<impl::CriticalSectionInternal*>(pimpl_);
	return pimpl->lockCount() > 0;
}



// --- Condition -----------------------------------------------------------------------------------

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

const WaitResult Condition::wait(unsigned long iMilliSeconds)
{
	LASS_ASSERT(pimpl_);
	return pimpl_->wait(iMilliSeconds);
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



// --- Thread --------------------------------------------------------------------------------------

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

/** bind this thread to a processor (this as in this-pointer)
 */
void Thread::bind(size_t processor)
{
	LASS_ASSERT(pimpl_);
	pimpl_->bind(processor);
}

void Thread::sleep(unsigned long iMilliSeconds)
{
	impl::ThreadInternal::sleep(iMilliSeconds);
}

void Thread::yield()
{
	impl::ThreadInternal::yield();
}

/** bind current thread to a processor (current as in callee's context)
 */
void Thread::bindCurrent(size_t processor)
{
	impl::ThreadInternal::bindCurrent(processor);
}



// --- ThreadLocalStorage --------------------------------------------------------------------------

ThreadLocalStorage::ThreadLocalStorage(void (*destructor)(void*))
{
	pimpl_ = new impl::ThreadLocalStorageInternal(destructor);
}

ThreadLocalStorage::~ThreadLocalStorage()
{
	delete pimpl_;
	pimpl_ = 0;
}

void* const ThreadLocalStorage::get() const
{
	LASS_ASSERT(pimpl_);
	return pimpl_->get();
}

void ThreadLocalStorage::set(void* value)
{
	LASS_ASSERT(pimpl_);
	pimpl_->set(value);
}

}
}

// EOF
