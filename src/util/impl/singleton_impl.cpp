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



#include "util_common.h"
#include "singleton_impl.h"
#include "../atomic.h"
#include <cstdlib>
#include <queue>

namespace lass
{
namespace util
{
namespace impl
{

/** helper class to compare destruction priorities fo lass::util::SingletonBase.
 *  @internal
 *  @author Bram de Greve [Bramz]
 */
class CompareDestructionPriority
{
public:

	bool operator()(SingletonBase* iA, SingletonBase* iB);
};

/** The singleton guard will take care of the destruction of all singletons.
 *  @internal
 *  @author Bram de Greve [Bramz]
 *
 *  All singletons will subscribe them to this guard with a DestructionPriority.  On an explicit
 *  call of killEmAll() (or on destruction of the guard), all singletons will be destructed.
 *  The singletons with the highest DestructionPriority will be killed first.  The order in which
 *  singletons of the same priority are destructed is undefined.
 *
 *  The system of using this destruction priority controlled by a guard that is destructed by
 *  @c ::atexit() , is inspired by Alexandrescu's @e longevity singletons [1].
 *
 *  @warning you should never call @c killEmAll() yourself!
 *
 *  @b Reference:
 *  -# ALEXANDRESCU A. (2001), <i>Modern C++ Design: Generic Programming and Design Patterns
 *     applied</i>, C++ in depth series, Addison-Wesley, pages 129-156
*/
class SingletonGuard
{
public:

	~SingletonGuard();

	void subscribe(SingletonBase* iSingleton);

	static SingletonGuard* instance();

private:

	SingletonGuard() {}

	/** Special semaphore for the guard.
	 *  @internal
	 *  The reason we need a custom semaphore implemented around a static int is because
	 *	the util::Semaphore is susceptible to the static initialization order fiasco.
	 *  To solve that, we need something that can be initialized statically (baked in
	 *	executable), and that would be - in this case - an int.  It will appear to have
	 *	been one since the beginning of the ages.  Case solved.
	 */
	class CustomSemaphore
	{
	public:
		CustomSemaphore()
		{
			int oldSlots, newSlots;
			do
			{
				oldSlots = freeSlots_;
				LASS_ASSERT(oldSlots >= 0);
				newSlots = oldSlots - 1;
			}
			while (oldSlots == 0 || !atomicCompareAndSwap(freeSlots_, oldSlots, newSlots));
		}
		~CustomSemaphore()
		{
			atomicIncrement(freeSlots_); 
		}
	private:
		static int freeSlots_;
	};

	typedef std::priority_queue
		<SingletonBase*, std::vector<SingletonBase*>, CompareDestructionPriority> TDeathRow;

	TDeathRow deathRow_;

	static void killEmAll();
	static bool deadReference(bool iSetReferenceToDead = false);

	static SingletonGuard* instance_;
	
};

SingletonGuard* SingletonGuard::instance_ = 0;
int SingletonGuard::CustomSemaphore::freeSlots_ = 1;



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



/** Subscribe to the singleton guard.
 *  Do this only once for each singleton, and in fact, don't do it yourself at all
 *  since Singleton<> already does it :)
 *
 *  @warning this isn't thread safe as it is, but that's ok because its only caller 
 *		Singleton::instance() is already locking on SingletonGuard level.
 */
void SingletonBase::subscribeInstance(int iDestructionPriority)
{
	destructionPriority_ = iDestructionPriority;
	if (SingletonGuard* guard = SingletonGuard::instance())
	{
		guard->subscribe(this);
	}
}



/** Return true if iA has to be killed before iB.
 */
bool CompareDestructionPriority::operator()(SingletonBase* iA, SingletonBase* iB)
{
	LASS_ASSERT(iA != 0 && iB != 0);
	return iA->destructionPriority() < iB->destructionPriority();
}



/** on destruction, you have to kill all singletons.
 *  
 *	This isn't thread safe as it is, but that's ok because it's only "caller" singletonCleanUp()
 *	is already locking on SingletonGuard level for us.
 */
SingletonGuard::~SingletonGuard()
{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL && !defined(_DEBUG)
#	pragma LASS_FIXME("~SingletonGuard causes access violation => hacked: leak resources")
#else
	while (!deathRow_.empty())
	{
		SingletonBase* deadManWalking = deathRow_.top();
		deathRow_.pop();
		delete deadManWalking;
	}
#endif
	deadReference(true);
}



/** subscribe singleton to the destruction list
 *  
 *	This isn't thread safe as it is, but that's ok because it's only indirect caller 
 *	Singleton::instance() is already locking on SingletonGuard level for us.
 */
void SingletonGuard::subscribe(SingletonBase* iSingleton)
{
	deathRow_.push(iSingleton);
}



/** return the SingletonGuard singleton instance =)
 */
SingletonGuard* SingletonGuard::instance()
{
	if (deadReference(false))
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR: Usage of dead reference to SingletonGuard!"
			<< std::endl;
		return 0;
	}

	if (!instance_)
	{
		CustomSemaphore lock;
		if (!instance_) // double check
		{
			instance_ = new SingletonGuard;
			::atexit(&SingletonGuard::killEmAll);
		}
	}

	return instance_;
}



/** kills singleton guard and all its guarded singleton.
 *  @relates SingletonGuard
 *
 *  This function is subscribed to ::atexit to kill all singletons at exit of the application<
 */
void SingletonGuard::killEmAll()
{
	CustomSemaphore lock;
	delete instance_;
	instance_ = 0;
}



/** return true if singleton guard is destructed.
 *  @param iSetReferenceToDead - call this method with true on destruction of singleton
 *                              - call this method with false to check it.
 */
bool SingletonGuard::deadReference(bool iSetReferenceToDead)
{
	static bool dead = false;

	if (iSetReferenceToDead)
	{
		dead = true;
	}

	return dead;
}



}

}

}
