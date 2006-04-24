/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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
util::CriticalSection PyObjectCounter::mutex_;

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
			std::cerr << "[LASS RUN MSG] DANGLING REFERENCE to lass::python::PyObjectPlus" << std::endl;
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

/** @ingroup Python
 *  @brief retrieve pointer to PyObject by its name in the script.
 *  @return borrowed reference to PyObject @a iName or NULL if 
 *		@a iName does not exist (_without_ setting an exception!)
 *	@throw Never
 */
PyObject* getPyObjectByName(const std::string& iName)
{
	PyObject* module = PyImport_AddModule("__main__");
	if (!module)
	{
		PyErr_Clear();
		return 0;
	}
	PyObject* dict = PyModule_GetDict(module);
	LASS_ASSERT(dict != 0);
	return PyDict_GetItemString(dict, const_cast<char*>(iName.c_str()));
}

// --- impl ----------------------------------------------------------------------------------------

namespace impl
{

StaticMemberEqual::StaticMemberEqual(const char* iName):
	name_(iName)
{
}

bool StaticMemberEqual::operator()(const StaticMember& iMember) const
{
	return iMember.name && strcmp(iMember.name, name_) == 0;
}

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
StaticMember createStaticMember(const char* iName, const char* iDocumentation, PyObject* iObject,
		PyTypeObject* iParentType, std::vector<PyMethodDef>* iMethods, std::vector<PyGetSetDef>* iGetSetters,
		const std::vector<StaticMember>* iStatics)
{
	StaticMember temp;
	temp.name = iName;
	temp.doc = iDocumentation;
	temp.object = iObject;
	temp.parentType = iParentType;
	temp.methods = iMethods;
	temp.getSetters = iGetSetters;
	temp.statics = iStatics;
	return temp;
}
		
/** @internal
*/
PyMethodDef createPyMethodDef( const char *ml_name, PyCFunction ml_meth, int ml_flags, const char *ml_doc )
{
	PyMethodDef temp;
	temp.ml_name = const_cast<char*>(ml_name);
	temp.ml_meth = ml_meth;
	temp.ml_flags = ml_flags;
	temp.ml_doc = const_cast<char*>(ml_doc);
	return temp;
}

/** @internal
*/
PyGetSetDef createPyGetSetDef( const char* name, getter get, setter set, const char* doc, void* closure )
{
	PyGetSetDef temp;
	temp.name = const_cast<char*>(name);
	temp.get = get;
	temp.set = set;
	temp.doc = const_cast<char*>(doc);
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
		PyDict_SetItemString(iPyType.tp_dict, const_cast<char*>(i->name), i->object);
	}
}

/** @internal
*/
void finalizePyType(PyTypeObject& iPyType, PyTypeObject& iPyParentType, 
	std::vector<PyMethodDef>& iMethods, std::vector<PyGetSetDef>& iGetSetters, 
	const std::vector<StaticMember>& iStatics, const char* iModuleName, const char* iDocumentation)
{
	std::string fullName;
	if (iModuleName)
		fullName = std::string(iModuleName) + std::string(".") + std::string(iPyType.tp_name);
	else	// without module, aimed at utility objects, such as PySequence, PyMap
		fullName = std::string(iPyType.tp_name);
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

/** @internal
*/
void addMessageHeader(const std::string& iHeader)
{
	if (!PyErr_Occurred() || !PyErr_ExceptionMatches(PyExc_TypeError))
	{
		return;
	}
	PyObject *type, *value, *traceback;
	PyErr_Fetch(&type, &value, &traceback);
	try
	{
		if (PyString_Check(value))
		{
			std::ostringstream buffer;
			buffer << iHeader << ": " << PyString_AsString(value);
			PyObject* temp = PyString_FromString(buffer.str().c_str());
			std::swap(value, temp);
			Py_DECREF(temp);
		}
	}
	catch (const std::exception&)
	{
	}
	PyErr_Restore(type, value, traceback);
}

/** @internal
*/
bool checkSequenceSize(PyObject* iValue, int iExpectedSize)
{
	if (!PySequence_Check(iValue))
	{
		PyErr_SetString(PyExc_TypeError, "not a python sequence (tuple, list, ...)");
		return false;
	}
	const int size = PySequence_Size(iValue);
	if (size != iExpectedSize)
	{
		std::ostringstream buffer;
		buffer << "tuple/list is not of expected size " << iExpectedSize
			<< " (size is " << size << ")";
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
		return false;
	}
	return true;
}

}
}
}
