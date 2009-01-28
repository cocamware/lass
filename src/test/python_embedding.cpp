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



#include "test_common.h"
#include "python_embedding.h"
#include "../util/python_api.h"
#include "../util/pyshadow_object.h"
#include "foo.h"
#include "bar.h"
#include "python_shadow.h"
#include <iostream>

#include "../util/callback_0.h"
#include "../util/callback_1.h"
#include "../util/callback_r_0.h"
#include "../util/callback_r_1.h"
#include "../util/callback_r_2.h"

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

typedef lass::test::PySpam TPySpam;

PY_SHADOW_DOWN_CASTERS(TPySpam)

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

	LASS_COUT << "int is shadow type : " << b << "\n";
	LASS_COUT << "python foo is shadow type : " << c << "\n";
	LASS_COUT << "bar is shadow type : " << d << "\n";
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
	LASS_COUT << "Simon says: " << iCallback() << std::endl;
}

void callR2(const util::CallbackR2<float, float, float>& iCallback)
{
	LASS_COUT << "f(5, 6) = " << iCallback(5, 6) << std::endl;
}

void overloadedA(int iA)
{
	LASS_COUT << "overloadedA " << iA << std::endl;
}

void overloadedB(const std::string& iA)
{
	LASS_COUT << "overloadedA " << iA << std::endl;
}

void overloadedB(const std::complex<float>& iA)
{
	LASS_COUT << "overloadedA " << iA << std::endl;
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
PY_MODULE_FUNCTION( embedding, testPolymorphism )
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

PY_DECLARE_CLASS( DerivedBar )
PY_CLASS_CONSTRUCTOR( DerivedBar , meta::NullType );

// expose member methods of object instances
PY_CLASS_METHOD( Bar, aMoreComplexFunction )
PY_CLASS_METHOD( Bar, testAutomaticFunctionExport );
PY_CLASS_METHOD( Bar, complexArguments );
PY_CLASS_METHOD( Bar, primArguments );
PY_CLASS_METHOD_NAME_DOC( Bar, complexArguments, "tester", "tester doc");
PY_CLASS_METHOD_NAME( Bar, primArguments, "tester");
PY_CLASS_METHOD_QUALIFIED_1( Bar, overloaded, void, int )
PY_CLASS_METHOD_QUALIFIED_1( Bar, overloaded, void, const std::string& )
PY_CLASS_METHOD_EX( Bar, operator(), lass::python::methods::_call_, 0, BarCallOperator )
PY_CLASS_METHOD_NAME( Bar, call, lass::python::methods::_call_)
PY_CLASS_FREE_METHOD(Bar, freeMethodA);
PY_CLASS_FREE_METHOD(Bar, freeMethodB);
PY_CLASS_FREE_METHOD_NAME(Bar, freeCall, lass::python::methods::_call_);
PY_CLASS_FREE_METHOD_NAME(Bar, freeRepr, lass::python::methods::_repr_);
PY_CLASS_FREE_METHOD_NAME(Bar, freeStr, lass::python::methods::_str_);


PY_CLASS_MEMBER_RW_NAME( Bar, getInt, setInt, "int" );
PY_CLASS_MEMBER_RW_NAME( Bar, getFoo, setFoo, "foo" );
PY_CLASS_MEMBER_RW_NAME( Bar, coolMember, coolMember, "cool" );


PY_CLASS_PUBLIC_MEMBER( Bar, publicInt );
PY_CLASS_PUBLIC_MEMBER( Bar, writeableMap );
PY_CLASS_PUBLIC_MEMBER( Bar, writeableVector );
PY_CLASS_PUBLIC_MEMBER( Bar, writeableList );
PY_CLASS_PUBLIC_MEMBER( Bar, writeableDeque );
PY_CLASS_PUBLIC_MEMBER_R( Bar, constMap );
PY_CLASS_PUBLIC_MEMBER_R( Bar, constVector );
PY_CLASS_PUBLIC_MEMBER_R( Bar, constList );
PY_CLASS_PUBLIC_MEMBER_R( Bar, constDeque );


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
	PY_INJECT_CLASS_IN_MODULE( Bar, embedding, "Documentation for class Bar." );
	PY_INJECT_CLASS_IN_MODULE( DerivedBar, embedding, "Documentation for class DerivedBar." );)

}
}


// --- shadow classes ------------------------------------------------------------------------------


namespace lass
{
namespace test
{

PY_DECLARE_CLASS_NAME(PySpam, "Spam")
PY_CLASS_METHOD(PySpam, who)
PY_CLASS_MEMBER_R(PySpam, address)
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
PY_CLASS_MEMBER_RW(PyEggs, number, setNumber)
LASS_EXECUTE_BEFORE_MAIN(PY_INJECT_CLASS_IN_MODULE(PyEggs, embedding, "shadow eggs");)


}
}

#include "../util/non_copyable.h"

namespace lass
{
	namespace test
	{
		class Base
		{
		public:
			Base() {}
			virtual ~Base() {}
		};
		class ClassA :  public Base 
		{
		public:
			ClassA() {};
			virtual ~ClassA() {};
			virtual void abstractMethod() = 0;
		};
		class ClassB : public ClassA
		{
		public:
			ClassB() {}
			virtual ~ClassB() {}

			void testConst(const ClassA& iArg) {}
			void testNonConst(ClassA& iArg) {}
			void testConstPtr(ClassA const * iArg) {}
			void testNonConstPtr(ClassA* iArg) {}
			virtual void abstractMethod() {}

			int getitem(int i) { return i; }
			int len() const { return 5; }
			void setitem(int i, std::pair<float,int> iarg) { }
		};

		class ClassSeq : public std::vector<float>
		{
		public:
			ClassSeq() {}
			virtual ~ClassSeq() {}
			void setItem(int i, float value) { at(i) = value; }
			float pop(int i) { float temp = at(i); erase(begin()+i); return temp; }
			float popwo() { float temp = back(); pop_back(); return temp; }
			ClassSeq& irepeat(int n) { lass::stde::inplace_repeat_c(*this,n);  return *this;}
			ClassSeq& iconcat(const std::vector<float>& iV) { insert(end(),iV.begin(), iV.end());  return *this;}
			ClassSeq& setSlice(int ilow, int ihigh, const std::vector<float>& iV) 
			{
				erase(	begin()+ilow,begin()+ihigh );
				insert(	begin()+ilow+1,iV.begin(),iV.end());			
				return *this;
			}

			lass::python::PyIteratorRange* iter() { return new lass::python::PyIteratorRange(begin(), end()); }
		};

		class ClassMap : public std::map<std::string,float>
		{
		public:
			ClassMap() {}
			virtual ~ClassMap() {}
			void setItem(const std::string& iKey, float value) { operator[](iKey) = value; }
		};

		std::map<std::string,float>::const_iterator freeBegin(ClassMap* iThis)
		{
			return iThis->begin();
		}
		std::map<std::string,float>::const_iterator freeEnd(ClassMap* iThis)
		{
			return iThis->end();
		}


		PyObject* getMemberImagine(const ClassB * iThis )
		{
			Py_XINCREF(Py_None);
			return Py_None;
		}
		int setMemberImagine(const ClassB * iThis, PyObject* iObject )
		{
			// success
			return 0;
		}
		int properGetMemberImagine(const ClassB * iThis )
		{
			return 1;
		}
		void properSetMemberImagine(ClassB * iThis, int iWhat)
		{

		}

		const ClassA& testFreeConst(const ClassA& iArg)
		{
			return *static_cast<ClassA const * >(&iArg);
		}

		void testCopyConstructor()
		{
			ClassB b;

			b.testConst(b);
			testFreeConst(b);
		}

		void testFree(ClassB* iThis, const ClassA& iArg )
		{
			return;
		}

		ClassB freeConstructor(int iTest)
		{
			return ClassB();
		}

		int testConvertor(PyObject* iObject, ClassB& oOut)
		{
			return 0;
		}

	}
}


PY_SHADOW_CLASS(LASS_DLL_EXPORT, PyBase, lass::test::Base)
//PY_SHADOW_DOWN_CASTERS_NOCONSTRUCTOR( PyBase )
PY_SHADOW_DOWN_CASTERS( PyBase )
PY_DECLARE_CLASS_NAME( PyBase, "Base")

PY_SHADOW_CLASS_DERIVED_NOCONSTRUCTOR(LASS_DLL_EXPORT, PyClassA, lass::test::ClassA, PyBase)
PY_SHADOW_DOWN_CASTERS_NOCONSTRUCTOR( PyClassA )
PY_DECLARE_CLASS_NAME( PyClassA, "ClassA")

PY_SHADOW_CLASS_DERIVED(LASS_DLL_EXPORT, PyClassB, lass::test::ClassB, PyClassA)
PY_SHADOW_DOWN_CASTERS( PyClassB )
PY_DECLARE_CLASS_NAME( PyClassB, "ClassB")
PY_CLASS_CONSTRUCTOR_0( PyClassB)
PY_CLASS_FREE_CONSTRUCTOR_1(PyClassB, lass::test::freeConstructor, int)
PY_CLASS_DEPRECATED_FREE_MEMBER_RW_NAME_DOC( PyClassB, lass::test::getMemberImagine, lass::test::setMemberImagine, "imagine", "blabla")
PY_CLASS_DEPRECATED_FREE_MEMBER_R_NAME_DOC( PyClassB, lass::test::getMemberImagine, "imagine", "blabla")
PY_CLASS_FREE_MEMBER_RW_NAME_DOC( PyClassB, lass::test::properGetMemberImagine, lass::test::properSetMemberImagine, "properImagine", "blabla")
PY_CLASS_FREE_MEMBER_R_NAME_DOC( PyClassB, lass::test::properGetMemberImagine, "properImagine", "blabla")


// Special methods
// sequence protocol
PY_CLASS_METHOD_NAME( PyClassB, getitem, lass::python::methods::_getitem_);
PY_CLASS_METHOD_NAME( PyClassB, len, lass::python::methods::_len_);
PY_CLASS_METHOD_NAME( PyClassB, setitem, lass::python::methods::_setitem_);
PY_CLASS_METHOD_NAME( PyClassB, len, lass::python::methods::_len_);

// full sequence protocol
PY_SHADOW_CLASS(LASS_DLL_EXPORT, PyClassSeq, lass::test::ClassSeq)
PY_SHADOW_DOWN_CASTERS( PyClassSeq )
PY_DECLARE_CLASS_NAME( PyClassSeq, "ClassSeq")
PY_CLASS_CONSTRUCTOR_0( PyClassSeq)
PY_CLASS_METHOD_NAME( PyClassSeq, push_back, "append");
PY_CLASS_METHOD_NAME( PyClassSeq, clear, "clear");
PY_CLASS_METHOD_NAME( PyClassSeq, pop, "pop");
PY_CLASS_METHOD_NAME( PyClassSeq, popwo, "pop");
PY_CLASS_METHOD_NAME( PyClassSeq, irepeat, lass::python::methods::_irepeat_);
PY_CLASS_METHOD_NAME( PyClassSeq, iconcat, lass::python::methods::_iconcat_);

PY_CLASS_METHOD_NAME( PyClassSeq, size, lass::python::methods::_len_);
PY_CLASS_METHOD_NAME( PyClassSeq, iter, lass::python::methods::_iter_);

PY_CLASS_METHOD_QUALIFIED_NAME_1( PyClassSeq, operator[], std::vector<float>::const_reference, std::vector<float>::size_type, lass::python::methods::_getitem_);
PY_CLASS_METHOD_NAME( PyClassSeq, setItem, lass::python::methods::_setitem_);
PY_CLASS_METHOD_NAME( PyClassSeq, setSlice, lass::python::methods::_setslice_);

// full map protocol
PY_SHADOW_CLASS(LASS_DLL_EXPORT, PyClassMap, lass::test::ClassMap)
PY_SHADOW_DOWN_CASTERS( PyClassMap )
PY_DECLARE_CLASS_NAME( PyClassMap, "ClassMap")
PY_CLASS_CONSTRUCTOR_0( PyClassMap)
PY_CLASS_METHOD_NAME( PyClassMap, setItem, lass::python::methods::map_setitem_);
PY_CLASS_METHOD_NAME( PyClassMap, operator[], lass::python::methods::map_getitem_);
PY_CLASS_METHOD_NAME( PyClassMap, size, lass::python::methods::map_len_);
PY_CLASS_ITERFUNC( PyClassMap, begin, end )
PY_CLASS_FREE_ITERFUNC( PyClassMap, lass::test::freeBegin, lass::test::freeEnd )


namespace lass { namespace test {
LASS_EXECUTE_BEFORE_MAIN(
	PY_INJECT_CLASS_IN_MODULE( PyClassA, embedding, "Documentation for class A." );
	PY_INJECT_CLASS_IN_MODULE( PyClassB, embedding, "Documentation for class B." );
	PY_INJECT_CLASS_IN_MODULE( PyClassSeq, embedding, "Documentation for class Seq." );
	PY_INJECT_CLASS_IN_MODULE( PyClassMap, embedding, "Documentation for class Map." );
	)
} }


//This won't be accepted due to A being an abstract class
//PY_CLASS_METHOD_QUALIFIED_1( PyClassB, testConst, void, const lass::test::ClassA& )

PY_CLASS_METHOD_CAST_1( PyClassB, testConst, void, lass::python::PointerCast<const lass::test::ClassA&>  )
//PY_CLASS_METHOD_CAST_1( PyClassB, testNonConst, void, lass::python::PointerCast<lass::test::ClassA&>  )

namespace lass { namespace python {
template<typename T>
struct CustomCast : public Caster
{
	typedef T TSelf;
	typedef T TTarget;
	static TSelf cast(TTarget iArg) 
	{ 
		TSelf temp = new TSelf();
		*temp = iArg;
		return temp; 
	}
};
template<typename T>
struct CustomCast<T&> : public Caster
{
	typedef T& TSelf;
	typedef T* TTarget;
	static TSelf cast(TTarget iArg) 
	{ 
		if (iArg)
			return *iArg; 
		LASS_THROW("Cannot use None as argument");
	}
};


template<typename T>
struct IsACaster<CustomCast<T> >
{
	typedef lass::meta::True TValue;
};

}
}
PY_CLASS_METHOD_CAST_NAME_1( PyClassB, testNonConst, void, lass::python::CustomCast<lass::test::ClassA&>, "name"  )

