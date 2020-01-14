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
 *	Copyright (C) 2004-2020 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_$x_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_$x_H

#include "dispatcher_allocator.h"
#include "../../meta/enable_if.h"

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
	virtual ~Dispatcher$x() {}
	
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
	typename FunctionType,
	typename Enable = void
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
		if (!function_)
		{
			return;
		}
		function_($(iP$x)$);
	}

	TFunction function_;
};



/** Dispatcher for lass::util::Callback$x to a callable that does not support operator!
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 * 
 *  With C++11, lambdas can also be used as callables. But the MSVC compiler did not support
 *  the unary ! operator. This was fixed in VS 2019 version 16.2.
 *  https://twitter.com/lunasorcery/status/1092870113374687232
 */
template
<
	$(typename P$x)$,
	typename FunctionType
>
class Dispatcher$xFunction
	<
		$(P$x)$,
		FunctionType,
		typename meta::EnableIf<!HasOperatorNot<FunctionType>::value>::Type
	>
	: public Dispatcher$x<$(P$x)$>
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
		if (!object_ || !method_)
		{
			return;
		}
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
