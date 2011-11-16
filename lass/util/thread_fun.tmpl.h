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

/** @class ThreadFun
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
 *  threadFun(foo, 5, "hello")->run();
 * 
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
 *  util::ScopedPtr<Thread> a = threadFun(foo, 5, "hello", threadJoinable);
 *  util::ScopedPtr<Thread> b = threadFun(foo, 6, "world!", threadJoinable);
 *  a->run();
 *  b->run();
 *  a->join();
 *  b->join();
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
 *  util::ScopedPtr<Thread> a = threadFun(bar, Bar::fun, 3.14, threadJoinable);
 *  util::ScopedPtr<Thread> b = threadFun(bar, Bar::beer, threadJoinable);
 *  a->run();
 *  b->run();
 *  a->join();
 *  b->join();
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
#include "callback_0.h"
#include "thread.h"
#include "../meta/type_traits.h"

namespace lass
{
namespace util
{

class LASS_DLL ThreadFun: public Thread
{
public:
	ThreadFun(const Callback0& fun, ThreadKind kind = threadDetached);
private:
	void doRun();
	Callback0 fun_;
};

// --- 0 arguments ---------------------------------------------------------------------------------

template <typename Function>
ThreadFun* threadFun(
	Function function,
	ThreadKind kind = threadDetached);

template <typename ObjectPtr, typename Method>
ThreadFun* threadMemFun(
	ObjectPtr object, Method method,
	ThreadKind kind = threadDetached);

$[
// --- $x argument(s) ----------------------------------------------------------------------------

template <$(typename P$x)$, typename Function>
ThreadFun* threadFun(
	Function function,
	$(const P$x& iP$x)$,
	ThreadKind kind = threadDetached);

template <$(typename P$x)$, typename ObjectPtr, typename Method>
ThreadFun* threadMemFun(
	ObjectPtr object, Method method,
	$(const P$x& iP$x)$,
	ThreadKind kind = threadDetached);
]$

}

}

#include "thread_fun.inl"

#endif

// EOF
