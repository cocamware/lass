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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

/** @defgroup Bind
 *  @brief bind call and arguments to a nullary callback.
 *
 *  @code
 *  #include <lass/util/bind.h>
 *  #include <lass/util/shared_ptr.h>
 *  namespace using lass::util;
 *
 *  void fun(const std::string& a);
 *  float moreFun(float a, float b);
 *  class Spam
 *  {
 *  	void ham(const std::string& something);
 *  	std::string eggs(int num) const;
 *  };
 *
 *  // ...
 *
 *  // regulare function call
 *  Callback0 boundFun = bind(fun, "hello world!");
 *
 *  // with return value.  type conversion is allowed both on arguments and result
 *  CallbackR1<double> boundMoreFun(moreFun, 5, 6);
 *
 *  // bound method call, you have to make sure spam1 is still alive when call is executed!
 *  Spam spam1;
 *  Callback0 boundHam = bind(&Spam::ham, &spam1, "eggs");
 *
 *  // bound method call with smart pointer, return value is ignored
 *  SharedPtr<Spam> spam2(new Spam);
 *  Callback0 boundEggs = bind(&Spam::eggs, spam2, 3);
 *
 *  // ...
 *
 *  boundFun();
 *  double result = boundMoreFun();
 *  boundHam();
 *  boundEggs();
 *  @endcode
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_BIND_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_BIND_H

#include "util_common.h"
#include "callback_0.h"
$(#include "callback_$x.h"
)$#include "callback_r_0.h"
$(#include "callback_r_$x.h"
)$

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4267) // for size_t parameters ...
#endif

namespace lass
{
namespace util
{

namespace impl
{

/** @ingroup Bind
 *  @internal
 */
template <typename R> struct BindCallback { typedef CallbackR0<R> Type; };
template <> struct BindCallback<void> { typedef Callback0 Type; };

/** @ingroup Bind
 *  @internal
 */
template <typename R> class BindDispatcher: public DispatcherR0<R> 
{
	bool doIsEquivalent(const DispatcherR0<R>*) const { return false; }
};
template <> class BindDispatcher<void>: public impl::Dispatcher0 
{
	bool doIsEquivalent(const Dispatcher0*) const { return false; }
};

}

// --- zero arguments ------------------------------------------------------------------------------

/** @ingroup Bind
 */
template <typename R>
typename impl::BindCallback<R>::Type 
bind(R (*fun)())
{
	return typename impl::BindCallback<R>::Type(fun);
}

/** @ingroup Bind
 */
template <typename R, typename Obj, typename ObjPtr>
typename impl::BindCallback<R>::Type 
bind(R (Obj::*fun)(), ObjPtr obj)
{
	return typename impl::BindCallback<R>::Type(obj, fun);
}

/** @ingroup Bind
 */
template <typename R, typename Obj, typename ObjPtr>
typename impl::BindCallback<R>::Type
bind(R (Obj::*fun)() const, ObjPtr obj)
{
	return typename impl::BindCallback<R>::Type(obj, fun);
}

/** @ingroup Bind
 */
inline Callback0 bind(const Callback0& fun)
{
	return fun;
}

/** @ingroup Bind
 */
template <typename R>
CallbackR0<R> bind(const CallbackR0<R>& fun)
{
	return fun;
}

$[

// --- $x argument(s) -----------------------------------------------------------------------------

namespace impl
{

/** @ingroup Bind
 *  @internal
 */
template <typename R, typename Fun, $(typename X$x)$>
class DispatcherBindFun$x: public BindDispatcher<R>
{
public:
	DispatcherBindFun$x(Fun fun, $(X$x x$x)$): fun_(fun), $(x$x_(x$x))$ {}
private:
	R doCall() const 
	{ 
		if (!fun_)
		{
			LASS_THROW_EX(EmptyCallback, "You've tried to call an empty CallbackR0.  Can't return a value.");        
		}
		return fun_($(x$x_)$); 
	}
	Fun fun_;
$(	typename CallTraits<X$x>::TValue x$x_;
)$};

/** @ingroup Bind
 *  @internal
 */
template <typename R, typename ObjPtr, typename Fun, $(typename X$x)$>
class DispatcherBindMemFun$x: public BindDispatcher<R>
{
public:
	DispatcherBindMemFun$x(ObjPtr obj, Fun fun, $(X$x x$x)$): obj_(obj), fun_(fun), $(x$x_(x$x))$ {}
private:
	R doCall() const 
	{ 
		if (!obj_ || !fun_)
		{
			LASS_THROW_EX(EmptyCallback, "You've tried to call an empty CallbackR0.  Can't return a value.");        
		}
		return ((*obj_).*fun_)($(x$x_)$); 
	}
	ObjPtr obj_;
	Fun fun_;
$(	typename CallTraits<X$x>::TValue x$x_;
)$};

}

/** @ingroup Bind
 */
template <typename R, $(typename P$x)$, $(typename X$x)$>
typename impl::BindCallback<R>::Type 
bind(R (*fun)($(P$x)$), $(X$x x$x)$)
{
	typedef R (*TFun)($(P$x)$);
	typedef typename impl::BindCallback<R>::Type TCallback;
	typedef impl::DispatcherBindFun$x<R, TFun, $(X$x)$> TDispatcher;
	return TCallback(TDispatcher(fun, $(x$x)$));
}

/** @ingroup Bind
 */
template <typename R, $(typename P$x)$, typename Obj, typename ObjPtr, $(typename X$x)$>
typename impl::BindCallback<R>::Type 
bind(R (Obj::*fun)($(P$x)$), ObjPtr obj, $(X$x x$x)$)
{
	typedef R (Obj::*TFun)($(P$x)$);
	typedef typename impl::BindCallback<R>::Type TCallback;
	typedef impl::DispatcherBindMemFun$x<R, ObjPtr, TFun, $(X$x)$> TDispatcher;
	return TCallback(TDispatcher(obj, fun, $(x$x)$));
}

/** @ingroup Bind
 */
template <typename R, $(typename P$x)$, typename Obj, typename ObjPtr, $(typename X$x)$>
typename impl::BindCallback<R>::Type 
bind(R (Obj::*fun)($(P$x)$) const, ObjPtr obj, $(X$x x$x)$)
{
	typedef R (Obj::*TFun)($(P$x)$) const;
	typedef typename impl::BindCallback<R>::Type TCallback;
	typedef impl::DispatcherBindMemFun$x<R, ObjPtr, TFun, $(X$x)$> TDispatcher;
	return TCallback(TDispatcher(obj, fun, $(x$x)$));
}

/** @ingroup Bind
 */
template <typename R, $(typename P$x)$, $(typename X$x)$>
CallbackR0<R>
bind(const CallbackR$x<R, $(P$x)$>& fun, $(X$x x$x)$)
{
	typedef CallbackR$x<R, $(P$x)$> TFun;
	typedef CallbackR0<R> TCallback;
	typedef impl::DispatcherBindFun$x<R, TFun, $(X$x)$> TDispatcher;
	return TCallback(TDispatcher(fun, $(x$x)$));
}

/** @ingroup Bind
 */
template <$(typename P$x)$, $(typename X$x)$>
Callback0
bind(const Callback$x<$(P$x)$>& fun, $(X$x x$x)$)
{
	typedef Callback$x<$(P$x)$> TFun;
	typedef Callback0 TCallback;
	typedef impl::DispatcherBindFun$x<void, TFun, $(X$x)$> TDispatcher;
	return TCallback(TDispatcher(fun, $(x$x)$));
}

]$

}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif

// EOF
