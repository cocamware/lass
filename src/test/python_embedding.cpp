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
#include <iostream>

typedef lass::test::PythonFoo TPythonFoo;
PY_DECLARE_CLASS_PLUS_EX( TPythonFoo, "PythonFoo" )

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

#pragma LASS_FIXME("This code borks under vs2003, needs to be fixed when the time comes") 
/*
typedef lass::python::PyShadowObject< Base >	TShadowBase;

PY_DECLARE_CLASS_PLUS_EX( TShadowBase, "Base" )
PY_CLASS_METHOD_EX( TShadowBase, getX, pygetXshadowBase, "getX", NULL );

*/
int test()
{
	int b = lass::python::IsShadowType< int >::value ;
	int c = lass::python::IsShadowType< lass::test::PythonFoo >::value ;
	int d = lass::python::IsShadowType< lass::test::Bar >::value;

	std::cout << "int is shadow type : " << b << "\n";
	std::cout << "python foo is shadow type : " << c << "\n";
	std::cout << "bar is shadow type : " << d << "\n";
	return 0;
}

namespace lass
{
namespace test
{



	lass::test::PythonFooPtr getAFoo(  )
	{
		return lass::test::PythonFooPtr(new lass::test::PythonFoo(5, "hello"));
	}
	int anotherFreeFunction( int i )
	{
		::test();
		return i+1;
	}
	PY_DECLARE_MODULE( embedding )
	PY_MODULE_FUNCTION( embedding, anotherFreeFunction )
	PY_MODULE_FUNCTION( embedding, listInfo )
	PY_MODULE_FUNCTION( embedding, getAFoo )
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
	//PY_CLASS_METHOD( Bar, complexArguments );
	//PY_CLASS_METHOD( Bar, primArguments );
	
//	PY_CLASS_DECLARE_OVERLOADED_METHOD( Bar, tester)
//	PY_CLASS_OVERLOADED_METHOD( Bar, tester, complexArguments )
//	PY_CLASS_OVERLOADED_METHOD( Bar, tester, primArguments )
	PY_CLASS_METHOD_NAME_DOC( Bar, complexArguments, "tester", "tester doc");
	PY_CLASS_METHOD_NAME( Bar, primArguments, "tester");
	PY_CLASS_METHOD_QUALIFIED( Bar, overloaded, void, LASS_TYPE_LIST_1(int) )
	PY_CLASS_METHOD_QUALIFIED( Bar, overloaded, void, LASS_TYPE_LIST_1(const std::string&) )

	PY_CLASS_MEMBER_RW( Bar, "int", getInt, setInt );   
	PY_CLASS_MEMBER_RW( Bar, "foo", getFoo, setFoo );   

#pragma LASS_FIXME("this is broken on intel700 but not on vc7, that's news!")
#if LASS_COMPILER_TYPE != LASS_COMPILER_TYPE_INTEL
	PY_CLASS_MEMBER_RW( Bar, "cool", coolMember, coolMember );   
#endif

	PY_CLASS_PUBLIC_MEMBER( Bar, publicInt );
	// inject the class in the module and provide documentation for it
	PY_INJECT_CLASS_IN_MODULE( Bar, embedding, "Documentation for class Bar." );
	
//	PY_INJECT_CLASS_IN_MODULE( TShadowBase, embedding, "Documentation for class shadow Base." );

}
}