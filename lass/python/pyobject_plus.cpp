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



#include "lass_common.h"
#include "pyobject_plus.h"
#include "pyobject_macros.h"
#include "py_stl.h"
#include "py_tuple.h"
#include <iostream>

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4996) // This function or variable may be unsafe ...
#endif


namespace lass
{
namespace python
{
namespace methods
{
	//const lass::python::impl::UnaryDispatcher _int_(std::string("__int__"));
	//const lass::python::impl::BinaryDispatcher _add_("__add__");

	//const lass::python::impl::LenDispatcher _len_("__len__");
}
}
}


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
	return pyBuildSimpleObject(static_cast<PyObjectPlus*>(obj)->doPyRepr());
}

/** @internal
 */
PyObject* str(PyObject* obj)
{
	return pyBuildSimpleObject(static_cast<PyObjectPlus*>(obj)->doPyStr());
}


/** @internal
 */
const std::string exceptionExtractMessage(const TPyObjPtr& type, const TPyObjPtr& value)
{
	std::ostringstream buffer;
	const TPyObjPtr typeStr(PyObject_Str(type.get()));
	std::string temp;
	if (typeStr && pyGetSimpleObject(typeStr.get(), temp) == 0)
	{
		buffer << temp;
	}
	else
	{
		buffer << "unknown python exception";
	}
	const TPyObjPtr valueStr(value.get() == Py_None ? 0 : PyObject_Str(value.get()));
	if (valueStr && pyGetSimpleObject(valueStr.get(), temp) == 0)
	{
		buffer << ": '" << temp << "'";
	}
	return buffer.str();
}


// --- ModuleDefinition ----------------------------------------------------------------------------

ModuleDefinition::ModuleDefinition(const char* name, const char* doc):
	module_(0),
	isInjected_(false)
{
#if PY_MAJOR_VERSION >= 3 
	PyModuleDef def = {
		PyModuleDef_HEAD_INIT,
		0, /* m_name */
		0, /* m_doc */
		0, /* m_size */
		0, /* m_methods */
		0, /* m_reload */
		0, /* m_traverse */
		0, /* m_clear */
		0, /* m_free */
	};
	def_ = def;
#endif
	setName(name);
	setDoc(doc);
}

namespace experimental
{
	typedef util::ScopedPtr< char, util::ArrayStorage > TScopedCString;
	void assignScopedCString(TScopedCString& out, const char* in)
	{
		if (in)
		{
			const size_t n = strlen(in) + 1;
			TScopedCString temp(new char[n]);
			memcpy(temp.get(), in, n);
			out.swap(temp);
		}
		else
		{
			out.reset();
		}
	}
}

void ModuleDefinition::setName(const char* name)
{
	experimental::assignScopedCString(name_, name);
}

void ModuleDefinition::setDoc(const char* doc)
{
	experimental::assignScopedCString(doc_, doc);
}

void ModuleDefinition::addFunctionDispatcher(
		PyCFunction dispatcher, const char* name, const char* doc, PyCFunction& overloadChain)
{
	TMethods::iterator i = ::std::find_if(methods_.begin(), methods_.end(), PyMethodEqual(name));
	if (i == methods_.end())
	{
		methods_.push_back(createPyMethodDef(name, dispatcher, METH_VARARGS , doc));
		overloadChain = 0;
	}
	else
	{
		overloadChain = i->ml_meth;
		i->ml_meth = dispatcher;
	};
}

PyObject* ModuleDefinition::inject()
{
	LASS_ASSERT(!isInjected_);
	LASS_ASSERT(name_.get());
	methods_.push_back(impl::createPyMethodDef(0, 0, 0, 0));
	Py_Initialize();
#if PY_MAJOR_VERSION < 3
	module_ = Py_InitModule3(name_.get(), &methods_[0], doc_.get());
#else
	def_.m_name = name_.get();
	def_.m_doc = doc_.get();
	def_.m_methods = &methods_[0];
	module_ = PyModule_Create(&def_);
#endif
	for (TObjects::const_iterator obj = objects_.begin(); obj != objects_.end(); ++obj)
	{
		PyModule_AddObject(module_, obj->name, obj->object);
	}
	isInjected_ = true;
	return module_;
}

// --- OverloadLink --------------------------------------------------------------------------------

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
void OverloadLink::setSsizeArgfunc(ssizeargfunc iOverload)
{
	setNull();
	ssizeargfunc_ = iOverload;
}
void OverloadLink::setSsizeSsizeArgfunc(ssizessizeargfunc iOverload)
{
	setNull();
	ssizessizeargfunc_ = iOverload;
}
void OverloadLink::setLenfunc(lenfunc iOverload)
{
	setNull();
	lenfunc_ = iOverload;
}
void OverloadLink::setSsizeObjArgProcfunc(ssizeobjargproc iOverload)
{
	setNull();
	ssizeobjargproc_ = iOverload;
}
void OverloadLink::setSsizeSsizeObjArgProcfunc(ssizessizeobjargproc iOverload)
{
	setNull();
	ssizessizeobjargproc_ = iOverload;
}
void OverloadLink::setObjObjProcfunc(objobjproc iOverload)
{
	setNull();
	objobjproc_ = iOverload;
}
void OverloadLink::setObjObjArgProcfunc(objobjargproc iOverload)
{
	setNull();
	objobjargproc_ = iOverload;
}
void OverloadLink::setIterfunc(getiterfunc iOverload)
{
	setNull();
	getiterfunc_ = iOverload;
}
void OverloadLink::setIterNextfunc(iternextfunc iOverload)
{
	setNull();
	iternextfunc_ = iOverload;
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
			return false;
		}
		temp = unaryfunc_(iSelf);
	}
	else if (binaryfunc_)
	{
		LASS_ASSERT(ternaryfunc_ == 0);
		TPyObjPtr arg;
		if (decodeTuple(iArgs, arg) != 0)
		{
			return false;
		}
		temp = binaryfunc_(iSelf, arg.get());
	}
	else if (ternaryfunc_)
	{
		temp = ternaryfunc_(iSelf, iArgs, 0);
	}
	else if (ssizeargfunc_)
	{
		Py_ssize_t size1;
		if (decodeTuple(iArgs, size1) != 0)
			return false;
		temp = ssizeargfunc_(iSelf, size1 );
	}
	else if (ssizessizeargfunc_)
	{
		Py_ssize_t size1;
		Py_ssize_t size2;
		if (decodeTuple(iArgs, size1, size2) != 0)
			return false;
		temp = ssizessizeargfunc_(iSelf, size1, size2 );
	}
	else if (lenfunc_)
	{
		temp = pyBuildSimpleObject(lenfunc_(iSelf));
	}
	else if (ssizeobjargproc_)
	{
		Py_ssize_t size1;
		TPyObjPtr obj1;
		if (decodeTuple(iArgs, size1, obj1) != 0)
			return false;
		temp = pyBuildSimpleObject(ssizeobjargproc_(iSelf,size1,obj1.get()));
	}
	else if (ssizessizeobjargproc_)
	{
		Py_ssize_t size1;
		Py_ssize_t size2;
		TPyObjPtr obj1;
		if (decodeTuple(iArgs, size1, size2, obj1) != 0)
			return false;
		temp = pyBuildSimpleObject(ssizessizeobjargproc_(iSelf,size1,size2,obj1.get()));
	}
	else if (objobjproc_)
	{
		TPyObjPtr obj1;
		if (decodeTuple(iArgs, obj1) != 0)
			return false;
		temp = pyBuildSimpleObject(objobjproc_(iSelf,obj1.get()));		
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
		const char* iName, const char* iDocumentation, const TStaticMemberHelperPtr& iMember, 
		PyTypeObject* iParentType, std::vector<PyMethodDef>* iMethods, 
		std::vector<PyGetSetDef>* iGetSetters, const TStaticMembers* iStatics)
{
	StaticMember temp;
	temp.name = iName;
	temp.doc = iDocumentation;
	temp.member = iMember;
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
		PyObject* member = i->member->build();
		if (PyType_Check(member))
		{
			// we have an innerclass
			finalizePyType(*reinterpret_cast<PyTypeObject*>(member), *i->parentType, 
				*i->methods, *i->getSetters, *i->statics, iPyType.tp_name, i->doc);
		}
		PyDict_SetItemString(iPyType.tp_dict, const_cast<char*>(i->name), member);
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
void addModuleFunction(std::vector<PyMethodDef>& ioModuleMethods, const char* iMethodName, 
	const char* iDocumentation, PyCFunction iMethodDispatcher, PyCFunction& oOverloadChain)
{
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

#define LASS_PY_OPERATOR_(s_name, i_protocol, t_protocol, i_hook, i_nary)\
	if (strcmp(methodName.name.c_str(), s_name) == 0)\
	{\
		if (pyType.i_protocol == 0)\
		{\
			pyType.i_protocol = new t_protocol;\
			::memset(pyType.i_protocol, 0, sizeof(t_protocol));\
		}\
		overloadChain.LASS_CONCATENATE_3(set, i_nary, func)(pyType.i_protocol->i_hook);\
		pyType.i_protocol->i_hook = dispatcher;\
		return;\
	}\
	/**/


#define LASS_PY_COMPARATOR(s_name, v_op)\
	if (strcmp(methodName, s_name) == 0)\
	{\
		compareFuncs.push_back(CompareFunc(dispatcher, v_op));\
	}\
	/**/


#define LASS_PY_COMPARATOR_(s_name, v_op)\
	if (strcmp(methodName.name.c_str(), s_name) == 0)\
	{\
		compareFuncs.push_back(CompareFunc(dispatcher, v_op));\
	}\
		


/** @internal
 */
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const char* methodName, const char* documentation, 
		PyCFunction dispatcher, 
		OverloadLink& overloadChain) 
{
	const int n = static_cast<int>(strlen(methodName));
	if (n > 2 && methodName[0] == '_' && methodName[1] == '_')
	{
		// throw a warning, or halt execution, but we are before the main, so not
		// everything is initialized yet
		//LASS_THROW("Special methods should use newer export mechanism!");
		printf("Special method shoud use newer export mechanism: %s\n", methodName);
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
	/**/
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::ComparatorSlot& methodName, const char* documentation, 
		PyCFunction dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_COMPARATOR_("__lt__", Py_LT)
	LASS_PY_COMPARATOR_("__le__", Py_LE)
	LASS_PY_COMPARATOR_("__eq__", Py_EQ)
	LASS_PY_COMPARATOR_("__ne__", Py_NE)
	LASS_PY_COMPARATOR_("__gt__", Py_GT)
	LASS_PY_COMPARATOR_("__ge__", Py_GE)
	LASS_ASSERT_UNREACHABLE;
}

	/**/
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::LenSlot& methodName, const char* documentation, 
		lenfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__len__", tp_as_sequence, PySequenceMethods, sq_length, Len) 
	LASS_PY_OPERATOR_("__seq_len__", tp_as_sequence, PySequenceMethods, sq_length, Len) 
	LASS_PY_OPERATOR_("__map_len__", tp_as_mapping, PyMappingMethods, mp_length, Len) 
	LASS_ASSERT_UNREACHABLE;
}
	/**/

void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::UnarySlot& methodName, const char* documentation, 
		unaryfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	if (methodName.name=="__str__")
	{
		pyType.tp_str = dispatcher;
		return;
	}
	if (methodName.name=="__repr__")
	{
		pyType.tp_repr = dispatcher;
		return;
	}
	LASS_PY_OPERATOR_("__neg__", tp_as_number, PyNumberMethods, nb_negative, Unary)
	LASS_PY_OPERATOR_("__pos__", tp_as_number, PyNumberMethods, nb_positive, Unary)
	LASS_PY_OPERATOR_("__abs__", tp_as_number, PyNumberMethods, nb_absolute, Unary)
	LASS_PY_OPERATOR_("__invert__", tp_as_number, PyNumberMethods, nb_invert, Unary)
	LASS_PY_OPERATOR_("__int__", tp_as_number, PyNumberMethods, nb_int, Unary)
	LASS_PY_OPERATOR_("__long__", tp_as_number, PyNumberMethods, nb_long, Unary)
	LASS_PY_OPERATOR_("__float__", tp_as_number, PyNumberMethods, nb_float, Unary)
#if PY_MAJOR_VERSION < 3
	LASS_PY_OPERATOR_("__oct__", tp_as_number, PyNumberMethods, nb_oct, Unary)
	LASS_PY_OPERATOR_("__hex__", tp_as_number, PyNumberMethods, nb_hex, Unary)
#endif
	LASS_ASSERT_UNREACHABLE;
}
	/**/

void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::BinarySlot& methodName, const char* documentation, 
		binaryfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__add__", tp_as_number, PyNumberMethods, nb_add, Binary)
	LASS_PY_OPERATOR_("__sub__", tp_as_number, PyNumberMethods, nb_subtract,Binary)
	LASS_PY_OPERATOR_("__mul__", tp_as_number, PyNumberMethods, nb_multiply, Binary)
	LASS_PY_OPERATOR_("__mod__", tp_as_number, PyNumberMethods, nb_remainder, Binary)
	LASS_PY_OPERATOR_("__lshift__", tp_as_number, PyNumberMethods, nb_lshift, Binary)
	LASS_PY_OPERATOR_("__rshift__", tp_as_number, PyNumberMethods, nb_rshift, Binary)
	LASS_PY_OPERATOR_("__and__", tp_as_number, PyNumberMethods, nb_and, Binary)
	LASS_PY_OPERATOR_("__xor__", tp_as_number, PyNumberMethods, nb_xor, Binary)
	LASS_PY_OPERATOR_("__or__", tp_as_number, PyNumberMethods, nb_or, Binary)
	LASS_PY_OPERATOR_("__iadd__", tp_as_number, PyNumberMethods, nb_inplace_add, Binary)
	LASS_PY_OPERATOR_("__isub__", tp_as_number, PyNumberMethods, nb_inplace_subtract, Binary)
	LASS_PY_OPERATOR_("__imul__", tp_as_number, PyNumberMethods, nb_inplace_multiply, Binary)
	LASS_PY_OPERATOR_("__imod__", tp_as_number, PyNumberMethods, nb_inplace_remainder, Binary)
	LASS_PY_OPERATOR_("__ilshift__", tp_as_number, PyNumberMethods, nb_inplace_lshift, Binary)
	LASS_PY_OPERATOR_("__irshift__", tp_as_number, PyNumberMethods, nb_inplace_rshift, Binary)
	LASS_PY_OPERATOR_("__iand__", tp_as_number, PyNumberMethods, nb_inplace_and, Binary)
	LASS_PY_OPERATOR_("__ixor__", tp_as_number, PyNumberMethods, nb_inplace_xor, Binary)
	LASS_PY_OPERATOR_("__ior__", tp_as_number, PyNumberMethods, nb_inplace_or, Binary)
#if PY_VERSION_HEX >= 0x02060000
	LASS_PY_OPERATOR_("__truediv__", tp_as_number, PyNumberMethods, nb_true_divide, Binary)
	LASS_PY_OPERATOR_("__itruediv__", tp_as_number, PyNumberMethods, nb_inplace_true_divide, Binary)
#endif
#if PY_MAJOR_VERSION < 3
	LASS_PY_OPERATOR_("__div__", tp_as_number, PyNumberMethods, nb_divide, Binary)
	LASS_PY_OPERATOR_("__idiv__", tp_as_number, PyNumberMethods, nb_inplace_divide, Binary)
#endif

	LASS_PY_OPERATOR_("__iconcat__", tp_as_sequence, PySequenceMethods, sq_inplace_concat, Binary) 
	LASS_PY_OPERATOR_("__concat__", tp_as_sequence, PySequenceMethods, sq_concat, Binary) 
	LASS_PY_OPERATOR_("__map_getitem__", tp_as_mapping, PyMappingMethods, mp_subscript, Binary) 
	LASS_ASSERT_UNREACHABLE;
}

	/**/

void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::TernarySlot& methodName, const char* documentation, 
		ternaryfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	if (methodName.name=="__call__")
	{
		overloadChain.setTernaryfunc(pyType.tp_call);
		pyType.tp_call = dispatcher;
		return;
	}
	LASS_ASSERT_UNREACHABLE;
}

/**/
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::SsizeArgSlot& methodName, const char* documentation, 
		ssizeargfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__seq_getitem__", tp_as_sequence, PySequenceMethods, sq_item, SsizeArg) 
	LASS_PY_OPERATOR_("__repeat__", tp_as_sequence, PySequenceMethods, sq_repeat, SsizeArg) 
	LASS_PY_OPERATOR_("__irepeat__", tp_as_sequence, PySequenceMethods, sq_inplace_repeat, SsizeArg) 
	LASS_ASSERT_UNREACHABLE;
}
/**/
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::SsizeSsizeArgSlot& methodName, const char* documentation, 
		ssizessizeargfunc dispatcher, 
		OverloadLink& overloadChain) 
{
#if PY_MAJOR_VERSION < 3
	LASS_PY_OPERATOR_("__getslice__", tp_as_sequence, PySequenceMethods, sq_slice, SsizeSsizeArg) 
#endif
	LASS_ASSERT_UNREACHABLE;
}
/**/
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::SsizeObjArgSlot& methodName, const char* documentation, 
		ssizeobjargproc dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__seq_setitem__", tp_as_sequence, PySequenceMethods, sq_ass_item, SsizeObjArgProc) 
	LASS_ASSERT_UNREACHABLE;
}
/**/
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::SsizeSsizeObjArgSlot& methodName, const char* documentation, 
		ssizessizeobjargproc dispatcher, 
		OverloadLink& overloadChain) 
{
#if PY_MAJOR_VERSION < 3
	LASS_PY_OPERATOR_("__setslice__", tp_as_sequence, PySequenceMethods, sq_ass_slice, SsizeSsizeObjArgProc) 
#endif
	LASS_ASSERT_UNREACHABLE;
}

/**/
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::ObjObjSlot& methodName, const char* documentation, 
		objobjproc dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__contains__", tp_as_sequence, PySequenceMethods, sq_contains, ObjObjProc) 
	LASS_ASSERT_UNREACHABLE;
}
/**/
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::ObjObjArgSlot& methodName, const char* documentation, 
		objobjargproc dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__map_setitem__", tp_as_mapping, PyMappingMethods, mp_ass_subscript, ObjObjArgProc) 
	LASS_ASSERT_UNREACHABLE;
}

void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::IterSlot& methodName, const char* documentation, 
		getiterfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	if (methodName.name=="__iter__")
	{
		overloadChain.setIterfunc(pyType.tp_iter);
		pyType.tp_iter = dispatcher;
		return;
	}
	LASS_ASSERT_UNREACHABLE;
}
void addClassMethod(
		PyTypeObject& pyType, 
		std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
		const lass::python::impl::IterNextSlot& methodName, const char* documentation, 
		iternextfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	if (methodName.name=="next")
	{
		overloadChain.setIterNextfunc(pyType.tp_iternext);
		pyType.tp_iternext = dispatcher;
		return;
	}
	LASS_ASSERT_UNREACHABLE;
}



/** @internal
*/
void addMessageHeader(const std::string& header)
{
	if (!PyErr_Occurred() || !PyErr_ExceptionMatches(PyExc_TypeError))
	{
		return;
	}
	PyObject *type, *value, *traceback;
	PyErr_Fetch(&type, &value, &traceback);
	try
	{
		if (PyUnicode_Check(value))
		{
			std::string left = header + ": ";
			TPyObjPtr pyLeft(PyUnicode_DecodeUTF8(left.data(), left.length(), 0));
			PyObject* newValue = PyUnicode_Concat(pyLeft.get(), value);
			std::swap(value, newValue);
			Py_DECREF(newValue);
		}
#if PY_MAJOR_VERSION < 3
		else if (PyString_Check(value))
		{			
			std::ostringstream buffer;
			buffer << header << ": " << PyString_AsString(value);
			PyObject* temp = pyBuildSimpleObject(buffer.str());
			std::swap(value, temp);
			Py_DECREF(temp);
		}
#endif
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

/** @internal
 *  Check that @a iValue is a sequence and of expected size, and return it as a "FAST sequence"
 *  so that you can use PySequence_Fast_GET_ITEM to get its items with borrowed references.
 */
TPyObjPtr checkedFastSequence(PyObject* iValue, Py_ssize_t iExpectedSize)
{
	TPyObjPtr result(PySequence_Fast(iValue, "expected a sequence (tuple, list, ...)"));
	const Py_ssize_t size = PySequence_Fast_GET_SIZE(result.get());
	if (size != iExpectedSize)
	{
		std::ostringstream buffer;
		buffer << "expected a sequence of size " << iExpectedSize
			<< " (your size is " << size << ")";
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
		return TPyObjPtr();
	}
	return result;
}

}
}
}

// EOF
