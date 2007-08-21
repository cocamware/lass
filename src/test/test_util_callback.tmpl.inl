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

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_CALLBACK_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_CALLBACK_INL

#include "test_common.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4244) // float to int
#endif

#include "../util/callback_0.h"
$(#include "../util/callback_$x.h"
)$
#include "../util/callback_r_0.h"
$(#include "../util/callback_r_$x.h"
)$

#define LASS_TEST_CALLBACK_EMPTY(callback)\
	LASS_TEST_CHECK_MESSAGE(callback.isEmpty(), "isEmpty() returned false on empty callback.");

#define LASS_TEST_CALLBACK_NOT_EMPTY(callback)\
	LASS_TEST_CHECK_MESSAGE(!callback.isEmpty(), "isEmpty() returned true on non-empty callback.");

#define LASS_TEST_CALLBACK_CALL(expression)\
	dispatcherIsCalled = false;\
	LASS_TEST_CHECK_NO_THROW(expression);\
	LASS_TEST_CHECK_MESSAGE(dispatcherIsCalled, "non-empty '" #expression "' didn't execute dispatcher.");

#define LASS_TEST_CALLBACK_CALL_R(expression)\
	dispatcherIsCalled = false;\
	{\
		int result = 666;\
		LASS_TEST_CHECK_NO_THROW(result = expression);\
		LASS_TEST_CHECK_MESSAGE(result == 1, "non-empty '" #expression "' didn't return correct value.");\
		LASS_TEST_CHECK_MESSAGE(dispatcherIsCalled, "non-empty '" #expression "' didn't execute dispatcher.");\
	}

#define LASS_TEST_CALLBACK_EMPTY_CALL(expression)\
	dispatcherIsCalled = false;\
	LASS_TEST_CHECK_NO_THROW(expression);\
	LASS_TEST_CHECK_MESSAGE(!dispatcherIsCalled, "empty '" #expression "' did execute dispatcher.");

#define LASS_TEST_CALLBACK_EMPTY_CALL_R(expression)\
	dispatcherIsCalled = false;\
	{\
		int result = 666;\
		LASS_TEST_CHECK_THROW(result = expression, util::Exception);\
		LASS_TEST_CHECK_MESSAGE(result != 1, "empty '" #expression "' did return value.");\
		LASS_TEST_CHECK_MESSAGE(!dispatcherIsCalled, "empty '" #expression "' did execute dispatcher.");\
	}

namespace
{

bool dispatcherIsCalled;

}

namespace lass
{
namespace test
{

void free0() { dispatcherIsCalled = true; }
$[void free$x($(int i_p$x)$) { dispatcherIsCalled = true; }
]$int freeR0() { dispatcherIsCalled = true; return 1; }
$[int freeR$x($(int i_p$x)$) { dispatcherIsCalled = true; return 1;}
]$
struct Foo
{
	void mem0() { dispatcherIsCalled = true; }
$[    void mem$x($(int i_p$x)$) { dispatcherIsCalled = true; }
]$    int memR0() { dispatcherIsCalled = true; return 1; }
$[    int memR$x($(int i_p$x)$) { dispatcherIsCalled = true; return 1;}
]$
	void const_mem0() const { dispatcherIsCalled = true; }
$[    void const_mem$x($(int i_p$x)$) const { dispatcherIsCalled = true; }
]$    int const_memR0() const { dispatcherIsCalled = true; return 1; }
$[    int const_memR$x($(int i_p$x)$) const { dispatcherIsCalled = true; return 1;}
]$};

void testUtilCallback()
{
	Foo foo;

	// Callback0

	util::Callback0 cb0_void_free(free0);
	LASS_TEST_CALLBACK_NOT_EMPTY(cb0_void_free);
	LASS_TEST_CALLBACK_CALL(cb0_void_free());

	util::Callback0 cb0_void_mem(&foo, &Foo::mem0);
	LASS_TEST_CALLBACK_NOT_EMPTY(cb0_void_mem);
	LASS_TEST_CALLBACK_CALL(cb0_void_mem());

	util::Callback0 cb0_void_const_mem(&foo, &Foo::const_mem0);
	LASS_TEST_CALLBACK_NOT_EMPTY(cb0_void_const_mem);
	LASS_TEST_CALLBACK_CALL(cb0_void_const_mem());

	util::Callback0 cb0_void;
	LASS_TEST_CALLBACK_EMPTY(cb0_void);
	LASS_TEST_CALLBACK_EMPTY_CALL(cb0_void());

	cb0_void.swap(cb0_void_free);
	//LASS_TEST_CHECK(cb0_void != cb0_void_free);
	LASS_TEST_CALLBACK_NOT_EMPTY(cb0_void);
	LASS_TEST_CALLBACK_EMPTY(cb0_void_free);
	LASS_TEST_CALLBACK_CALL(cb0_void());
	LASS_TEST_CALLBACK_EMPTY_CALL(cb0_void_free());

	cb0_void_free = cb0_void;
	//LASS_TEST_CHECK(cb0_void == cb0_void_free);
	LASS_TEST_CALLBACK_CALL(cb0_void());
	LASS_TEST_CALLBACK_CALL(cb0_void_free());

	cb0_void = cb0_void_mem;
	//LASS_TEST_CHECK(cb0_void == cb0_void_mem);
	LASS_TEST_CALLBACK_CALL(cb0_void());
	LASS_TEST_CALLBACK_CALL(cb0_void_mem());

	cb0_void = cb0_void_const_mem;
	//LASS_TEST_CHECK(cb0_void == cb0_void_const_mem);
	LASS_TEST_CALLBACK_CALL(cb0_void());
	LASS_TEST_CALLBACK_CALL(cb0_void_const_mem());
$[

	// Callback$x

	util::Callback$x<$(int)$> cb$x_int_free(free$x);
	LASS_TEST_CALLBACK_NOT_EMPTY(cb$x_int_free);
	LASS_TEST_CALLBACK_CALL(cb$x_int_free($($x)$));

	//util::Callback$x<$(float)$> cb$x_float_free(free$x);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cb$x_float_free);
	//LASS_TEST_CALLBACK_CALL(cb$x_float_free($($x.f)$));

	util::Callback$x<$(int)$> cb$x_int_mem(&foo, &Foo::mem$x);
	LASS_TEST_CALLBACK_NOT_EMPTY(cb$x_int_mem);
	LASS_TEST_CALLBACK_CALL(cb$x_int_mem($($x)$));

	//util::Callback$x<$(float)$> cb$x_float_mem(&foo, &Foo::mem$x);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cb$x_float_mem);
	//LASS_TEST_CALLBACK_CALL(cb$x_float_mem($($x.f)$));

	util::Callback$x<$(int)$> cb$x_int_const_mem(&foo, &Foo::const_mem$x);
	LASS_TEST_CALLBACK_NOT_EMPTY(cb$x_int_const_mem);
	LASS_TEST_CALLBACK_CALL(cb$x_int_const_mem($($x)$));

	//util::Callback$x<$(float)$> cb$x_float_const_mem(&foo, &Foo::const_mem$x);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cb$x_float_const_mem);
	//LASS_TEST_CALLBACK_CALL(cb$x_float_const_mem($($x.f)$));

	//util::Callback$x<$(float)$> cb$x_float;
	//LASS_TEST_CALLBACK_EMPTY(cb$x_float);
	//LASS_TEST_CALLBACK_EMPTY_CALL(cb$x_float($($x.f)$));

	//cb$x_float.swap(cb$x_float_free);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cb$x_float);
	//LASS_TEST_CALLBACK_EMPTY(cb$x_float_free);
	//LASS_TEST_CALLBACK_CALL(cb$x_float($($x.f)$));
	//LASS_TEST_CALLBACK_EMPTY_CALL(cb$x_float_free($($x.f)$));

	//cb$x_float_free = cb$x_float;
	//LASS_TEST_CALLBACK_CALL(cb$x_float($($x.f)$));
	//LASS_TEST_CALLBACK_CALL(cb$x_float_free($($x.f)$));

	//cb$x_float = cb$x_float_mem;
	//LASS_TEST_CALLBACK_CALL(cb$x_float($($x.f)$));
	//LASS_TEST_CALLBACK_CALL(cb$x_float_mem($($x.f)$));

	//cb$x_float = cb$x_float_const_mem;
	//LASS_TEST_CALLBACK_CALL(cb$x_float($($x.f)$));
	//LASS_TEST_CALLBACK_CALL(cb$x_float_const_mem($($x.f)$));
]$

	// CallbackR0

	util::CallbackR0<int> cbr0_int_void_free(freeR0);
	LASS_TEST_CALLBACK_NOT_EMPTY(cbr0_int_void_free);
	LASS_TEST_CALLBACK_CALL_R(cbr0_int_void_free());

	//util::CallbackR0<float> cbr0_float_void_free(freeR0);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cbr0_float_void_free);
	//LASS_TEST_CALLBACK_CALL_R(cbr0_float_void_free());

	util::CallbackR0<int> cbr0_int_void_mem(&foo, &Foo::memR0);
	LASS_TEST_CALLBACK_NOT_EMPTY(cbr0_int_void_mem);
	LASS_TEST_CALLBACK_CALL_R(cbr0_int_void_mem());

	//util::CallbackR0<float> cbr0_float_void_mem(&foo, &Foo::memR0);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cbr0_float_void_mem);
	//LASS_TEST_CALLBACK_CALL_R(cbr0_float_void_mem());

	util::CallbackR0<int> cbr0_int_void_const_mem(&foo, &Foo::const_memR0);
	LASS_TEST_CALLBACK_NOT_EMPTY(cbr0_int_void_const_mem);
	LASS_TEST_CALLBACK_CALL_R(cbr0_int_void_const_mem());

	//util::CallbackR0<float> cbr0_float_void_const_mem(&foo, &Foo::const_memR0);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cbr0_float_void_const_mem);
	//LASS_TEST_CALLBACK_CALL_R(cbr0_float_void_const_mem());

	//util::CallbackR0<float> cbr0_float_void;
	//LASS_TEST_CALLBACK_EMPTY(cbr0_float_void);
	//LASS_TEST_CALLBACK_EMPTY_CALL_R(cbr0_float_void());

	//cbr0_float_void.swap(cbr0_float_void_free);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cbr0_float_void);
	//LASS_TEST_CALLBACK_EMPTY(cbr0_float_void_free);
	//LASS_TEST_CALLBACK_CALL_R(cbr0_float_void());
	//LASS_TEST_CALLBACK_EMPTY_CALL_R(cbr0_float_void_free());

	//cbr0_float_void_free = cbr0_float_void;
	//LASS_TEST_CALLBACK_CALL_R(cbr0_float_void());
	//LASS_TEST_CALLBACK_CALL_R(cbr0_float_void_free());

	//cbr0_float_void = cbr0_float_void_mem;
	//LASS_TEST_CALLBACK_CALL_R(cbr0_float_void());
	//LASS_TEST_CALLBACK_CALL_R(cbr0_float_void_mem());

	//cbr0_float_void = cbr0_float_void_const_mem;
	//LASS_TEST_CALLBACK_CALL_R(cbr0_float_void());
	//LASS_TEST_CALLBACK_CALL_R(cbr0_float_void_const_mem());
$[

	// CallbackR$x

	util::CallbackR$x<int, $(int)$> cbr$x_int_int_free(freeR$x);
	LASS_TEST_CALLBACK_NOT_EMPTY(cbr$x_int_int_free);
	LASS_TEST_CALLBACK_CALL_R(cbr$x_int_int_free($($x)$));

	//util::CallbackR$x<float, $(float)$> cbr$x_float_float_free(freeR$x);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cbr$x_float_float_free);
	//LASS_TEST_CALLBACK_CALL_R(cbr$x_float_float_free($($x.f)$));

	util::CallbackR$x<int, $(int)$> cbr$x_int_int_mem(&foo, &Foo::memR$x);
	LASS_TEST_CALLBACK_NOT_EMPTY(cbr$x_int_int_mem);
	LASS_TEST_CALLBACK_CALL_R(cbr$x_int_int_mem($($x)$));

	//util::CallbackR$x<float, $(float)$> cbr$x_float_float_mem(&foo, &Foo::memR$x);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cbr$x_float_float_mem);
	//LASS_TEST_CALLBACK_CALL_R(cbr$x_float_float_mem($($x.f)$));

	util::CallbackR$x<int, $(int)$> cbr$x_int_int_const_mem(&foo, &Foo::const_memR$x);
	LASS_TEST_CALLBACK_NOT_EMPTY(cbr$x_int_int_const_mem);
	LASS_TEST_CALLBACK_CALL_R(cbr$x_int_int_const_mem($($x)$));

	//util::CallbackR$x<float, $(float)$> cbr$x_float_float_const_mem(&foo, &Foo::const_memR$x);
	//LASS_TEST_CALLBACK_NOT_EMPTY(cbr$x_float_float_const_mem);
	//LASS_TEST_CALLBACK_CALL_R(cbr$x_float_float_const_mem($($x.f)$));
	/*
	util::CallbackR$x<float, $(float)$> cbr$x_float_float;
	LASS_TEST_CALLBACK_EMPTY(cbr$x_float_float);
	LASS_TEST_CALLBACK_EMPTY_CALL_R(cbr$x_float_float($($x.f)$));

	cbr$x_float_float.swap(cbr$x_float_float_free);
	LASS_TEST_CALLBACK_NOT_EMPTY(cbr$x_float_float);
	LASS_TEST_CALLBACK_EMPTY(cbr$x_float_float_free);
	LASS_TEST_CALLBACK_CALL_R(cbr$x_float_float($($x.f)$));
	LASS_TEST_CALLBACK_EMPTY_CALL_R(cbr$x_float_float_free($($x.f)$));

	cbr$x_float_float_free = cbr$x_float_float;
	LASS_TEST_CALLBACK_CALL_R(cbr$x_float_float($($x.f)$));
	LASS_TEST_CALLBACK_CALL_R(cbr$x_float_float_free($($x.f)$));

	cbr$x_float_float = cbr$x_float_float_mem;
	LASS_TEST_CALLBACK_CALL_R(cbr$x_float_float($($x.f)$));
	LASS_TEST_CALLBACK_CALL_R(cbr$x_float_float_mem($($x.f)$));

	cbr$x_float_float = cbr$x_float_float_const_mem;
	LASS_TEST_CALLBACK_CALL_R(cbr$x_float_float($($x.f)$));
	LASS_TEST_CALLBACK_CALL_R(cbr$x_float_float_const_mem($($x.f)$));
	*/
]$
}

}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif

// EOF
