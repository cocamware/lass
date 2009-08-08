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
 *	Copyright (C) 2004-2009 the Initial Developer.
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
#include "../singleton.h"
#include "../thread.h"
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
	bool operator()(SingletonBase* a, SingletonBase* b) const
	{
		LASS_ASSERT(a != 0 && b != 0);
		return a->destructionPriority() < b->destructionPriority();
	}
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
	void subscribe(SingletonBase* singleton)
	{
		LASS_LOCK_INTEGRAL(lock_)
		{
			deathRow_.push(singleton);
		}
	}
	static SingletonGuard* instance()
	{
		LASS_LOCK_INTEGRAL(lock_)
		{
			if (isDeadReference_)
			{
				std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR: Usage of dead reference to SingletonGuard!" << std::endl;
				return 0;
			}
			if (!instance_)
			{
				instance_ = new SingletonGuard;
				::atexit(&SingletonGuard::killEmAll);
			}
		}
		return instance_;
	}

private:
	typedef std::priority_queue<SingletonBase*, std::vector<SingletonBase*>, CompareDestructionPriority> TDeathRow;

	SingletonGuard() 
	{
	}
	~SingletonGuard()
	{
		while (!deathRow_.empty())
		{
			SingletonBase* deadManWalking = deathRow_.top();
			deathRow_.pop();
			delete deadManWalking;
		}
	}
	static void killEmAll()
	{
		// this should be called only once from ::atexit.
		LASS_ASSERT(!isDeadReference_);
		isDeadReference_ = true;
		delete instance_;
		instance_ = 0;
	}

	TDeathRow deathRow_;
	static SingletonGuard* instance_;
	static bool isDeadReference_;
	static int lock_;	
};

SingletonGuard* SingletonGuard::instance_ = 0;
bool SingletonGuard::isDeadReference_ = false;
int SingletonGuard::lock_ = 1;





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
 *		Singleton::instance() is already locking.
 */
void SingletonBase::subscribeInstance(int destructionPriority)
{
	destructionPriority_ = destructionPriority;
	if (destructionPriority_ == destructionPriorityNever)
	{
		return;
	}
	if (SingletonGuard* guard = SingletonGuard::instance())
	{
		guard->subscribe(this);
	}
}



}
}
}

// EOF

