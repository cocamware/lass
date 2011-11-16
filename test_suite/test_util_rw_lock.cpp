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
#include "../lass/util/rw_lock.h"

namespace lass
{
namespace test
{
namespace rwlock_test
{	
	class Bar
	{
	public:
		Bar() : lock_(10) {}
		void doReads() 
		{
			for (int i=0;i<1000;++i)
			{
				util::Thread::sleep(1);
				lock_.lockr();
				std::cout << "R";
				lock_.unlockr();
			}
		}
		void doWrites() const 
		{ 
			for (int j=0;j<4;++j)
			{
				util::Thread::sleep(150);
				for (int i=0;i<16;++i)
				{
					lock_.lockw();
					util::Thread::sleep(5);
					std::cout << "(W)";
					lock_.unlockw();
				}
			}
		}
	private:
		mutable util::RWLock lock_;
	};

}

void testUtilRWLock()
{
	rwlock_test::Bar bar;

	LASS_COUT << "thread spam ...\n";
	util::ScopedPtr<util::Thread> readThread(util::threadMemFun(&bar, &rwlock_test::Bar::doReads, util::threadJoinable));
	util::ScopedPtr<util::Thread> writeThread(util::threadMemFun(&bar, &rwlock_test::Bar::doWrites, util::threadJoinable));
	readThread->run();
	writeThread->run();
	LASS_COUT << "joining\n";
	readThread->join();
	writeThread->join();
	LASS_COUT << "joined\n";

}

TUnitTest test_util_rw_lock()
{
	return TUnitTest(1, LASS_TEST_CASE(testUtilRWLock));
}

}

}

// EOF
