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
 *	Copyright (C) 2004-2009 the Initial Developer.
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
#include "../stde/extended_string.h"

namespace lass
{

namespace test
{
		void listInfo( PyObject* iObject )
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

		std::string Bar::doPyRepr(void)
		{
			return std::string( "The representation of Bar object at " + lass::util::stringCast<std::string>(this) );
		}

		std::string Bar::doPyStr(void)
		{
			return std::string( "The string of Bar object at " + lass::util::stringCast<std::string>(this) );
		}

		int Bar::aStaticMethod( float ia )
		{
			return static_cast<int>(ia);
		}

		Bar::Bar() 
		{
			privateInt_ = 0;
			privateString_ = "uninitialized string";
			coolMember_ = 1.f;
		}

		Bar::Bar(int iA, const std::string& iB) 
		{
			privateInt_ = iA;
			privateString_ = iB;
			coolMember_ = 1.f;
		}

		Bar::~Bar()
		{

		}

		float Bar::aMoreComplexFunction( float iA, float iB )
		{
			return iA+iB;
		}

		void  Bar::testAutomaticFunctionExport( int iA, float iB )
		{
			LASS_COUT << "C++ testAutomaticFunctionExport : " << iA + iB << "\n";
		}

		void Bar::complexArguments( const std::string& iA )
		{
			LASS_COUT << "complexArguments: '" << iA << "'\n";
		}

		prim::Point3D<float> Bar::primArguments(const prim::Aabb3D<float>& iAabb, prim::XYZ iAxis,
			const prim::Transformation3D<double>& iTransformation)
		{
			LASS_COUT << "size of box along " << iAxis << " axis is " << iAabb.size()[iAxis] << "\n";
			LASS_COUT << "transformation: " << iTransformation << std::endl;
			return iAabb.center().affine();
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

		int Bar::getInt() const
		{
			return privateInt_;
		}

		void Bar::setInt( int iArg )
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
		void Bar::setFooRef( const PythonFoo& iFoo)
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

		void freeMethodA(const Bar& bar, const std::string& a)
		{
			LASS_COUT << "void (const Bar&, const std::string&): " << &bar << " " << a << std::endl;
		}
		void freeMethodB(Bar* bar, const std::string& a)
		{
			LASS_COUT << "void (Bar*, const std::string&): " << bar << " " << a << std::endl;
		}
		float freeCall(const Bar* bar, float a)
		{
			LASS_COUT << "float freeCall(const Bar*, float a): " << bar << " " << a << std::endl;
			return a / 2;
		}
		std::string freeStr(const Bar* bar)
		{
			return std::string("A free __str__ representation of Bar");
		}
		std::string freeRepr(const Bar* bar)
		{
			return std::string("A free __repr__ representation of Bar");
		}

		// --- shadow classes ---

		/*typedef python::PyShadowClass<PythonFoo> ShadowFoo;

		PY_DECLARE_CLASS_EX ( ShadowFoo, "ShadowFoo" )
		PY_DECLARE_CLASS_EX ( ShadowBar, "ShadowBar" )
		PY_CLASS_METHOD( ShadowBar, setFooRef )
		*/
	}
}
