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
 *	Copyright (C) 2004-2025 the Initial Developer.
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
#include "bar.h"
#include "../lass/stde/extended_string.h"

PY_DECLARE_STR_ENUM_EX(lass::test::Bar::Shape)("Shape", "Shape of a Bar", {
	{ "CIRCLE", lass::test::Bar::Shape::Circle, "circle"},
	{ "SQUARE", lass::test::Bar::Shape::Square, "square"},
	{ "TRIANGLE", lass::test::Bar::Shape::Triangle, "triangle"},
	});

namespace lass
{
	namespace test
	{
		PY_DECLARE_CLASS( Bar )
		PY_CLASS_CONSTRUCTOR( Bar , meta::NullType );
		PY_CLASS_CONSTRUCTOR_2( Bar, int, const std::string& );
		PY_CLASS_STATIC_METHOD( Bar, aStaticMethod );
		PY_CLASS_METHOD( Bar, aMoreComplexFunction )
		PY_CLASS_METHOD( Bar, testAutomaticFunctionExport );
		PY_CLASS_METHOD( Bar, complexArguments );
		PY_CLASS_METHOD( Bar, primArguments );
		PY_CLASS_METHOD( Bar, rgba );
		PY_CLASS_METHOD( Bar, opaquePointer );
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
		PY_CLASS_ENUM(Bar, Bar::Shape);
		PY_CLASS_STATIC_METHOD(Bar, passShape);
		PY_CLASS_STATIC_METHOD(Bar, getShape);
		PY_CLASS_STATIC_METHOD(Bar, badShape);
		PY_CLASS_STATIC_METHOD(Bar, isTriangle);
		PY_CLASS_MEMBER_RW_NAME( Bar, getInt, setInt, "myInt" );
		PY_CLASS_MEMBER_RW_NAME( Bar, getFoo, setFoo, "foo" );
		PY_CLASS_MEMBER_RW_NAME( Bar, coolMember, coolMember, "cool" );
		PY_CLASS_PUBLIC_MEMBER( Bar, publicInt );
		PY_CLASS_PUBLIC_MEMBER( Bar, writeableMap );
		PY_CLASS_PUBLIC_MEMBER( Bar, writeableVectorMap );
		PY_CLASS_PUBLIC_MEMBER( Bar, writeableVector );
		PY_CLASS_PUBLIC_MEMBER( Bar, writeableList );
		PY_CLASS_PUBLIC_MEMBER( Bar, writeableDeque );
		PY_CLASS_PUBLIC_MEMBER( Bar, writeableStaticVector );
		PY_CLASS_PUBLIC_MEMBER_R( Bar, constMap );
		PY_CLASS_PUBLIC_MEMBER_R( Bar, constVector );
		PY_CLASS_PUBLIC_MEMBER_R( Bar, constList );
		PY_CLASS_PUBLIC_MEMBER_R( Bar, constDeque );
		PY_CLASS_METHOD( Bar, testConstMap );
		PY_CLASS_STATIC_CONST( Bar, "CONST", 5 ); // a const value as class member
		PY_CLASS_METHOD_NAME( Bar, operator==, python::methods::_eq_ );
		PY_CLASS_METHOD_NAME( Bar, operator<, python::methods::_lt_ );
		PY_CLASS_FREE_METHOD_NAME( Bar, operator!=, python::methods::_ne_ );
		PY_CLASS_FREE_METHOD_NAME( Bar, operator>, python::methods::_gt_ );
		PY_CLASS_FREE_METHOD_NAME( Bar, operator<=, python::methods::_le_ );
		PY_CLASS_FREE_METHOD_NAME( Bar, operator>=, python::methods::_ge_ );
		PY_CLASS_METHOD_NAME(Bar, getItem, python::methods::map_getitem_);
		PY_CLASS_METHOD_NAME(Bar, setItem, python::methods::map_setitem_);
		PY_CLASS_METHOD_NAME(Bar, setItem2, python::methods::map_setitem_);
		PY_CLASS_METHOD_NAME(Bar, delItem2, python::methods::map_delitem_);
		PY_CLASS_METHOD_NAME(Bar, delItem, python::methods::map_delitem_);
		PY_CLASS_METHOD_NAME(Bar, contains, python::methods::_contains_);
		PY_CLASS_METHOD_NAME(Bar, size, python::methods::map_len_);
		PY_CLASS_METHOD_NAME(Bar, iter, lass::python::methods::_iter_);
		PY_CLASS_STATIC_METHOD( Bar, makeUniquePtr )

		// innerclass of Bar
		typedef Bar::InnerClass TBarInnerClass;
		PY_DECLARE_CLASS_NAME( TBarInnerClass, "InnerClass" )
		PY_CLASS_CONSTRUCTOR_1( TBarInnerClass, const std::string& );
		PY_CLASS_METHOD( TBarInnerClass, talkTo );

		constexpr auto innerclass_doc = R"(InnerClass is an inner class of Bar.
InnerClass has a method talkTo that returns a string.
InnerClass is used to demonstrate inner classes in Python.)";
		PY_CLASS_INNER_CLASS_NAME_DOC( Bar, TBarInnerClass, "InnerClass", innerclass_doc);

		PY_DECLARE_CLASS( DerivedBar )
		PY_CLASS_CONSTRUCTOR( DerivedBar , meta::NullType );

		void listInfo( const python::TPyObjPtr& iObject )
		{
			LASS_COUT << "Name : " << iObject->ob_type->tp_name << "\n";
			LASS_COUT << "Methods : \n";
			int i=0;
			if (iObject->ob_type->tp_methods)
			{
				while (true)
				{

					if (iObject->ob_type->tp_methods[i].ml_name !=NULL)
					{
						LASS_COUT << iObject->ob_type->tp_methods[i].ml_name << "\n";
						++i;
					}
					else
						break;
				}
			}
		}

		int freeFunction( int i )
		{
			LASS_COUT << "Freefunction " << i << "\n";
			return i+1;
		}

		int Bar::aStaticMethod( float ia )
		{
			return static_cast<int>(ia);
		}

		namespace impl
		{
			std::map<std::string, std::string> makeMap()
			{
				std::map<std::string, std::string> map;
				map["spam"] = "spam spam spam";
				return map;
			}
		}

		Bar::Bar():
			constMap(impl::makeMap())
		{
			privateInt_ = 0;
			privateString_ = "uninitialized string";
			coolMember_ = 1.f;
			publicInt = 42;
			writeableMap.reset(new std::map<std::string, std::string>);
			writeableVectorMap.reset(new stde::vector_map<std::string, std::string>);
			writeableVector.reset(new std::vector<double>);
			writeableList.reset(new std::list<double>);
			writeableDeque.reset(new std::deque<double>);
			writeableStaticVector.reset(new stde::static_vector<double, 128>);
		}

		Bar::Bar(int iA, const std::string& iB) 
		{
			privateInt_ = iA;
			privateString_ = iB;
			coolMember_ = 1.f;
			publicInt = 42;
		}

		Bar::~Bar()
		{

		}

		float Bar::aMoreComplexFunction( float iA, float iB )
		{
			return iA+iB;
		}

		std::string Bar::testAutomaticFunctionExport( int iA, float iB )
		{
			std::ostringstream stream;
			stream << "C++ testAutomaticFunctionExport : " << static_cast<float>(iA) + iB;
			return stream.str();
		}

		std::string Bar::complexArguments( const std::string& iA )
		{
			std::ostringstream stream;
			stream << "complexArguments: '" << iA << "'";
			return stream.str();
		}

		std::string Bar::primArguments(const prim::Aabb3D<float>& iAabb, prim::XYZ iAxis,
			const prim::Transformation3D<double>& iTransformation)
		{
			std::ostringstream stream;
			stream << "size of box along " << iAxis << " axis is " << iAabb.size()[iAxis] << "\n";
			stream << "transformation: " << iTransformation << std::endl;
			return stream.str();
		}

		prim::ColorRGBA Bar::rgba(const prim::ColorRGBA& c)  noexcept
		{
			return c;
		}

		void Bar::overloaded( int iA )
		{
			LASS_COUT << "Bar::overloaded(int): " << iA << std::endl;
		}

		void Bar::overloaded( const std::string& iB )
		{
			LASS_COUT << "Bar::overloaded(const std::string&): " << iB << std::endl;
		}

		int Bar::operator()(int iA) const
		{
			LASS_COUT << "Bar::operator()(int): " << this << " " << iA << std::endl;
			return 2 * iA;
		}
		std::string Bar::call(const std::string& iA) const
		{
			LASS_COUT << "Bar::call(const std::string&): " << this << " " << iA << std::endl;
			return stde::toupper(iA);
		}

		Bar::Shape Bar::passShape(Shape shape)
		{
			return shape;
		}
		Bar::Shape Bar::getShape()
		{
			return Shape::Square;
		}
		Bar::Shape Bar::badShape()
		{
			return static_cast<Shape>(123);
		}
		bool Bar::isTriangle(Shape shape)
		{
			return shape == Shape::Triangle;
		}

		int Bar::getInt() const noexcept
		{
			return privateInt_;
		}
		
		int Bar::getInt() noexcept
		{
			return privateInt_;
		}

		void Bar::setInt( int iArg ) noexcept
		{
			privateInt_ = iArg;
		}

		const PythonFooPtr& Bar::getFoo() const
		{
			return ptr2Foo_;
		}

		void Bar::setFoo( const PythonFooPtr& iFoo)
		{
			ptr2Foo_ = iFoo;
		}

		const PythonFoo& Bar::getFooRef() const
		{
			return *ptr2Foo_.get();
		}
		void Bar::setFooRef( const PythonFoo&)
		{
			// just function signature test
		}

		float Bar::coolMember() const
		{
			return coolMember_;
		}

		float& Bar::coolMember()
		{
			return coolMember_;
		}

		std::map<std::string, std::string> Bar::testConstMap(const std::map<std::string, std::string>& map)
		{
			return std::map<std::string, std::string>(map);
		}

		void* Bar::opaquePointer() const
		{
			return 0;
		}

		Bar::InnerClass::InnerClass(const std::string& iSayWhat):
			sayWhat_(iSayWhat)
		{
		}

		float DerivedBar::aMoreComplexFunction( float iA, float iB )
		{
			return iA*iB;
		}


		std::string Bar::InnerClass::talkTo(const std::string& iName)
		{
			return iName + ", " + sayWhat_ + ".\n";
		}

		bool Bar::operator==(const Bar& other) const
		{
			return privateInt_ == other.privateInt_;
		}

		bool Bar::operator<(const Bar& other) const
		{
			return privateInt_ < other.privateInt_;
		}

		bool operator!=(const Bar& a, const Bar& b)
		{
			return !(a == b);
		}

		bool operator>(const Bar& a, const Bar& b)
		{
			return b < a;
		}

		bool operator<=(const Bar& a, const Bar& b)
		{
			return !(b < a);
		}

		bool operator>=(const Bar& a, const Bar& b)
		{
			return !(a < b);
		}

		std::string Bar::getItem(const std::string& key) const
		{
			const TMap::const_iterator i = map_.find(key);
			if (i == map_.end())
			{
				python::LockGIL LASS_UNUSED(lock);
				throw python::PythonException(PyExc_KeyError, key);
			}
			return i->second;
		}
		void Bar::setItem(const std::string& key, const std::string& value)
		{
			map_[key] = value;
		}
		void Bar::setItem2(const std::string& key, int value)
		{
			map_[key] = util::stringCast<std::string>(value);
		}
		void Bar::delItem(const std::string& key)
		{
			if (!map_.erase(key))
			{
				python::LockGIL LASS_UNUSED(lock);
				throw python::PythonException(PyExc_KeyError, key);
			}
		}
		void Bar::delItem2(int index)
		{
			throw python::PythonException(PyExc_IndexError, util::stringCast<std::string>(index));
		}
		bool Bar::contains(const std::string& key)
		{
			return map_.find(key) != map_.end();
		}
		size_t Bar::size() const
		{
			return map_.size();
		}
		lass::python::PyIteratorRange* Bar::iter()
		{
			return new lass::python::PyIteratorRange(map_.begin(), map_.end());
		}

		std::string freeMethodA(const Bar& bar, const std::string& a)
		{
			std::ostringstream stream;
			stream << "freeMethodA(const Bar&, const std::string&): " << &bar << " " << a;
			return stream.str();
		}
		std::string freeMethodB(Bar& bar, const std::string& a)
		{
			std::ostringstream stream;
			stream << "freeMethodB(Bar*, const std::string&): " << &bar << " " << a;
			return stream.str();
		}
		float freeCall(const Bar& bar, float a)
		{
			LASS_COUT << "float freeCall(const Bar*, float a): " << &bar << " " << a << std::endl;
			return a / 2;
		}
		std::string freeStr(const Bar&)
		{
			return std::string("A free __str__ representation of Bar");
		}
		std::string freeRepr(const Bar&)
		{
			return std::string("A free __repr__ representation of Bar");
		}
		std::unique_ptr<Bar> Bar::makeUniquePtr()
		{
			std::unique_ptr<Bar> ptr(new Bar);
			return ptr;
		}

		// --- shadow classes ---

		/*typedef python::PyShadowClass<PythonFoo> ShadowFoo;

		PY_DECLARE_CLASS_EX ( ShadowFoo, "ShadowFoo" )
		PY_DECLARE_CLASS_EX ( ShadowBar, "ShadowBar" )
		PY_CLASS_METHOD( ShadowBar, setFooRef )
		*/
	}
}
