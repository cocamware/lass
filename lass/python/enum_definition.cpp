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
 *	Copyright (C) 2022-2025 the Initial Developer.
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

#include "enum_definition.h"
#include "pyobject_ptr.h"
#include "py_tuple.h"
#include "../stde/extended_cstring.h"

namespace lass
{
	namespace python
	{
		namespace impl
		{
			TPyObjPtr makeEnumType(const char* name, TPyObjPtr&& enumerators, TPyObjPtr&& kwargs)
			{
				TPyObjPtr enumMod(PyImport_ImportModule("enum"));
				if (!enumMod)
				{
					return TPyObjPtr();
				}
				TPyObjPtr intEnumType(PyObject_GetAttrString(enumMod.get(), "Enum"));
				if (!intEnumType)
				{
					return TPyObjPtr();
				}

				TPyObjPtr args = makeTuple(name, std::move(enumerators));
				if (!args)
				{
					return TPyObjPtr();
				}
				TPyObjPtr type{ PyObject_Call(intEnumType.get(), args.get(), kwargs.get()) };
				return type;
			}

			TPyObjPtr makeIntEnumType(const char* name, TPyObjPtr&& enumerators, TPyObjPtr&& kwargs)
			{
				TPyObjPtr enumMod(PyImport_ImportModule("enum"));
				if (!enumMod)
				{
					return TPyObjPtr();
				}
				TPyObjPtr intEnumType(PyObject_GetAttrString(enumMod.get(), "IntEnum"));
				if (!intEnumType)
				{
					return TPyObjPtr();
				}

				TPyObjPtr args = makeTuple(name, std::move(enumerators));
				if (!args)
				{
					return TPyObjPtr();
				}
				TPyObjPtr type{ PyObject_Call(intEnumType.get(), args.get(), kwargs.get()) };

#if PY_VERSION_HEX < 0x030b0000 // < 3.11
				if (!type)
				{
					return TPyObjPtr();
				}

				// set it's __str__ method to int.__repr__, to mimic 3.11 IntEnum behavior.
				// Not int.__str__ because that is object.__str__, which resolves back to enum.__repr__
				PyObject* intType = reinterpret_cast<PyObject*>(&PyLong_Type);
				TPyObjPtr intRepr{ PyObject_GetAttrString(intType, "__repr__") };
				if (!intRepr || PyObject_SetAttrString(type.get(), "__str__", intRepr.get()) != 0)
				{
					return TPyObjPtr();
				}
#endif

				return type;
			}

			TPyObjPtr makeStrEnumType(const char* name, TPyObjPtr&& enumerators, TPyObjPtr&& kwargs)
			{
#if PY_VERSION_HEX < 0x030b0000 // < 3.11
				// mix in str type, so that they also behave like strings ...
				PyObject* strType = reinterpret_cast<PyObject*>(&PyUnicode_Type);
				if (PyDict_SetItemString(kwargs.get(), "type", strType) != 0)
				{
					return TPyObjPtr();
				}

				// build it as a normal Enum
				TPyObjPtr type = makeEnumType(name, std::move(enumerators), std::move(kwargs));
				if (!type)
				{
					return TPyObjPtr();
				}

				// set it's __str__ method to str.__str__, to mimic 3.11 StrEnum behavior.
				TPyObjPtr strStr{ PyObject_GetAttrString(strType, "__str__") };
				if (!strStr || PyObject_SetAttrString(type.get(), "__str__", strStr.get()) != 0)
				{
					return TPyObjPtr();
				}

				return type;
#else
				TPyObjPtr enumMod(PY_ENFORCE_POINTER(PyImport_ImportModule("enum")));
				if (!enumMod)
				{
					return TPyObjPtr();
				}
				TPyObjPtr strEnumType(PY_ENFORCE_POINTER(PyObject_GetAttrString(enumMod.get(), "StrEnum")));
				if (!strEnumType)
				{
					return TPyObjPtr();
				}

				TPyObjPtr args = makeTuple(name, std::move(enumerators));
				if (!args)
				{
					return TPyObjPtr();
				}
				TPyObjPtr type{ PyObject_Call(strEnumType.get(), args.get(), kwargs.get()) };
				return type;
#endif
			}
		}

		EnumDefinitionBase::EnumDefinitionBase(const char* name) :
			name_(name),
			doc_(nullptr)
		{
		}
		EnumDefinitionBase::EnumDefinitionBase(const char* name, const char* doc) :
			name_(name),
			doc_(doc)
		{
		}

		EnumDefinitionBase::~EnumDefinitionBase()
		{
		}

		const char* EnumDefinitionBase::name() const
		{
			return name_;
		}

		const char* EnumDefinitionBase::doc() const
		{
			return doc_;
		}

		PyObject* EnumDefinitionBase::type() const
		{
			return type_.get();
		}

		TPyObjPtr EnumDefinitionBase::valueObject(PyObject* obj) const
		{
			return doValueObject(obj);
		}

		TPyObjPtr EnumDefinitionBase::doValueObject(PyObject* obj) const
		{
			if (PyObject_TypeCheck(obj, (PyTypeObject*) type()))
			{
				return TPyObjPtr(PyObject_GetAttrString(obj, "value"));
			}

			// try to convert it to an enum first ...
			TPyObjPtr o(PyObject_CallFunctionObjArgs(type(), obj, nullptr));
			if (!o)
			{
				return TPyObjPtr();
			}
			return TPyObjPtr(PyObject_GetAttrString(o.get(), "value"));
		}

		PyObject* EnumDefinitionBase::freezeDefinition(const char* moduleName, const char* scopeName)
		{
			if (!type_)
			{
				TPyObjPtr kwargs(PyDict_New());
				if (!kwargs)
				{
					return nullptr;
				}
				if (moduleName)
				{
					TPyObjPtr moduleNameObj(pyBuildSimpleObject(moduleName));
					if (!moduleNameObj || PyDict_SetItemString(kwargs.get(), "module", moduleNameObj.get()) != 0)
					{
						return nullptr;
					}
				}
				if (scopeName)
				{
					std::string qualName = stde::safe_format("%s.%s", scopeName, name_);
					TPyObjPtr qualNameObj(pyBuildSimpleObject(std::move(qualName)));
					if (!qualNameObj || PyDict_SetItemString(kwargs.get(), "qualname", qualNameObj.get()) != 0)
					{
						return nullptr;
					}
				}
				type_ = doFreezeDefinition(std::move(kwargs));
				if (!type_)
				{	
					return nullptr;
				}
			}
			if (doc_)
			{
				// set the docstring
				TPyObjPtr docStr(pyBuildSimpleObject(doc_));
				if (!docStr || PyObject_SetAttrString(type_.get(), "__doc__", docStr.get()) != 0)
				{
					return nullptr;
				}
			}
			return type_.get();
		}
	}
}
