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
 *  @sa CallbackR0
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
	R call() const { return doCall(); }

private:

	virtual R doCall() const = 0;

	DispatcherR0(const DispatcherR0<R>& iOther);
	DispatcherR0& operator=(const DispatcherR0<R>& iOther);
};



/** Dispatcher for lass::util::CallbackR0 to a free function:
 *  @internal
 *  @sa CallbackR0
 *  @author Bram de Greve [Bramz]
 */
template
<
	typename R,
	typename FunctionType
>
class DispatcherR0Function: public DispatcherR0<R>
{
public:

	typedef FunctionType TFunction;

	DispatcherR0Function(typename CallTraits<TFunction>::TParam iFunction):
		function_(iFunction)
	{
	}

private:

	R doCall() const
	{
		return function_();
	}

	TFunction function_;
};



/** Dispatcher for lass::util::CallbackR0 to an object/method pair.
 *  @internal
 *  @sa CallbackR0
 *  @author Bram de Greve [Bramz]
 */
template
<
	typename R,
	typename ObjectPtr,
	typename Method
>
class DispatcherR0Method: public DispatcherR0<R>
{
public:

	DispatcherR0Method(typename CallTraits<ObjectPtr>::TParam iObject,
					   typename CallTraits<Method>::TParam iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

private:

	R doCall() const
	{
		return (object_->*method_)();
	}

	ObjectPtr object_;
	Method method_;
};

}

}

}

#endif // Guardian of Inclusion


// EOF
