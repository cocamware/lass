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
 *	Copyright (C) 2004-2025 the Initial Developer.
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



#include "python_common.h"
#include "class_definition.h"
#include "enum_definition.h"
#include "overload_link.h"
#include "pyobject_plus.h"
#include <iostream>
#include <cstring>

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	pragma warning(disable: 4996) // This function or variable may be unsafe ...
#endif

namespace lass
{
namespace python
{
namespace impl
{



PyMethodDef createPyMethodDef(const char *ml_name, PyCFunction ml_meth, int ml_flags, const char *ml_doc)
{
	PyMethodDef temp;
	temp.ml_name = const_cast<char*>(ml_name);
	temp.ml_meth = ml_meth;
	temp.ml_flags = ml_flags;
	temp.ml_doc = const_cast<char*>(ml_doc);
	return temp;
}



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



void dealloc(PyObject* obj)
{
	delete static_cast<PyObjectPlus*>(obj);
};


#if PY_VERSION_HEX >= 0x03080000 && PY_VERSION_HEX < 0x03090000 // == 3.8
#	if LASS_COMPILER_TYPE != LASS_COMPILER_TYPE_MSVC
#		pragma GCC diagnostic push
#		pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#	endif
#endif

ClassDefinition::ClassDefinition(
		const char* name, const char* doc, Py_ssize_t typeSize, 
		richcmpfunc richcmp, ClassDefinition* parent, TClassRegisterHook registerHook):
	parent_(parent),
	classRegisterHook_(registerHook),
	implicitConvertersSlot_(0),
	isFrozen_(false)
{
	PyTypeObject type = { 
		PyVarObject_HEAD_INIT(&PyType_Type, 0)
		(char*)name,	/*tp_name*/
		typeSize,	/*tp_basicsize*/
		0,	/*tp_itemsize*/
		dealloc,	/*tp_dealloc*/
		0,	/*tp_vectorcall_offset (was tp_print in 3.7 and earlier, for Python 2.x print formatting) */
		0,	/*tp_getattr*/
		0,	/*tp_setattr*/
		0,	/*tp_compare*/
		0,	/*tp_repr*/
		0,	/*tp_as_number*/
		0,	/*tp_as_sequence*/
		0,	/*tp_as_mapping*/
		0,	/*tp_hash*/
		0,	/*tp_call */
		0,	/*tp_str */
		0,/*PyObject_GenericGetAttr ,*/	/*tp_getattro */
		0,/*PyObject_GenericSetAttr,*/	/*tp_setattro */
		0,	/*tp_as_buffer*/
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE  , /*| Py_TPFLAGS_HAVE_CLASS ,*/	/*tp_flags*/
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
		0,	/*tp_version_tag*/
		0,	/*tp_finalize*/
		0,	/*tp_vectorcall, exists in 3.8 but only used since 3.9 */
#if PY_VERSION_HEX >= 0x03080000 && PY_VERSION_HEX < 0x03090000 // == 3.8
        0,	/*tp_print, only exists in 3.8 for backwards compatibility, bpo-37250 */
#endif
#if PY_VERSION_HEX >= 0x030C0000 // >= 3.12
		0,	/*tp_watched, internal only */
#endif
#if PY_VERSION_HEX >= 0x030D0000 // >= 3.13
		0,	/*tp_versions_used */
#endif
	};
	type_ = type;
	methods_.push_back(impl::createPyMethodDef( 0, 0, 0, 0 ));
	getSetters_.push_back(impl::createPyGetSetDef( 0, 0, 0, 0, 0 ));
}

#if PY_VERSION_HEX >= 0x03080000 && PY_VERSION_HEX < 0x03090000 // == 3.8
#	if LASS_COMPILER_TYPE != LASS_COMPILER_TYPE_MSVC
#		pragma GCC diagnostic pop
#	endif
#endif


ClassDefinition::~ClassDefinition()
{
}



void ClassDefinition::setDocIfNotNull(const char* doc)
{
	if (doc)
	{
		setDoc(doc);
	}
}



#define LASS_PY_OPERATOR_(s_name, i_protocol, t_protocol, i_hook, i_nary)\
	if (strcmp(slot.name, s_name) == 0)\
	{\
		if (type_.i_protocol == 0)\
		{\
			type_.i_protocol = new t_protocol;\
			::memset(type_.i_protocol, 0, sizeof(t_protocol));\
		}\
		overloadChain.LASS_CONCATENATE_3(set, i_nary, func)(type_.i_protocol->i_hook);\
		type_.i_protocol->i_hook = dispatcher;\
		return;\
	}\
	/**/

#define LASS_PY_OPERATOR_NO_OVERLOAD(s_name, i_protocol, t_protocol, i_hook)\
	if (strcmp(slot.name, s_name) == 0)\
	{\
		if (type_.i_protocol == 0)\
		{\
			type_.i_protocol = new t_protocol;\
			::memset(type_.i_protocol, 0, sizeof(t_protocol));\
		}\
		type_.i_protocol->i_hook = dispatcher;\
		return;\
	}\
	/**/

#define LASS_PY_COMPARATOR_(s_name, v_op)\
	if (strcmp(slot.name, s_name) == 0)\
	{\
		compareFuncs_.push_back(CompareFunc(dispatcher, v_op));\
		return; \
	}\
	/**/	


void ClassDefinition::addConstructor(newfunc dispatcher, newfunc& overloadChain)
{
	overloadChain = type_.tp_new;
	type_.tp_new = dispatcher;
}


void ClassDefinition::addMethod(const char* name, const char* doc, PyCFunction dispatcher, OverloadLink& overloadChain) 
{
	TMethods::iterator i = std::find_if(methods_.begin(), methods_.end(), NamePredicate(name));
	if (i == methods_.end())
	{
		methods_.insert(methods_.begin(), createPyMethodDef(name, dispatcher, METH_VARARGS , doc));
		overloadChain.setNull();
	}
	else
	{
		LASS_ASSERT(i->ml_flags == METH_VARARGS);
		overloadChain.setPyCFunction(i->ml_meth);
		i->ml_meth = dispatcher;
		if (i->ml_doc == 0)
		{
			i->ml_doc = const_cast<char*>(doc);
		}
	};
}

void ClassDefinition::addMethod(const ComparatorSlot& slot, const char*, PyCFunction dispatcher, OverloadLink&) 
{
	LASS_PY_COMPARATOR_("__lt__", Py_LT)
	LASS_PY_COMPARATOR_("__le__", Py_LE)
	LASS_PY_COMPARATOR_("__eq__", Py_EQ)
	LASS_PY_COMPARATOR_("__ne__", Py_NE)
	LASS_PY_COMPARATOR_("__gt__", Py_GT)
	LASS_PY_COMPARATOR_("__ge__", Py_GE)
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const LenSlot& slot, const char*, lenfunc dispatcher, OverloadLink&) 
{
	LASS_PY_OPERATOR_NO_OVERLOAD("__len__", tp_as_sequence, PySequenceMethods, sq_length) 
	LASS_PY_OPERATOR_NO_OVERLOAD("__seq_len__", tp_as_sequence, PySequenceMethods, sq_length) 
	LASS_PY_OPERATOR_NO_OVERLOAD("__map_len__", tp_as_mapping, PyMappingMethods, mp_length) 
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const UnarySlot& slot, const char*, unaryfunc dispatcher, OverloadLink&) 
{
	if (strcmp(slot.name, "__str__") == 0)
	{
		type_.tp_str = dispatcher;
		return;
	}
	if (strcmp(slot.name, "__repr__") == 0)
	{
		type_.tp_repr = dispatcher;
		return;
	}
	LASS_PY_OPERATOR_NO_OVERLOAD("__neg__", tp_as_number, PyNumberMethods, nb_negative)
	LASS_PY_OPERATOR_NO_OVERLOAD("__pos__", tp_as_number, PyNumberMethods, nb_positive)
	LASS_PY_OPERATOR_NO_OVERLOAD("__abs__", tp_as_number, PyNumberMethods, nb_absolute)
	LASS_PY_OPERATOR_NO_OVERLOAD("__invert__", tp_as_number, PyNumberMethods, nb_invert)
	LASS_PY_OPERATOR_NO_OVERLOAD("__int__", tp_as_number, PyNumberMethods, nb_int)
	LASS_PY_OPERATOR_NO_OVERLOAD("__float__", tp_as_number, PyNumberMethods, nb_float)
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const BinarySlot& slot, const char*, binaryfunc dispatcher, OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__add__", tp_as_number, PyNumberMethods, nb_add, Binary)// r?
	LASS_PY_OPERATOR_("__sub__", tp_as_number, PyNumberMethods, nb_subtract,Binary)// r?
	LASS_PY_OPERATOR_("__mul__", tp_as_number, PyNumberMethods, nb_multiply, Binary)// r?
	LASS_PY_OPERATOR_("__mod__", tp_as_number, PyNumberMethods, nb_remainder, Binary)// r?
	LASS_PY_OPERATOR_("__lshift__", tp_as_number, PyNumberMethods, nb_lshift, Binary)// r?
	LASS_PY_OPERATOR_("__rshift__", tp_as_number, PyNumberMethods, nb_rshift, Binary)// r?
	LASS_PY_OPERATOR_("__and__", tp_as_number, PyNumberMethods, nb_and, Binary)// r?
	LASS_PY_OPERATOR_("__xor__", tp_as_number, PyNumberMethods, nb_xor, Binary)// r?
	LASS_PY_OPERATOR_("__or__", tp_as_number, PyNumberMethods, nb_or, Binary)// r?
	LASS_PY_OPERATOR_("__truediv__", tp_as_number, PyNumberMethods, nb_true_divide, Binary)// r?
	LASS_PY_OPERATOR_("__floordiv__", tp_as_number, PyNumberMethods, nb_floor_divide, Binary)// r?
	LASS_PY_OPERATOR_("__iadd__", tp_as_number, PyNumberMethods, nb_inplace_add, Binary)
	LASS_PY_OPERATOR_("__isub__", tp_as_number, PyNumberMethods, nb_inplace_subtract, Binary)
	LASS_PY_OPERATOR_("__imul__", tp_as_number, PyNumberMethods, nb_inplace_multiply, Binary)
	LASS_PY_OPERATOR_("__imod__", tp_as_number, PyNumberMethods, nb_inplace_remainder, Binary)
	LASS_PY_OPERATOR_("__ilshift__", tp_as_number, PyNumberMethods, nb_inplace_lshift, Binary)
	LASS_PY_OPERATOR_("__irshift__", tp_as_number, PyNumberMethods, nb_inplace_rshift, Binary)
	LASS_PY_OPERATOR_("__iand__", tp_as_number, PyNumberMethods, nb_inplace_and, Binary)
	LASS_PY_OPERATOR_("__ixor__", tp_as_number, PyNumberMethods, nb_inplace_xor, Binary)
	LASS_PY_OPERATOR_("__ior__", tp_as_number, PyNumberMethods, nb_inplace_or, Binary)
	LASS_PY_OPERATOR_("__itruediv__", tp_as_number, PyNumberMethods, nb_inplace_true_divide, Binary)
	LASS_PY_OPERATOR_("__ifloordiv__", tp_as_number, PyNumberMethods, nb_inplace_floor_divide, Binary)
	LASS_PY_OPERATOR_("__matmul__", tp_as_number, PyNumberMethods, nb_matrix_multiply, Binary)
	LASS_PY_OPERATOR_("__imatmul__", tp_as_number, PyNumberMethods, nb_inplace_matrix_multiply, Binary)
	LASS_PY_OPERATOR_("__iconcat__", tp_as_sequence, PySequenceMethods, sq_inplace_concat, Binary)
	LASS_PY_OPERATOR_("__concat__", tp_as_sequence, PySequenceMethods, sq_concat, Binary) 
	LASS_PY_OPERATOR_("__map_getitem__", tp_as_mapping, PyMappingMethods, mp_subscript, Binary) 
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const TernarySlot& slot, const char*, ternaryfunc dispatcher, OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__pow__", tp_as_number, PyNumberMethods, nb_power, Ternary)// r?
	LASS_PY_OPERATOR_("__ipow__", tp_as_number, PyNumberMethods, nb_inplace_power, Ternary)
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const SsizeArgSlot& slot, const char*, ssizeargfunc dispatcher, OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__seq_getitem__", tp_as_sequence, PySequenceMethods, sq_item, SsizeArg) 
	LASS_PY_OPERATOR_("__repeat__", tp_as_sequence, PySequenceMethods, sq_repeat, SsizeArg) 
	LASS_PY_OPERATOR_("__irepeat__", tp_as_sequence, PySequenceMethods, sq_inplace_repeat, SsizeArg) 
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const SsizeObjArgSlot& slot, const char*, ssizeobjargproc dispatcher, OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__seq_setitem__", tp_as_sequence, PySequenceMethods, sq_ass_item, SsizeObjArgProc) 
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const ObjObjSlot& slot, const char*, objobjproc dispatcher, OverloadLink& overloadChain) 
{
	LASS_PY_OPERATOR_("__contains__", tp_as_sequence, PySequenceMethods, sq_contains, ObjObjProc) 
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const ObjObjArgSlot& slot, const char*,	objobjargproc dispatcher, OverloadLink& overloadChain) 
{
	// objobjargproc slots return 0 on success and -1 on failure
	LASS_PY_OPERATOR_("__map_setitem__", tp_as_mapping, PyMappingMethods, mp_ass_subscript, ObjObjArgProc) 
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const IterSlot& slot, const char*, getiterfunc dispatcher, OverloadLink&) 
{
	if (strcmp(slot.name, "__iter__") == 0)
	{
		type_.tp_iter = dispatcher;
		return;
	}
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const IterNextSlot& slot, const char*, iternextfunc dispatcher, OverloadLink&) 
{
	if (strcmp(slot.name, "next") == 0)
	{
		type_.tp_iternext = dispatcher;
		return;
	}
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const ArgKwSlot& slot, const char*, ternaryfunc dispatcher, OverloadLink& overloadChain) 
{
	if (strcmp(slot.name, "__call__") == 0)
	{
		overloadChain.setArgKwfunc(type_.tp_call);
		type_.tp_call = dispatcher;
		return;
	}
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const InquirySlot& slot, const char*, inquiry dispatcher, OverloadLink&) 
{
	LASS_PY_OPERATOR_NO_OVERLOAD("__bool__", tp_as_number, PyNumberMethods, nb_bool);
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addGetSetter(const char* name, const char* doc, getter get, setter set)
{
	getSetters_.insert(getSetters_.begin(), impl::createPyGetSetDef(name, get, set, doc, 0));
}

void ClassDefinition::addStaticMethod(const char* name, const char* doc, PyCFunction dispatcher, PyCFunction& overloadChain)
{
	std::vector<PyMethodDef>::iterator i = ::std::find_if(methods_.begin(), methods_.end(), NamePredicate(name));
	if (i == methods_.end())
	{
		methods_.insert(methods_.begin(), createPyMethodDef(name, dispatcher, METH_VARARGS | METH_STATIC, doc));
		overloadChain = 0;
	}
	else
	{
		LASS_ASSERT(i->ml_flags == (METH_VARARGS | METH_STATIC));
		overloadChain = i->ml_meth;
		i->ml_meth = dispatcher;
		if (i->ml_doc == 0)
		{
			i->ml_doc = const_cast<char*>(doc);
		}
	}
}	

void ClassDefinition::addInnerClass(ClassDefinition& innerClass)
{
	LASS_ASSERT(std::count_if(innerClasses_.begin(), innerClasses_.end(), NamePredicate(innerClass.name())) == 0);
	innerClasses_.push_back(&innerClass);
}

void ClassDefinition::addInnerEnum(EnumDefinitionBase* enumDefinition)
{
	// LASS_ASSERT(std::count_if(innerClasses_.begin(), innerClasses_.end(), NamePredicate(innerClass.name())) == 0);
	innerEnums_.push_back(enumDefinition);
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

	PY_ENFORCE_ZERO( PyType_Ready( &type_ ) );
	
	for (TStaticMembers::const_iterator i = statics_.begin(); i != statics_.end(); ++i)
	{
		PyDict_SetItemString(type_.tp_dict, const_cast<char*>(i->name()), i->member()->build().get());
	}
	for (TClassDefs::const_iterator i = innerClasses_.begin(); i != innerClasses_.end(); ++i)
	{
		ClassDefinition* innerClass = *i;
		const char* shortName = innerClass->name();
		innerClass->freezeDefinition(type_.tp_name);
		PyDict_SetItemString(type_.tp_dict, const_cast<char*>(shortName), reinterpret_cast<PyObject*>(innerClass->type()));
	}
	for (auto def : innerEnums_)
	{
		def->freezeDefinition(nullptr, nullptr); // TODO: figure out modulename and qualname ...
		PyDict_SetItemString(type_.tp_dict, def->name(), def->type());
	}

	if (classRegisterHook_)
	{
		classRegisterHook_();
	}
}

PyObject* ClassDefinition::callRichCompare(PyObject* self, PyObject* other, int op)
{
	// don't worry about GIL, as we should still have it when we get here ...

	if (other == Py_None)
	{
		// we need to treat the None type differently because the pyGet/BuildSimpleObject are able to cast
		// from None but if you give that thing to a reference, then you are in big trouble
		switch (op)
		{
		case Py_EQ:
			{
				if (self == other)
					Py_RETURN_TRUE;
				else
					Py_RETURN_FALSE;
			}
		case Py_NE:
			{
				if (self != other)
					Py_RETURN_TRUE;
				else
					Py_RETURN_FALSE;
			}
		// don't define any order relation on None
		default:
			Py_RETURN_FALSE;
		};
	}

	TPyObjPtr args(Py_BuildValue("(O)", other));
	const TCompareFuncs::const_iterator end = compareFuncs_.end();
	for (TCompareFuncs::const_iterator i = compareFuncs_.begin(); i != end; ++i)
	{
		if (i->op == op)
		{
			PyObject* result = (i->dispatcher)(self, args.get());\
			if (result || (PyErr_Occurred() && !PyErr_ExceptionMatches(PyExc_TypeError)))
			{
				return result;
			}
		}
	}

	if (!parent_)
	{
		static const char* symbols[] = { "<", "<=", "==", "!=", ">", ">=" };
		LASS_ASSERT(op >= 0 && op <= Py_GE);
		std::ostringstream buffer;
		buffer << "Comparison operator " << symbols[op] << " not implemented for this type";
		PyErr_SetString(PyExc_NotImplementedError, buffer.str().c_str());
		return 0;	
	}

	return parent_->callRichCompare(self, other, op);
}

}
}
}

// EOF
