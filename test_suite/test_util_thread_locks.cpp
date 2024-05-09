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
 *	Copyright (C) 2004-2024 the Initial Developer.
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

#include "test_common.h"

#include "../lass/util/thread.h"
#include "../lass/util/thread_fun.h"

namespace
{
	using namespace lass;

	util::Condition condition;
	std::atomic<bool> starting;
	std::atomic<bool> done;

	template <typename LockType>
	void blocker(LockType* lock)
	{
		LASS_LOCK(*lock)
		{
			starting = false;
			condition.signal();
			util::Thread::sleep(200);
			done = true;
		}
	}

	template <typename LockType>
	void blockerIntegral(LockType* lock)
	{
		LASS_LOCK_INTEGRAL(*lock)
		{
			starting = false;
			condition.signal();
			util::Thread::sleep(200);
			done = true;
		}
	}
}

namespace lass
{
namespace test
{

template <typename LockType>
void testUtilThreadLock()
{
	LockType lock;

	LASS_TEST_CHECK(!lock.isLocked());

	bool beenHere = false;
	LASS_LOCK(lock)
	{
		LASS_TEST_CHECK(lock.isLocked());
		beenHere = true;
	}
	else
	{
		LASS_TEST_ERROR("unreachable");
	}
	LASS_TEST_CHECK(!lock.isLocked());
	LASS_TEST_CHECK(beenHere);

	done = false;
	starting = true;
	beenHere = false;
	std::unique_ptr<util::Thread> other(util::threadFun(blocker<LockType>, &lock, util::threadJoinable));
	other->run();
	while (starting)
	{
		condition.wait(10);
	}
	LASS_LOCK(lock)
	{
		LASS_TEST_CHECK(done);
		LASS_TEST_CHECK(lock.isLocked());
		beenHere = true;
	}
	else
	{
		LASS_TEST_ERROR("unreachable");
	}
	other->join();
	LASS_TEST_CHECK(!lock.isLocked());
	LASS_TEST_CHECK(done);
	LASS_TEST_CHECK(beenHere);
}


template <typename LockType>
void testUtilThreadTryLock()
{
	LockType lock;

	LASS_TEST_CHECK(!lock.isLocked());

	bool beenHere = false;
	LASS_TRY_LOCK(lock)
	{
		LASS_TEST_CHECK(lock.isLocked());
		beenHere = true;
	}
	else
	{
		LASS_TEST_ERROR("unreachable");
	}
	LASS_TEST_CHECK(!lock.isLocked());
	LASS_TEST_CHECK(beenHere);

	done = false;
	starting = true;
	beenHere = false;
	std::unique_ptr<util::Thread> other(util::threadFun(blocker<LockType>, &lock, util::threadJoinable));
	other->run();
	while (starting)
	{
		condition.wait(10);
	}
	LASS_TRY_LOCK(lock)
	{
		LASS_TEST_ERROR("unreachable");
	}
	else
	{
		LASS_TEST_CHECK(!done);
		LASS_TEST_CHECK(lock.isLocked());
		beenHere = true;
	}
	other->join();
	LASS_TEST_CHECK(!lock.isLocked());
	LASS_TEST_CHECK(done);
	LASS_TEST_CHECK(beenHere);
}


template <typename LockType>
void testUtilThreadReentrantLockHelper(LockType& lock, bool isReentrant)
{
	LASS_TRY_LOCK(lock)
	{
		LASS_TEST_CHECK(lock.isLocked());
		LASS_TEST_CHECK(isReentrant);
	}
	else
	{
		LASS_TEST_CHECK(lock.isLocked());
		LASS_TEST_CHECK(!isReentrant);
	}
}


template <typename LockType, bool isReentrant>
void testUtilThreadReentrantLock()
{
	LockType lock;

	LASS_TEST_CHECK(!lock.isLocked());

	LASS_LOCK(lock)
	{
		LASS_TEST_CHECK(lock.isLocked());
		testUtilThreadReentrantLockHelper(lock, isReentrant);
	}

	LASS_TEST_CHECK(!lock.isLocked());
}


template <typename LockType>
void testUtilThreadIntegralLock()
{
	std::atomic<LockType> lock { 1 };

	bool beenHere = false;
	LASS_LOCK_INTEGRAL(lock)
	{
		LASS_TEST_CHECK_EQUAL(lock, LockType(0));
		beenHere = true;
	}
	else
	{
		LASS_TEST_ERROR("unreachable");
	}
	LASS_TEST_CHECK_EQUAL(lock, LockType(1));
	LASS_TEST_CHECK(beenHere);

	done = false;
	starting = true;
	beenHere = false;
	std::unique_ptr<util::Thread> other(util::threadFun(blockerIntegral<std::atomic<LockType>>, &lock, util::threadJoinable));
	other->run();
	while (starting)
	{
		condition.wait(10);
	}
	LASS_LOCK_INTEGRAL(lock)
	{
		LASS_TEST_CHECK(done);
		LASS_TEST_CHECK_EQUAL(lock, LockType(0));
		beenHere = true;
	}
	else
	{
		LASS_TEST_ERROR("unreachable");
	}
	other->join();
	LASS_TEST_CHECK_EQUAL(lock, LockType(1));
	LASS_TEST_CHECK(done);
	LASS_TEST_CHECK(beenHere);
}



TUnitTest test_util_thread_locks()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testUtilThreadLock<util::Mutex>));
	result.push_back(LASS_TEST_CASE(testUtilThreadLock<util::CriticalSection>));
	result.push_back(LASS_TEST_CASE(testUtilThreadLock<util::Semaphore>));
	result.push_back(LASS_TEST_CASE(testUtilThreadTryLock<util::Mutex>));
	result.push_back(LASS_TEST_CASE(testUtilThreadTryLock<util::CriticalSection>));
	result.push_back(LASS_TEST_CASE(testUtilThreadTryLock<util::Semaphore>));
	result.push_back(LASS_TEST_CASE((testUtilThreadReentrantLock<util::Mutex, true>)));
	result.push_back(LASS_TEST_CASE((testUtilThreadReentrantLock<util::CriticalSection, true>)));
	result.push_back(LASS_TEST_CASE((testUtilThreadReentrantLock<util::Semaphore, false>)));
	result.push_back(LASS_TEST_CASE(testUtilThreadIntegralLock<int>));
	result.push_back(LASS_TEST_CASE(testUtilThreadIntegralLock<unsigned>));
	result.push_back(LASS_TEST_CASE(testUtilThreadIntegralLock<size_t>));
	return result;
}

}

}

// EOF
