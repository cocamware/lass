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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_$x_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_$x_H

#include "../small_object.h"

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
template <$(typename P$x)$>
class Dispatcher$x: public SmallObject<>
{
public:

	Dispatcher$x() {}
	virtual void operator()($(typename util::CallTraits<P$x>::TParam iP$x)$) const = 0;

private:
	Dispatcher$x(const Dispatcher$x<$(P$x)$>& iOther);
	Dispatcher$x& operator=(const Dispatcher$x<$(P$x)$>& iOther);
};



/** Dispatcher for lass::util::Callback$x to a free function:
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template<$(typename P$x)$, $(typename Q$x)$>
class Dispatcher$xFunction: public Dispatcher$x<$(P$x)$>
{
private:

	typedef void (*TFunction) ($(Q$x)$);
	TFunction function_;

public:

	Dispatcher$xFunction(TFunction iFunction):
		function_(iFunction)
	{
	}

	void operator()($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		(*function_)($(iP$x)$);
	}
};



/** Dispatcher for lass::util::Callback$x to an object/method pair.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template <class Object, $(typename P$x)$, $(typename Q$x)$>
class Dispatcher$xMethod: public Dispatcher$x<$(P$x)$>
{
private:

	typedef void (Object::*TMethod) ($(Q$x)$);
	Object* object_;
	TMethod method_;

public:

	Dispatcher$xMethod(Object* iObject, TMethod iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

	void operator()($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		(object_->*method_)($(iP$x)$);
	}
};



/** Dispatcher for lass::util::Callback$x to an object/const method pair.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template <class Object, $(typename P$x)$, $(typename Q$x)$>
class Dispatcher$xConstMethod: public Dispatcher$x<$(P$x)$>
{
private:

	typedef void (Object::*TConstMethod) ($(Q$x)$) const;
	Object* object_;
	TConstMethod method_;

public:

	Dispatcher$xConstMethod(Object* iObject, TConstMethod iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

	void operator()($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		(object_->*method_)($(iP$x)$);
	}
};



}

}

}

#endif // Guardian of Inclusion


// EOF
