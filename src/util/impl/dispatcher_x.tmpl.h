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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_$x_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_$x_H

#include "dispatcher_allocator.h"

// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{
namespace impl
{

/** abstract base class of all dispatchers for lass::util::Callback$x.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template 
<
	$(typename P$x)$
>
class Dispatcher$x: public TDispatcherAllocatorBase
{
public:

	Dispatcher$x() {}
	void call($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		doCall($(iP$x)$);
	}

private:

	virtual void doCall($(typename util::CallTraits<P$x>::TParam iP$x)$) const = 0;

	Dispatcher$x(const Dispatcher$x<$(P$x)$>& iOther);
	Dispatcher$x& operator=(const Dispatcher$x<$(P$x)$>& iOther);
};



/** Dispatcher for lass::util::Callback$x to a free function:
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template
<
	$(typename P$x)$, 
	typename FunctionType
>
class Dispatcher$xFunction: public Dispatcher$x<$(P$x)$>
{
public:

	typedef FunctionType TFunction;

	Dispatcher$xFunction(typename CallTraits<TFunction>::TParam iFunction):
		function_(iFunction)
	{
	}

private:

	void doCall($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		function_($(iP$x)$);
	}

	TFunction function_;
};



/** Dispatcher for lass::util::Callback$x to an object/method pair.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template 
<
	$(typename P$x)$, 
	typename ObjectPtr, typename Method
>
class Dispatcher$xMethod: public Dispatcher$x<$(P$x)$>
{
public:

	Dispatcher$xMethod(typename CallTraits<ObjectPtr>::TParam iObject,
					   typename CallTraits<Method>::TParam iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

private:

	void doCall($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		((*object_).*method_)($(iP$x)$);
	}

	ObjectPtr object_;
	Method method_;
};



}

}

}

#endif // Guardian of Inclusion


// EOF
