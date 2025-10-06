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
 *	Copyright (C) 2004-2025 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_R_0_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_R_0_H

#include "dispatcher_allocator.h"
#include "../callback_common.h"

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
class DispatcherR0: public TDispatcherAllocatorBase
{
public:

	DispatcherR0() {}
	virtual ~DispatcherR0() {}
	R call() const 
	{ 
		return doCall(); 
	}
	bool isEquivalent(const DispatcherR0<R>* iOther) const
	{
		return doIsEquivalent(iOther);
	}

protected:

	DispatcherR0(const DispatcherR0<R>& /*iOther*/) {}

private:

	virtual R doCall() const = 0;
	virtual bool doIsEquivalent(const DispatcherR0<R>* iOther) const = 0;

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
	typename FunctionType,
	typename Enable = void
>
class DispatcherR0Function: public DispatcherR0<R>
{
public:

	typedef DispatcherR0Function<R, FunctionType, Enable> TSelf;
	typedef FunctionType TFunction;

	DispatcherR0Function(typename CallTraits<TFunction>::TParam iFunction):
		function_(iFunction)
	{
	}

	const TFunction& function() const
	{
		return function_;
	}

private:

	R doCall() const override
	{
		if (!function_)
		{
			LASS_THROW_EX(EmptyCallback, "You've tried to call an empty CallbackR0.  Can't return a value.");
		}
		return function_();
	}
	bool doIsEquivalent(const DispatcherR0<R>* other) const override
	{
		if constexpr (impl::IsEqualityComparable<TFunction>::value)
		{
			return other && typeid( *other ) == typeid( TSelf )
				&& static_cast<const TSelf*>(other)->function_ == function_;
		}
		else
		{
			return false;
		}
	}

	TFunction function_;
};



#if LASS_HAVE_CPP_STD_11 && !LASS_HAVE_LAMBDA_OPERATOR_NOT

/** Dispatcher for lass::util::CallbackR0 to a callable that does not support operator!
 *  @internal
 *  @sa CallbackR0
 *  @author Bram de Greve [Bramz]
 * 
 *  With C++11, lambdas can also be used as callables. But the MSVC compiler did not support
 *  the unary ! operator. This was fixed in VS 2019 version 16.2.
 *  https://twitter.com/lunasorcery/status/1092870113374687232
 */
template
<
	typename R,
	typename FunctionType
>
class DispatcherR0Function
	<
		R,
		FunctionType,
		typename meta::EnableIf<!HasOperatorNot<FunctionType>::value>::Type
	>
	: public DispatcherR0<R>
{
public:

	typedef FunctionType TFunction;

	DispatcherR0Function(typename CallTraits<TFunction>::TParam iFunction):
		function_(iFunction)
	{
	}

	const TFunction& function() const
	{
		return function_;
	}

private:

	R doCall() const override
	{
		return function_();
	}
	bool doIsEquivalent(const DispatcherR0<R>* /*iOther*/) const override
	{
		return false;
	}

	TFunction function_;
};

#endif



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

	typedef DispatcherR0Method<R, ObjectPtr, Method> TSelf;

	DispatcherR0Method(typename CallTraits<ObjectPtr>::TParam iObject,
					   typename CallTraits<Method>::TParam iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

private:

	R doCall() const override
	{
		if (!object_ || !method_)
		{
			LASS_THROW_EX(EmptyCallback, "You've tried to call an empty CallbackR0.  Can't return a value.");
		}
		return ((*object_).*method_)();
	}
	bool doIsEquivalent(const DispatcherR0<R>* iOther) const override
	{
		const TSelf* other = dynamic_cast<const TSelf*>(iOther);
		return other && object_ == other->object_ && method_ == other->method_;
	}

	ObjectPtr object_;
	Method method_;
};

}

}

}

#endif // Guardian of Inclusion


// EOF
