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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_0_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_0_H

#include "dispatcher_allocator.h"

// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{
namespace impl
{

/** abstract base class of all dispatchers for lass::util::Callback0.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
class Dispatcher0: public TDispatcherAllocatorBase
{
public:

	Dispatcher0() {}
	void call() const { doCall(); }
	bool isEquivalent(const Dispatcher0* iOther) const { return doIsEquivalent(iOther); }

private:

	virtual void doCall() const = 0;
	virtual bool doIsEquivalent(const Dispatcher0* iOther) const = 0;

	Dispatcher0(const Dispatcher0& iOther);
	Dispatcher0& operator=(const Dispatcher0& iOther);
};



/** Dispatcher for lass::util::Callback0 to a fuction or equivalent callable entity.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template 
<
	typename Function
>
class Dispatcher0Function: public Dispatcher0
{
public:

	typedef Dispatcher0Function<Function> TSelf;
	typedef Function TFunction;

	Dispatcher0Function(typename CallTraits<TFunction>::TParam iFunction):
		function_(iFunction)
	{
	}

private:

	void doCall() const
	{
		function_();
	}

	bool doIsEquivalent(const Dispatcher0* iOther) const
	{
		const TSelf* other = dynamic_cast<const TSelf*>(iOther);
		return other && function_ == other->function_;
	}

	TFunction function_;
};



/** Dispatcher for lass::util::Callback0 to an object/method pair.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template 
<
	typename ObjectPtr,
	typename Method
>
class Dispatcher0Method: public Dispatcher0
{
public:

	typedef Dispatcher0Method<ObjectPtr, Method> TSelf;
	typedef ObjectPtr TObjectPtr;
	typedef Method TMethod;

	Dispatcher0Method(typename CallTraits<TObjectPtr>::TParam iObject, 
					  typename CallTraits<TMethod>::TParam iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

private:

	void doCall() const
	{
		(object_->*method_)();
	}

	bool doIsEquivalent(const Dispatcher0* iOther) const
	{
		const TSelf* other = dynamic_cast<const TSelf*>(iOther);
		return other && object_ == other->object_ && method_ == other->method_;
	}

	TObjectPtr object_;
	TMethod method_;
};



}

}

}

#endif // Guardian of Inclusion


// EOF
