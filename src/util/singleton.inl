/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SINGLETON_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SINGLETON_INL
#pragma once

#include "singleton.h"
#include "thread.h"

namespace lass
{
namespace util
{

/** destroy and deallocate all current singletons.
 *  @relates Singleton
 *  to be used with care!
 *  @warning this isn't thread safe
 */
inline void destructSingletons()
{
	impl::singletonCleanUp();
}



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
    static TSelf* neo = 0;
	
	if (deadReference(false))
	{
		std::cerr << "Dead reference detected at '" << neo << "' of singleton '" << typeid(T).name()
			<< "' with destruction priority '" << DP << "'";
		return 0;
	}

	// if instance hasn't been created yet, you might want to do it now :)
	if (neo == 0)
	{
		initCriticalSection();	
		LASS_LOCK(*criticalSection_)
		{
			if (neo == 0)
			{
				neo = new TSelf;
				neo->subscribeInstance(DP);
			}
		}
	}

	return neo->instance_.get();
}


/** return true if singleton is destructed.
 *  @param iSetReferenceToDead - call this method with true on destruction of singleton
 *                              - call this method with false to check it.
 */
template <class T, int DP>
bool Singleton<T, DP>::deadReference(bool iSetReferenceToDead)
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

#endif
