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
 *	Copyright (C) 2004-2011 the Initial Developer.
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
#include "overload_link.h"
#include "pyobject_plus.h"
#include <iostream>
#include <cstring>

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4996) // This function or variable may be unsafe ...
#endif

#if PY_MAJOR_VERSION < 3
#	define PyVarObject_HEAD_INIT(type, size) PyObject_HEAD_INIT(type) size,
#else
#	define Py_TPFLAGS_CHECKTYPES 0 // flag no longer exists
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



ClassDefinition::ClassDefinition(
		const char* name, const char* doc, Py_ssize_t typeSize, 
		richcmpfunc richcmp, ClassDefinition* parent, TClassRegisterHook registerHook):
	parent_(parent),
	classRegisterHook_(registerHook),
	implicitConvertersSlot_(0),
	isFrozen_(false)
{
#if LASS_USE_PYTYPE_SPEC
	PyType_Spec spec = {
		name, /* name */
		static_cast<int>(typeSize), /* basicsize */
		0, /* itemsize */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /* flags */
		0, /* slots */
	};
	spec_ = spec;
	setSlot(Py_tp_dealloc, &dealloc);
	if (richcmp)
	{
		setSlot(Py_tp_richcompare, richcmp);
	}
#else
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
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES  , /*| Py_TPFLAGS_HAVE_CLASS ,*/	/*tp_flags*/
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
#if PY_VERSION_HEX >= 0x02060000 // >= 2.6
		0,	/*tp_version_tag*/
#endif
#if PY_VERSION_HEX >= 0x03040000 // >= 3.4
		0,	/*tp_finalize*/
#endif
	};
	type_ = type;
#endif
	methods_.push_back(impl::createPyMethodDef( 0, 0, 0, 0 ));
	getSetters_.push_back(impl::createPyGetSetDef( 0, 0, 0, 0, 0 ));
}



ClassDefinition::~ClassDefinition()
{
}



const PyTypeObject* ClassDefinition::type() const
{
#if LASS_USE_PYTYPE_SPEC
	return reinterpret_cast<PyTypeObject *>(type_.get());
#else
	return &type_;
#endif
}



PyTypeObject* ClassDefinition::type()
{
#if LASS_USE_PYTYPE_SPEC
	return reinterpret_cast<PyTypeObject*>(type_.get());
#else
	return &type_;
#endif
}



const char* ClassDefinition::name() const
{
#if LASS_USE_PYTYPE_SPEC
	return spec_.name;
#else
	return type_.tp_name;
#endif
}



const char* ClassDefinition::doc() const
{
#if LASS_USE_PYTYPE_SPEC
	return doc_;
#else
	return type_.tp_doc;
#endif
}



void ClassDefinition::setDoc(const char* doc)
{ 
#if LASS_USE_PYTYPE_SPEC
	doc_ = doc;
#else
	type_.tp_doc = const_cast<char*>(doc);
#endif
}


#if LASS_USE_PYTYPE_SPEC

namespace
{

struct LessSlot: public std::binary_function<PyType_Slot, PyType_Slot, bool>
{
	bool operator()(const PyType_Slot& a, const PyType_Slot& b) const
	{
		return a.slot < b.slot;
	}
};

}

void* ClassDefinition::setSlot(int slotId, void* value)
{
	LASS_ASSERT(slotId > 0);
	LASS_ASSERT(!isFrozen_);
	LASS_ASSERT(value);
	PyType_Slot slot = { slotId, value };
	TSlots::iterator i = std::lower_bound(slots_.begin(), slots_.end(), slot, LessSlot());
	if (i != slots_.end() && i->slot == slotId)
	{
		void* old = i->pfunc;
		i->pfunc = value;
		return old;
	}
	else
	{
		slots_.insert(i, slot);
		return 0;
	}
}

#define LASS_PY_SLOT_(s_name, i_hook, i_nary) \
	if (strcmp(slot.name, s_name) == 0)\
	{\
		overloadChain.LASS_CONCATENATE_3(set, i_nary, func)( setSlot(LASS_CONCATENATE(Py_, i_hook), dispatcher) );\
		return;\
	}\
	/**/

#define LASS_PY_SLOT_NO_OVERLOAD(s_name, i_hook) \
	if (strcmp(slot.name, s_name) == 0)\
	{\
		setSlot(LASS_CONCATENATE(Py_, i_hook), dispatcher); \
		return; \
	}\
	/**/

#define LASS_PY_OPERATOR_(s_name, i_protocol, t_protocol, i_hook, i_nary)\
	LASS_PY_SLOT_(s_name, i_hook, i_nary)

#define LASS_PY_OPERATOR_NO_OVERLOAD(s_name, i_protocol, t_protocol, i_hook)\
	LASS_PY_SLOT_NO_OVERLOAD(s_name, i_hook)

#else

#define LASS_PY_SLOT_(s_name, i_hook, i_nary) \
	if (strcmp(slot.name, s_name) == 0)\
	{\
		overloadChain.LASS_CONCATENATE_3(set, i_nary, func)(type_.i_hook);\
		type_.i_hook = dispatcher; \
		return; \
	}\
	/**/

#define LASS_PY_SLOT_NO_OVERLOAD(s_name, i_hook) \
	if (strcmp(slot.name, s_name) == 0)\
	{\
		type_.i_hook = dispatcher;\
		return;\
	}\
	/**/

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

#endif

#define LASS_PY_COMPARATOR_(s_name, v_op)\
	if (strcmp(slot.name, s_name) == 0)\
	{\
		compareFuncs_.push_back(CompareFunc(dispatcher, v_op));\
		return; \
	}\
	/**/

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
	LASS_PY_SLOT_NO_OVERLOAD("__str__", tp_str)
	LASS_PY_SLOT_NO_OVERLOAD("__repr__", tp_repr)
	LASS_PY_OPERATOR_NO_OVERLOAD("__neg__", tp_as_number, PyNumberMethods, nb_negative)
	LASS_PY_OPERATOR_NO_OVERLOAD("__pos__", tp_as_number, PyNumberMethods, nb_positive)
	LASS_PY_OPERATOR_NO_OVERLOAD("__abs__", tp_as_number, PyNumberMethods, nb_absolute)
	LASS_PY_OPERATOR_NO_OVERLOAD("__invert__", tp_as_number, PyNumberMethods, nb_invert)
	LASS_PY_OPERATOR_NO_OVERLOAD("__int__", tp_as_number, PyNumberMethods, nb_int)
	LASS_PY_OPERATOR_NO_OVERLOAD("__float__", tp_as_number, PyNumberMethods, nb_float)
#if PY_MAJOR_VERSION < 3
	LASS_PY_OPERATOR_NO_OVERLOAD("__long__", tp_as_number, PyNumberMethods, nb_long)
	LASS_PY_OPERATOR_NO_OVERLOAD("__oct__", tp_as_number, PyNumberMethods, nb_oct)
	LASS_PY_OPERATOR_NO_OVERLOAD("__hex__", tp_as_number, PyNumberMethods, nb_hex)
#endif
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
#if PY_MAJOR_VERSION < 3
	LASS_PY_OPERATOR_("__div__", tp_as_number, PyNumberMethods, nb_divide, Binary)
	LASS_PY_OPERATOR_("__idiv__", tp_as_number, PyNumberMethods, nb_inplace_divide, Binary)
#endif

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

#if PY_MAJOR_VERSION < 3
void ClassDefinition::addMethod(const SsizeSsizeArgSlot& slot, const char*, ssizessizeargfunc dispatcher, OverloadLink& overloadChain)
{

	LASS_PY_OPERATOR_("__getslice__", tp_as_sequence, PySequenceMethods, sq_slice, SsizeSsizeArg)
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const SsizeSsizeObjArgSlot& slot, const char*, ssizessizeobjargproc dispatcher, OverloadLink& overloadChain)
{
	LASS_PY_OPERATOR_("__setslice__", tp_as_sequence, PySequenceMethods, sq_ass_slice, SsizeSsizeObjArgProc)
	LASS_ASSERT_UNREACHABLE;
}
#endif

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
	LASS_PY_SLOT_NO_OVERLOAD("__iter__", tp_iter)
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const IterNextSlot& slot, const char*, iternextfunc dispatcher, OverloadLink&) 
{
	LASS_PY_SLOT_NO_OVERLOAD("next", tp_iternext)
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const ArgKwSlot& slot, const char*, ternaryfunc dispatcher, OverloadLink& overloadChain) 
{
	LASS_PY_SLOT_("__call__", tp_call, ArgKw)
	LASS_ASSERT_UNREACHABLE;
}

void ClassDefinition::addMethod(const InquirySlot& slot, const char*, inquiry dispatcher, OverloadLink&) 
{
#if PY_MAJOR_VERSION < 3
	LASS_PY_OPERATOR_NO_OVERLOAD("__bool__", tp_as_number, PyNumberMethods, nb_nonzero);
#else
	LASS_PY_OPERATOR_NO_OVERLOAD("__bool__", tp_as_number, PyNumberMethods, nb_bool);
#endif
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


void ClassDefinition::addConstructor(newfunc dispatcher, newfunc& overloadChain)
{
#if LASS_USE_PYTYPE_SPEC
	overloadChain = setSlot(Py_tp_new, dispatcher);
#else
	overloadChain = type_.tp_new;
	type_.tp_new = dispatcher;
#endif
}


/** @internal
*	The iFinal sets the flags for final classes from which no new types can be derived.  
*/
void ClassDefinition::freezeDefinition(const char* scopeName)
{
	LASS_ASSERT(!isFrozen_);
	if (isFrozen_)
	{
		return;
	}

	if (parent_)
	{
		// In the general case, we can't freeze the parent's definition, as we can't be sure of its scopeName.
		// However, in case of PyObjectPlus, nobody else will do it.  Luckily, we know its scopeName's empty.
		if (parent_ == &PyObjectPlus::_lassPyClassDef && !PyObjectPlus::_lassPyClassDef.isFrozen_)
		{
			PyObjectPlus::_lassPyClassDef.freezeDefinition();
		}
		LASS_ASSERT(parent_->isFrozen_);
		parent_->subClasses_.push_back(this);
	}

	const char* fullName = name();
	if (scopeName)
	{
		const size_t n = std::strlen(scopeName) + std::strlen(name()) + 2; // one extra for dot, and one extra for null
		char* buf = static_cast<char*>(std::malloc(n));
		const int r = ::snprintf(buf, n, "%s.%s", scopeName, name());
		LASS_ENFORCE(r > 0 && r < n);
		fullName = buf;
	}

#if LASS_USE_PYTYPE_SPEC
	spec_.name = fullName;

	LASS_ASSERT(slots_.empty() || slots_.back().slot != 0);
	setSlot(Py_tp_base, parent_ ? parent_->type() : &PyBaseObject_Type); // INCREF???
	setSlot(Py_tp_methods, &methods_[0]);
	setSlot(Py_tp_getset, &getSetters_[0]);
	if (doc_) // a nullptr as Py_tp_doc causes access violation in PyType_FromSpec 
	{
		setSlot(Py_tp_doc, const_cast<char*>(doc_));
	}

	PyType_Slot nullSlot = { 0, 0 };
	slots_.push_back(nullSlot);
	LASS_ASSERT(!spec_.slots);
	spec_.slots = &slots_[0];

	type_.reset(PY_ENFORCE_POINTER(PyType_FromSpec(&spec_)));

	PyObject* typ = type_.get();
#else
	type_.tp_base = parent_ ? parent_->type() : &PyBaseObject_Type;
	Py_INCREF( type_.tp_base );

	type_.tp_name = fullName;
	type_.tp_methods = &methods_[0];
	type_.tp_getset = &getSetters_[0];
	type_.tp_flags &= (~Py_TPFLAGS_HAVE_GC); // we take care of collecting garbage ourselves

	LASS_ENFORCE( PyType_Ready( &type_ ) >= 0 );
	Py_INCREF( &type_ ); 

	PyObject* typ = reinterpret_cast<PyObject*>(&type_);
#endif

	LASS_ASSERT(!PyType_IS_GC(this->type())); // we don't support GC!

	for (TStaticMembers::const_iterator i = statics_.begin(); i != statics_.end(); ++i)
	{
		PyDict_SetItemString(type()->tp_dict, const_cast<char*>(i->name()), i->member()->build());
		//PyObject_SetAttrString(typ, const_cast<char*>(i->name()), i->member()->build());
	}
	for (TClassDefs::const_iterator i = innerClasses_.begin(); i != innerClasses_.end(); ++i)
	{
		ClassDefinition* innerClass = *i;
		const char* shortName = innerClass->name();
		innerClass->freezeDefinition(name());
		PyDict_SetItemString(type()->tp_dict, const_cast<char*>(shortName), reinterpret_cast<PyObject*>(innerClass->type()));
		// PyObject_SetAttrString(typ, const_cast<char*>(shortName), reinterpret_cast<PyObject*>(innerClass->type()));
	}
	if (classRegisterHook_)
	{
		classRegisterHook_();
	}

	isFrozen_ = true;
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
