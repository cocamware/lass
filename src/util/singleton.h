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

/** @class lass::util::Singleton
 *  @brief non-intrusive singleton holder.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  With this class, it's possible to create a singleton of a class Foo.  It's a non-instrusive
 *  singleton because it's not required to derive Foo from this Singleton class.  Instead,
 *  this class will be a holder to a singleton object of Foo.  To use class Foo as a singleton
 *  is very straight forward, but can be done in a couple of ways:
 *
 *  1. The @e typedef way: With a typedef, you create a new type that will act as singleton holder
 *     for class @c Foo.  To access the singleton, you must now use the @c instance() method on
 *     that new type:
 *
 *  @code
 *  typedef lass::util::Singleton<Foo> TFooSingleton;
 *
 *  TFooSingleton()::instance()->bar(fun);
 *  @endcode
 *
 *  2. The @e function way (@b prefered/b>): you write a nice little function that returns a pointer
 *     to the singleton.  To access the singleton, you simple call that function:
 *
 *  @code
 *  Foo* foo()
 *  {
 *      return lass::util::Singleton<Foo>::instance();
 *  }
 *
 *  foo()->bar(fun);
 *  @endcode
 *
 *  Both ways only differ in how to access the instance.  All other properties are common:
 *
 *  - The instance of the singleton is constructed on the heap on first access on the heap.  i.e.
 *    the first time in the program @c TFooSingleton::instance() or @c foo() is called, a new object
 *    of @c Foo is allocated on the heap, and a pointer to it is returned.  All the following calls
 *    will return that same pointer.  This is the way how to control the order of construction:
 *    on first access.
 *
 *  - The order of destruction can be set by the @c DestructionPriority.  Singletons with a
 *    smaller destruction priority will be destructed later than those with a higher destruction
 *    priority.  It's not specified in which order singletons with identical destruction priorities
 *    will be destructed (i.e. its not guaranteed for such singletons that the last one constructed
 *    will be the first to be destroyed).  You can set the destruction priority by the second
 *    template argument of @c Singleton: @c lass::util::Singleton<Foo,2000> will specify the
 *    singleton to have a destruction priority of 2000.  The default destruction priority is set to
 *    be 1000.  Priorities below 500 are reserved for the Lass implementation (and for you who
 *    know what you're doing ;)
 *
 *    @b warning: lass::io::proxyMan() is a proxy man that is needed for the implementation of
 *    all logging activities etc.  Its destruction priority is set to 0 and there should be
 *    no other singleton have a destruction priority lower or equal than this.
 *
 *  - Requirements for the class @c Foo: Since it has to be constructed on the heap without any
 *    parameters in the neighbourhoud, it has to have a default constructor.
 *
 *  @b Reference:
 *  -# ALEXANDRESCU A. (2001), <i>Modern C++ Design: Generic Programming and Design Patterns
 *     applied</i>, C++ in depth series, Addison-Wesley, pages 129-156
 *  -# GAMMA E., HELM R., JOHNSON R. & VLISSIDES J. (1995), <i>Design Patters: Elements of
 *     Reusable Object-Oriented Software</i>, Addison-Wesley.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SINGLETON_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SINGLETON_H

#include "util_common.h"
#include "scoped_ptr.h"
#include "impl/singleton_impl.h"

namespace lass
{
namespace util
{

/** Destruction priority constants.
 *	@relates Singleton
 *	Priorities levels below singletonDestructionPriorityBeginUserRange (=500) are reserved for the 
 *	implementation of Lass, and should not be used by user code (unless you know what you're doing
 *	of course ;)
 */
enum DestructionPriorities
{
	destructionPriorityDefault = 1000, /**< default priority level =) */
	destructionPriorityBeginUser = 500, /**< lowest legal level for user code */

	// the following levels are internal and should not be used by user code
	//
	destructionPriorityInternalProxyMan = 0, /**< @internal */
	destructionPriorityInternalPythonMutex = 50, /**< @internal */
	destructionPriorityInternalTlsDestructors = 100, /**< @internal */
	destructionPriorityInternalAllocators = 200, /**< @internal */
	destructionPriorityInternalTlsLocalsMain = 400, /**< @internal */
};

template<class T, int destructPriority = destructionPriorityDefault>
class Singleton: public impl::SingletonBase
{
public:

	typedef Singleton<T, destructPriority> TSelf;

	typedef T TInstance;
	enum { destructionPriority = destructPriority };

	Singleton();
	virtual ~Singleton();

	static TInstance* instance();

private:

	static bool deadReference(bool iSetReferenceToDead = false);

	ScopedPtr<TInstance> instance_;
};

}
}

#include "singleton.inl"

#endif

// EOF

