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

// --- Mutex ---------------------------------------------------------------------------------------

Mutex::Mutex(void )
{
	pimpl_ = new impl::MutexInternal;
}

Mutex::~Mutex(void)
{
	impl::MutexInternal* pimpl = static_cast<impl::MutexInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
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

void Mutex::lock()
{
	impl::MutexInternal* pimpl = static_cast<impl::MutexInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	pimpl->lock();
}

const LockResult Mutex::tryLock()
{
	impl::MutexInternal* pimpl = static_cast<impl::MutexInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	return pimpl->tryLock();
}

void Mutex::unlock()
{
	impl::MutexInternal* pimpl = static_cast<impl::MutexInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	pimpl->unlock();
}

const bool Mutex::isLocked() const
{
	impl::MutexInternal* pimpl = static_cast<impl::MutexInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	return pimpl->lockCount() > 0;
}




// --- CriticalSection -----------------------------------------------------------------------------

CriticalSection::CriticalSection(void )
{
	pimpl_ = new impl::CriticalSectionInternal;
}

CriticalSection::~CriticalSection(void)
{
	impl::CriticalSectionInternal* pimpl = static_cast<impl::CriticalSectionInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	LASS_ASSERT(pimpl->lockCount() == 0);
	if (pimpl->lockCount() > 0)
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
			<< "destroying a CriticalSection that still has " 
			<< pimpl->lockCount() << " locks." << std::endl;
	}
	delete pimpl;
	pimpl = 0;
}

void CriticalSection::lock()
{
	impl::CriticalSectionInternal* pimpl = static_cast<impl::CriticalSectionInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	pimpl->lock();
}

const LockResult CriticalSection::tryLock()
{
	impl::CriticalSectionInternal* pimpl = static_cast<impl::CriticalSectionInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	return pimpl->tryLock();
}

void CriticalSection::unlock()
{
	impl::CriticalSectionInternal* pimpl = static_cast<impl::CriticalSectionInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	pimpl->unlock();
}

const bool CriticalSection::isLocked() const
{
	impl::CriticalSectionInternal* pimpl = static_cast<impl::CriticalSectionInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	return pimpl->lockCount() > 0;
}



// --- Condition -----------------------------------------------------------------------------------

Condition::Condition(void)
{
	pimpl_ = new impl::ConditionInternal;
}

Condition::~Condition(void)
{
	impl::ConditionInternal* pimpl = static_cast<impl::ConditionInternal*>(pimpl_);
	delete pimpl;
	pimpl = 0;
}

void Condition::wait()
{
	impl::ConditionInternal* pimpl = static_cast<impl::ConditionInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	pimpl->wait();
}

const WaitResult Condition::wait(unsigned long iMilliSeconds)
{
	impl::ConditionInternal* pimpl = static_cast<impl::ConditionInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	return pimpl->wait(iMilliSeconds);
}

void Condition::signal()
{
	impl::ConditionInternal* pimpl = static_cast<impl::ConditionInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	pimpl->signal();
}

void Condition::broadcast()
{
	impl::ConditionInternal* pimpl = static_cast<impl::ConditionInternal*>(pimpl_);
	LASS_ASSERT(pimpl);
	pimpl->broadcast();
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

// EOF
