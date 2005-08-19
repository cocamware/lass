/** @file
 *  @internal
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
	bool functionIsCalled = false;

	void foo(int a, int b) { functionIsCalled = true; }
	class Bar
	{
	public:
		void spam() { functionIsCalled = true; }
		void ham(int a) const { functionIsCalled = true; }
	};

	void eggs() { Sleep(10); functionIsCalled = true;}
}

void testUtilThreadFun()
{
	thread_test::functionIsCalled = false;
	util::ScopedPtr<util::Thread> thread(util::threadFun(thread_test::foo, 1, 2, util::THREAD_JOINABLE));
	thread->wait();
	BOOST_CHECK(thread_test::functionIsCalled);

	thread_test::Bar bar;

	thread_test::functionIsCalled = false;
	thread.reset(util::threadMemFun(&bar, thread_test::Bar::spam, util::THREAD_JOINABLE));
	thread->wait();
	BOOST_CHECK(thread_test::functionIsCalled);

	thread_test::functionIsCalled = false;
	thread.reset(util::threadMemFun(&bar, thread_test::Bar::ham, 3, util::THREAD_JOINABLE));
	thread->wait();
	BOOST_CHECK(thread_test::functionIsCalled);

	thread_test::functionIsCalled = false;
	thread.reset(util::threadFun(util::makeCallback(&bar, thread_test::Bar::ham), 3, util::THREAD_JOINABLE));
	thread->wait();
	BOOST_CHECK(thread_test::functionIsCalled);


	thread_test::functionIsCalled = false;
	util::threadFun(thread_test::eggs);
	BOOST_CHECK(!thread_test::functionIsCalled);
	Sleep(100);
	BOOST_CHECK(thread_test::functionIsCalled);
}

}

}

#endif

// EOF
