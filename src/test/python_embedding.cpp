/**	@file
 *  @internal
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#include "test_common.h"
#include "python_embedding.h"
#include "foo.h"
#include "bar.h"
#include "python_shadow.h"
#include <iostream>

#include "../util/pyshadow_object.h"

namespace lass
{
namespace test
{

PY_DECLARE_CLASS_EX( PythonFoo, "PythonFoo" )


class Base
{
	int x_;
public:
	Base()  : x_(0) {}
	virtual ~Base() {}

	virtual int getX() const { return x_; }
};

class Derived : public Base
{
	int x_;
public:
	Derived()  : x_(666) {}
	virtual ~Derived() {}

	virtual int get2X() const { return 2*x_; }
};

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


PY_DECLARE_MODULE( embedding )
PY_MODULE_FUNCTION( embedding, anotherFreeFunction )
PY_MODULE_FUNCTION( embedding, listInfo )
PY_MODULE_FUNCTION( embedding, getAFoo )
PY_MODULE_FUNCTION( embedding, makeSpam )
PY_MODULE_FUNCTION( embedding, spamToCppByCopy )
PY_MODULE_FUNCTION( embedding, spamToCppByConstReference )
PY_MODULE_FUNCTION( embedding, spamToCppByReference )
PY_MODULE_FUNCTION( embedding, spamToCppByPointer )
PY_INJECT_MODULE( embedding, "Documentation for module embedding" )


//PY_DECLARE_CLASS( PythonFoo )

// expose member methods of object instances

PY_CLASS_CONSTRUCTOR_2( PythonFoo, int, std::string )
PY_CLASS_METHOD( PythonFoo, aFooMoreComplexFunction )
PY_CLASS_METHOD( PythonFoo, testFooAutomaticFunctionExport );
//PY_CLASS_STATIC_METHOD( PythonFoo, getMeAFoo )

// inject the class in the module and provide documentation for it
PY_INJECT_CLASS_IN_MODULE( PythonFoo, embedding, "Documentation for class Foo." );


// declare a new pythonable class
PY_DECLARE_CLASS( Bar )
PY_CLASS_CONSTRUCTOR( Bar, meta::NullType );	
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

#pragma LASS_FIXME("this is broken on intel700 but not on vc7, that's news!")
#if LASS_COMPILER_TYPE != LASS_COMPILER_TYPE_INTEL
PY_CLASS_MEMBER_RW( Bar, "cool", coolMember, coolMember );   
#endif

PY_CLASS_PUBLIC_MEMBER( Bar, publicInt );
// inject the class in the module and provide documentation for it
PY_INJECT_CLASS_IN_MODULE( Bar, embedding, "Documentation for class Bar." );


}
}

// --- shadow classes ------------------------------------------------------------------------------

namespace lass
{
namespace test
{

PY_SHADOW_CLASS(ShadowSpam, Spam)
PY_DECLARE_CLASS_EX(ShadowSpam, "Spam")
PY_CLASS_METHOD(ShadowSpam, who)
PY_CLASS_MEMBER_R(ShadowSpam, "address", address)
PY_INJECT_CLASS_IN_MODULE(ShadowSpam, embedding, "shadow spam")

PY_SHADOW_CLASS_DERIVED(ShadowHam, Ham, ShadowSpam)
PY_DECLARE_CLASS_EX(ShadowHam, "Ham")
PY_CLASS_CONSTRUCTOR_0(ShadowHam)
PY_CLASS_STATIC_METHOD(ShadowHam, say)
PY_INJECT_CLASS_IN_MODULE(ShadowHam, embedding, "shadow ham");

PY_SHADOW_CLASS_DERIVED(ShadowBacon, Bacon, ShadowHam)
PY_DECLARE_CLASS_EX(ShadowBacon, "Bacon")
PY_CLASS_CONSTRUCTOR_0(ShadowBacon)
PY_INJECT_CLASS_IN_MODULE(ShadowBacon, embedding, "shadow bacon");

PY_SHADOW_CLASS_DERIVED(ShadowEggs, Eggs, ShadowSpam)
PY_DECLARE_CLASS_EX(ShadowEggs, "Eggs")
PY_CLASS_CONSTRUCTOR_1(ShadowEggs, int)
PY_CLASS_MEMBER_RW(ShadowEggs, "number", number, setNumber)
PY_INJECT_CLASS_IN_MODULE(ShadowEggs, embedding, "shadow eggs");

}
}

PY_SHADOW_CASTERS(lass::test::ShadowSpam)

