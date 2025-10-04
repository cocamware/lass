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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_CLASS_DEFINITION_H
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_CLASS_DEFINITION_H

#include "python_common.h"
#include "pyobject_special_methods.h"
#include "overload_link.h"
#include "export_traits.h"
#include "../util/shared_ptr.h"

namespace lass
{
	namespace python
	{
		/** @defgroup ClassDefinition Class Definitions
		 *  @brief Defining Python classes from C++ with methods, properties, operators, and nested types.
		 *
		 *  This module provides the internal machinery used by the class export macros to define
		 *  Python classes backed by C++ types. A class definition aggregates constructors, methods
		 *  (including Python special methods/operators), properties, static members, nested classes
		 *  and enums, and then materializes a Python type when frozen.
		 *
		 *
		 *  ### Class Components
		 *
		 *  A Python class can contain:
		 *
		 *  - **Constructors**: Multiple overloads via \_\_new__ dispatchers
		 *  - **Methods**: Regular instance methods with overload support
		 *  - **Free Methods**: Functions that operate on the instance but are not members
		 *  - **Static Methods**: Class-level methods accessible without instances
		 *  - **Operators**: Python special methods (\_\_add__, \_\_eq__, etc.)
		 *  - **Properties**: Getter/setter pairs for attribute access
		 *  - **Public Members**: Direct access to public member variables
		 *  - **Static Constants**: Class-level constant values
		 *  - **Nested Classes**: Inner classes defined within the outer class
		 *  - **Nested Enums**: Enum types scoped to the class
		 *
		 *
		 *  ### Usage Overview
		 *
		 *  To export a C++ class to Python, you typically use macros that work with ClassDefinition:
		 *
		 *  ```cpp
		 *  // Define class in header
		 *  class MyClass: public PyObjectPlus
		 *  {
		 *      PY_HEADER(PyObjectPlus)
		 *  public:
		 *      MyClass();
		 *      MyClass(int value);
		 *      MyClass(const std::string& name, double factor);
		 *      
		 *      void process();
		 *      void process(int iterations);                    // Overload #1 (member)
		 *      int calculate(int input) const;
		 *      std::string getName() const;
		 *      void setName(const std::string& name);
		 *      static int getGlobalCount();
		 *      std::string toString() const;                    // For __str__
		 *      
		 *      int publicValue;
		 *      static const int MAX_SIZE = 100;
		 *  };
		 *
		 *  // Free functions for export as methods
		 *  void reset(MyClass* obj);
		 *  void process(MyClass* obj, const std::string& mode); // Overload #2 (free function)
		 *  double computeScore(const MyClass& obj, double multiplier);
		 *  MyClass combine(const MyClass& a, const MyClass& b);
		 *  
		 *  // Special method implementations
		 *  bool operator==(const MyClass& a, const MyClass& b);
		 *  bool operator<(const MyClass& a, const MyClass& b);
		 *  MyClass operator+(const MyClass& a, const MyClass& b);
		 *  MyClass operator-(const MyClass& a, const MyClass& b);
		 *  std::string repr(const MyClass& obj);                // For __repr__
		 *  size_t hash(const MyClass& obj);                     // For __hash__
		 *
		 *  // Register class methods and properties in source
		 *  PY_DECLARE_CLASS_NAME_DOC(MyClass, "MyClass", "Demonstration class with various binding types")
		 *  
		 *  // Multiple constructor overloads
		 *  PY_CLASS_CONSTRUCTOR_0(MyClass)
		 *  PY_CLASS_CONSTRUCTOR_1(MyClass, int)
		 *  PY_CLASS_CONSTRUCTOR_2(MyClass, const std::string&, double)
		 *  
		 *  // Mixed overloading: regular methods + free functions for same Python method
		 *  PY_CLASS_METHOD(MyClass, process)                           // void process()
		 *  PY_CLASS_METHOD_1(MyClass, process, int)                    // void process(int)
		 *  PY_CLASS_FREE_METHOD_NAME(MyClass, process, "process")      // void process(MyClass*, const std::string&)
		 *  
		 *  // Other regular methods
		 *  PY_CLASS_METHOD_DOC(MyClass, calculate, "Calculate result from input value")
		 *  PY_CLASS_STATIC_METHOD(MyClass, getGlobalCount)
		 *  
		 *  // Properties (getter/setter pairs)
		 *  PY_CLASS_MEMBER_RW_DOC(MyClass, name, getName, setName, "Object name property")
		 *  
		 *  // Public member access
		 *  PY_CLASS_MEMBER_R(MyClass, publicValue)
		 *  
		 *  // Free functions as methods (various forms)
		 *  PY_CLASS_FREE_METHOD(MyClass, reset)
		 *  PY_CLASS_FREE_METHOD_NAME_DOC(MyClass, computeScore, "compute_score", "Calculate score with multiplier")
		 *  PY_CLASS_FREE_METHOD_DOC(MyClass, combine, "Combine two objects")
		 *  
		 *  // Python special methods via member and free functions
		 *  PY_CLASS_METHOD_NAME(MyClass, toString, methods::_str_)          // Member function for __str__
		 *  PY_CLASS_FREE_METHOD_NAME(MyClass, repr, methods::_repr_)       // Free function for __repr__
		 *  PY_CLASS_FREE_METHOD_NAME(MyClass, hash, methods::_hash_)       // Free function for __hash__
		 *  
		 *  // Comparison and arithmetic operators
		 *  PY_CLASS_FREE_METHOD_NAME(MyClass, operator==, methods::_eq_)
		 *  PY_CLASS_FREE_METHOD_NAME(MyClass, operator<, methods::_lt_)
		 *  PY_CLASS_FREE_METHOD_NAME(MyClass, operator+, methods::_add_)
		 *  PY_CLASS_FREE_METHOD_NAME(MyClass, operator-, methods::_sub_)
		 *  
		 *  // Static constants and values
		 *  PY_CLASS_STATIC_CONST(MyClass, "MAX_SIZE", MyClass::MAX_SIZE)
		 *  PY_CLASS_STATIC_CONST(MyClass, "VERSION", "1.2.3")
		 *
		 *  // Add to module
		 *  PY_MODULE_CLASS(mymodule, MyClass)
		 *  ```
		 *	
		 *  @ingroup Python
		 */
		class EnumDefinitionBase;

		namespace impl
		{
			LASS_PYTHON_DLL PyMethodDef LASS_CALL createPyMethodDef(
				const char *ml_name, PyCFunction ml_meth, int ml_flags, 
				const char *ml_doc);
			LASS_PYTHON_DLL PyGetSetDef LASS_CALL createPyGetSetDef(
				const char* name, getter get, setter set, const char* doc, void* closure);

			LASS_PYTHON_DLL void LASS_CALL dealloc(PyObject* obj);
			LASS_PYTHON_DLL PyObject* LASS_CALL repr(PyObject* obj);
			LASS_PYTHON_DLL PyObject* LASS_CALL str(PyObject* obj);

			class NamePredicate
			{
			public:
				NamePredicate(const char* name): name_(name) {}
				template <typename T> bool operator()(const T& x) const { return cmp(x.name()); }
				bool operator()(const PyMethodDef& x) const { return cmp(x.ml_name); }
				template <typename T> bool operator()(T* x) const { return (*this)(*x); }
			private:
				bool cmp(const char* name) const { return name && strcmp(name, name_) == 0; }
				const char* name_;
			};

			class StaticMemberHelper
			{
			public:
				virtual ~StaticMemberHelper() {}
				virtual TPyObjPtr build() const = 0;
			};
			typedef util::SharedPtr<StaticMemberHelper> TStaticMemberHelperPtr;

			template <typename T>
			class StaticMemberHelperObject: public StaticMemberHelper
			{
			public:
				StaticMemberHelperObject(const T& obj): obj_(obj) {}
				TPyObjPtr build() const override { return TPyObjPtr(PyExportTraits<const T>::build(obj_)); }
			private:
				const T obj_;
			};
			template <typename T, size_t N>
			class StaticMemberHelperObject<T[N]>: public StaticMemberHelper
			{
			public:
				StaticMemberHelperObject(const T obj[N]): obj_(obj) {}
				TPyObjPtr build() const override { return TPyObjPtr(PyExportTraits<const T*>::build(obj_)); }
			private:
				const T* obj_;
			};
			template <>
			class StaticMemberHelperObject<PyObject*>: public StaticMemberHelper
			{
			public:
				StaticMemberHelperObject(PyObject* obj): obj_(obj) {}
				TPyObjPtr build() const override { return obj_; }
			private:
				TPyObjPtr obj_;
			};
			template <typename T>
			inline TStaticMemberHelperPtr staticMemberHelperObject(const T& obj)
			{
				return TStaticMemberHelperPtr(new StaticMemberHelperObject<T>(obj));
			}

			class StaticMember
			{
			public:
				StaticMember(const char* name, const TStaticMemberHelperPtr member): member_(member), name_(name) {}
				const TStaticMemberHelperPtr& member() const { return member_; }
				const char* name() const { return name_; }
			private:
				TStaticMemberHelperPtr member_;
				const char* name_;
			};

			struct LASS_PYTHON_DLL CompareFunc
			{
				PyCFunction dispatcher;
				int op;
				CompareFunc(PyCFunction dispatcher, int op): dispatcher(dispatcher), op(op) {}
			};

			template <typename CppClass> PyObject* richCompareDispatcher(PyObject* self, PyObject* other, int op)
			{
				return CppClass::_lassPyClassDef.callRichCompare(self, other, op);
			}

			/** Definition of a Python class.
			 *
			 *  Collects all information required to define a Python type that wraps a C++ class.
			 *  The definition accumulates constructors, methods (including Python slot methods),
			 *  properties, static members, and nested types. When ready, call freezeDefinition()
			 *  to create the underlying `PyTypeObject` and (optionally) inject it into a module
			 *  or as a nested type.
			 *
			 *  The ClassDefinition class is typically not used directly by user code. Instead, use the
			 *  provided macros in pyobject_macros.h that work with ClassDefinition instances.
			 *
			 *  @ingroup ClassDefinition
			 */
			class LASS_PYTHON_DLL ClassDefinition
			{
			public:
				typedef void(*TClassRegisterHook)(); /// Function to call during registration of the class (optional).
				typedef int TSlotID; ///< Identifier type for indexing into the internal Python slot table.

				/** Construct a class definition.
				 *  @param name      Python class name
				 *  @param doc       Class docstring (must outlive the definition unless changed via setDoc())
				 *  @param typeSize  Size of the Python instance struct (derived from PyObject)
				 *  @param richcmp   Optional rich-compare function (may be nullptr)
				 *  @param parent    Optional parent to nest this class into (may be nullptr)
				 *  @param registerHook Optional hook called when the class is registered
				 */
				ClassDefinition(const char* name, const char* doc, Py_ssize_t typeSize,
					richcmpfunc richcmp, ClassDefinition* parent, TClassRegisterHook registerHook);
				/** Destructor. */
				~ClassDefinition();

				/** Get the Python type object  (available after freezeDefinition() has been called). */
				/// @{
				PyTypeObject* type();
				const PyTypeObject* type() const;
				/// @}

				/** Get the class name. */
				const char* name() const;
				/** Get the class docstring. */
				const char* doc() const;
				/** Set the class docstring.
				 *  Note: the provided string must remain valid until another docstring is set.
				 */
				void setDoc(const char* doc);
				/** Set the class docstring if non-null (keeps existing one if nullptr). */
				void setDocIfNotNull(const char* doc);

				/** Add a constructor overload (\_\_init__ dispatcher).
				 *  @param dispatcher   New-function dispatcher
				 *  @param overloadChain Reference to overload chain
				 */
				void addConstructor(newfunc dispatcher, newfunc& overloadChain);

				/** Add a named method.
				 *  @param name, slot Python method name or special slot
				 *  @param doc  Method docstring
				 *  @param dispatcher Dispatcher implementing the method
				 *  @param overloadChain Overload chain for this name
				 */
				/// @{
				void addMethod(const char* name, const char* doc, PyCFunction dispatcher, OverloadLink& overloadChain);
				/** Add a comparator-slot method (rich compare operator). */
				void addMethod(const ComparatorSlot& slot, const char* doc, PyCFunction dispatcher, OverloadLink& overloadChain);
				/** Add a unary-slot method (e.g., \_\_neg__, \_\_pos__, ...). */ 
				void addMethod(const UnarySlot& slot, const char* doc, unaryfunc dispatcher, OverloadLink& overloadChain); 
				/** Add a binary-slot method (e.g., arithmetic, comparisons). */ 
				void addMethod(const BinarySlot& slot, const char* doc, binaryfunc dispatcher, OverloadLink& overloadChain); 
				/** Add a ternary-slot method. */
				void addMethod(const TernarySlot& slot, const char* doc, ternaryfunc dispatcher, OverloadLink& overloadChain);
				/** Add a length-slot method (\_\_len__). */
				void addMethod(const LenSlot& slot, const char* doc, lenfunc dispatcher, OverloadLink& overloadChain);
				/** Add an ssize-arg-slot method. */
				void addMethod(const SsizeArgSlot& slot, const char* doc, ssizeargfunc dispatcher, OverloadLink& overloadChain);
				/** Add an ssize-obj-arg-slot method. */
				void addMethod(const SsizeObjArgSlot& slot, const char* doc, ssizeobjargproc dispatcher, OverloadLink& overloadChain);
				/** Add an obj-obj-slot method. */
				void addMethod(const ObjObjSlot& slot, const char* doc, objobjproc dispatcher, OverloadLink& overloadChain);
				/** Add an obj-obj-arg-slot method. */
				void addMethod(const ObjObjArgSlot& slot, const char* doc, objobjargproc dispatcher, OverloadLink& overloadChain);
				/** Add an iterator-slot method (\_\_iter__). */
				void addMethod(const IterSlot& slot, const char* doc, getiterfunc dispatcher, OverloadLink& overloadChain);
				/** Add an iternext-slot method (\_\_next__). */
				void addMethod(const IterNextSlot& slot, const char* doc, iternextfunc dispatcher, OverloadLink& overloadChain);
				/** Add an arg+kw-slot method (callable semantics). */
				void addMethod(const ArgKwSlot& slot, const char* doc, ternaryfunc dispatcher, OverloadLink& overloadChain);
				/** Add an inquiry-slot method. */
				void addMethod(const InquirySlot& slot, const char* doc, inquiry dispatcher, OverloadLink& overloadChain);
				/** Add a property with optional getter/setter. */
				void addGetSetter(const char* name, const char* doc, getter get, setter set);
				/** Add a static method with overload support. */
				void addStaticMethod(const char* name, const char* doc, PyCFunction dispatcher, PyCFunction& overloadChain);
				/// @}
			
				template <typename T> void addStaticConst(const char* name, const T& value)
				{
					LASS_ASSERT(std::count_if(statics_.begin(), statics_.end(), NamePredicate(name)) == 0);
					statics_.push_back(StaticMember(name, staticMemberHelperObject(value)));
				}

				/// @{
				/** Add a nested class definition (inner class). */
				void addInnerClass(ClassDefinition& innerClass);
				/** Add a nested enum definition (inner enum). */
				void addInnerEnum(EnumDefinitionBase* enumDefinition);
				/// @}

				/// @{
				/** Get raw pointer from a given slot id. */
				void* getSlot(TSlotID slotId);
				/** Set raw pointer for a given slot id. */
				void* setSlot(TSlotID slotId, void* value);
				template <typename Ptr> Ptr setSlot(TSlotID slotId, Ptr value)
				{
					return reinterpret_cast<Ptr>(setSlot(slotId, reinterpret_cast<void*>(value)));
				}
				/// @}

				/** Finalize the definition and create the Python type.
				 *  If a module is provided, the type is added to that module.
				 */
				PyObject* freezeDefinition(PyObject* module = nullptr);

				/** Dispatch rich-compare for this class (used by operator slots). */
				PyObject* callRichCompare(PyObject* self, PyObject* other, int op);

			private:

				friend LASS_PYTHON_DLL PyObject* LASS_CALL establishMagicalBackLinks(PyObject* result, PyObject* self);

				template <typename T> friend struct ShadowTraits; // to have acces to implicitConvertersSlot_, see below.

				typedef std::vector<PyMethodDef> TMethods;
				typedef std::vector<PyGetSetDef> TGetSetters;
				typedef std::vector<CompareFunc> TCompareFuncs;
				typedef std::vector<StaticMember> TStaticMembers;
				typedef std::vector<ClassDefinition*> TClassDefs;
				typedef std::vector<EnumDefinitionBase*> TEnumDefs;
				typedef std::vector<PyType_Slot> TSlots;

				PyObject* freezeDefinition(PyObject* module, const char* scopeName);

				PyType_Spec spec_;
				TPyObjPtr type_;
				TSlots slots_;
				TMethods methods_;
				TGetSetters getSetters_;
				TCompareFuncs compareFuncs_;
				TStaticMembers statics_;
				TClassDefs innerClasses_;
				TClassDefs subClasses_;
				TEnumDefs innerEnums_;
				ClassDefinition* parent_;
				TClassRegisterHook classRegisterHook_;
				const char* className_;
				const char* doc_;

				/** Typeless slot for ExportTraits to store implicit converters.
				*  This used to be a static in the templated ExportTraits, but since
				*  the initial value is then defined in a header (PY_SHADOW_CASTERS
				*  is usually invoked in a header), this causes a static variable
				*  to be defined _per_ DLL, if that type is used in multiple DLLs.
				*  To avoid that, we moved it to header, which we know will be
				*  defined in a source file, and will thus exist only once.
				*/
				void* implicitConvertersSlot_;

				bool isFrozen_;
			};
		}
	}
}

#endif
