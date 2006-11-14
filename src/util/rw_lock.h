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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_RW_LOCK_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_RW_LOCK_H

#include "util_common.h"
#include "atomic.h"
#include "thread.h"	

namespace lass
{
namespace util
{

namespace impl
{
} //namespace impl


/** Lean and mean synchronisation object, without OS support.
*   @ingroup Threading
*   @see RWLocker
*   @author Tom De Muer
*   @date 2006
*   
*	This lock is built upon atomic operations that are programmed in assembly.  It makes a difference
*	between reading and writing.  A maximum number of readers is allowed while only 1 writer is allowed.
*	The writer will only enter if there are no readers are reading at the same moment.  This RWLock is 
*	a blocking and spinning synchronization object.  Priority is given over writers, as soon as a writer is
*	trying to enter, only one subsequent reader will in worst case be able to enter.
*/

class LASS_DLL RWLock : NonCopyable
{

public:
	RWLock(util::CallTraits<size_t>::TParam iMaxReaders);
	~RWLock();

	void lockr();
	void lockw();
	void unlockr();
	void unlockw();

	const LockResult tryLockr();
	const LockResult tryLockw();

private:
	size_t maxReaders_;		/**< the maximum of simultaneous readers allowed */
	int spinLock_;
	int writersTrying_;		/**< the number of writers trying to enter */
};


RWLock::RWLock(util::CallTraits<size_t>::TParam iMaxReaders)
{
	maxReaders_ = iMaxReaders;
	spinLock_ = maxReaders_;
	writersTrying_ = 0;
}

RWLock::~RWLock()
{
	LASS_ENFORCE(spinLock_==maxReaders_);
}

void RWLock::lockr()
{
	int newSpinLock;
	int oldSpinLock;
	do
	{
		oldSpinLock = spinLock_;
		LASS_ASSERT(oldSpinLock>=0);
		newSpinLock = oldSpinLock-1;
	} while (writersTrying_!=0 || oldSpinLock==0 || !lass::util::atomicCompareAndSwap(spinLock_,oldSpinLock,newSpinLock));
}

void RWLock::lockw()
{
	lass::util::atomicIncrement(writersTrying_);
	do
	{
	} while (!lass::util::atomicCompareAndSwap<int>(spinLock_,maxReaders_,0));
}

void RWLock::unlockw()
{
	LASS_ENFORCE(spinLock_==0);
	do
	{
	} while (!lass::util::atomicCompareAndSwap<int>(spinLock_,0,maxReaders_));
	lass::util::atomicDecrement(writersTrying_);
}

void RWLock::unlockr()
{
	lass::util::atomicIncrement(spinLock_);
	LASS_ASSERT(spinLock_<=maxReaders_);
}


const LockResult RWLock::tryLockr()
{
	int oldSpinLock;
	int newSpinLock;
	do
	{
		oldSpinLock = spinLock_;
		LASS_ASSERT(spinLock_ >= 0);
		if (spinLock_ == 0)
		{
			return lockBusy;
		}
		newSpinLock = oldSpinLock - 1;
	}
	while (!atomicCompareAndSwap(spinLock_, oldSpinLock, newSpinLock));
	return lockSuccess;
}


const LockResult RWLock::tryLockw()
{
	lass::util::atomicIncrement(writersTrying_);
	int newSpinLock;
	do
	{
		LASS_ASSERT(spinLock_ >= 0);
		if (spinLock_ != maxReaders_)
		{
			lass::util::atomicDecrement(writersTrying_);
			return lockBusy;
		}
	}
	while (!atomicCompareAndSwap<int>(spinLock_, maxReaders_, 0));
	return lockSuccess;
}

} //namespace util
} //namespace lass


#endif

// EOF
