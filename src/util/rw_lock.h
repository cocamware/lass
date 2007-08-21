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
