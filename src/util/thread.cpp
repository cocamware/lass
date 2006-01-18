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



// --- MutexLocker ---------------------------------------------------------------------------------

MutexLocker::MutexLocker( Mutex& iMutex ) : 
	mutex_(iMutex)
{
	mutex_.lock();
}

MutexLocker::~MutexLocker()
{
	try
	{
		mutex_.unlock();
	}
	catch (std::exception& error)
	{
		std::cerr << "[LASS RUN MSG] WARNING: exception thrown in ~MutexLocker(): "
			<< error.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "[LASS RUN MSG] WARNING: unknown exception thrown in ~MutexLocker()" 
			<< std::endl;
	}
}



// --- CriticalSection -----------------------------------------------------------------------------

CriticalSection::CriticalSection(void )
{
	pimpl_ = new impl::CriticalSectionInternal;
}

CriticalSection::~CriticalSection(void)
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



// --- CriticalSectionLocker -----------------------------------------------------------------------

CriticalSectionLocker::CriticalSectionLocker( CriticalSection& iCriticalSection) :
	criticalSection_(iCriticalSection)
{
	criticalSection_.lock();
}

CriticalSectionLocker::~CriticalSectionLocker()
{
	try
	{
		criticalSection_.unlock();
	}
	catch (std::exception& error)
	{
		std::cerr << "[LASS RUN MSG] WARNING: exception thrown in ~CriticalSectionLocker(): "
			<< error.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "[LASS RUN MSG] WARNING: unknown exception thrown in ~CriticalSectionLocker()" 
			<< std::endl;
	}
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
