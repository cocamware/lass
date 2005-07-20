/** @file
 *  @internal
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



#include "test_common.h"
#include "python_embedding.h"
#include "foo.h"
#include "bar.h"
#include "python_shadow.h"
#include <iostream>

#include "../util/pyshadow_object.h"
#include "../util/callback_0.h"
#include "../util/callback_1.h"
#include "../util/callback_r_0.h"
#include "../util/callback_r_2.h"
#include "../util/callback_python.h"

namespace lass
{
namespace test
{
PY_SHADOW_CLASS(LASS_DLL_EXPORT, PySpam, Spam);
PY_SHADOW_CLASS_DERIVED(LASS_DLL_EXPORT, PyHam, Ham, PySpam)
PY_SHADOW_CLASS_DERIVED(LASS_DLL_EXPORT, PyBacon, Bacon, PyHam)
PY_SHADOW_CLASS_DERIVED(LASS_DLL_EXPORT, PyEggs, Eggs, PySpam)
}
}

PY_SHADOW_CASTERS(lass::test::PySpam)

namespace lass
{
namespace test
{

PY_DECLARE_CLASS_NAME( PythonFoo, "PythonFoo" )



int test()
{
	int b = lass::python::impl::ShadowTraits< int >::isShadow ;
	int c = lass::python::impl::ShadowTraits< lass::test::PythonFoo >::isShadow ;
	int d = lass::python::impl::ShadowTraits< lass::test::Bar >::isShadow;

	std::cout << "int is shadow type : " << b << "\n";
	std::cout << "python foo is shadow type : " << c << "\n";
	std::cout << "bar is shadow type : " << d << "\n";
	return 0;
}

lass::test::PythonFooPtr getAFoo(  )
{
	return lass::test::PythonFooPtr(new lass::test::PythonFoo(5, "hello"));
}
int anotherFreeFunction( int i )
{
	test();
	return i+1;
}

void call0(const util::Callback0& iCallback)
{
	iCallback();
}

void call1(const util::Callback1<const std::string&>& iCallback)
{
	iCallback("Hello world!");
}

void callR0(const util::CallbackR0<std::string>& iCallback)
{
	std::cout << "Simon says: " << iCallback() << std::endl;
}

void callR2(const util::CallbackR2<float, float, float>& iCallback)
{
	std::cout << "f(5, 6) = " << iCallback(5, 6) << std::endl;
}

void overloadedA(int iA)
{
	std::cout << "overloadedA " << iA << std::endl;
}

void overloadedB(const std::string& iA)
{
	std::cout << "overloadedA " << iA << std::endl;
}

void overloadedB(const std::complex<float>& iA)
{
	std::cout << "overloadedA " << iA << std::endl;
}

PY_DECLARE_MODULE( embedding )
PY_MODULE_FUNCTION( embedding, anotherFreeFunction )
PY_MODULE_FUNCTION( embedding, listInfo )
PY_MODULE_FUNCTION( embedding, getAFoo )
PY_MODULE_FUNCTION( embedding, makeSpam )
PY_MODULE_FUNCTION( embedding, spamToCppByCopy )
PY_MODULE_FUNCTION( embedding, spamToCppByConstReference )
PY_MODULE_FUNCTION( embedding, spamToCppByReference )
PY_MODULE_FUNCTION( embedding, spamToCppByPointer )
PY_MODULE_FUNCTION( embedding, call0 )
PY_MODULE_FUNCTION( embedding, call1 )
PY_MODULE_FUNCTION( embedding, callR0 )
PY_MODULE_FUNCTION( embedding, callR2 )
PY_MODULE_FUNCTION_NAME( embedding, overloadedA, "overloaded" )
PY_MODULE_FUNCTION_QUALIFIED_NAME_1( embedding, overloadedB, void, const std::string&, "overloaded" )
PY_MODULE_FUNCTION_QUALIFIED_NAME_1( embedding, overloadedB, void, const std::complex<float>&, "overloaded" )

LASS_EXECUTE_BEFORE_MAIN(
	PY_INJECT_MODULE_DOC(embedding, "Documentation for module embedding" ))


//PY_DECLARE_CLASS( PythonFoo )

// expose member methods of object instances

PY_CLASS_CONSTRUCTOR_2( PythonFoo, int, std::string )
PY_CLASS_METHOD( PythonFoo, aFooMoreComplexFunction )
PY_CLASS_METHOD( PythonFoo, testFooAutomaticFunctionExport );
//PY_CLASS_STATIC_METHOD( PythonFoo, getMeAFoo )

// inject the class in the module and provide documentation for it
LASS_EXECUTE_BEFORE_MAIN(
	PY_INJECT_CLASS_IN_MODULE( PythonFoo, embedding, "Documentation for class Foo." );)

// declare a new pythonable class
PY_DECLARE_CLASS( Bar )
PY_CLASS_CONSTRUCTOR( Bar , meta::NullType );
PY_CLASS_CONSTRUCTOR_2( Bar, int, const std::string& );
PY_CLASS_STATIC_METHOD( Bar, aStaticMethod );

// expose member methods of object instances
PY_CLASS_METHOD( Bar, aMoreComplexFunction )
PY_CLASS_METHOD( Bar, testAutomaticFunctionExport );
PY_CLASS_METHOD( Bar, complexArguments );
PY_CLASS_METHOD( Bar, primArguments );

PY_CLASS_METHOD_NAME_DOC( Bar, complexArguments, "tester", "tester doc");
PY_CLASS_METHOD_NAME( Bar, primArguments, "tester");
PY_CLASS_METHOD_QUALIFIED_1( Bar, overloaded, void, int )
PY_CLASS_METHOD_QUALIFIED_1( Bar, overloaded, void, const std::string& )
PY_CLASS_MEMBER_RW( Bar, "int", getInt, setInt );
PY_CLASS_MEMBER_RW( Bar, "foo", getFoo, setFoo );
PY_CLASS_MEMBER_RW( Bar, "cool", coolMember, coolMember );

PY_CLASS_PUBLIC_MEMBER( Bar, publicInt );

// a const value as class member
PY_CLASS_STATIC_CONST( Bar, "CONST", 5 );

// innerclass of Bar
typedef Bar::InnerClass TBarInnerClass;
PY_DECLARE_CLASS_NAME( TBarInnerClass, "InnerClass" )
PY_CLASS_CONSTRUCTOR_1( TBarInnerClass, const std::string& );
PY_CLASS_METHOD( TBarInnerClass, talkTo );
PY_CLASS_INNER_CLASS_NAME( Bar, TBarInnerClass, "InnerClass" )

// inject the class in the module and provide documentation for it
LASS_EXECUTE_BEFORE_MAIN(
	PY_INJECT_CLASS_IN_MODULE( Bar, embedding, "Documentation for class Bar." );)

}
}


// --- shadow classes ------------------------------------------------------------------------------


namespace lass
{
namespace test
{

PY_DECLARE_CLASS_NAME(PySpam, "Spam")
PY_CLASS_METHOD(PySpam, who)
PY_CLASS_MEMBER_R(PySpam, "address", address)
LASS_EXECUTE_BEFORE_MAIN(PY_INJECT_CLASS_IN_MODULE(PySpam, embedding, "shadow spam");)

PY_DECLARE_CLASS_NAME(PyHam, "Ham")
PY_CLASS_CONSTRUCTOR_0(PyHam)
PY_CLASS_STATIC_METHOD(PyHam, say)
LASS_EXECUTE_BEFORE_MAIN(PY_INJECT_CLASS_IN_MODULE(PyHam, embedding, "shadow ham");)

PY_DECLARE_CLASS_NAME(PyBacon, "Bacon")
PY_CLASS_CONSTRUCTOR_0(PyBacon)
LASS_EXECUTE_BEFORE_MAIN(PY_INJECT_CLASS_IN_MODULE(PyBacon, embedding, "shadow bacon");)

PY_DECLARE_CLASS_NAME(PyEggs, "Eggs")
PY_CLASS_CONSTRUCTOR_1(PyEggs, int)
PY_CLASS_MEMBER_RW(PyEggs, "number", number, setNumber)
LASS_EXECUTE_BEFORE_MAIN(PY_INJECT_CLASS_IN_MODULE(PyEggs, embedding, "shadow eggs");)

}
}
