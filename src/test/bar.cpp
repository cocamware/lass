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
#include "bar.h"

namespace lass
{

namespace test
{
		void listInfo( PyObject* iObject )
		{
			std::cout << "Name : " << iObject->ob_type->tp_name << "\n";
			std::cout << "Methods : \n";
			int i=0;
			if (iObject->ob_type->tp_methods)
			{
				while (true)
				{

					if (iObject->ob_type->tp_methods[i].ml_name !=NULL)
					{
						std::cout << iObject->ob_type->tp_methods[i].ml_name << "\n";
						++i;
					}
					else
						break;
				}
			}
		}	

		int freeFunction( int i )
		{
			std::cout << "Freefunction " << i << "\n";
			return i+1;
		}

		std::string	Bar::repr(void)
		{
			return std::string( "This string is the representation of Bar object at " + lass::num::str((long)this) );
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

		Bar::~Bar()
		{

		}

		float Bar::aMoreComplexFunction( float iA, float iB )
		{
			return iA+iB;
		}

		void  Bar::testAutomaticFunctionExport( int iA, float iB )
		{
			std::cout << "C++ testAutomaticFunctionExport : " << iA + iB << "\n";
		}

        void Bar::complexArguments( const std::string& iA )
        {
            std::cout << "complexArguments: '" << iA << "'\n";
        };

		prim::Point3D<float> Bar::primArguments(const prim::Aabb3D<float>& iAabb, prim::XYZ iAxis)
		{
			std::cout << "size of box along " << iAxis << " axis is " << iAabb.size()[iAxis] << "\n";
			return iAabb.center().affine();
		}

		void Bar::overloaded( int iA )
		{
			std::cout << "Bar::overloaded(int): " << iA << std::endl;
		}

		void Bar::overloaded( const std::string& iB )
		{
			std::cout << "Bar::overloaded(const std::string&): " << iB << std::endl;
		}

		int	Bar::getInt() const
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

		/*
		template< class T>
		class ShadowObject 
			: public lass::python::PyObjectPlus
		{
			PY_HEADER( lass::python::PyObjectPlus );

			T*	CppObject_;
			typedef T	OriginalCppClass;
		public:

			ShadowObject() : PyObjectPlus( &Type ), CppObject_(NULL) {}
			ShadowObject( const T& iC ) : PyObjectPlus(&Type), CppObject_(&iC) {}
			virtual ~ShadowObject () {}

		};

		typedef ShadowObject<PythonFoo> ShadowFoo;
		typedef ShadowObject<PythonBar> ShadowBar;

		PY_DECLARE_CLASS_EX ( ShadowFoo, "ShadowFoo" )
		PY_DECLARE_CLASS_EX ( ShadowBar, "ShadowBar" )
		PY_CLASS_METHOD( ShadowBar, setFooRef ) 
		*/




}
}