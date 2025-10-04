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

#pragma once

#include "python_common.h"
#include "pyobject_ptr.h"
#include "py_tuple.h"
#include "../num/num_traits.h"
#include "../stde/vector_map.h"

#include <unordered_map>
#include <optional>

namespace lass
{
	namespace python
	{
		/** @defgroup EnumDefinition Enum Definitions
		 *  @brief Defining first-class enum types in Python from C++ enums.
		 *
		 *  This module provides helper macros and classes to define Python enum types that derive
		 *  from `enum.Enum`, `enum.IntEnum`, `enum.StrEnum` or `enum.IntFlag`.
		 * 
		 *
		 *  ### Python Enum Types
		 * 
		 *  The following Python enum types can be created:
		 * 
		 *  - `enum.IntEnum`: Values are integers, allows direct int conversion and comparison (recommended for most C++ enums)
		 *  - `enum.StrEnum`: Values are strings, allows direct string operations 
		 *  - `enum.IntFlag`: Integer flags supporting bitwise operations (`|`, `&`, `^`, `~`)
		 *  - `enum.Enum`: Generic enums with custom value types (advanced use)
		 *
		 *
		 *  ### Usage Overview
		 * 
		 *  To export a C++ enum to Python, you need **3 macros**:
		 *  
		 *  1. One `PY_SHADOW_*` macro in the header file to specialize `PyExportTraits`
		 *  2. One `PY_DECLARE_*` macro in the source file to define the enum mapping
		 *  3. One PY_MODULE_ENUM() or PY_CLASS_ENUM() macro to add the enum to a module or class
		 * 
		 *  Once defined, the enum can be used as parameters or return types in function signatures,
		 *  as class members, etc. The header defining `PY_SHADOW_*` must be included in source
		 *  files that export these functions to Python, or add the enum to a module or class.
		 * 
		 *  #### For `enum.IntEnum` (most common case):
		 *  ```cpp
		 *  // In header:
		 *  PY_SHADOW_INT_ENUM(LASS_DLL_EXPORT, MyEnum);
		 * 
		 *  // In source:
		 *  PY_DECLARE_INT_ENUM_EX(MyEnum)("MyEnum", "MyEnum documentation", {
		 *      { "VALUE1", MyEnum::Value1 },
		 *      { "VALUE2", MyEnum::Value2 },
		 *  });
		 * 
		 *  // Add to module or class:
		 *  PY_MODULE_ENUM(mymodule, MyEnum);   // or PY_CLASS_ENUM(MyClass, MyEnum);
		 *  ```
		 * 
		 *  #### For `enum.StrEnum` (string-based enums):
		 *  ```cpp
		 *  // In header:
		 *  PY_SHADOW_STR_ENUM(LASS_DLL_EXPORT, Shape);
		 * 
		 *  // In source:
		 *  PY_DECLARE_STR_ENUM_EX(Shape)("Shape", "Shape documentation", {
		 *      { "CIRCLE", Shape::Circle, "circle" },
		 *      { "SQUARE", Shape::Square, "square" },
		 *  });
		 * 
		 *  // Add to module or class:
		 *  PY_MODULE_ENUM(mymodule, Shape);   // or PY_CLASS_ENUM(MyClass, Shape);
		 *  ```
		 * 
		 *  #### For `enum.IntFlag` (bitfield enums):
		 *  ```cpp
		 *  // In header:
		 *  PY_SHADOW_INT_FLAG(LASS_DLL_EXPORT, Breakfast);
		 * 
		 *  // In source:
		 *  PY_DECLARE_INT_FLAG_EX(Breakfast)("Breakfast", "Breakfast documentation", FlagBoundary::Keep, {
		 *      { "EGG", Breakfast::Egg },
		 *      { "BACON", Breakfast::Bacon },
		 *      { "ALL", Breakfast::All },
		 *  });
		 * 
		 *  // Add to module or class:
		 *  PY_MODULE_ENUM(mymodule, Breakfast);   // or PY_CLASS_ENUM(MyClass, Breakfast);
		 *  ```
		 * 
		 *  #### For `enum.Enum` (generic enums):
		 *  ```cpp
		 *  // In header:
		 *  using WeekdayPair = std::pair<int, std::string>;
		 *  PY_SHADOW_ENUM(LASS_DLL_EXPORT, Weekday, WeekdayPair)
		 * 
		 *  // In source:
		 *  PY_DECLARE_ENUM_EX(Weekday, WeekdayPair)("Weekday", "Weekday documentation", {
		 *      { "MONDAY", Weekday::Monday, std::make_pair(Weekday::Monday, "Monday") },
		 *      { "TUESDAY", Weekday::Tuesday, std::make_pair(Weekday::Tuesday, "Tuesday") },
		 *  });
		 * 
		 *  // Add to module or class:
		 *  PY_MODULE_ENUM(mymodule, Weekday);   // or PY_CLASS_ENUM(MyClass, Weekday);
		 *  ```
		 *  @ingroup Python
		 */

		/** Defines the boundary behavior for `enum.IntFlag`-derived enums.
		 *
		 *  Controls how Python handles flag values that are outside the defined enumerators
		 * 
		 *  @note This feature requires Python 3.11 or later. On earlier versions, only `Keep` is supported.
		 *
		 *  @sa IntFlagDefinition
		 *  @sa PY_DECLARE_INT_FLAG_EX
		 *  @ingroup EnumDefinition
		 */
		enum class FlagBoundary
		{
			Keep,   ///< Allow any integer value (most permissive)
			Strict, ///< Only allow exact flag combinations
			Conform, ///< Mask to defined bits only
		};

		namespace impl
		{
			/** Creates a basic `enum.Enum` type.
			 *  @param name Python class name for the enum
			 *  @param enumerators Tuple of (name, value) pairs
			 *  @param kwargs Additional keyword arguments for enum constructor
			 *  @return New enum type object
			 *  @ingroup EnumDefinition
			 */
			LASS_PYTHON_DLL TPyObjPtr makeEnumType(const char* name, TPyObjPtr&& enumerators, TPyObjPtr&& kwargs);

			/** Creates an `enum.IntEnum` type.
			 *  @param name Python class name for the enum
			 *  @param enumerators Tuple of (name, value) pairs  
			 *  @param kwargs Additional keyword arguments for enum constructor
			 *  @return New `enum.IntEnum` type object
			 *  @ingroup EnumDefinition
			 */
			LASS_PYTHON_DLL TPyObjPtr makeIntEnumType(const char* name, TPyObjPtr&& enumerators, TPyObjPtr&& kwargs);

			/** Creates an `enum.IntFlag` type.
			 *  @param name Python class name for the flag enum
			 *  @param enumerators Tuple of (name, value) pairs
			 *  @param kwargs Additional keyword arguments for enum constructor  
			 *  @param boundary Boundary behavior for invalid flag combinations
			 *  @return New `enum.IntFlag` type object
			 *  @ingroup EnumDefinition
			 */
			LASS_PYTHON_DLL TPyObjPtr makeIntFlagType(const char* name, TPyObjPtr&& enumerators, TPyObjPtr&& kwargs, FlagBoundary boundary=FlagBoundary::Keep);

			/** Creates an `enum.StrEnum` type (or equivalent for Python < 3.11).
			 *  @param name Python class name for the string enum
			 *  @param enumerators Tuple of (name, value) pairs where values are strings
			 *  @param kwargs Additional keyword arguments for enum constructor
			 *  @return New `enum.StrEnum` type object  
			 *  @ingroup EnumDefinition
			 */
			LASS_PYTHON_DLL TPyObjPtr makeStrEnumType(const char* name, TPyObjPtr&& enumerators, TPyObjPtr&& kwargs);
		}

		/** Base class of all enum definitions.
		 *
		 *  If you define your own custom enum definition, you must derive from this class so that
		 *  you can add your enum to a module or class with `PY_MODULE_ENUM()` or `PY_CLASS_ENUM()`.
		 *
		 *  This class is typically not used directly by user code. Use the provided
		 *  IntEnumDefinition, StrEnumDefinition, IntFlagDefinition, or EnumDefinition
		 *  template classes instead.
		 *
		 *  @ingroup EnumDefinition
		 */
		class LASS_PYTHON_DLL EnumDefinitionBase
		{
		public:
			virtual ~EnumDefinitionBase();

			const char* name() const; ///< The name of the Python enum type
			const char* doc() const; ///< Optional docstring for the Python enum type
			PyObject* type() const; ///< The Python enum type object, after freezeDefinition() has been called

			/** Freeze the enum definition and create the Python enum type.
			 *
			 *  @note This method should _not_ be called directly. Instead, use `PY_MODULE_ENUM()` or 
			 *        `PY_CLASS_ENUM()` to add the enum to a module or class, which will call this method
			 *        at the appropriate time.
			 */
			PyObject* freezeDefinition(const char* moduleName, const char* scopeName);

		protected:
			/** Construct enum definition with name only.
			 *  @param name Python class name for the enum type (must have static storage duration)
			 */
			EnumDefinitionBase(const char* name);
			
			/** Construct enum definition with name and documentation.
			 *  @param name Python class name for the enum type (must have static storage duration)
			 *  @param doc Optional docstring for the Python enum type (must have static storage duration, or nullptr)
			 */
			EnumDefinitionBase(const char* name, const char* doc);

			/** Returns the value of an enum instances as a Python object
			 * 
			 *  If the object is not an instance of this enum type, a Python exception will be set
			 *  and a null pointer will be returned.
			 */
			TPyObjPtr valueObject(PyObject* obj) const;

		private:
			/** Creates the actual Python enum type object.
			 *  Derived classes implement this to create the appropriate enum type (IntEnum, StrEnum, etc.).
			 */
			virtual TPyObjPtr doFreezeDefinition(TPyObjPtr&& kwargs) = 0;
			
			/** Extracts the value from a Python object for type conversion.
			 *  Derived classes can override this to customize value extraction logic.
			 */
			virtual TPyObjPtr doValueObject(PyObject* obj) const;

			TPyObjPtr type_;
			const char* name_;
			const char* doc_;
		};

		/** Definition of an `enum.IntEnum`-derived enum type in Python.
		 * 
		 *  C++ enums exported using IntEnumDefinition will generate a new
		 *  enum type derived from `enum.IntEnum`. The values of the Python enum
		 *  will map directly on the C++ enum values, but then names need to
		 *  be defined at export.
		 *
		 *  As `enum.IntEnum` is used, the Python enum instances will also be
		 *  valid int instances, and int instances are accepted when converting
		 *  from Python to C++.
		 *
		 *  Because this defines a new type, it must be added to a module or
		 *  other class.
		 *
		 *  This class is typically not used directly. Use PY_SHADOW_INT_ENUM
		 *  and PY_DECLARE_INT_ENUM_* macros instead.
		 *
		 *  @par Usage Example:
		 *  ```cpp
		 *  // In header:
		 *  PY_SHADOW_INT_ENUM(LASS_DLL_EXPORT, MyEnum);
		 * 
		 *  // In source:
		 *  PY_DECLARE_INT_ENUM_EX(MyEnum)("MyEnum", "MyEnum documentation", {
		 *      { "VALUE1", MyEnum::Value1 },
		 *      { "VALUE2", MyEnum::Value2 },
		 *  });
		 * 
		 *  // Add to module or class:
		 *  PY_MODULE_ENUM(mymodule, MyEnum);   // or PY_CLASS_ENUM(MyClass, MyEnum);
		 *  ```
		 *
		 *  @sa PY_SHADOW_INT_ENUM
		 *  @sa PY_DECLARE_INT_ENUM_NAME
		 *  @sa PY_DECLARE_INT_ENUM_NAME_DOC
		 *  @sa PY_DECLARE_INT_ENUM_EX
		 *  @sa PY_MODULE_ENUM
		 *  @sa PY_CLASS_ENUM
		 *  @ingroup EnumDefinition
		 */
		template <typename EnumType>
		class IntEnumDefinition : public EnumDefinitionBase
		{
		public:
			using TEnum = EnumType;
			using TBase = typename std::underlying_type<EnumType>::type;

			/** Represents a single enumerator member with name and C++ enum value. */
			struct Enumerator
			{
				std::string name;  ///< Python name for the member
				TEnum enumerator;  ///< Corresponding C++ enum value
			};

			/** Construct with enum name only.
			 *  @param name Python class name for the enum type (must have static storage duration)
			 */
			IntEnumDefinition(const char* name) :
				EnumDefinitionBase(name)
			{
			}
			
			/** Construct with name and documentation.
			 *  @param name Python class name for the enum type (must have static storage duration)
			 *  @param doc Optional docstring for the Python enum type (must have static storage duration, or nullptr)
			 */
			IntEnumDefinition(const char* name, const char* doc) :
				EnumDefinitionBase(name, doc)
			{
			}
			
			/** Construct with name and enumerators list.
			 *  @param name Python class name for the enum type (must have static storage duration)
			 *  @param enumerators List of name-value pairs defining the enum members
			 */
			IntEnumDefinition(const char* name, std::initializer_list<Enumerator> enumerators) :
				EnumDefinitionBase(name),
				enumerators_(enumerators)
			{
			}
			
			/** Construct with name, documentation, and enumerators list.
			 *  @param name Python class name for the enum type (must have static storage duration)
			 *  @param doc Optional docstring for the Python enum type (must have static storage duration, or nullptr)
			 *  @param enumerators List of name-value pairs defining the enum members
			 */
			IntEnumDefinition(const char* name, const char* doc, std::initializer_list<Enumerator> enumerators) :
				EnumDefinitionBase(name, doc),
				enumerators_(enumerators)
			{
			}

			/** Add an enumerator member to the definition.
			 *  @param name Python name for the enumerator member
			 *  @param value C++ enum value
			 */
			void addEnumerator(std::string name, TEnum value)
			{
				enumerators_.emplace_back(std::move(name), value);
			}

			/** Creates a Python enum instance from a C++ enum value.
			 *  Converts the C++ enum to its corresponding Python enum instance.
			 */
			PyObject* build(TEnum value) const
			{
				LockGIL LASS_UNUSED(lock);
				TPyObjPtr args = makeTuple(static_cast<TBase>(value));
				return PyObject_Call(type(), args.get(), nullptr);
			}

			/** Extracts a C++ enum value from a Python object.
			 *  Accepts both enum instances and raw integers. Returns 0 on success, 1 on failure.
			 */
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
					// this can only happen for IntFlag with FlagBoundary::KEEP
					return 1;
				}
				value = static_cast<TEnum>(v);
				return 0;
			}

		protected:
			/** Creates an `enum.IntEnum`-derived type.
			 *  Converts enumerators to Python tuple format and creates the integer enum type.
			 */
			TPyObjPtr doFreezeDefinition(TPyObjPtr&& kwargs) override
			{
				const Py_ssize_t n = static_cast<Py_ssize_t>(enumerators_.size());
				TPyObjPtr pyEnumerators(PyTuple_New(n));
				if (!pyEnumerators)
				{
					return TPyObjPtr();
				}
				for (Py_ssize_t i = 0; i < n; ++i)
				{
					const Enumerator& enumerator = enumerators_[static_cast<size_t>(i)];
					TPyObjPtr pyEnumerator(makeTuple(enumerator.name, static_cast<TBase>(enumerator.enumerator)));
					if (!pyEnumerator || PyTuple_SetItem(pyEnumerators.get(), i, fromSharedPtrToNakedCast(pyEnumerator)) != 0)
					{
						return TPyObjPtr();
					}
				}

				return this->doMakeEnumType(std::move(pyEnumerators), std::move(kwargs));
			}

			/** Extracts integer values from enum instances or integer objects.
			 *  Handles both integer enum instances and raw Python integers.
			 */
			TPyObjPtr doValueObject(PyObject* obj) const override
			{
				PyObject* type = this->type();
				if (PyObject_TypeCheck(obj, (PyTypeObject*) type))
				{
					return TPyObjPtr(PyObject_GetAttrString(obj, "value"));
				}

				// try to convert it to an enum first ...
				if (!PyLong_Check(obj))
				{
					PyErr_Format(PyExc_TypeError, "Expected %S or int, got %S", type, obj);
					return TPyObjPtr();
				}
				TPyObjPtr o(PyObject_CallFunctionObjArgs(type, obj, nullptr));
				if (!o)
				{
					return TPyObjPtr();
				}
				return TPyObjPtr(PyObject_GetAttrString(o.get(), "value"));
			}

			/** Creates the actual `enum.IntEnum` type object.
			 *  Can be overridden by derived classes to create different enum types (e.g., IntFlag).
			 */
			virtual TPyObjPtr doMakeEnumType(TPyObjPtr&& enumerators, TPyObjPtr&& kwargs)
			{
				return impl::makeIntEnumType(this->name(), std::move(enumerators), std::move(kwargs));
			}

		private:
			std::vector<Enumerator> enumerators_;
		};

		/** Definition of an `enum.IntFlag`-derived enum type in Python.
		 * 
		 *  C++ enums exported using IntFlagDefinition will generate a new
		 *  enum type derived from `enum.IntFlag`. The values of the Python enum
		 *  will map directly on the C++ enum values, but then names need to
		 *  be defined at export.
		 * 
		 *  As `enum.IntFlag` is used, the Python enum instances will also be
		 *  valid int instances, and int instances are accepted when converting
		 *  from Python to C++. Additionally, bitwise operations are supported.
		 * 
		 *  Because this defines a new type, it must be added to a module or
		 *  other class.
		 * 
		 *  This class is typically not used directly. Use PY_SHADOW_INT_FLAG
		 *  and PY_DECLARE_INT_FLAG_* macros instead.
		 * 
		 *  @par Usage Example:
		 *  ```cpp
		 *  // In header:
		 *  PY_SHADOW_INT_FLAG(LASS_DLL_EXPORT, Breakfast);
		 * 
		 *  // In source:
		 *  PY_DECLARE_INT_FLAG_EX(Breakfast)("Breakfast", "Breakfast documentation", FlagBoundary::Keep, {
		 *      { "EGG", Breakfast::Egg },
		 *      { "BACON", Breakfast::Bacon },
		 *      { "ALL", Breakfast::All },
		 *  });
		 * 
		 *  // Add to module or class:
		 *  PY_MODULE_ENUM(mymodule, Breakfast);   // or PY_CLASS_ENUM(MyClass, Breakfast);
		 *  ```
		 * 
		 *  @sa PY_SHADOW_INT_FLAG
		 *  @sa PY_DECLARE_INT_FLAG_NAME
		 *  @sa PY_DECLARE_INT_FLAG_NAME_DOC
		 *  @sa PY_DECLARE_INT_FLAG_EX
		 *  @sa PY_MODULE_ENUM
		 *  @sa PY_CLASS_ENUM
		 *  @sa FlagBoundary
		 *  @ingroup EnumDefinition
		 */
		template <typename EnumType>
		class IntFlagDefinition : public IntEnumDefinition<EnumType>
		{
		public:
			using TEnum = typename IntFlagDefinition<EnumType>::TEnum;
			using TBase = typename IntFlagDefinition<EnumType>::TBase;
			using Enumerator = typename IntEnumDefinition<EnumType>::Enumerator;

			/// Inherits all constructors from IntEnumDefinition
			using IntEnumDefinition<EnumType>::IntEnumDefinition;

			/** Construct with name, documentation, boundary behavior, and enumerators.
			 *  @param name Python class name for the flag enum type (must have static storage duration)
			 *  @param doc Optional docstring for the Python enum type (must have static storage duration, or nullptr)
			 *  @param boundary Flag boundary behavior controlling how invalid flag combinations are handled
			 *  @param enumerators List of name-value pairs defining the flag members
			 *  
			 *  @note The boundary parameter requires Python 3.11 or later. On earlier versions, only `Keep` is supported
			 *        (which is the default if this constructor is not used).
			 */
			IntFlagDefinition(const char* name, const char* doc, FlagBoundary boundary, std::initializer_list<Enumerator> enumerators) :
				IntEnumDefinition<EnumType>(name, doc, enumerators),
				boundary_(boundary)
			{
			}

		protected:
			/** Creates an `enum.IntFlag`-derived type with boundary behavior.
			 *  Overrides the base class to create a flag enum with bitwise operations support.
			 */
			TPyObjPtr doMakeEnumType(TPyObjPtr&& enumerators, TPyObjPtr&& kwargs) override
			{
				return impl::makeIntFlagType(this->name(), std::move(enumerators), std::move(kwargs), boundary_);
			}

		private:
			FlagBoundary boundary_ = FlagBoundary::Keep;
		};

		/** Definition of a general `enum.Enum`-derived enum type in Python.
		 * 
		 *  This creates enums with custom value types (not just integers or strings).
		 *  This is the base class for StrEnumDefinition and can be used directly
		 *  for advanced enum types with custom value types.
		 * 
		 *  This class is typically not used directly. Use PY_SHADOW_ENUM
		 *  and PY_DECLARE_ENUM_* macros instead.
		 * 
		 *  @par Usage Example:
		 *  ```cpp
		 *  // In header:
		 *  PY_SHADOW_ENUM(LASS_DLL_EXPORT, MyEnum, std::string);
		 * 
		 *  // In source:
		 *  PY_DECLARE_ENUM_EX(MyEnum, std::string)("MyEnum", "MyEnum documentation", {
		 *      { "VALUE1", MyEnum::Value1, "custom_value_1" },
		 *      { "VALUE2", MyEnum::Value2, "custom_value_2" },
		 *  });
		 * 
		 *  // Add to module or class:
		 *  PY_MODULE_ENUM(mymodule, MyEnum);   // or PY_CLASS_ENUM(MyClass, MyEnum);
		 *  ```
		 * 
		 *  @sa PY_SHADOW_ENUM
		 *  @sa PY_DECLARE_ENUM_NAME
		 *  @sa PY_DECLARE_ENUM_NAME_DOC
		 *  @sa PY_DECLARE_ENUM_EX
		 *  @sa PY_MODULE_ENUM
		 *  @sa PY_CLASS_ENUM
		 *  @ingroup EnumDefinition
		 */
		template <typename EnumType, typename ValueType>
		class EnumDefinition : public EnumDefinitionBase
		{
		public:
			using TEnum = EnumType;
			using TValue = ValueType;

			/** Represents a single enumerator with name, C++ enum value, and Python value. */
			struct Enumerator
			{
				std::string name;  ///< Python name for the enumerator
				TEnum enumerator;  ///< Corresponding C++ enum value
				TValue value;      ///< Python value for the enumerator
			};

			/** Construct with enum name only.
			 *  @param name Python class name for the enum type (must have static storage duration)
			 */
			EnumDefinition(const char* name) :
				EnumDefinitionBase(name)
			{
			}
			
			/** Construct with name and documentation.
			 *  @param name Python class name for the enum type (must have static storage duration)
			 *  @param doc Optional docstring for the Python enum type (must have static storage duration, or nullptr)
			 */
			EnumDefinition(const char* name, const char* doc) :
				EnumDefinitionBase(name, doc)
			{
			}
			
			/** Construct with name and enumerators list.
			 *  @param name Python class name for the enum type (must have static storage duration)
			 *  @param enumerators List of name-enum-value triples defining the enum members
			 */
			EnumDefinition(const char* name, std::initializer_list<Enumerator> enumerators) :
				EnumDefinitionBase(name),
				enumerators_(enumerators)
			{
			}
			
			/** Construct with name, documentation, and enumerators list.
			 *  @param name Python class name for the enum type (must have static storage duration)
			 *  @param doc Optional docstring for the Python enum type (must have static storage duration, or nullptr)
			 *  @param enumerators List of name-enum-value triples defining the enum members
			 */
			EnumDefinition(const char* name, const char* doc, std::initializer_list<Enumerator> enumerators) :
				EnumDefinitionBase(name, doc),
				enumerators_(enumerators)
			{
			}

			/** Add an enumerator to the definition.
			 *  @param name Python name for the enumerator
			 *  @param enumerator C++ enum value
			 *  @param value Python value for the enumerator
			 */
			void addEnumerator(std::string name, TEnum enumerator, TValue value)
			{
				enumerators_.emplace_back(std::move(name), enumerator, std::move(value));
			}

			/** Creates a Python enum instance from a C++ enum value.
			 *  Looks up the Python value corresponding to the C++ enum and creates an enum instance.
			 */
			PyObject* build(TEnum value) const
			{
				LockGIL LASS_UNUSED(lock);
				auto val = this->getValue(value);
				if (!val)
				{
					PyErr_Format(PyExc_ValueError, "%lld is not a valid C++ value for %S", 
						static_cast<PY_LONG_LONG>(value), type());
					return nullptr;
				}
				TPyObjPtr args = makeTuple(*val);
				return PyObject_Call(type(), args.get(), nullptr);
			}

			/** Extracts a C++ enum value from a Python object.
			 *  Accepts enum instances and compatible value types. Returns 0 on success, 1 on failure.
			 */
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
				auto val = getEnum(v);
				if (!val)
				{
					// this should never happen, by construction o _must_ be a valid enumerator
					PyErr_Format(PyExc_ValueError, "%S is not a valid value for %S", o.get(), type());
					return 1;
				}
				value = *val;
				return 0;
			}

			std::optional<TValue> getValue(TEnum enumerator) const
			{
				auto it = enumToValue_.find(enumerator);
				if (it == enumToValue_.end())
				{
					return std::nullopt;
				}
				return it->second;
			}

			std::optional<TEnum> getEnum(const TValue& value) const
			{
				auto it = valueToEnum_.find(value);
				if (it == valueToEnum_.end())
				{
					return std::nullopt;
				}
				return it->second;
			}

		protected:
			/** Converts stored enumerators to Python tuple format and builds lookup tables.
			 *  Creates bidirectional mappings between C++ enum values and Python values.
			 */
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

			/** Creates a generic `enum.Enum`-derived type.
			 *  Processes enumerators and creates a Python enum type with custom value types.
			 */
			TPyObjPtr doFreezeDefinition(TPyObjPtr &&kwargs) override
			{
				TPyObjPtr pyEnumerators = freezeEnumerators();
				if (!pyEnumerators)
				{
					return TPyObjPtr();
				}
				return impl::makeEnumType(this->name(), std::move(pyEnumerators), std::move(kwargs));
			}

		private:
			std::vector<Enumerator> enumerators_;
			stde::vector_map<TEnum, TValue> enumToValue_;
			stde::vector_map<TValue, TEnum> valueToEnum_;
		};

		/** Definition of an `enum.StrEnum`-derived enum type in Python.
		 * 
		 *  C++ enums exported using StrEnumDefinition will generate a new
		 *  enum type derived from `enum.StrEnum` (or `enum.Enum` + str for Python < 3.11).
		 *  The values of the Python enum will be strings, and how they map on the
		 *  C++ enum values needs to be defined at export.
		 * 
		 *  As `enum.StrEnum` is used, the Python enum instances will also be
		 *  valid str instances, and str instances are accepted when
		 *  converting from Python to C++.
		 * 
		 *  Because this defines a new type, it must be added to a module or
		 *  other class.
		 * 
		 *  This class is typically not used directly. Use PY_SHADOW_STR_ENUM
		 *  and PY_DECLARE_STR_ENUM_* macros instead.
		 * 
		 *  @par Usage Example:
		 *  ```cpp
		 *  // In header:
		 *  PY_SHADOW_STR_ENUM(LASS_DLL_EXPORT, Shape);
		 * 
		 *  // In source:
		 *  PY_DECLARE_STR_ENUM_EX(Shape)("Shape", "Shape documentation", {
		 *      { "CIRCLE", Shape::Circle, "circle" },
		 *      { "SQUARE", Shape::Square, "square" },
		 *  });
		 * 
		 *  // Add to module or class:
		 *  PY_MODULE_ENUM(mymodule, Shape);   // or PY_CLASS_ENUM(MyClass, Shape);
		 *  ```
		 * 
		 *  @sa PY_SHADOW_STR_ENUM
		 *  @sa PY_DECLARE_STR_ENUM_NAME
		 *  @sa PY_DECLARE_STR_ENUM_NAME_DOC
		 *  @sa PY_DECLARE_STR_ENUM_EX
		 *  @sa PY_MODULE_ENUM
		 *  @sa PY_CLASS_ENUM
		 *  @ingroup EnumDefinition
		 */
		template <typename EnumType, typename ValueType=std::string>
		class StrEnumDefinition : public EnumDefinition<EnumType, ValueType>
		{
		public:
			using EnumDefinition<EnumType, ValueType>::EnumDefinition;

		protected:
			/** Creates an `enum.StrEnum`-derived type.
			 *  Processes enumerators and creates a Python string enum type.
			 */
			TPyObjPtr doFreezeDefinition(TPyObjPtr&& kwargs) override
			{
				return impl::makeStrEnumType(this->name(), this->freezeEnumerators(), std::move(kwargs));
			}

			/** Extracts string values from enum instances or string objects.
			 *  Handles both string enum instances and raw Python strings.
			 */
			TPyObjPtr doValueObject(PyObject* obj) const override
			{
				PyObject* type = this->type();
				if (PyObject_TypeCheck(obj, (PyTypeObject*) type))
				{
					return TPyObjPtr(PyObject_GetAttrString(obj, "value"));
				}

				// try to convert it to an enum first ...

				if (!PyUnicode_Check(obj))
				{
					PyErr_Format(PyExc_TypeError, "Expected %S or str, got %S", type, obj);
					return TPyObjPtr();
				}
				TPyObjPtr o(PyObject_CallFunctionObjArgs(type, obj, nullptr));
				if (!o)
				{
					return TPyObjPtr();
				}
				return TPyObjPtr(PyObject_GetAttrString(o.get(), "value"));
			}
		};
	}
}



/** @addtogroup EnumDefinition
 *  @name Integer Enum Support
 *  Macros for exporting C++ integer enums as Python `enum.IntEnum` types.
 *  @{
 */

/** Specializes PyExportTraits for a C++ enum using `enum.IntEnum`.
 *  Use this in header files, then use PY_DECLARE_INT_ENUM_* in source files.
 *  @param dllInterface Export specification (e.g., LASS_DLL_EXPORT)
 *  @param t_cppEnum C++ enum type
 *  @sa lass::python::IntEnumDefinition
 *  @ingroup EnumDefinition
 */
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

/** Defines the enumDefinition with name only for `enum.IntEnum`.
 *  @param t_cppEnum C++ enum type
 *  @param s_name Python enum name (const char* string with static storage duration)
 *  @sa lass::python::IntEnumDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_INT_ENUM_NAME(t_cppEnum, s_name) \
	::lass::python::IntEnumDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition(s_name);

/** Defines the enumDefinition with name and docstring for `enum.IntEnum`.
 *  @param t_cppEnum C++ enum type
 *  @param s_name Python enum name (const char* string with static storage duration)
 *  @param s_doc Documentation string (const char* string with static storage duration, or nullptr)
 *  @sa lass::python::IntEnumDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_INT_ENUM_NAME_DOC(t_cppEnum, s_name, s_doc) \
	::lass::python::IntEnumDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition(s_name, s_doc);

/** Defines the enumDefinition for initialization with constructor arguments for `enum.IntEnum`.
 *  Use this with initializer list: PY_DECLARE_INT_ENUM_EX(MyEnum)("MyEnum", "docs", { ... });
 *  @param t_cppEnum C++ enum type
 *  @sa lass::python::IntEnumDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_INT_ENUM_EX(t_cppEnum) \
	::lass::python::IntEnumDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition

/** @} */



/** @addtogroup EnumDefinition
 *  @name Integer Flag Support
 *  Macros for exporting C++ flag enums as Python `enum.IntFlag` types with bitwise operations.
 *  @{
 */

/** Specializes PyExportTraits for a C++ enum using `enum.IntFlag`.
 *  Use this in header files for bitfield enums, then use PY_DECLARE_INT_FLAG_* in source files.
 *  @param dllInterface Export specification (e.g., LASS_DLL_EXPORT)
 *  @param t_cppEnum C++ enum type (should be a bitfield enum)
 *  @sa lass::python::IntFlagDefinition
 *  @sa lass::python::FlagBoundary
 *  @ingroup EnumDefinition
 */
#define PY_SHADOW_INT_FLAG(dllInterface, t_cppEnum)\
	namespace lass \
	{ \
	namespace python \
	{ \
		template <> \
		struct PyExportTraits<t_cppEnum> \
		{ \
			using TEnum = t_cppEnum; \
			using TEnumDefinition = IntFlagDefinition<TEnum>; \
			static dllInterface TEnumDefinition enumDefinition; \
			static PyObject* build(TEnum value) { return enumDefinition.build(value); } \
			static int get(PyObject* obj, TEnum& value) { return enumDefinition.get(obj, value); } \
		}; \
	} \
	} \
	/**/

/** Defines the enumDefinition with name only for `enum.IntFlag`.
 *  @param t_cppEnum C++ enum type
 *  @param s_name Python enum name (const char* string with static storage duration)
 *  @sa lass::python::IntFlagDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_INT_FLAG_NAME(t_cppEnum, s_name) \
	::lass::python::IntFlagDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition(s_name);

/** Defines the enumDefinition with name and docstring for `enum.IntFlag`.
 *  @param t_cppEnum C++ enum type
 *  @param s_name Python enum name (const char* string with static storage duration)
 *  @param s_doc Documentation string (const char* string with static storage duration, or nullptr)
 *  @sa lass::python::IntFlagDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_INT_FLAG_NAME_DOC(t_cppEnum, s_name, s_doc) \
	::lass::python::IntFlagDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition(s_name, s_doc);

/** Defines the enumDefinition for initialization with constructor arguments for `enum.IntFlag`.
 *  Use with: PY_DECLARE_INT_FLAG_EX(MyEnum)("MyEnum", "docs", FlagBoundary::Keep, { ... });
 *  @param t_cppEnum C++ enum type
 *  @sa lass::python::IntFlagDefinition
 *  @sa lass::python::FlagBoundary
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_INT_FLAG_EX(t_cppEnum) \
	::lass::python::IntFlagDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition

/** @} */



/** @addtogroup EnumDefinition
 *  @name Generic Enum Support
 *  Macros for exporting C++ enums as Python `enum.Enum` types with custom value types.
 *  @{
 */

/** Specializes PyExportTraits for a C++ enum using generic `enum.Enum`.
 *  Use this in header files for enums with custom value types, then use PY_DECLARE_ENUM_* in source files.
 *  @param dllInterface Export specification (e.g., LASS_DLL_EXPORT)  
 *  @param t_cppEnum C++ enum type
 *  @param t_valueType Python value type for enum values
 *  @sa lass::python::EnumDefinition
 *  @ingroup EnumDefinition
 */
#define PY_SHADOW_ENUM(dllInterface, t_cppEnum, t_valueType)\
	namespace lass \
	{ \
	namespace python \
	{ \
		template <> \
		struct PyExportTraits<t_cppEnum> \
		{ \
			using TEnum = t_cppEnum; \
			using TEnumDefinition = EnumDefinition<TEnum, t_valueType>; \
			static dllInterface TEnumDefinition enumDefinition; \
			static PyObject* build(TEnum value) { return enumDefinition.build(value); } \
			static int get(PyObject* obj, TEnum& value) { return enumDefinition.get(obj, value); } \
		}; \
	} \
	} \
	/**/

/** Defines the enumDefinition with name only for generic `enum.Enum`.
 *  @param t_cppEnum C++ enum type
 *  @param t_valueType Python value type
 *  @param s_name Python enum name (const char* string with static storage duration)
 *  @sa lass::python::EnumDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_ENUM_NAME(t_cppEnum, t_valueType, s_name) \
	::lass::python::EnumDefinition<t_cppEnum, t_valueType> lass::python::PyExportTraits<t_cppEnum>::enumDefinition(s_name);

/** Defines the enumDefinition with name and docstring for generic `enum.Enum`.
 *  @param t_cppEnum C++ enum type
 *  @param t_valueType Python value type
 *  @param s_name Python enum name (const char* string with static storage duration)
 *  @param s_doc Documentation string (const char* string with static storage duration, or nullptr)
 *  @sa lass::python::EnumDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_ENUM_NAME_DOC(t_cppEnum, t_valueType, s_name, s_doc) \
	::lass::python::EnumDefinition<t_cppEnum, t_valueType> lass::python::PyExportTraits<t_cppEnum>::enumDefinition(s_name, s_doc);

/** Defines the enumDefinition for initialization with constructor arguments for generic `enum.Enum`.
 *  Use with: PY_DECLARE_ENUM_EX(MyEnum, std::string)("MyEnum", "docs", { ... });
 *  @param t_cppEnum C++ enum type
 *  @param t_valueType Python value type
 *  @sa lass::python::EnumDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_ENUM_EX(t_cppEnum, t_valueType) \
	::lass::python::EnumDefinition<t_cppEnum, t_valueType> lass::python::PyExportTraits<t_cppEnum>::enumDefinition

/** @} */



/** @addtogroup EnumDefinition
 *  @name String Enum Support
 *  Macros for exporting C++ enums as Python `enum.StrEnum` types with string values.
 *  @{
 */

/** Specializes PyExportTraits for a C++ enum using `enum.StrEnum`.
 *  Use this in header files for string-based enums, then use PY_DECLARE_STR_ENUM_* in source files.
 *  @param dllInterface Export specification (e.g., LASS_DLL_EXPORT)
 *  @param t_cppEnum C++ enum type
 *  @sa lass::python::StrEnumDefinition
 *  @ingroup EnumDefinition
 */
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
	}

/** Defines the enumDefinition with name only for `enum.StrEnum`.
 *  @param t_cppEnum C++ enum type
 *  @param s_name Python enum name (const char* string with static storage duration)
 *  @sa lass::python::StrEnumDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_STR_ENUM_NAME(t_cppEnum, s_name) \
	::lass::python::StrEnumDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition(s_name);

/** Defines the enumDefinition with name and docstring for `enum.StrEnum`.
 *  @param t_cppEnum C++ enum type
 *  @param s_name Python enum name (const char* string with static storage duration)
 *  @param s_doc Documentation string (const char* string with static storage duration, or nullptr)
 *  @sa lass::python::StrEnumDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_STR_ENUM_NAME_DOC(t_cppEnum, s_name, s_doc) \
	::lass::python::StrEnumDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition(s_name, s_doc);

/** Defines the enumDefinition for initialization with constructor arguments for `enum.StrEnum`.
 *  Use with: PY_DECLARE_STR_ENUM_EX(MyEnum)("MyEnum", "docs", { ... });
 *  @param t_cppEnum C++ enum type
 *  @sa lass::python::StrEnumDefinition
 *  @ingroup EnumDefinition
 */
#define PY_DECLARE_STR_ENUM_EX(t_cppEnum) \
	::lass::python::StrEnumDefinition<t_cppEnum> lass::python::PyExportTraits<t_cppEnum>::enumDefinition

/** @} */



/** @addtogroup EnumDefinition
 *  @name Common Enum Utilities
 *  Utility macros for adding enum definitions to modules and classes.
 *  @{
 */

/** Adds an enum definition to a Python module.
 *  Use this after declaring the enum with PY_DECLARE_*_ENUM_* macros.
 *  The enum will be accessible as module.EnumName in Python.
 *  @param i_module Module variable identifier (must be unscoped identifier for token concatenation)
 *  @param t_cppEnum C++ enum type
 * 
 *  @par Usage Example:
 *  ```cpp
 *  // In header:
 *  PY_SHADOW_INT_ENUM(LASS_DLL_EXPORT, MyEnum);
 * 
 *  // In source:
 *  PY_DECLARE_MODULE_DOC(mymodule, "MyModule documentation");
 *  PY_MODULE_ENUM(mymodule, MyEnum);
 *  ```
 * 
 *  @note this must be called in the same translation unit (source file) that declares the module
 *        with `PY_DECLARE_MODULE_*` macros.
 *
 *  @ingroup EnumDefinition
 */
#define PY_MODULE_ENUM( i_module, t_cppEnum ) \
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleEnum_, i_module ),\
		i_module.addEnum( &::lass::python::PyExportTraits<t_cppEnum>::enumDefinition ); \
	)

/** Adds an enum definition as a nested enum to a Python class.
 *  Use this after declaring the enum with PY_DECLARE_*_ENUM_* macros.
 *  The enum will be accessible as ClassName.EnumName in Python.
 *  @param i_cppClass C++ class identifier (must be unscoped identifier for token concatenation)
 *  @param t_cppEnum C++ enum type
 * 
 *  @par Usage Example:
 *  ```cpp
 *  // In header:
 *  PY_SHADOW_INT_ENUM(LASS_DLL_EXPORT, MyEnum);
 * 
 *  // In source:
 *  PY_DECLARE_CLASS_NAME(MyClass, "MyClass documentation");
 *  PY_CLASS_ENUM(MyClass, MyEnum);
 *  ```
 
 *  @note this must be called in the same translation unit (source file) that declares the class
 *        with `PY_DECLARE_CLASS_*` macros.
 *
 *  @ingroup EnumDefinition
 */
#define PY_CLASS_ENUM( i_cppClass, t_cppEnum ) \
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyClassEnum_, i_cppClass ),\
		i_cppClass ::_lassPyClassDef.addInnerEnum( &::lass::python::PyExportTraits<t_cppEnum>::enumDefinition ); \
	)

/** @} */
