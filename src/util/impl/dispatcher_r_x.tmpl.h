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
 *
 *  $g
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_R_$x_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_R_$x_H

#include "../small_object.h"

// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{
namespace impl
{

/** abstract base class of all dispatchers for lass::util::CallbackR$x.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template
<
	typename R, $(typename P$x)$
>
class DispatcherR$x: public SmallObject<>
{
public:

	DispatcherR$x() {}
	virtual R operator()($(P$x iP$x)$) const = 0;

private:
	DispatcherR$x(const DispatcherR$x<R, $(P$x)$>& iOther);
	DispatcherR$x& operator=(const DispatcherR$x<R, $(P$x)$>& iOther);
};



/** Dispatcher for lass::util::CallbackR$x to a free function:
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template
<
	typename R, $(typename P$x)$,
	typename S, $(typename Q$x)$
>
class DispatcherR$xFunction: public DispatcherR$x<R, $(P$x)$>
{
private:

	typedef S (*TFunction) ($(Q$x)$);
	TFunction function_;

public:

	DispatcherR$xFunction(TFunction iFunction):
		function_(iFunction)
	{
	}

	R operator()($(P$x iP$x)$) const
	{
		return (*function_)($(iP$x)$);
	}
};



/** Dispatcher for lass::util::CallbackR$x to an object/method pair.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template
<
	class Object,
	typename R, $(typename P$x)$,
	typename S, $(typename Q$x)$
>
class DispatcherR$xMethod: public DispatcherR$x<R, $(P$x)$>
{
private:

	typedef S (Object::*TMethod) ($(Q$x)$);
	Object* object_;
	TMethod method_;

public:

	DispatcherR$xMethod(Object* iObject, TMethod iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

	R operator()($(P$x iP$x)$) const
	{
		return (object_->*method_)($(iP$x)$);
	}
};



/** Dispatcher for lass::util::CallbackR$x to an object/const method pair.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template
<
	class Object,
	typename R, $(typename P$x)$,
	typename S, $(typename Q$x)$
>
class DispatcherR$xConstMethod: public DispatcherR$x<R, $(P$x)$>
{
private:

	typedef S (Object::*TConstMethod) ($(Q$x)$) const;
	Object* object_;
	TConstMethod method_;

public:

	DispatcherR$xConstMethod(Object* iObject, TConstMethod iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

	R operator()($(P$x iP$x)$) const
	{
		return (object_->*method_)($(iP$x)$);
	}
};



}

}

}

#endif // Guardian of Inclusion


// EOF
