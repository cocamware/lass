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

/** @defgroup ThreadFun ThreadFun
 *  @brief library to run (member) functions in a thread
 *  @author [Bramz]
 *
 *  With these objects/functions, you can run (member) functions in threads instead of calling
 *  them directly.  you even can specify the arguments to be used.  Let's illustrate this with
 *  a simple example:
 *
 *  @code
 *  void foo(int iNumber, const std::string& iText);
 *
 *  // direct call
 *  foo(5, "hello");
 *
 *  // through thread
 *  Thread* thread = threadFun(foo, 5, "hello");
 *  @endcode
 *
 *  threadFun2 allocates a new thread with the necessary information to call the function,
 *  then creates and runs the thread.  It returns a pointer to the allocated thread.  If you do
 *  nothing, the thread runs in @e detached mode and will kill itself at completion (so you don't
 *  have to call @c delete on @c thread yourself.  So you even don't have to catch the return
 *  value.  However, if you want to run it in @e joinable, you'll have delete the thread
 *  yourself.  DO NOT FORGET THIS!  Maybe we should do something about this situation?
 *
 *  @code
 *  // run two threads at the same time.
 *  util::ScopedPtr<Thread> a = threadFun(foo, 5, "hello", THREAD_JOINABLE);
 *  util::ScopedPtr<Thread> b = threadFun(foo, 6, "world!", THREAD_JOINABLE);
 *  a->wait();
 *  b->wait();
 *  @endcode
 *
 *  You can also call member functions of specific objects in the thread, both const as non-const
 *  member functions.
 *
 *  @code
 *  class Bar
 *  {
 *      void fun(float iValue) const;
 *      void beer();
 *  };
 *
 *  Bar bar;
 *  util::ScopedPtr<Thread> a = threadFun(bar, Bar::fun, 3.14, THREAD_JOINABLE);
 *  util::ScopedPtr<Thread> b = threadFun(bar, Bar::beer, THREAD_JOINABLE);
 *  a->wait();
 *  b->wait();
 *  @endcode
 *
 *  Limitations:
 *  - the functions can't have return values, or they are ignored (who should they pass it
 *    through to, anyway?)
 *  - You can't have more than $x arguments (unless you tweak the prebuild ;)
 *
 *  @warning Only ThreadFun0 and ThreadFun1 are partially tested.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_FUN_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_FUN_H

#include "util_common.h"
#include "callback_0.h"$(
#include "callback_$x.h")$
#include "thread.h"
#include "../meta/type_traits.h"

namespace lass
{
namespace util
{

// --- null arguments ------------------------------------------------------------------------------

/** @class ThreadFun0
 *  @brief ThreadFun without arguments
 *  @ingroup ThreadFun
 */
class ThreadFun0: public Thread
{
public:
	ThreadFun0(const Callback0& iFun, ThreadKind iKind = THREAD_DETACHED);
	virtual void* entry();
private:
	Callback0 fun_;
};

ThreadFun0* threadFun(
	void (*iFun)(),
	ThreadKind iKind = THREAD_DETACHED);

template <typename Obj>
ThreadFun0* threadFun(
	const Obj& iObj, void (Obj::*iMemFun)(),
	ThreadKind iKind = THREAD_DETACHED);

template <typename Obj>
ThreadFun0* threadFun(
	const Obj& iObj, void (Obj::*iMemFun)() const,
	ThreadKind iKind = THREAD_DETACHED);



$[
// --- $x argument(s) ----------------------------------------------------------------------------

/** @class ThreadFun$x
 *  @brief ThreadFun with $x argument(s)
 *  @ingroup ThreadFun
 */
template <$(typename P$x)$>
class ThreadFun$x: public Thread
{
public:
	ThreadFun$x(const Callback$x<$(P$x)$>& iFun,
			   $(typename CallTraits<P$x>::TParam iP$x)$,
			   ThreadKind iKind = THREAD_DETACHED);
	virtual void* entry();
private:
	Callback$x<$(P$x)$> fun_;$(
	typename meta::TypeTraits<P$x>::TStorage p$x_;)$
};

template <$(typename P$x)$, $(typename Q$x)$>
ThreadFun$x<$(P$x)$>* threadFun(
	void (*iFun)($(P$x)$),
	$(const Q$x& iQ$x)$,
	ThreadKind iKind = THREAD_DETACHED);

template <typename Obj, $(typename P$x)$, $(typename Q$x)$>
ThreadFun$x<$(P$x)$>* threadFun(
	const Obj& iObj, void (Obj::*iMemFun)($(P$x)$),
	$(const Q$x& iQ$x)$,
	ThreadKind iKind = THREAD_DETACHED);

template <typename Obj, $(typename P$x)$, $(typename Q$x)$>
ThreadFun$x<$(P$x)$>* threadFun(
	const Obj& iObj, void (Obj::*iMemFun)($(P$x)$) const,
	$(const Q$x& iQ$x)$,
	ThreadKind iKind = THREAD_DETACHED);



]$

}

}

#include "thread_fun.inl"

#endif

// EOF
