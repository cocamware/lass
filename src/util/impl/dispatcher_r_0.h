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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_R_0_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_R_0_H

#include "../small_object.h"

// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{
namespace impl
{

/** abstract base class of all dispatchers for lass::util::CallbackR0.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template
<
	typename R
>
class DispatcherR0: public SmallObject<>
{
public:

	DispatcherR0() {}
	virtual R operator()() const = 0;

private:
	DispatcherR0(const DispatcherR0<R>& iOther);
	DispatcherR0& operator=(const DispatcherR0<R>& iOther);
};



/** Dispatcher for lass::util::CallbackR0 to a free function:
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template
<
	typename R,
	typename S
>
class DispatcherR0Function: public DispatcherR0<R>
{
private:

	typedef S (*TFunction) ();
	TFunction function_;

public:

	DispatcherR0Function(TFunction iFunction):
		function_(iFunction)
	{
	}

	R operator()() const
	{
		return (*function_)();
	}
};



/** Dispatcher for lass::util::CallbackR0 to an object/method pair.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template
<
	class Object,
	typename R,
	typename S
>
class DispatcherR0Method: public DispatcherR0<R>
{
private:

	typedef S (Object::*TMethod) ();
	Object* object_;
	TMethod method_;

public:

	DispatcherR0Method(Object* iObject, TMethod iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

	R operator()() const
	{
		return (object_->*method_)();
	}
};



/** Dispatcher for lass::util::CallbackR0 to an object/const method pair.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template
<
	class Object,
	typename R,
	typename S
>
class DispatcherR0ConstMethod: public DispatcherR0<R>
{
private:

	typedef S (Object::*TConstMethod) () const;
	Object* object_;
	TConstMethod method_;

public:

	DispatcherR0ConstMethod(Object* iObject, TConstMethod iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

	R operator()() const
	{
		return (object_->*method_)();
	}
};



}

}

}

#endif // Guardian of Inclusion


// EOF
