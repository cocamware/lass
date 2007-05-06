/** @file
 *  @internal
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_THREAD_FUN_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_THREAD_FUN_INL

#include "test_common.h"

#include "../util/thread_fun.h"
#include "../util/scoped_ptr.h"

namespace lass
{
namespace test
{
namespace thread_test
{
	volatile bool functionIsCalled = false;

	void foo(int a, int b) 
	{ 
		util::Thread::sleep(500); 
		functionIsCalled = true; 
	}
	
	class Bar
	{
	public:
		void spam() 
		{ 
			util::Thread::sleep(500); 
			functionIsCalled = true; 
		}
		void ham(int a) const 
		{ 
			util::Thread::sleep(500); 
			functionIsCalled = true; 
		}
	};

	void eggs() 
	{ 
		std::cerr << "cooking eggs ...\n";
		util::Thread::sleep(100); 
		std::cerr << "eggs are cooked!\n"; 
		functionIsCalled = true;
	}
}

void testUtilThreadFun()
{
	LASS_COUT << "thread foo ...\n";
	thread_test::functionIsCalled = false;
	util::ScopedPtr<util::Thread> thread(util::threadFun(thread_test::foo, 1, 2, util::threadJoinable));
	thread->run();
	LASS_COUT << "joining\n";
	thread->join();
	LASS_COUT << "joined\n";
	LASS_TEST_CHECK(thread_test::functionIsCalled);

	thread_test::Bar bar;

	LASS_COUT << "thread spam ...\n";
	thread_test::functionIsCalled = false;
	thread.reset(util::threadMemFun(&bar, &thread_test::Bar::spam, util::threadJoinable));
	thread->run();
	LASS_COUT << "joining\n";
	thread->join();
	LASS_COUT << "joined\n";
	LASS_TEST_CHECK(thread_test::functionIsCalled);

	LASS_COUT << "thread ham\n";
	thread_test::functionIsCalled = false;
	thread.reset(util::threadMemFun(&bar, &thread_test::Bar::ham, 3, util::threadJoinable));
	thread->run();
	LASS_COUT << "joining\n";
	thread->join();
	LASS_COUT << "joined\n";
	LASS_TEST_CHECK(thread_test::functionIsCalled);

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC && LASS_COMPILER_VERSION == 1400
#	pragma LASS_FIXME("vc8 hates this overload [Bramz]")
#else
	LASS_COUT << "thread ham via makeCallback ...\n";
	thread_test::functionIsCalled = false;
	thread.reset(util::threadFun(util::makeCallback(&bar, &thread_test::Bar::ham), 3, util::threadJoinable));
	thread->run();
	LASS_COUT << "joining\n";
	thread->join();
	LASS_COUT << "joined\n";
	LASS_TEST_CHECK(thread_test::functionIsCalled);
#endif

	LASS_COUT << "thread eggs ...\n";
	thread_test::functionIsCalled = false;
	util::threadFun(thread_test::eggs, util::threadDetached)->run();
	LASS_TEST_CHECK(!thread_test::functionIsCalled);
	LASS_COUT << "sleeping\n";
	util::Thread::sleep(1000);
	LASS_COUT << "finishing\n";
	LASS_TEST_CHECK(thread_test::functionIsCalled);
}

}

}

#endif

// EOF
