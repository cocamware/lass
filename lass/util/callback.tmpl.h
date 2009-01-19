/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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



/** @defgroup Callback Callback
 *  @brief library to wrap generalized callback functions in first class objects.
 *  @date 2002-2004
 *  @author Bram de Greve [Bramz] (contact: bramz@users.sourceforge.net)
 *
 *  The use of this library is to treat function calls as first class objects, so you can pass them
 *  around to be called at a later time.
 *
 *  @code
 *  int fun(int a, int b) { return a + b; }
 *  // ...
 *  CallbackR2<int, int, int> cb(fun);
 *  // ...
 *  int result = cb(1, 2); // result will be 3.
 *  @endcode
 *
 *  Of course, the above example could be done just as easy with plain old C function pointers.
 *  That's true, but the advantage of this library will come forward when you want to call a
 *  method of a specific object instead.  You can tell what method must be called, and on what
 *  object.  Later, the caller can execute this without being aware this is a different case than
 *  above.  There's no way you can do this with plain old function pointers.
 *
 *  @code
 *  class Foo
 *  {
 *      int m_;
 *  public:
 *      Foo(int m): m_(m) {}
 *      int bar(int a, b) const { return m * (a + b); }
 *  };
 *  // ...
 *  Foo foo(4);
 *  CallbackR2<int, int, int> cb2(&foo, Foo::bar);
 *  // ...
 *  int result2 = cb2(1, 2); // result2 will be 12.
 *  @endcode
 *
 *  Both constant as non-constant member functions are supported as dispatchers.
 *
 *  It is also possible to bind a function with a little different signature, but of which the
 *  arguments can be implicit converted to the right type.
 *
 *  @code
 *  float spam(float a, float b) { return a / b; }
 *  // ...
 *  CallbackR2<int, int, int> cb3(spam);
 *  // ...
 *  int result3 = cb3(5, 2); // result3 will be 2
 *  @endcode
 *
 *  Callbacks can also be in an empty state.  This is when the callback has no dispatcher, no
 *  function call is bounded to it.  Callbacks will be empty when constructed by the default
 *  constructor, or when one of the arguments is a NULL pointer
 *
 *  @code
 *  CallbackR0<double> cb4;
 *  assert(cb4.empty());
 *
 *  CallbackR1<bool, float> cb5(0); // NULL function pointer
 *  assert(cb5.empty());
 *
 *  CallbackR2<int, int, int> cb6(0, Foo::bar); // NULL ojbect pointer
 *  assert(cb6.empty());
 *  @endcode
 *
 *  When executing empty callbacks, we have to distinguish two situations: callbacks without
 *  return value, and callbacks with return value.
 *  - In the former case, there's no danger.  When the callback is executed, it has no dispatcher to
 *    call.  Hence, it won't do anything and it will immediately return.
 *  - In the latter case, an exception will be thrown.  When the callback is executed, it has to
 *    return a value.  Since there's no dispatcher to call that can return this value, we're in
 *    troubles.  An Exception will be thrown to handle this error.
 *
 *  @code
 *  Callback2<int, const std::string&> cb7;
 *  assert(cb7.empty());
 *  cb7(4, "hello"); // OK, empty callback but no return value expected.
 *
 *  Callback3<std::string, int, int> cb8;
 *  assert(cb8.empty());
 *  try
 *  {
 *      std::string result8 = cb8(12, 34); // ERROR, an exception will be thrown.
 *  }
 *  catch(CallbackEmptyCall)
 *  {
 *      // you'll end up here on an empty call.
 *  }
 *
 *  std::string result8b = cb8 ? cb8(12, 34) : "bah"; // result8b will be "bah".
 *  @endcode
 *
 *  @par Callbacks without return value:
 *
 *  - @ref lass::util::Callback0
 *  $(- @ref lass::util::Callback$x
 *	)$
 *  @par Callbacks with return value:
 *
 *  - @ref lass::util::CallbackR0
 *  $(- @ref lass::util::CallbackR$x
 *	)$ *
 *  @par Acknowledgements:
 *  I want to thank Altair, Jaap Suter & especially Dirk Gerrits (who has spent an entire afternoon
 *  on this to help me.  It was a waste of time for both of us, but we had a lot of fun, didn't we?)
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_H

#include "callback_0.h"
$(#include "callback_$x.h"
)$
#include "callback_r_0.h"
$(#include "callback_r_$x.h"
)$
#endif

// EOF
