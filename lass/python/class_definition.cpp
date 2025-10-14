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
#include "_lass_module.h"
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



int traverse(PyObject* self, visitproc visit, void* arg)
{
	Py_VISIT(Py_TYPE(self));
	return 0;
}



ClassDefinition::ClassDefinition(
		const char* name, const char* doc, Py_ssize_t typeSize, 
		richcmpfunc richcmp, ClassDefinition* parent, TClassRegisterHook registerHook):
	slots_({{ 0, nullptr }}),
	parent_(parent),
	classRegisterHook_(registerHook),
	className_(name),
	doc_(doc),
	implicitConvertersSlot_(0),
	isFrozen_(false)
{
	PyType_Spec spec = {
		nullptr, /* name */
		static_cast<int>(typeSize), /* basicsize */
		0, /* itemsize */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC, /* flags */
		nullptr, /* slots */
	};
	spec_ = spec;
	setSlot(Py_tp_dealloc, &dealloc);
	setSlot(Py_tp_traverse, &traverse);
	if (richcmp)
	{
		setSlot(Py_tp_richcompare, richcmp);
	}
	methods_.push_back(impl::createPyMethodDef( 0, 0, 0, 0 ));
	getSetters_.push_back(impl::createPyGetSetDef( 0, 0, 0, 0, 0 ));
}



ClassDefinition::~ClassDefinition()
{
}



const PyTypeObject* ClassDefinition::type() const
{
	LASS_ENFORCE(type_)(name())(" is not frozen yet");
	return reinterpret_cast<PyTypeObject*>(type_.get());
}



PyTypeObject* ClassDefinition::type()
{
	LASS_ENFORCE(type_)(name())(" is not frozen yet");
	return reinterpret_cast<PyTypeObject*>(type_.get());
}



const char* ClassDefinition::name() const
{
	return className_;
}



const char* ClassDefinition::doc() const
{
	return doc_;
}



void ClassDefinition::setDoc(const char* doc)
{
	doc_ = doc;
}


void ClassDefinition::setDocIfNotNull(const char* doc)
{
	if (doc)
	{
		setDoc(doc);
	}
}


void* ClassDefinition::getSlot(TSlotID slotId)
{
	LASS_ASSERT(!slots_.empty() && slots_.back().slot == 0);
	LASS_ASSERT(slotId > 0);
	TSlots::iterator i = std::lower_bound(slots_.begin(), slots_.end() - 1, slotId, [](const PyType_Slot& a, TSlotID b) { return a.slot < b; });
	if (i != slots_.end() && i->slot == slotId)
	{
		return i->pfunc;
	}
	return nullptr;
}


void* ClassDefinition::setSlot(TSlotID slotId, void* value)
{
	LASS_ASSERT(!isFrozen_);
	LASS_ASSERT(!slots_.empty() && slots_.back().slot == 0);
	LASS_ASSERT(slotId > 0);
	LASS_ASSERT(value);
	TSlots::iterator i = std::lower_bound(slots_.begin(), slots_.end() - 1, slotId, [](const PyType_Slot& a, TSlotID b) { return a.slot < b; });
	if (i != slots_.end() && i->slot == slotId)
	{
		void* old = i->pfunc;
		i->pfunc = value;
		return old;
	}
	else
	{
		PyType_Slot slot = { slotId, value };
		slots_.insert(i, slot);
		return nullptr;
	}
}


void ClassDefinition::addConstructor(newfunc dispatcher, newfunc& overloadChain)
{
	overloadChain = setSlot(Py_tp_new, dispatcher);
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
	compareFuncs_.push_back(CompareFunc(dispatcher, slot.slot));
}

void ClassDefinition::addMethod(const LenSlot& slot, const char*, lenfunc dispatcher, OverloadLink&) 
{
	setSlot(slot.slot, dispatcher);
}

void ClassDefinition::addMethod(const UnarySlot& slot, const char*, unaryfunc dispatcher, OverloadLink&) 
{
	setSlot(slot.slot, dispatcher);
}

void ClassDefinition::addMethod(const BinarySlot& slot, const char*, binaryfunc dispatcher, OverloadLink& overloadChain) 
{
	overloadChain.setBinaryfunc(setSlot(slot.slot, dispatcher));
}

void ClassDefinition::addMethod(const TernarySlot& slot, const char*, ternaryfunc dispatcher, OverloadLink& overloadChain) 
{
	overloadChain.setTernaryfunc(setSlot(slot.slot, dispatcher));
}

void ClassDefinition::addMethod(const SsizeArgSlot& slot, const char*, ssizeargfunc dispatcher, OverloadLink& overloadChain) 
{
	overloadChain.setSsizeArgfunc(setSlot(slot.slot, dispatcher));
}

void ClassDefinition::addMethod(const SsizeObjArgSlot& slot, const char*, ssizeobjargproc dispatcher, OverloadLink& overloadChain) 
{
	overloadChain.setSsizeObjArgProcfunc(setSlot(slot.slot, dispatcher));
}

void ClassDefinition::addMethod(const ObjObjSlot& slot, const char*, objobjproc dispatcher, OverloadLink& overloadChain) 
{
	overloadChain.setObjObjProcfunc(setSlot(slot.slot, dispatcher));
}

void ClassDefinition::addMethod(const ObjObjArgSlot& slot, const char*, objobjargproc dispatcher, OverloadLink& overloadChain) 
{
	overloadChain.setObjObjArgProcfunc(setSlot(slot.slot, dispatcher));
}

void ClassDefinition::addMethod(const IterSlot& slot, const char*, getiterfunc dispatcher, OverloadLink& overloadChain)
{
	overloadChain.setGetIterFunc(setSlot(slot.slot, dispatcher));
}

void ClassDefinition::addMethod(const IterNextSlot& slot, const char*, iternextfunc dispatcher, OverloadLink& overloadChain)
{
	overloadChain.setIterNextFunc(setSlot(slot.slot, dispatcher));
}

void ClassDefinition::addMethod(const ArgKwSlot& slot, const char*, ternaryfunc dispatcher, OverloadLink& overloadChain) 
{
	overloadChain.setArgKwfunc(setSlot(slot.slot, dispatcher));
}

void ClassDefinition::addMethod(const InquirySlot& slot, const char*, inquiry dispatcher, OverloadLink&) 
{
	setSlot(slot.slot, dispatcher);
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

PyObject* ClassDefinition::freezeDefinition(PyObject* module)
{
	return freezeDefinition(module, nullptr);
}

PyObject* ClassDefinition::freezeDefinition(PyObject* module, const char* scopeName)
{
	LASS_ASSERT(!isFrozen_);
	if (isFrozen_)
	{
		return type_.get();
	}

	if (parent_)
	{
		// In the general case, we can't freeze the parent's definition, as we can't be sure of its module or scopeName.
		// However, in case of PyObjectPlus, nobody else will do it.
		if (parent_ == &PyObjectPlus::_lassPyClassDef)
		{
			if (impl::initLassModule() != 0)
			{
				return nullptr;
			}
		}
		if (!parent_->type_)
		{
			PyErr_Format(PyExc_AssertionError, "Parent class %s of %s is not frozen yet", parent_->className_, className_);
		}
		parent_->subClasses_.push_back(this);
	}

	const char* moduleName = nullptr;
	if (module)
	{
		moduleName = PyModule_GetName(module);
		if (!moduleName)
		{
			return nullptr;
		}
		LASS_ASSERT(!spec_.name || std::strcmp(spec_.name, moduleName) == 0);
		if (!spec_.name)
		{
			const size_t n = std::strlen(moduleName) + std::strlen(className_) + 2; // one extra for dot, and one extra for null
			char* buf = static_cast<char*>(std::malloc(n));
			const int r = ::snprintf(buf, n, "%s.%s", moduleName, className_);
			LASS_ENFORCE(r > 0 && static_cast<size_t>(r) < n);
			spec_.name = buf;
		}
	}
	else
	{
		LASS_ASSERT(!spec_.name || std::strcmp(spec_.name, className_) == 0);
		if (!spec_.name)
		{
			spec_.name = className_;
		}
	}

	if (!type_)
	{
		setSlot(Py_tp_base, parent_ ? parent_->type() : &PyBaseObject_Type); // INCREF???
		setSlot(Py_tp_methods, &methods_[0]);
		setSlot(Py_tp_getset, &getSetters_[0]);
		if (doc_) // a nullptr as Py_tp_doc causes access violation in PyType_FromSpec 
		{
			setSlot(Py_tp_doc, const_cast<char*>(doc_));
		}
		LASS_ASSERT(slots_.back().slot == 0);
		spec_.slots = &slots_[0];

#if PY_VERSION_HEX >= 0x030a0000 // >= 3.10
		if (getSlot(Py_tp_new) == nullptr)
		{
			// We don't have a constructor, so we disallow instantiation.
			spec_.flags |= Py_TPFLAGS_DISALLOW_INSTANTIATION;
		}
#endif

		type_.reset(PyType_FromModuleAndSpec(module, &spec_, nullptr));
		if (!type_)
		{
			return nullptr;
		}
	}

	PyObject* type = type_.get();

	const char* qualname = className_;
	if (scopeName)
	{
		const size_t n = std::strlen(scopeName) + std::strlen(className_) + 2; // one extra for dot, and one extra for null
		char* buf = static_cast<char*>(std::malloc(n));
		const int r = ::snprintf(buf, n, "%s.%s", scopeName, className_);
		LASS_ENFORCE(r > 0 && static_cast<size_t>(r) < n);
		qualname = buf;
		TPyObjPtr objQualname(pyBuildSimpleObject(qualname));
		if (!objQualname || PyObject_SetAttrString(type, "__qualname__", objQualname.get()) != 0)
		{
			return nullptr;
		}
	}
	for (TStaticMembers::const_iterator i = statics_.begin(); i != statics_.end(); ++i)
	{
		TPyObjPtr obj = i->member()->build();
		if (!obj || PyObject_SetAttrString(type, i->name(), obj.get()) != 0)
		{
			return nullptr;
		}
	}
	for (TClassDefs::const_iterator i = innerClasses_.begin(); i != innerClasses_.end(); ++i)
	{
		ClassDefinition* innerClass = *i;
		const char* shortName = innerClass->name();
		PyObject* innerType = innerClass->freezeDefinition(module, qualname);
		if (!innerType || PyObject_SetAttrString(type, shortName, innerType) != 0)
		{
			return nullptr;
		}
	}
	for (auto def : innerEnums_)
	{
		PyObject* enumType = def->freezeDefinition(moduleName, qualname);
		if (!enumType || PyObject_SetAttrString(type, def->name(), enumType) != 0)
		{
			return nullptr;
		}
	}

	if (classRegisterHook_)
	{
		classRegisterHook_();
	}

#if PY_VERSION_HEX >= 0x030a0000 // >= 3.10
	reinterpret_cast<PyTypeObject*>(type)->tp_flags |= Py_TPFLAGS_IMMUTABLETYPE;
#endif

	isFrozen_ = true;
	return type;
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
