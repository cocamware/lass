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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_SINGLETON_IMPL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_SINGLETON_IMPL_H

#include "../util_common.h"
#include "../non_copyable.h"
#include "../thread.h"

#include <queue>

namespace lass
{
namespace util
{
namespace impl
{

/** @internal
 */
typedef CriticalSection TSingletonLock;

/** @internal
 */
LASS_DLL void LASS_CALL singletonCleanUp();

/** base class of all singletons.
 *  @internal
 *  @author Bram de Greve [Bramz]
 */
class LASS_DLL SingletonBase: NonCopyable
{
public:

	SingletonBase();
	virtual ~SingletonBase();

	int destructionPriority() const;

	static void initLock();  ///< do NOT call this yourself, implementation detail!
	static void cleanLock();    ///< do NOT call this yourself, implementation detail!

protected:

	void subscribeInstance(int iDestructionPriority);

	static TSingletonLock* lock_;

private:

	int destructionPriority_;
};



/** helper class to compare destruction priorities fo lass::util::SingletonBase.
 *  @internal
 *  @author Bram de Greve [Bramz]
 */
class LASS_DLL CompareDestructionPriority
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
class LASS_DLL SingletonGuard
{
public:

	~SingletonGuard();

	void subscribe(SingletonBase* iSingleton);

private:

	typedef std::priority_queue
		<SingletonBase*, std::vector<SingletonBase*>, CompareDestructionPriority> TDeathRow;

	TDeathRow deathRow_;
	TSingletonLock lock_;
};

}

}

}

LASS_EXECUTE_BEFORE_MAIN_EX(
	lassUtilImplSingletonBase,
	lass::util::impl::SingletonBase::initLock();
)

#endif
