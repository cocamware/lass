/** @file
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



#include "util_common.h"
#include "singleton_impl.h"
//#include "../singleton.h"
#include <cstdlib>

namespace
{

lass::util::impl::SingletonGuard* singletonGuard = 0;

}

namespace lass
{
namespace util
{
namespace impl
{

TSingletonLock* SingletonBase::lock_ = 0;

/** kills singleton guard and all its guarded singleton.
 *  @relates SingletonGuard
 *
 *  @warning this isn't thread safe, but that's OK since it should only be called once from
 *      @c ::atexit.  How do we know this?  Well, for one thing we assume the user doesn't
 *      subscribe it to @c ::atexit himself.  Secondly, the function that does subscribe it
 *      to @c ::atexit is the constructor of SingletonGuard, and only one instance of this
 *      will be created, granted SingletonBase::subscribeInstance is thread safe and the only
 *      one doing this!
 */
void singletonCleanUp()
{
	delete singletonGuard;
	SingletonBase::cleanLock();
}



/** constructor.
 *  @warning The singleton base DOES NOT subsribes itself at construction.  Singleton<> must do this
 *           at the construction of a new singleton.  Why?  We better don't do it here, because
 *           the singleton is being constructed!  it might not be complete yet.
 */
SingletonBase::SingletonBase():
	destructionPriority_(0)
{
}



SingletonBase::~SingletonBase()
{
}



int SingletonBase::destructionPriority() const
{
	return destructionPriority_;
}



void SingletonBase::initLock()
{
	if (lock_ == 0)
	{
		lock_ = new TSingletonLock;
	}
}



void SingletonBase::cleanLock()
{
	delete lock_;
	lock_ = 0;
}



/** Subscribe to the singleton guard.
 *  Do this only once for each singleton, and in fact, don't do it yourself at all
 *  since Singleton<> already does it :)
 *
 *  @warning this is an implementation detail, don't use it yourself (unless you know damn
 *      good what you're doing).  This isn't thread safe, but that's OK since the one calling
 *      this is Singleton::instance and that one is safe over all singletons!  (all singletons
 *      share one critical section declared in SingletonBase.
 */
void SingletonBase::subscribeInstance(int iDestructionPriority)
{
	destructionPriority_ = iDestructionPriority;
	if (!singletonGuard)
	{
		singletonGuard = new SingletonGuard;
		::atexit(singletonCleanUp);
	}

	singletonGuard->subscribe(this);
}



/** Return true if iA has to be killed before iB.
 */
bool CompareDestructionPriority::operator()(SingletonBase* iA, SingletonBase* iB)
{
	LASS_ASSERT(iA != 0 && iB != 0);
	return iA->destructionPriority() < iB->destructionPriority();
}



/** on destruction, you have to kill all singletons.
 *  @warning not thread safe but that's OK since only killEmAll() will cause destruction,
 *      so the same reasoning applies.
 */
SingletonGuard::~SingletonGuard()
{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL && !defined(_DEBUG)
#	pragma LASS_FIXME("~SingletonGuard causes access violation => hacked: leak resources")
#else
	while (!deathRow_.empty())
	{
		SingletonBase* deadManWalking = deathRow_.top();
		if (!deadManWalking)
		{
			std::cerr << "WARNING: NULL pointer in lass SingletonGuard::deathRow_ detected\n";
		}

		deathRow_.pop();
		delete deadManWalking;
	}
#endif
}



/** @warning not thread safe but that's OK since we assue only
 *      SingletonBase::subscribeInstance calls this, so the same reasoning applies.
 */
void SingletonGuard::subscribe(SingletonBase* iSingleton)
{
	deathRow_.push(iSingleton);
}



}

}

}
