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

/** @defgroup Bind
 *  @brief bind call and arguments to a nullary callback.
 *
 *	@code
 *	#include <lass/util/bind.h>
 *	#include <lass/util/shared_ptr.h>
 *	namespace using lass::util;
 *
 *	void fun(const std::string& a);
 *	float moreFun(float a, float b);
 *  class Spam
 *	{
 *		void ham(const std::string& something);
 *		std::string eggs(int num) const;
 *	};
 *
 *	// ...
 *
 *	// regulare function call
 *	Callback0 boundFun = bind(fun, "hello world!");
 *
 *  // with return value.  type conversion is allowed both on arguments and result
 *	CallbackR1<double> boundMoreFun(moreFun, 5, 6);
 *
 *	// bound method call, you have to make sure spam1 is still alive when call is executed!
 *	Spam spam1;
 *  Callback0 boundHam = bind(&Spam::ham, &spam1, "eggs");
 *
 *	// bound method call with smart pointer, return value is ignored
 *	SharedPtr<Spam> spam2(new Spam);
 *	Callback0 boundEggs = bind(&Spam::eggs, spam2, 3);
 *
 *	// ...
 *
 *	boundFun();
 *	double result = boundMoreFun();
 *	boundHam();
 *	boundEggs();
 *	@endcode
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_BIND_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_BIND_H

#include "util_common.h"
#include "callback_0.h"
$(#include "callback_$x.h"
)$#include "callback_r_0.h"
$(#include "callback_r_$x.h"
)$

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
	bool doIsEquivalent(const DispatcherR0<R>* other) const { return false; }
};
template <> class BindDispatcher<void>: public impl::Dispatcher0 
{
	bool doIsEquivalent(const Dispatcher0* other) const { return false; }
};

}

// --- zero arguments ------------------------------------------------------------------------------

/** @ingroup Bind
 */
template <typename R>
typename impl::BindCallback<R>::Type 
bind(R (*fun)())
{
	return impl::BindCallback<R>::Type(fun);
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
	R doCall() const { return fun_($(x$x_)$); }
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
	R doCall() const { return ((*obj_).*fun_)($(x$x_)$); }
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
	return impl::BindCallback<R>::Type(impl::DispatcherBindFun$x<R, TFun, $(X$x)$>(
		fun, $(x$x)$));
}

/** @ingroup Bind
 */
template <typename R, $(typename P$x)$, typename Obj, typename ObjPtr, $(typename X$x)$>
typename impl::BindCallback<R>::Type 
bind(R (Obj::*fun)($(P$x)$), ObjPtr obj, $(X$x x$x)$)
{
	typedef R (Obj::*TFun)($(P$x)$);
	return impl::BindCallback<R>::Type(impl::DispatcherBindMemFun$x<R, ObjPtr, TFun, $(X$x)$>(
		obj, fun, $(x$x)$));
}

/** @ingroup Bind
 */
template <typename R, $(typename P$x)$, typename Obj, typename ObjPtr, $(typename X$x)$>
typename impl::BindCallback<R>::Type 
bind(R (Obj::*fun)($(P$x)$) const, ObjPtr obj, $(X$x x$x)$)
{
	typedef R (Obj::*TFun)($(P$x)$) const;
	return impl::BindCallback<R>::Type(impl::DispatcherBindMemFun$x<R, ObjPtr, TFun, $(X$x)$>(
		obj, fun, $(x$x)$));
}

/** @ingroup Bind
 */
template <typename R, $(typename P$x)$, $(typename X$x)$>
CallbackR0<R>
bind(const CallbackR$x<R, $(P$x)$>& fun, $(X$x x$x)$)
{
	typedef CallbackR$x<R, $(P$x)$> TFun;
	return CallbackR0<R>(impl::DispatcherBindFun$x<R, TFun, $(X$x)$>(
		fun, $(x$x)$));
}

/** @ingroup Bind
 */
template <$(typename P$x)$, $(typename X$x)$>
Callback0
bind(const Callback$x<$(P$x)$>& fun, $(X$x x$x)$)
{
	typedef Callback$x<$(P$x)$> TFun;
	return Callback0(impl::DispatcherBindFun$x<void, TFun, $(X$x)$>(
		fun, $(x$x)$));
}

]$

}

}


#endif

// EOF
