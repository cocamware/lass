/**	@file
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



#include "util_common.h"
#include "pyobject_plus.h"
#include "pyobject_util.h"

namespace lass
{
	namespace python
	{
		PyTypeObject PyObjectPlus::Type = { PY_STATIC_FUNCTION_FORWARD( PyObjectPlus, "PyObjectPlus" ) };

		std::vector<PyMethodDef> initAbstractMethods()
		{
			std::vector<PyMethodDef> temp;
			temp.push_back( createPyMethodDef( 0, 0, 0, 0 ) );
			return temp;
			 
		}
		std::vector<PyMethodDef> PyObjectPlus::Methods = initAbstractMethods();


		PyObjectPlus::PyObjectPlus(PyTypeObject *T) 
		{
			this->ob_type = T;
			_Py_NewReference( this );
		};

		PyObjectPlus::~PyObjectPlus() 
		{  
		};
		
		void PyObjectPlus::__dealloc(PyObject *P)    
		{  
			delete ((PyObjectPlus *) P);  
		};

		PyObject*	PyObjectPlus::__repr(PyObject *PyObj) 
		{	
			return pyBuildSimpleObject(((TSelf*)PyObj)->pyRepr());
		}

		PyObject*	PyObjectPlus::__str(PyObject *PyObj) 
		{ 
			return pyBuildSimpleObject(((TSelf*)PyObj)->pyStr()); 
		}

		PyObjectPlus* PyObjectPlus::PyPlus_INCREF(void) 
		{	
			Py_INCREF(this);
			return this;
		};

		PyObjectPlus* PyObjectPlus::PyPlus_DECREF(void) 
		{
			Py_DECREF(this);
			return this;
		};
	}
}
