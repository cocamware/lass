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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SINGLETON_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SINGLETON_INL


#include "singleton.h"
#include "thread.h"
#include <atomic>

namespace lass
{
namespace util
{


template <class T, int DP>
Singleton<T, DP>::Singleton():
	impl::SingletonBase(),
	instance_(new TInstance)
{
}



template <class T, int DP>
Singleton<T, DP>::~Singleton()
{
	deadReference(true);
}



/** Return pointer to singleton instance.
 *  An instance is created on first call.
 *  Return NULL if instance is dead already (after first destruction of singletons).
 *  @note this should be thread safe over all singletons.
 */
template <class T, int DP>
typename Singleton<T, DP>::TInstance* Singleton<T, DP>::instance()
{
	static std::atomic<TSelf*> neo{ nullptr };
	static std::atomic<int> semaphore{ 1 };

	if (deadReference(false))
	{
		std::cerr << "[LASS RUN MSG] Dead reference detected at '" << neo.load(std::memory_order_relaxed) 
			<< "' of singleton '" << typeid(TInstance).name() << "' with destruction priority '"
			<< destructionPriority << "'" << std::endl;
		return nullptr;
	}

	TSelf* one = neo.load(std::memory_order_acquire);
	if (!one)
	{
		LASS_LOCK_INTEGRAL(semaphore)
		{
			one = neo.load(std::memory_order_relaxed);
			if (!one)
			{
				one = new TSelf;
				one->subscribeInstance(destructionPriority);
				neo.store(one, std::memory_order_release);
			}
		}
	}
	LASS_ASSERT(one);
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(suppress: 6011) // Dereferencing NULL pointer 'one'
#endif
	return one->instance_.get();
}



/** return true if singleton is destructed.
 *  @param iSetReferenceToDead - call this method with true on destruction of singleton
 *                              - call this method with false to check it.
 */
template <class T, int DP>
bool Singleton<T, DP>::deadReference(bool setReferenceToDead)
{
	static std::atomic<int> dead{ false };
	if (setReferenceToDead)
	{
		dead.store(true, std::memory_order_release);
		return true;
	}
	else
	{
		return dead.load(std::memory_order_acquire);
	}
}


}
}

#endif
