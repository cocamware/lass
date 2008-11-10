/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#include "lass_common.h"
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

unsigned numberOfProcessors = impl::numberOfProcessors();

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

Thread::Thread(ThreadKind iKind, const char* name)
{
	pimpl_ = new impl::ThreadInternal(*this, iKind, name);
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
void Thread::bind(unsigned processor)
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
void Thread::bindCurrent(unsigned processor)
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
