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

#include "lass_common.h"
#include "../atomic.h"

#ifdef LASS_UTIL_ATOMIC_HAVE_POOR_MANS_IMPL

#if LASS_HAVE_LDREX_STREX || LASS_KUSER_HELPER_VERSION >= 2
#   if LASS_HAVE_LDREXB_STREXB && LASS_HAVE_LDREXH_STREXH
#       if LASS_HAVE_LDREXD_STREXD || LASS_KUSER_HELPER_VERSION >= 5
            // all should be ok (except for 8-byte adjacent CAS, which shouldn't be used anyway, can we protect against that?)
#       else
#           warning "[LASS BUILD MSG] Poor man's implementation of 4-byte adjacent CAS (used in TaggedPtr), and all 8-byte atomics. Performance will suffer!"
#       endif
#   elif LASS_HAVE_LDREXD_STREXD || LASS_KUSER_HELPER_VERSION >= 5
#       warning "[LASS BUILD MSG] Poor man's implementation of 1- and 2-byte atomics (except 2-byte adjacent CAS). All 4- and 8-byte atomics are good."
#   else
#       warning "[LASS BUILD MSG] Poor man's implementation of 1-, 2- and 8-byte atomics, and 4-byte adjacent CAS (used in TaggedPtr). Performance will suffer!"
#   endif
#else
#   warning "[LASS BUILD MSG] Poor man's implementation of ALL atomics using a mutex! performance will SEVERELY suffer!"
#endif

#if LASS_KUSER_HELPER_VERSION >= 2

namespace
{
	typedef int (kuser_cmpxchg_t)(int oldval, int newval, volatile int *ptr);
	#define kuser_cmpxchg (*(kuser_cmpxchg_t *)0xffff0fc0)

	enum
	{
		unlocked = 0,
		locked = 1,
	};
	volatile int globalLock = unlocked;
}

namespace lass
{
namespace util
{
namespace impl
{

PoorMansGlobalAtomicLock::PoorMansGlobalAtomicLock()
{
	int old;
	do
	{
		old = globalLock;
		if ( old == locked )
		{
			continue;
		}
	}
	while (kuser_cmpxchg(old, locked, &globalLock));
}

PoorMansGlobalAtomicLock::~PoorMansGlobalAtomicLock()
{
	LASS_ASSERT(globalLock == locked);
	kuser_cmpxchg(locked, unlocked, &globalLock);
};

}
}
}

#else

#include "../thread.h"

namespace
{

using ::lass::util::Mutex;

Mutex* globalMutex()
{
	static Mutex* mutex = 0;
	if (!mutex)
	{
		mutex = new Mutex;
	}
	return mutex;
}

Mutex* const initializeBeforeMain = globalMutex();

}

namespace lass
{
namespace util
{
namespace impl
{

PoorMansGlobalAtomicLock::PoorMansGlobalAtomicLock()
{
	globalMutex()->lock();
}

PoorMansGlobalAtomicLock::~PoorMansGlobalAtomicLock()
{
	globalMutex()->unlock();
};

}
}
}

#endif

#endif

// EOF
