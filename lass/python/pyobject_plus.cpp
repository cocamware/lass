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
namespace impl
{

#if PY_MAJOR_VERSION < 3
#	define PyVarObject_HEAD_INIT(type, size) PyObject_HEAD_INIT(type) size,
#else
#	define Py_TPFLAGS_CHECKTYPES 0 // flag no longer exists
#endif

class NamePredicate
{
public:
	NamePredicate(const std::string& name): name_(name) {}
	template <typename T> bool operator()(T& x) { return name_ != x.name(); }
	template <typename T> bool operator()(T* x) { return name_ != x->name(); }
private:
	std::string name_;
};

ClassDefinition::ClassDefinition(
		const char* name, const char* doc, Py_ssize_t typeSize, 
		richcmpfunc richcmp, ClassDefinition* parent, TClassRegisterHook registerHook):
	parent_(parent),
	classRegisterHook_(registerHook),
	isFrozen_(false)
{
	PyTypeObject type = { 
		PyVarObject_HEAD_INIT(&PyType_Type, 0)
		(char*)name,	/*tp_name*/
		typeSize,	/*tp_basicsize*/
		0,	/*tp_itemsize*/
		dealloc,	/*tp_dealloc*/
		0,	/*tp_print*/
		0,	/*tp_getattr*/
		0,	/*tp_setattr*/
		0,	/*tp_compare*/
		repr,	/*tp_repr*/
		0,	/*tp_as_number*/
		0,	/*tp_as_sequence*/
		0,	/*tp_as_mapping*/
		0,	/*tp_hash*/
		0,	/*tp_call */
		str,	/*tp_str */
		0,/*PyObject_GenericGetAttr ,*/	/*tp_getattro */
		0,/*PyObject_GenericSetAttr,*/	/*tp_setattro */
		0,	/*tp_as_buffer*/
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES  , /*| Py_TPFLAGS_HAVE_CLASS ,	/*tp_flags*/
		(char*)doc,	/*tp_doc*/
		0,	/*tp_traverse*/
		0,	/*tp_clear*/
		richcmp,	/*tp_richcompare*/
		0,	/*tp_weaklistoffset*/
		0,	/*tp_iter*/
		0,	/*tp_iternext*/
		0,	/*tp_methods*/
		0,	/*tp_members*/
		0,	/*tp_getset*/
		0,	/*tp_base*/
		0,	/*tp_dict*/
		0,	/*tp_descr_get*/
		0,	/*tp_descr_set*/
		0,	/*tp_dictoffset*/
		0,	/*tp_init*/
		0,	/*tp_alloc*/
		0,	/*tp_new*/
		0,	/*tp_free*/
		0,	/*tp_is_gc*/
		0,	/*tp_bases*/
		0,	/*tp_mro*/
		0,	/*tp_cache*/
		0,	/*tp_subclasses*/
		0,	/*tp_weaklist*/
		0,	/*tp_del*/
	};
	type_ = type;
	methods_.push_back(impl::createPyMethodDef( 0, 0, 0, 0 ));
	getSetters_.push_back(impl::createPyGetSetDef( 0, 0, 0, 0, 0 ));
}

void ClassDefinition::addInnerClass(ClassDefinition& innerClass)
{
	LASS_ASSERT(std::count_if(innerClasses_.begin(), innerClasses_.end(), NamePredicate(innerClass.name())) == 0);
	innerClasses_.push_back(&innerClass);
}

/** @internal
*	The iFinal sets the flags for final classes from which no new types can be derived.  
*/
void ClassDefinition::freezeDefinition(const char* scopeName)
{
	LASS_ASSERT(!isFrozen_);
	isFrozen_ = true;

	if (parent_)
	{
		// In the general case, we can't freeze the parent's definition, as we can't be sure of its scopeName.
		// However, in case of PyObjectPlus, nobody else will do it.  Luckily, we know its scopeName's empty.
		if (parent_ == &PyObjectPlus::_lassPyClassDef && !PyObjectPlus::_lassPyClassDef.isFrozen_)
		{
			PyObjectPlus::_lassPyClassDef.freezeDefinition();
		}
		LASS_ASSERT(parent_->isFrozen_);
		type_.tp_base = parent_->type();

		parent_->subClasses_.push_back(this);
	}
	else
	{
		type_.tp_base = &PyBaseObject_Type;
	}
	Py_INCREF( type_.tp_base );

	std::string fullName = type_.tp_name;
	if (scopeName)
	{
		fullName = std::string(scopeName) + "." + fullName;
	}
	char* fullNameCharPtr = new char[fullName.size()+1];
	strcpy(fullNameCharPtr,fullName.c_str());
	type_.tp_name = fullNameCharPtr;

	type_.tp_methods = &methods_[0];
	type_.tp_getset = &getSetters_[0];
	type_.tp_flags &= (~Py_TPFLAGS_HAVE_GC); // we take care of collecting garbage ourselves

	LASS_ENFORCE( PyType_Ready( &type_ ) >= 0 );
	Py_INCREF( &type_ ); 
	
	for (TStaticMembers::const_iterator i = statics_.begin(); i != statics_.end(); ++i)
	{
		PyDict_SetItemString(type_.tp_dict, const_cast<char*>(i->name), i->member->build());
	}
	for (TClassDefList::const_iterator i = innerClasses_.begin(); i != innerClasses_.end(); ++i)
	{
		ClassDefinition* innerClass = *i;
		const char* shortName = innerClass->name();
		innerClass->freezeDefinition(type_.tp_name);
		PyDict_SetItemString(type_.tp_dict, const_cast<char*>(shortName), reinterpret_cast<PyObject*>(innerClass->type()));
	}

	if (classRegisterHook_)
	{
		classRegisterHook_();
	}
}

}

impl::ClassDefinition PyObjectPlus::_lassPyClassDef(
	"PyObjectPlus", 0, sizeof(PyObjectPlus), 0, 0, &PyObjectPlus::_lassPyClassRegisterHook);

/** This function can be used to execute some code at injection time.
 *  By default it doens't do anything, but you can override it for
 *  any type.  Be aware that the top level implementation is the
 *  only one that counts.  Consider it as a 'virtual' function at
 *  static level
 */
void PyObjectPlus::_lassPyClassRegisterHook()
{
}

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



// --- OverloadLink --------------------------------------------------------------------------------

OverloadLink::OverloadLink()
{
	setNull();
}

void OverloadLink::setNull()
{
	signature_ = sNull;
}

void OverloadLink::setPyCFunction(PyCFunction iOverload)
{
	signature_ = iOverload ? sPyCFunction : sNull;
	pyCFunction_ = iOverload;
}

void OverloadLink::setBinaryfunc(binaryfunc iOverload)
{
	signature_ = iOverload ? sBinary : sNull;
	binaryfunc_ = iOverload;
}

void OverloadLink::setTernaryfunc(ternaryfunc iOverload)
{
	signature_ = iOverload ? sTernary : sNull;
	ternaryfunc_ = iOverload;
}
void OverloadLink::setSsizeArgfunc(ssizeargfunc iOverload)
{
	signature_ = iOverload ? sSsizeArg : sNull;
	ssizeargfunc_ = iOverload;
}
void OverloadLink::setSsizeSsizeArgfunc(ssizessizeargfunc iOverload)
{
	signature_ = iOverload ? sSsizeSsizeArg : sNull;
	ssizessizeargfunc_ = iOverload;
}

void OverloadLink::setSsizeObjArgProcfunc(ssizeobjargproc iOverload)
{
	signature_ = iOverload ? sSsizeObjArg : sNull;
	ssizeobjargproc_ = iOverload;
}
void OverloadLink::setSsizeSsizeObjArgProcfunc(ssizessizeobjargproc iOverload)
{
	signature_ = iOverload ? sSsizeSsizeObjArg : sNull;
	ssizessizeobjargproc_ = iOverload;
}
void OverloadLink::setObjObjProcfunc(objobjproc iOverload)
{
	signature_ = iOverload ? sObjObjArg : sNull;
	objobjproc_ = iOverload;
}
void OverloadLink::setObjObjArgProcfunc(objobjargproc iOverload)
{
	signature_ = iOverload ? sObjObjArg : sNull;
	objobjargproc_ = iOverload;
}

bool OverloadLink::operator ()(PyObject* iSelf, PyObject* iArgs, PyObject*& oResult) const
{
	if (signature_ == sNull)
	{
		return false;
	}
	PyObject* temp = call(iSelf, iArgs);
	if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError))
	{
		PyErr_Clear();
		Py_XDECREF(temp);
		return false;
	}
	oResult = temp;
	return true;
}

PyObject* OverloadLink::call(PyObject* iSelf, PyObject* iArgs) const
{
	switch (signature_)
	{
	case sNull:
		return 0;

	case sPyCFunction:
		return pyCFunction_(iSelf, iArgs);

	case sBinary:
		{
			TPyObjPtr arg;
			if (decodeTuple(iArgs, arg) != 0)
			{
				return 0;
			}
			return binaryfunc_(iSelf, arg.get());
		}

	case sTernary:
		return ternaryfunc_(iSelf, iArgs, 0);

	case sSsizeArg:
		{
			Py_ssize_t size1;
			if (decodeTuple(iArgs, size1) != 0)
			{
				return 0;
			}
			return ssizeargfunc_(iSelf, size1 );
		}

	case sSsizeSsizeArg:
		{
			Py_ssize_t size1;
			Py_ssize_t size2;
			if (decodeTuple(iArgs, size1, size2) != 0)
			{
				return 0;
			}
			return ssizessizeargfunc_(iSelf, size1, size2 );
		}
	
	case sSsizeObjArg:
		{
			Py_ssize_t size1;
			TPyObjPtr obj1;
			if (decodeTuple(iArgs, size1, obj1) != 0)
			{
				return 0;
			}
			return pyBuildSimpleObject(ssizeobjargproc_(iSelf,size1,obj1.get()));
		}

	case sSsizeSsizeObjArg:
		{
			Py_ssize_t size1;
			Py_ssize_t size2;
			TPyObjPtr obj1;
			if (decodeTuple(iArgs, size1, size2, obj1) != 0)
			{
				return 0;
			}
			return pyBuildSimpleObject(ssizessizeobjargproc_(iSelf,size1,size2,obj1.get()));
		}

	case sObjObj:
		{
			TPyObjPtr obj1;
			if (decodeTuple(iArgs, obj1) != 0)
			{
				return 0;
			}
			return pyBuildSimpleObject(objobjproc_(iSelf,obj1.get()));		
		}

	default:
		PyErr_SetString(PyExc_AssertionError, "OverloadChain: invalid signature type");
		return 0;
	};
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
		const char* iName, const TStaticMemberHelperPtr& iMember)
{
	StaticMember temp;
	temp.name = iName;
	temp.member = iMember;
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







#define LASS_PY_OPERATOR_(s_name, i_protocol, t_protocol, i_hook, i_nary)\
	if (strcmp(methodName.name.c_str(), s_name) == 0)\
	{\
		if (classDef.type_.i_protocol == 0)\
		{\
			classDef.type_.i_protocol = new t_protocol;\
			::memset(classDef.type_.i_protocol, 0, sizeof(t_protocol));\
		}\
		overloadChain.LASS_CONCATENATE_3(set, i_nary, func)(classDef.type_.i_protocol->i_hook);\
		classDef.type_.i_protocol->i_hook = dispatcher;\
		return;\
	}\
	/**/



#define LASS_PY_OPERATOR_NO_OVERLOAD(s_name, i_protocol, t_protocol, i_hook)\
	if (strcmp(methodName.name.c_str(), s_name) == 0)\
	{\
		if (classDef.type_.i_protocol == 0)\
		{\
			classDef.type_.i_protocol = new t_protocol;\
			::memset(classDef.type_.i_protocol, 0, sizeof(t_protocol));\
		}\
		classDef.type_.i_protocol->i_hook = dispatcher;\
		return;\
	}\
	/**/



#define LASS_PY_COMPARATOR_(s_name, v_op)\
	if (strcmp(methodName.name.c_str(), s_name) == 0)\
	{\
		classDef.compareFuncs_.push_back(CompareFunc(dispatcher, v_op));\
		return; \
	}\
	/**/
		


/** @internal
 */
void addClassMethod(
		ClassDefinition& classDef,
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
		classDef.methods_.begin(), classDef.methods_.end(), PyMethodEqual(methodName));
	if (i == classDef.methods_.end())
	{
		classDef.methods_.insert(classDef.methods_.begin(), createPyMethodDef(
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
		ClassDefinition& classDef,
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
		ClassDefinition& classDef,
		const lass::python::impl::LenSlot& methodName, const char* documentation, 
		lenfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_NO_OVERLOAD("__len__", tp_as_sequence, PySequenceMethods, sq_length) 
	LASS_PY_OPERATOR_NO_OVERLOAD("__seq_len__", tp_as_sequence, PySequenceMethods, sq_length) 
	LASS_PY_OPERATOR_NO_OVERLOAD("__map_len__", tp_as_mapping, PyMappingMethods, mp_length) 
	LASS_ASSERT_UNREACHABLE;
}
	/**/

void addClassMethod(
		ClassDefinition& classDef,
		const lass::python::impl::UnarySlot& methodName, const char* documentation, 
		unaryfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	if (methodName.name=="__str__")
	{
		classDef.type_.tp_str = dispatcher;
		return;
	}
	if (methodName.name=="__repr__")
	{
		classDef.type_.tp_repr = dispatcher;
		return;
	}
	LASS_PY_OPERATOR_NO_OVERLOAD("__neg__", tp_as_number, PyNumberMethods, nb_negative)
	LASS_PY_OPERATOR_NO_OVERLOAD("__pos__", tp_as_number, PyNumberMethods, nb_positive)
	LASS_PY_OPERATOR_NO_OVERLOAD("__abs__", tp_as_number, PyNumberMethods, nb_absolute)
	LASS_PY_OPERATOR_NO_OVERLOAD("__invert__", tp_as_number, PyNumberMethods, nb_invert)
	LASS_PY_OPERATOR_NO_OVERLOAD("__int__", tp_as_number, PyNumberMethods, nb_int)
	LASS_PY_OPERATOR_NO_OVERLOAD("__long__", tp_as_number, PyNumberMethods, nb_long)
	LASS_PY_OPERATOR_NO_OVERLOAD("__float__", tp_as_number, PyNumberMethods, nb_float)
#if PY_MAJOR_VERSION < 3
	LASS_PY_OPERATOR_NO_OVERLOAD("__oct__", tp_as_number, PyNumberMethods, nb_oct)
	LASS_PY_OPERATOR_NO_OVERLOAD("__hex__", tp_as_number, PyNumberMethods, nb_hex)
#endif
	LASS_ASSERT_UNREACHABLE;
}
	/**/

void addClassMethod(
		ClassDefinition& classDef,
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
	LASS_PY_OPERATOR_("__truediv__", tp_as_number, PyNumberMethods, nb_true_divide, Binary)
	LASS_PY_OPERATOR_("__itruediv__", tp_as_number, PyNumberMethods, nb_inplace_true_divide, Binary)
	LASS_PY_OPERATOR_("__floordiv__", tp_as_number, PyNumberMethods, nb_floor_divide, Binary)
	LASS_PY_OPERATOR_("__i__floordiv____", tp_as_number, PyNumberMethods, nb_inplace_floor_divide, Binary)
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
		ClassDefinition& classDef,
		const lass::python::impl::TernarySlot& methodName, const char* documentation, 
		ternaryfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	if (methodName.name=="__call__")
	{
		overloadChain.setTernaryfunc(classDef.type_.tp_call);
		classDef.type_.tp_call = dispatcher;
		return;
	}
	LASS_ASSERT_UNREACHABLE;
}

/**/
void addClassMethod(
		ClassDefinition& classDef,
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
		ClassDefinition& classDef,
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
		ClassDefinition& classDef,
		const lass::python::impl::SsizeObjArgSlot& methodName, const char* documentation, 
		ssizeobjargproc dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__seq_setitem__", tp_as_sequence, PySequenceMethods, sq_ass_item, SsizeObjArgProc) 
	LASS_ASSERT_UNREACHABLE;
}
/**/
void addClassMethod(
		ClassDefinition& classDef,
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
		ClassDefinition& classDef,
		const lass::python::impl::ObjObjSlot& methodName, const char* documentation, 
		objobjproc dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__contains__", tp_as_sequence, PySequenceMethods, sq_contains, ObjObjProc) 
	LASS_ASSERT_UNREACHABLE;
}
/**/
void addClassMethod(
		ClassDefinition& classDef,
		const lass::python::impl::ObjObjArgSlot& methodName, const char* documentation, 
		objobjargproc dispatcher, 
		OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__map_setitem__", tp_as_mapping, PyMappingMethods, mp_ass_subscript, ObjObjArgProc) 
	LASS_ASSERT_UNREACHABLE;
}

void addClassMethod(
		ClassDefinition& classDef,
		const lass::python::impl::IterSlot& methodName, const char* documentation, 
		getiterfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	if (methodName.name=="__iter__")
	{
		classDef.type_.tp_iter = dispatcher;
		return;
	}
	LASS_ASSERT_UNREACHABLE;
}
void addClassMethod(
		ClassDefinition& classDef,
		const lass::python::impl::IterNextSlot& methodName, const char* documentation, 
		iternextfunc dispatcher, 
		OverloadLink& overloadChain) 
{
	if (methodName.name=="next")
	{
		classDef.type_.tp_iternext = dispatcher;
		return;
	}
	LASS_ASSERT_UNREACHABLE;
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
	if (!result)
	{
		return TPyObjPtr();
	}
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
