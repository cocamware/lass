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
 *	Copyright (C) 2004-2022 the Initial Developer.
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
#include "rw_lock.h"	

namespace lass
{
namespace util
{

RWLock::RWLock(int iMaxReaders):
	maxReaders_(iMaxReaders),
	spinLock_(iMaxReaders),
	writersTrying_(0)
{
}

RWLock::~RWLock()
{
	LASS_ASSERT(spinLock_.load(std::memory_order_relaxed) == maxReaders_);
	LASS_ASSERT(writersTrying_.load(std::memory_order_relaxed) == 0);
}

void RWLock::lockr()
{
	int oldSpinLock;
	do
	{
		while (writersTrying_.load() != 0)
		{
			LASS_SPIN_PAUSE;
		}
		oldSpinLock = spinLock_.load();
		LASS_ASSERT(oldSpinLock>=0);
		while (oldSpinLock <= 0)
		{
			oldSpinLock = spinLock_.load();
			LASS_SPIN_PAUSE;
		}
	}
	while (!spinLock_.compare_exchange_weak(oldSpinLock, oldSpinLock - 1));
}

void RWLock::lockw()
{
	writersTrying_.fetch_add(1);
	int expected;
	do
	{
		expected = maxReaders_;
		while (spinLock_.load() != expected)
		{
			LASS_SPIN_PAUSE;
		}
	}
	while (!spinLock_.compare_exchange_weak(expected, 0));
}

void RWLock::unlockw()
{
	int expected = 0;
	LASS_ENFORCE(spinLock_.compare_exchange_strong(expected, maxReaders_));
	writersTrying_.fetch_sub(1);
}

void RWLock::unlockr()
{
	const int LASS_UNUSED(oldSpinLock) = spinLock_.fetch_add(1);
	LASS_ASSERT(oldSpinLock <= maxReaders_);
}


LockResult RWLock::tryLockr()
{
	int oldSpinLock;
	do
	{
		if (writersTrying_.load() != 0)
		{
			return lockBusy;
		}
		oldSpinLock = spinLock_.load();
		LASS_ASSERT(oldSpinLock >= 0);
		if (oldSpinLock <= 0)
		{
			return lockBusy;
		}
	}
	while (!spinLock_.compare_exchange_weak(oldSpinLock, oldSpinLock - 1));
	return lockSuccess;
}


LockResult RWLock::tryLockw()
{
	int expected = maxReaders_;
	writersTrying_.fetch_add(1);
	if (spinLock_.compare_exchange_strong(expected, 0))
	{
		return lockSuccess;
	}
	else
	{
		writersTrying_.fetch_sub(1);
		return lockBusy;
	}
}

} //namespace util
} //namespace lass

