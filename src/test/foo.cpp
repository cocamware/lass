/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/
#include "test_common.h"
#include "Foo.h"

namespace lass
{

namespace test
{
		std::string	PythonFoo ::pyRepr(void)
		{
			return std::string( "This string is the representation of Foo object at " + lass::num::str((long)this) );
		}
		
		PyObject* PythonFoo ::pyMake(PyObject *ignored, PyObject *args)
		{
			return (PyObject*)(new PythonFoo ());// Make new Python-able object
		}

		PythonFoo ::PythonFoo () 
		{
			privateInt_ = 0;
			privateString_ = "uninitialized string";
		}

		PythonFoo ::~PythonFoo ()
		{

		}

		float PythonFoo ::aFooMoreComplexFunction( float iA, float iB )
		{
			return iA+iB;
		}

		void  PythonFoo ::testFooAutomaticFunctionExport( int iA, float iB )
		{
			std::cout << "C++ testAutomaticFunctionExport : " << iA + iB << "\n";
		}
	}
}