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



#include "lass_common.h"
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
#pragma LASS_TODO("Examine if util::Semaphore is indeed scusceptilbe to SIOF [Bramz]")
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
