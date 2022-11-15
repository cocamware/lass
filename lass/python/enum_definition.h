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
 *	Copyright (C) 2022 the Initial Developer.
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

#pragma once

#include "python_common.h"
#include "pyobject_ptr.h"
#include "py_tuple.h"
#include "../num/num_traits.h"
#include "../stde/vector_map.h"

#include <unordered_map>

namespace lass
{
	namespace python
	{
		namespace impl
		{
			LASS_PYTHON_DLL TPyObjPtr makeEnumType(const std::string& name, TPyObjPtr&& enumerators, TPyObjPtr&& kwargs);
			LASS_PYTHON_DLL TPyObjPtr makeIntEnumType(const std::string& name, TPyObjPtr&& enumerators, TPyObjPtr&& kwargs);
			LASS_PYTHON_DLL TPyObjPtr makeStrEnumType(const std::string& name, TPyObjPtr&& enumerators, TPyObjPtr&& kwargs);
		}

		class LASS_PYTHON_DLL EnumDefinitionBase
		{
		public:
			const std::string& name() const;
			PyObject* type() const;

			void freezeDefinition(const char* moduleName, const char* qualName);

		protected:
			EnumDefinitionBase(std::string&& name);

			TPyObjPtr valueObject(PyObject* obj) const;

			virtual TPyObjPtr doFreezeDefinition(TPyObjPtr&& kwargs) = 0;

			TPyObjPtr type_;
			std::string name_;
		};

		/** @ingroup Python
		 * 
		 *  C++ enums exported using IntEnumDefinition will generate a new
		 *  enum type derived from enum.IntEnum. The values of the Python enum
		 *  will map directly on the C++ enum values, but then names need to
		 *  be defined at export.
		 * 
		 *  As enum.IntEnum is used, the Python enum instances will also be
		 *  valid int instances, and int instances are accepted when converting
		 *  from Python to C++.
		 * 
		 *  Because this defines a new type, it must be added to a module or
		 *  other class.
		 */
		template <typename EnumType>
		class IntEnumDefinition : public EnumDefinitionBase
		{
		public:
			using TEnum = EnumType;
			using TBase = typename std::underlying_type<EnumType>::type;

			struct Enumerator
			{
				std::string name;
				TEnum enumerator;
			};

			IntEnumDefinition(std::string&& name) :
				EnumDefinitionBase(std::move(name))
			{
			}
			IntEnumDefinition(std::string&& name, std::initializer_list<Enumerator> enumerators) :
				EnumDefinitionBase(std::move(name)),
				enumerators_(enumerators)
			{
			}

			void addEnumerator(std::string&& name, TEnum value)
			{
				enumerators_.emplace_back(std::move(name), value);
			}

			PyObject* build(TEnum value) const
			{
				LockGIL LASS_UNUSED(lock);
				TPyObjPtr args = makeTuple(static_cast<TBase>(value));
				return PyObject_Call(type(), args.get(), nullptr);
			}

			int get(PyObject* obj, TEnum& value) const
			{
				LockGIL LASS_UNUSED(lock);
				const TPyObjPtr o = valueObject(obj);
				if (!o)
				{
					return 1;
				}
				TBase v;
				if (PyExportTraits<TBase>::get(o.get(), v) != 0)
				{
					// by construction this should never happen
					// o should always be the right type ...
					return 1;
				}
				value = static_cast<TEnum>(v);
				return 0;
			}

		protected:
			TPyObjPtr doFreezeDefinition(TPyObjPtr&& kwargs) override
			{
				const Py_ssize_t n = static_cast<Py_ssize_t>(enumerators_.size());
				TPyObjPtr pyEnumerators(PyTuple_New(n));
				for (Py_ssize_t i = 0; i < n; ++i)
				{
					const Enumerator& enumerator = enumerators_[static_cast<size_t>(i)];
					TPyObjPtr pyEnumerator(makeTuple(enumerator.name, static_cast<TBase>(enumerator.enumerator)));
					PyTuple_SetItem(pyEnumerators.get(), i, fromSharedPtrToNakedCast(pyEnumerator));
				}

				return impl::makeIntEnumType(name(), std::move(pyEnumerators), std::move(kwargs));
			}
		
		private:
			std::vector<Enumerator> enumerators_;
		};

		template <typename EnumType, typename ValueType>
		class EnumDefinition : public EnumDefinitionBase
		{
		public:
			using TEnum = EnumType;
			using TValue = ValueType;

			struct Enumerator
			{
				std::string name;
				TEnum enumerator;
				TValue value;
			};

			EnumDefinition(std::string&& name) :
				EnumDefinitionBase(std::move(name))
			{
			}
			EnumDefinition(std::string&& name, std::initializer_list<Enumerator> enumerators) :
				EnumDefinitionBase(std::move(name)),
				enumerators_(enumerators)
			{
			}

			void addEnumerator(std::string&& name, TEnum enumerator, TValue&& value)
			{
				enumerators_.emplace_back(std::move(name), enumerator, std::move(value));
			}

			PyObject* build(TEnum value) const
			{
				LockGIL LASS_UNUSED(lock);
				auto it = enumToValue_.find(value);
				if (it == enumToValue_.end())
				{
					PyErr_Format(PyExc_ValueError, "%lld is not a valid C++ value for %S", 
						static_cast<PY_LONG_LONG>(value), type());
					return nullptr;
				}
				TPyObjPtr args = makeTuple(it->second);
				return PyObject_Call(type(), args.get(), nullptr);
			}

			int get(PyObject* obj, TEnum& value) const
			{
				LockGIL LASS_UNUSED(lock);
				const TPyObjPtr o = valueObject(obj);
				if (!o)
				{
					return 1;
				}
				TValue v;
				if (PyExportTraits<TValue>::get(o.get(), v) != 0)
				{
					// by construction this should never happen
					// o should always be the right type ...
					return 1;
				}
				auto it = valueToEnum_.find(v);
				if (it == valueToEnum_.end())
				{
					// this should never happen, by construction o _must_ be a valid enumerator
					PyErr_Format(PyExc_ValueError, "%S is not a valid value for %S", o.get(), type());
					return 1;
				}
				value = it->second;
				return 0;
			}

		protected:
			TPyObjPtr freezeEnumerators()
			{
				const Py_ssize_t n = static_cast<Py_ssize_t>(enumerators_.size());
				TPyObjPtr pyEnumerators(PyTuple_New(n));
				for (Py_ssize_t i = 0; i < n; ++i)
				{
					const Enumerator& enumerator = enumerators_[static_cast<size_t>(i)];
					assert(!enumToValue_.contains(enumerator.enumerator));
					enumToValue_.emplace(enumerator.enumerator, enumerator.value);
					assert(!valueToEnum_.contains(enumerator.value));
					valueToEnum_.emplace(enumerator.value, enumerator.enumerator);
					TPyObjPtr pyEnumerator(makeTuple(enumerator.name, enumerator.value));
					PyTuple_SetItem(pyEnumerators.get(), i, fromSharedPtrToNakedCast(pyEnumerator));
				}
				return pyEnumerators;
			}

			TPyObjPtr doFreezeDefinition(TPyObjPtr &&kwargs) override
			{
				return impl::makeEnumType(this->name(), this->freezeEnumerators(), std::move(kwargs));
			}

		private:
			std::vector<Enumerator> enumerators_;
			stde::vector_map<TEnum, TValue> enumToValue_;
			stde::vector_map<TValue, TEnum> valueToEnum_;
		};

		/** @ingroup Python
		 * 
		 *  C++ enums exported using StrEnumDefinition will generate a new
		 *  enum type derived from enum.Enum *and* str. The values of the Python
		 *  enum will be strings, and how they map on the C++ enum values needs
		 *  to be defined at export.
		 * 
		 *  As the enum is also derived from str, the Python enum instances will
		 *  also be valid str instances, and str instances are accepted when
		 *  converting from Python to C++.
		 * 
		 *  Because this defines a new type, it must be added to a module or
		 *  other class.
		 */
		template <typename EnumType, typename ValueType=std::string>
		class StrEnumDefinition : public EnumDefinition<EnumType, ValueType>
		{
		public:
			using EnumDefinition<EnumType, ValueType>::EnumDefinition;

		protected:
			TPyObjPtr doFreezeDefinition(TPyObjPtr&& kwargs) override
			{
				return impl::makeStrEnumType(this->name(), this->freezeEnumerators(), std::move(kwargs));
			}
		};
	}
}



#define PY_SHADOW_INT_ENUM(dllInterface, t_cppEnum)\
	namespace lass \
	{ \
	namespace python \
	{ \
		template <> \
		struct PyExportTraits<t_cppEnum> \
		{ \
			using TEnum = t_cppEnum; \
			using TEnumDefinition = IntEnumDefinition<TEnum>; \
			static dllInterface TEnumDefinition enumDefinition; \
			static PyObject* build(TEnum value) { return enumDefinition.build(value); } \
			static int get(PyObject* obj, TEnum& value) { return enumDefinition.get(obj, value); } \
		}; \
	} \
	} \
	/**/

#define PY_DECLARE_INT_ENUM_NAME(t_cppEnum, s_name) \
	::lass::python::IntEnumDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition(s_name);

#define PY_DECLARE_INT_ENUM_EX(t_cppEnum) \
	::lass::python::IntEnumDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition


#define PY_SHADOW_STR_ENUM(dllInterface, t_cppEnum)\
	namespace lass \
	{ \
	namespace python \
	{ \
		template <> \
		struct PyExportTraits<t_cppEnum> \
		{ \
			using TEnum = t_cppEnum; \
			using TEnumDefinition = StrEnumDefinition<TEnum>; \
			static dllInterface TEnumDefinition enumDefinition; \
			static PyObject* build(TEnum value) { return enumDefinition.build(value); } \
			static int get(PyObject* obj, TEnum& value) { return enumDefinition.get(obj, value); } \
		}; \
	} \
	} \
	/**/

#define PY_DECLARE_STR_ENUM_NAME(t_cppEnum, s_name) \
	::lass::python::StrEnumDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition(s_name);

#define PY_DECLARE_STR_ENUM_EX(t_cppEnum) \
	::lass::python::StrEnumDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition


#define PY_MODULE_ENUM( i_module, t_cppEnum ) \
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleEnum_, i_module ),\
		i_module.addEnum( &::lass::python::PyExportTraits<t_cppEnum>::enumDefinition ); \
	)

#define PY_CLASS_ENUM( i_cppClass, t_cppEnum ) \
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyClassEnum_, i_cppClass ),\
		i_cppClass ::_lassPyClassDef.addInnerEnum( &::lass::python::PyExportTraits<t_cppEnum>::enumDefinition ); \
	)

