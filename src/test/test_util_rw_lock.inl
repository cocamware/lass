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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_RW_LOCK_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_RW_LOCK_INL

#include "test_common.h"

#include "../util/thread_fun.h"
#include "../util/scoped_ptr.h"
#include "../util/rw_lock.h"

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
				util::Thread::sleep(2);
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
					util::Thread::sleep(1);
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

}

}

#endif

// EOF
