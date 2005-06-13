/** @file
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
	temp.push_back( impl::createPyMethodDef( 0, 0, 0, 0 ) );
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
	if (this->ob_refcnt>0)
	{
		if (this->ob_refcnt>1)
			std::cerr << "DANGLING REFERENCE to lass::python::PyObjectPlus" << std::endl;
		--this->ob_refcnt;
		_Py_ForgetReference( this );

	}
	LASS_ASSERT(this->ob_refcnt==0);
};

void PyObjectPlus::__dealloc(PyObject *P)
{
	delete ((PyObjectPlus *) P);
};

PyObject*   PyObjectPlus::__repr(PyObject *PyObj)
{
	return pyBuildSimpleObject(((TSelf*)PyObj)->pyRepr());
}

PyObject*   PyObjectPlus::__str(PyObject *PyObj)
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
}

PyObjectPlus::PyObjectPlus(const PyObjectPlus& iOther)
{
	this->ob_type = iOther.ob_type;
	_Py_NewReference( this );
}

PyObjectPlus& PyObjectPlus::operator =(const PyObjectPlus& iOther)
{
	LASS_ASSERT(this->ob_type == iOther.ob_type);
	return *this;
}

// --- impl ----------------------------------------------------------------------------------------

namespace impl
{

PyMethodEqual::PyMethodEqual( const char* iName ):
	name_(iName)
{
}

bool PyMethodEqual::operator()(const PyMethodDef& iMethod) const
{
	return iMethod.ml_name && strcmp(iMethod.ml_name, name_) == 0;
}

/** @internal
*/
PyMethodDef createPyMethodDef( char *ml_name, PyCFunction ml_meth, int ml_flags, char *ml_doc )
{
	PyMethodDef temp;
	temp.ml_name = ml_name;
	temp.ml_meth = ml_meth;
	temp.ml_flags = ml_flags;
	temp.ml_doc = ml_doc;
	return temp;
}

/** @internal
*/
PyGetSetDef createPyGetSetDef( char* name, getter get, setter set, char* doc, void* closure )
{
	PyGetSetDef temp;
	temp.name = name;
	temp.get = get;
	temp.set = set;
	temp.doc = doc;
	temp.closure = closure;
	return temp;
}

/** @internal
*/
void injectStaticMembers(PyTypeObject& iPyType, const std::vector<StaticMember>& iStatics)
{
	for (std::vector<StaticMember>::const_iterator i = iStatics.begin(); i != iStatics.end(); ++i)
	{
		if (PyType_Check(i->object))
		{
			finalizePyType(*reinterpret_cast<PyTypeObject*>(i->object), *i->parentType, 
				*i->methods, *i->getSetters, *i->statics, iPyType.tp_name, i->doc);
		}
		PyDict_SetItemString(iPyType.tp_dict, i->name, i->object);
	}
}

/** @internal
*/
void finalizePyType(PyTypeObject& iPyType, PyTypeObject& iPyParentType, 
	std::vector<PyMethodDef>& iMethods, std::vector<PyGetSetDef>& iGetSetters, 
	const std::vector<StaticMember>& iStatics, const char* iModuleName, const char* iDocumentation)
{
	std::string fullName = 
		std::string(iModuleName) + std::string(".") + std::string(iPyType.tp_name);
	char* fullNameCharPtr = new char[fullName.size()+1];
	strcpy(fullNameCharPtr,fullName.c_str());

	iPyType.tp_name = fullNameCharPtr;
	iPyType.tp_methods = &iMethods[0];
	iPyType.tp_getset = &iGetSetters[0];
	iPyType.tp_doc = const_cast<char*>(iDocumentation);
	iPyType.tp_base = &iPyParentType;
	LASS_ENFORCE( PyType_Ready( &iPyType ) >= 0 );
	Py_INCREF( &iPyType );

	injectStaticMembers(iPyType, iStatics);
}

/** @internal
*/
void addModuleFunction(std::vector<PyMethodDef>& ioModuleMethods, char* iMethodName, 
	char* iDocumentation, PyCFunction iMethodDispatcher, PyCFunction& oOverloadChain)
{
	::std::vector<PyMethodDef>::iterator i = ::std::find_if(
		ioModuleMethods.begin(), ioModuleMethods.end(),	PyMethodEqual(iMethodName));
	if (i == ioModuleMethods.end())
	{
		ioModuleMethods.insert(ioModuleMethods.begin(), createPyMethodDef(
			iMethodName, iMethodDispatcher, METH_VARARGS , iDocumentation));
		oOverloadChain = 0;
	}
	else
	{
		oOverloadChain = i->ml_meth;
		i->ml_meth = iMethodDispatcher;
	};
}

}
}
}
