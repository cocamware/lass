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
 */

#include "util_common.h"
#include "thread.h"

namespace lass
{
namespace util
{

enum ThreadState
{
    STATE_NEW,          // didn't start execution yet (=> RUNNING)
    STATE_RUNNING,      // thread is running (=> PAUSED, CANCELED)
    STATE_PAUSED,       // thread is temporarily suspended (=> RUNNING)
    STATE_CANCELED,     // thread should terminate a.s.a.p. (=> EXITED)
    STATE_EXITED        // thread is terminating
};

}
}

#if (LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32)
	#include "thread_win32.cpp"
#else
	#include "thread_unix.cpp"
#endif

namespace lass
{
namespace util
{

MutexLocker::MutexLocker( Mutex& iMutex ) : m_mutex(iMutex)
{
	m_mutex.lock();
}
MutexLocker::~MutexLocker()
{
	m_mutex.unlock();
}

CriticalSectionLocker::CriticalSectionLocker( CriticalSection& iCriticalSection) : m_criticalsection(iCriticalSection)
{
	m_criticalsection.lock();
}
CriticalSectionLocker::~CriticalSectionLocker()
{
	m_criticalsection.unlock();
}

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



}
}
