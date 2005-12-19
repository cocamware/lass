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
#include "bar.h"

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

		std::string Bar::pyRepr(void)
		{
			return std::string( "The representation of Bar object at " + lass::util::stringCast<std::string>(this) );
		}

		std::string Bar::pyStr(void)
		{
			return std::string( "The string of Bar object at " + lass::util::stringCast<std::string>(this) );
		}

		int Bar::aStaticMethod( float ia )
		{
			return static_cast<int>(ia);
		}

		Bar::Bar() : PyObjectPlus(&Type )
		{
			privateInt_ = 0;
			privateString_ = "uninitialized string";
			coolMember_ = 1.f;
		}

		Bar::Bar(int iA, const std::string& iB) : PyObjectPlus(&Type )
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
		};

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
			PyObjectPlus(&Type),
			sayWhat_(iSayWhat)
		{
		}

		std::string Bar::InnerClass::talkTo(const std::string& iName)
		{
			return iName + ", " + sayWhat_ + ".\n";
		}

		// --- shadow classes ---

		/*typedef python::PyShadowClass<PythonFoo> ShadowFoo;

		PY_DECLARE_CLASS_EX ( ShadowFoo, "ShadowFoo" )
		PY_DECLARE_CLASS_EX ( ShadowBar, "ShadowBar" )
		PY_CLASS_METHOD( ShadowBar, setFooRef )
		*/
	}
}
