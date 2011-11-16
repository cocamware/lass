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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#include "../lass/util/thread_fun.h"
#include "../lass/util/scoped_ptr.h"

namespace lass
{
namespace test
{
namespace thread_test
{
	volatile bool functionIsCalled = false;

	void foo(int, int) 
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
		void ham(int) const 
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

TUnitTest test_util_thread_fun()
{
	return TUnitTest(1, LASS_TEST_CASE(testUtilThreadFun));
}

}

}

// EOF
