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



#include "util_common.h"
#include "pyobject_plus.h"
#include "pyobject_macros.h"
#include "py_stl.h"
#include "py_tuple.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4996) // This function or variable may be unsafe ...
#endif

namespace lass
{
namespace python
{

PyTypeObject PyObjectPlus::_lassPyType = { PY_STATIC_FUNCTION_FORWARD( PyObjectPlus, "PyObjectPlus" ) };

std::vector<PyMethodDef> initAbstractMethods()
{
	std::vector<PyMethodDef> temp;
	temp.push_back( impl::createPyMethodDef( 0, 0, 0, 0 ) );
	return temp;

}
std::vector<PyMethodDef> PyObjectPlus::_lassPyMethods = initAbstractMethods();

PyObjectPlus::PyObjectPlus()
{
	// initializing the type to NULL, when the object is exported to python the type is fixed
	this->ob_type = NULL;
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
	dict_ = PyDict_New();
#endif
	_Py_NewReference( this );
};

PyObjectPlus::~PyObjectPlus()
{
	if (this->ob_refcnt>0)
	{
		if (this->ob_refcnt>1)
		{
			std::cerr << "[LASS RUN MSG] DANGLING REFERENCE to lass::python::PyObjectPlus" 
				<< std::endl;
		}
		--this->ob_refcnt;
		_Py_ForgetReference( this );

	}
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
	//if (dict_)
	//	Py_XDECREF(dict_);
#endif
	LASS_ASSERT(this->ob_refcnt==0);
};

PyObjectPlus::PyObjectPlus(const PyObjectPlus& iOther)
{
	this->ob_type = iOther.ob_type;
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
	// [TDM] or just copy the reference? 
	dict_ = PyDict_Copy(iOther.dict_);
#endif
	_Py_NewReference( this );
}

PyObjectPlus& PyObjectPlus::operator =(const PyObjectPlus& iOther)
{
	LASS_ASSERT(!this->ob_type || this->ob_type == iOther.ob_type);
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
	// [TDM] or just copy the reference? 
	dict_ = PyDict_Copy(iOther.dict_);
#endif
	return *this;
}

/** @ingroup Python
 *  @brief retrieve pointer to PyObject by its name in the script.
 *  @return 
 *	new reference to PyObject @a iName or NULL if 
 *	@a iName does not exist (_without_ setting an exception!)
 *  @throw Never
 */
TPyObjPtr getPyObjectByName(const std::string& iName)
{
	PyObject* module = PyImport_AddModule("__main__");
	if (!module)
	{
		PyErr_Clear();
		return TPyObjPtr();
	}
	PyObject* dict = PyModule_GetDict(module);
	LASS_ASSERT(dict != 0);
	PyObject* object = PyDict_GetItemString(dict, const_cast<char*>(iName.c_str()));
	Py_XINCREF(object);
	return TPyObjPtr(object);
}

// --- impl ----------------------------------------------------------------------------------------

namespace impl
{

/** @internal
 */
void dealloc(PyObject* obj)
{
	delete static_cast<PyObjectPlus*>(obj);
};

/** @internal
 */
PyObject* repr(PyObject* obj)
{
	return pyExportTraitBuild(static_cast<PyObjectPlus*>(obj)->doPyRepr());
}

/** @internal
 */
PyObject* str(PyObject* obj)
{
	return pyExportTraitBuild(static_cast<PyObjectPlus*>(obj)->doPyStr());
}


/** @internal
 */
const std::string exceptionExtractMessage(const TPyObjPtr& type, const TPyObjPtr& value)
{
	std::ostringstream buffer;
	const TPyObjPtr typeStr(PyObject_Str(type.get()));
	buffer << (typeStr ? PyString_AsString(typeStr.get()) : "unknown python exception");
	const TPyObjPtr valueStr(value.get() == Py_None ? 0 : PyObject_Str(value.get()));
	if (valueStr)
	{
		buffer << ": '" << PyString_AsString(valueStr.get()) << "'";
	}
	return buffer.str();
}



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
		TPyObjPtr arg;
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
StaticMember createStaticMember(
		const char* iName, const char* iDocumentation, PyObject* iObject, 
		PyTypeObject* iParentType, std::vector<PyMethodDef>* iMethods, 
		std::vector<PyGetSetDef>* iGetSetters, const TStaticMembers* iStatics)
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
PyMethodDef createPyMethodDef(const char *ml_name, PyCFunction ml_meth, int ml_flags, const char *ml_doc)
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
void injectStaticMembers(PyTypeObject& iPyType, const TStaticMembers& iStatics)
{
	for (TStaticMembers::const_iterator i = iStatics.begin(); i != iStatics.end(); ++i)
	{
		if (PyType_Check(i->object))
		{
			// we have an innerclass
			finalizePyType(*reinterpret_cast<PyTypeObject*>(i->object), *i->parentType, 
				*i->methods, *i->getSetters, *i->statics, iPyType.tp_name, i->doc);
		}
		PyDict_SetItemString(iPyType.tp_dict, const_cast<char*>(i->name), i->object);
	}
}

/** @internal
*	The iFinal sets the flags for final classes from which no new types can be derived.  
*/
void finalizePyType(PyTypeObject& iPyType, PyTypeObject& iPyParentType, 
	std::vector<PyMethodDef>& iMethods, std::vector<PyGetSetDef>& iGetSetters, 
	const TStaticMembers& iStatics, const char* iModuleName, const char* iDocumentation)
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
	// we take care of collecting garbage ourselves
	iPyType.tp_flags &= (~Py_TPFLAGS_HAVE_GC);
	Py_XINCREF( iPyType.tp_base );
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



#define LASS_PY_OPERATOR(s_name, i_protocol, t_protocol, i_hook, i_nary)\
	if (strcmp(methodName, s_name) == 0)\
	{\
		if (pyType.i_protocol == 0)\
		{\
			pyType.i_protocol = new t_protocol;\
			::memset(pyType.i_protocol, 0, sizeof(t_protocol));\
		}\
		overloadChain.LASS_CONCATENATE_3(set, i_nary, func)(pyType.i_protocol->i_hook);\
		pyType.i_protocol->i_hook = LASS_CONCATENATE(dispatcher, i_nary);\
		return;\
	}\
	/**/



#define LASS_PY_COMPARATOR(s_name, v_op)\
	if (strcmp(methodName, s_name) == 0)\
	{\
		compareFuncs.push_back(CompareFunc(dispatcher, v_op));\
	}\
		

/** @internal
 */
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const char* methodName, const char* documentation, 
		PyCFunction dispatcher, unaryfunc dispatcherUnary, 
		binaryfunc dispatcherBinary, ternaryfunc dispatcherTernary, 
		OverloadLink& overloadChain) 
{
	const int n = static_cast<int>(strlen(methodName));
	LASS_ASSERT(n >= 0);
	if (n > 2 && methodName[0] == '_' && methodName[1] == '_')
	{
		if (strcmp(methodName, "__call__") == 0)
		{
			overloadChain.setTernaryfunc(pyType.tp_call);
			pyType.tp_call = dispatcherTernary;
			return;
		}
		LASS_PY_OPERATOR("__add__", tp_as_number, PyNumberMethods, nb_add, Binary)
		LASS_PY_OPERATOR("__sub__", tp_as_number, PyNumberMethods, nb_subtract,Binary)
		LASS_PY_OPERATOR("__mul__", tp_as_number, PyNumberMethods, nb_multiply, Binary)
		LASS_PY_OPERATOR("__div__", tp_as_number, PyNumberMethods, nb_divide, Binary)
		LASS_PY_OPERATOR("__mod__", tp_as_number, PyNumberMethods, nb_remainder, Binary)
		LASS_PY_OPERATOR("__neg__", tp_as_number, PyNumberMethods, nb_negative, Unary)
		LASS_PY_OPERATOR("__pos__", tp_as_number, PyNumberMethods, nb_positive, Unary)
		LASS_PY_OPERATOR("__abs__", tp_as_number, PyNumberMethods, nb_absolute, Unary)
		LASS_PY_OPERATOR("__invert__", tp_as_number, PyNumberMethods, nb_invert, Unary)
		LASS_PY_OPERATOR("__lshift__", tp_as_number, PyNumberMethods, nb_lshift, Binary)
		LASS_PY_OPERATOR("__rshift__", tp_as_number, PyNumberMethods, nb_rshift, Binary)
		LASS_PY_OPERATOR("__and__", tp_as_number, PyNumberMethods, nb_and, Binary)
		LASS_PY_OPERATOR("__xor__", tp_as_number, PyNumberMethods, nb_xor, Binary)
		LASS_PY_OPERATOR("__or__", tp_as_number, PyNumberMethods, nb_or, Binary)
		LASS_PY_OPERATOR("__int__", tp_as_number, PyNumberMethods, nb_int, Unary)
		LASS_PY_OPERATOR("__long__", tp_as_number, PyNumberMethods, nb_long, Unary)
		LASS_PY_OPERATOR("__float__", tp_as_number, PyNumberMethods, nb_float, Unary)
		LASS_PY_OPERATOR("__oct__", tp_as_number, PyNumberMethods, nb_oct, Unary)
		LASS_PY_OPERATOR("__hex__", tp_as_number, PyNumberMethods, nb_hex, Unary)
		LASS_PY_OPERATOR("__iadd__", tp_as_number, PyNumberMethods, nb_inplace_add, Binary)
		LASS_PY_OPERATOR("__isub__", tp_as_number, PyNumberMethods, nb_inplace_subtract, Binary)
		LASS_PY_OPERATOR("__imul__", tp_as_number, PyNumberMethods, nb_inplace_multiply, Binary)
		LASS_PY_OPERATOR("__idiv__", tp_as_number, PyNumberMethods, nb_inplace_divide, Binary)
		LASS_PY_OPERATOR("__imod__", tp_as_number, PyNumberMethods, nb_inplace_remainder, Binary)
		LASS_PY_OPERATOR("__ilshift__", tp_as_number, PyNumberMethods, nb_inplace_lshift, Binary)
		LASS_PY_OPERATOR("__irshift__", tp_as_number, PyNumberMethods, nb_inplace_rshift, Binary)
		LASS_PY_OPERATOR("__iand__", tp_as_number, PyNumberMethods, nb_inplace_and, Binary)
		LASS_PY_OPERATOR("__ixor__", tp_as_number, PyNumberMethods, nb_inplace_xor, Binary)
		LASS_PY_OPERATOR("__ior__", tp_as_number, PyNumberMethods, nb_inplace_or, Binary)
		LASS_PY_COMPARATOR("__lt__", Py_LT)
		LASS_PY_COMPARATOR("__le__", Py_LE)
		LASS_PY_COMPARATOR("__eq__", Py_EQ)
		LASS_PY_COMPARATOR("__ne__", Py_NE)
		LASS_PY_COMPARATOR("__gt__", Py_GT)
		LASS_PY_COMPARATOR("__ge__", Py_GE)

		if (strcmp(methodName, "__str__") == 0)
		{
			pyType.tp_str = dispatcherUnary;
			return;
		}
		if (strcmp(methodName, "__repr__") == 0)
		{
			pyType.tp_repr = dispatcherUnary;
			return;
		}
	}

	// normal method mechanism

	std::vector<PyMethodDef>::iterator i = std::find_if(
		classMethods.begin(), classMethods.end(), PyMethodEqual(methodName));
	if (i == classMethods.end())
	{
		classMethods.insert(classMethods.begin(), createPyMethodDef(
			methodName, dispatcher, METH_VARARGS , documentation));
		overloadChain.setNull();
	}
	else
	{
		LASS_ASSERT(i->ml_flags == METH_VARARGS);
		overloadChain.setPyCFunction(i->ml_meth);
		i->ml_meth = dispatcher;
		if (i->ml_doc == 0)
		{
			i->ml_doc = const_cast<char*>(documentation);
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
bool checkSequenceSize(PyObject* iValue, Py_ssize_t iExpectedSize)
{
	if (!PySequence_Check(iValue))
	{
		PyErr_SetString(PyExc_TypeError, "not a python sequence (tuple, list, ...)");
		return false;
	}
	const Py_ssize_t size = PySequence_Size(iValue);
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

// EOF
