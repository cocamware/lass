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
//util::Semaphore PyObjectCounter::sync_;

std::vector<PyMethodDef> initAbstractMethods()
{
	std::vector<PyMethodDef> temp;
	temp.push_back( impl::createPyMethodDef( 0, 0, 0, 0 ) );
	return temp;

}
std::vector<PyMethodDef> PyObjectPlus::Methods = initAbstractMethods();


PyObjectPlus::PyObjectPlus()
{
	// initializing the type to NULL, when the object is exported to python the type is fixed
	this->ob_type = NULL;	
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
	LASS_ASSERT(!this->ob_type || this->ob_type == iOther.ob_type);
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

OverloadLink::OverloadLink()
{
	setNull();
}

void OverloadLink::setNull()
{
	pyCFunction_ = 0;
	unaryfunc_ = 0;
	binaryfunc_ = 0;
	ternaryfunc_ = 0;
}

void OverloadLink::setPyCFunction(PyCFunction iOverload)
{
	setNull();
	pyCFunction_ = iOverload;
}

void OverloadLink::setUnaryfunc(unaryfunc iOverload)
{
	setNull();
	unaryfunc_ = iOverload;
}

void OverloadLink::setBinaryfunc(binaryfunc iOverload)
{
	setNull();
	binaryfunc_ = iOverload;
}

void OverloadLink::setTernaryfunc(ternaryfunc iOverload)
{
	setNull();
	ternaryfunc_ = iOverload;
}

bool OverloadLink::operator ()(PyObject* iSelf, PyObject* iArgs, PyObject*& oResult) const
{
	PyObject* temp = 0;
	if (pyCFunction_)
	{
		LASS_ASSERT(unaryfunc_ == 0 && binaryfunc_ == 0 && ternaryfunc_ == 0);
		temp = pyCFunction_(iSelf, iArgs);
	}
	else if (unaryfunc_)
	{
		LASS_ASSERT(binaryfunc_ == 0 && ternaryfunc_ == 0);
		if (decodeTuple(iArgs)  != 0)
		{
			return 0;
		}
		temp = unaryfunc_(iSelf);
	}
	else if (binaryfunc_)
	{
		LASS_ASSERT(ternaryfunc_ == 0);
		PyObjectPtr<PyObject>::Type arg;
		if (decodeTuple(iArgs, arg) != 0)
		{
			return 0;
		}
		temp = binaryfunc_(iSelf, arg.get());
	}
	else if (ternaryfunc_)
	{
		temp = ternaryfunc_(iSelf, iArgs, 0);
	}
	else
	{
		return false;
	}
	if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError))
	{
		PyErr_Clear();
		Py_XDECREF(temp);
		return false;
	}
	oResult = temp;
	return true;
}



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



#define LASS_PY_CLASS_SPECIAL_METHOD(s_name, i_protocol, t_protocol, i_hook, i_nary)\
	if (strcmp(iMethodName, s_name) == 0)\
	{\
		if (ioPyType.i_protocol == 0)\
		{\
			ioPyType.i_protocol = new t_protocol;\
			::memset(ioPyType.i_protocol, 0, sizeof(t_protocol));\
		}\
		oOverloadChain.LASS_CONCATENATE_3(set, i_nary, func)(ioPyType.i_protocol->i_hook);\
		ioPyType.i_protocol->i_hook = LASS_CONCATENATE_3(i, i_nary, Dispatcher);\
		return;\
	}\
	/**/


/** @internal
 */
void addClassMethod(
		PyTypeObject& ioPyType, std::vector<PyMethodDef>& ioClassMethods,
		const char* iMethodName, const char* iDocumentation, 
		PyCFunction iMethodDispatcher, unaryfunc iUnaryDispatcher, 
		binaryfunc iBinaryDispatcher, ternaryfunc iTernaryDispatcher, 
		OverloadLink& oOverloadChain) 
{
	const int n = static_cast<int>(strlen(iMethodName));
	LASS_ASSERT(n >= 0);
	if (n > 2 && iMethodName[0] == '_' && iMethodName[1] == '_')
	{
		if (strcmp(iMethodName, "__call__") == 0)
		{
			oOverloadChain.setTernaryfunc(ioPyType.tp_call);
			ioPyType.tp_call = iTernaryDispatcher;
			return;
		}
		LASS_PY_CLASS_SPECIAL_METHOD("__add__", tp_as_number, PyNumberMethods, nb_add, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__sub__", tp_as_number, PyNumberMethods, nb_subtract, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__mul__", tp_as_number, PyNumberMethods, nb_multiply, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__div__", tp_as_number, PyNumberMethods, nb_divide, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__mod__", tp_as_number, PyNumberMethods, nb_remainder, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__neg__", tp_as_number, PyNumberMethods, nb_negative, Unary)
		LASS_PY_CLASS_SPECIAL_METHOD("__pos__", tp_as_number, PyNumberMethods, nb_positive, Unary)
		LASS_PY_CLASS_SPECIAL_METHOD("__abs__", tp_as_number, PyNumberMethods, nb_absolute, Unary)
		LASS_PY_CLASS_SPECIAL_METHOD("__invert__", tp_as_number, PyNumberMethods, nb_invert, Unary)
		LASS_PY_CLASS_SPECIAL_METHOD("__lshift__", tp_as_number, PyNumberMethods, nb_lshift, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__rshift__", tp_as_number, PyNumberMethods, nb_rshift, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__and__", tp_as_number, PyNumberMethods, nb_and, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__xor__", tp_as_number, PyNumberMethods, nb_xor, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__or__", tp_as_number, PyNumberMethods, nb_or, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__int__", tp_as_number, PyNumberMethods, nb_int, Unary)
		LASS_PY_CLASS_SPECIAL_METHOD("__long__", tp_as_number, PyNumberMethods, nb_long, Unary)
		LASS_PY_CLASS_SPECIAL_METHOD("__float__", tp_as_number, PyNumberMethods, nb_float, Unary)
		LASS_PY_CLASS_SPECIAL_METHOD("__oct__", tp_as_number, PyNumberMethods, nb_oct, Unary)
		LASS_PY_CLASS_SPECIAL_METHOD("__hex__", tp_as_number, PyNumberMethods, nb_hex, Unary)
		LASS_PY_CLASS_SPECIAL_METHOD("__iadd__", tp_as_number, PyNumberMethods, nb_inplace_add, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__isub__", tp_as_number, PyNumberMethods, nb_inplace_subtract, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__imul__", tp_as_number, PyNumberMethods, nb_inplace_multiply, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__idiv__", tp_as_number, PyNumberMethods, nb_inplace_divide, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__imod__", tp_as_number, PyNumberMethods, nb_inplace_remainder, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__ilshift__", tp_as_number, PyNumberMethods, nb_inplace_lshift, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__irshift__", tp_as_number, PyNumberMethods, nb_inplace_rshift, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__iand__", tp_as_number, PyNumberMethods, nb_inplace_and, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__ixor__", tp_as_number, PyNumberMethods, nb_inplace_xor, Binary)
		LASS_PY_CLASS_SPECIAL_METHOD("__ior__", tp_as_number, PyNumberMethods, nb_inplace_or, Binary)
	}

	// normal method mechanism

	std::vector<PyMethodDef>::iterator i = std::find_if(
		ioClassMethods.begin(), ioClassMethods.end(), PyMethodEqual(iMethodName));
	if (i == ioClassMethods.end())
	{
		ioClassMethods.insert(ioClassMethods.begin(), createPyMethodDef(
			iMethodName, iMethodDispatcher, METH_VARARGS , iDocumentation));
		oOverloadChain.setNull();
	}
	else
	{
		LASS_ASSERT(i->ml_flags == METH_VARARGS);
		oOverloadChain.setPyCFunction(i->ml_meth);
		i->ml_meth = iMethodDispatcher;
		if (i->ml_doc == 0)
		{
			i->ml_doc = const_cast<char*>(iDocumentation);
		}
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
