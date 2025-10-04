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

// convention for names of macro arguments:
//
// i_foo: must be an identifier (bar is an identifier, spam::bar is not)
// s_foo: must be a string literal ("bar" is a string literal, bar is not)
// t_foo: must be an declared type and may be qualified (could be spam::Bar<Ham>)
// o_foo: must be an existing object (could be spam::bar if spam::bar exists as object)
// f_foo: must be a declared function (could be spam::bar if spam::bar(...) is declared as function)

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_MACROS_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_MACROS_H

#include "pyobject_call.inl"
#include "module_definition.h"
#include "../meta/is_member.h"
#include "../meta/is_charptr.h"

// --- modules -------------------------------------------------------------------------------------

/** @addtogroup ModuleDefinition
 *  @name Module Declaration Macros
 *
 *  These macros declare and define ModuleDefinition objects that represent Python modules.
 *  They create the fundamental module object that serves as the container for all exported
 *  functions, classes, and constants.
 *
 *  @{
 */

/** @ingroup ModuleDefinition
 *  Declare and define a ModuleDefinition object representing a Python module.
 *
 *  Creates a ModuleDefinition instance that can be used to build a Python module
 *  with the specified name and documentation.
 *
 *  @param i_module Identifier of the module to be used in C++ (must be unscoped identifier for token concatenation)
 *  @param s_name Python module name (const char* string, will be copied and stored internally)
 *  @param s_doc Optional module documentation string (const char* string, will be copied and stored internally, or nullptr)
 */
#define PY_DECLARE_MODULE_NAME_DOC( i_module, s_name, s_doc ) \
	::lass::python::ModuleDefinition i_module( s_name, s_doc );

/** @ingroup ModuleDefinition
 *  Declare a module with name only (no documentation).
 *  Convenience macro that wraps PY_DECLARE_MODULE_NAME_DOC() with s_doc = nullptr.
 *
 *  @param i_module Identifier of the module to be used as C++ variable name (must be valid C++ identifier)
 *  @param s_name Python module name (const char* string, will be copied and stored internally)
 */
#define PY_DECLARE_MODULE_NAME( i_module, s_name ) \
	PY_DECLARE_MODULE_NAME_DOC( i_module, s_name, 0)

/** @ingroup ModuleDefinition
 *  Declare a module with documentation, using the identifier as the module name.
 *  Convenience macro that wraps PY_DECLARE_MODULE_NAME_DOC() with s_name derived from i_module.
 *
 *  @param i_module Identifier of the module (also used as Python module name)
 *  @param s_doc Module documentation string (const char* string, will be copied and stored internally)
 */
#define PY_DECLARE_MODULE_DOC( i_module, s_doc ) \
	PY_DECLARE_MODULE_NAME_DOC( i_module, LASS_STRINGIFY(i_module), s_doc)

/** @ingroup ModuleDefinition
 *  Declare a module with minimal setup (name derived from identifier, no documentation).
 *  Convenience macro that wraps PY_DECLARE_MODULE_NAME_DOC() with defaults.
 *
 *  @param i_module Identifier of the module (also used as Python module name)
 */
#define PY_DECLARE_MODULE( i_module ) \
	PY_DECLARE_MODULE_NAME_DOC( i_module, LASS_STRINGIFY(i_module), 0)

/** @} */

/** @addtogroup ModuleDefinition
 *  @name Module Entrypoint and Injection Macros
 *
 *  These macros handle module initialization, injection, and extension module creation.
 *  They manage the Python module lifecycle from creation to registration with the Python interpreter.
 *
 *  If you're building a Python extension module (a .pyd or .so file), you will typically use
 *  the entrypoint macros to create the required `PyInit_*` function that Python calls:
 *  ```cpp
 *  // Declare module
 *  PY_DECLARE_MODULE_NAME_DOC(mymodule, "mymodule", "My example module")
 *  
 *  // Add functions and classes
 *  // ...
 * 
 *  // Create module entrypoint for Python extension
 *  PY_MODULE_ENTRYPOINT(mymodule)
 *  ```
 * 
 *  The injection macros are more low-level and can be used to create modules at runtime to be
 *  registered with an embedded Python interpreter.
 *
 *  @{
 */

/** @ingroup ModuleDefinition
 *  Create a `PyInit_*` Python module initialization function with custom name.
 *
 *  Generates the `PyInit_*` function required for Python extension modules.
 *  This function will be called by Python when the module is imported.
 * 
 *  ```cpp
 *  PY_DECLARE_MODULE_NAME_DOC(mymodule, "mymodule", "My example module")
 *  // ...
 *  PY_MODULE_ENTRYPOINT(mymodule)
 *  ```
 *
 *  @param i_module Module identifier declared with PY_DECLARE_MODULE_*
 *  @param i_name Name for the initialization function (PyInit_<i_name> will be generated)
 */
#define PY_MODULE_ENTRYPOINT_NAME( i_module, i_name ) \
	PyMODINIT_FUNC LASS_CONCATENATE(PyInit_, i_name)() { return i_module.inject(); }

/** @ingroup ModuleDefinition
 *  Create a Python module initialization function using the module identifier as the function name.
 *  Convenience macro that wraps PY_MODULE_ENTRYPOINT_NAME() with i_name = i_module.
 *
 *  @param i_module Module identifier (used for both module reference and function name)
 */
#define PY_MODULE_ENTRYPOINT( i_module ) PY_MODULE_ENTRYPOINT_NAME( i_module, i_module )

/** @ingroup ModuleDefinition
 *  Inject a Python module so Python becomes aware of it.
 *
 *  Creates the actual Python module object with all accumulated definitions
 *  (functions, classes, enums, constants). This should be done at runtime,
 *  typically in your main() function or during Python initialization.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 */
#define PY_INJECT_MODULE( i_module )\
	i_module.inject();

/** @ingroup ModuleDefinition
 *  @deprecated Use PY_DECLARE_MODULE_NAME_DOC and PY_INJECT_MODULE instead
 *  Inject a module with name and documentation override.
 */
#define PY_INJECT_MODULE_EX( i_module, s_moduleName, s_doc ) \
	i_module.setName(s_moduleName); \
	i_module.setDoc(s_doc); \
	i_module.inject();

/** @ingroup ModuleDefinition
 *  @deprecated Use PY_DECLARE_MODULE_NAME and PY_INJECT_MODULE instead
 *  Inject a module with name override.
 */
#define PY_INJECT_MODULE_NAME( i_module, s_moduleName )\
	i_module.setName(s_moduleName); \
	i_module.inject();

/** @ingroup ModuleDefinition
 *  @deprecated Use PY_DECLARE_MODULE_DOC and PY_INJECT_MODULE instead
 *  Inject a module with documentation override.
 */
#define PY_INJECT_MODULE_DOC( i_module, s_doc )\
	i_module.setDoc(s_doc);\
	i_module.inject();



/** @ingroup Python
 *	Inject a python module so Python is aware of it and produce all necessary code so a
 *  module can be used as extension of Python.  A limitation in comparison with embedded
 *  modules is that the name of the module cannot be changed anymore upon injection.
 *
 *  @param i_module
 *		the identifier of a module declared by PY_DECLARE_MODULE
 *  @param f_injection
 *		the function that will inject all the classes for this module
 *	@param s_doc
 *      documentation of module as shown in Python (zero terminated C string)
 */
#define PY_EXTENSION_MODULE_EX( i_module, f_injection, s_doc ) \
	extern "C" __declspec(dllexport)\
	void LASS_CONCATENATE(init, i_module) () {\
		PY_INJECT_MODULE_EX(i_module, const_cast<char*>( LASS_STRINGIFY(i_module) ), s_doc);\
		f_injection ();\
	}

/** @ingroup ModuleDefinition
 *  Create an extension module with documentation.
 *  Convenience macro that wraps PY_EXTENSION_MODULE_EX() with s_doc parameter.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_injection Injection function to call during module creation
 *  @param s_doc Module documentation string (const char* string with static storage duration)
 */
#define PY_EXTENSION_MODULE_DOC( i_module, f_injection, s_doc )\
	PY_EXTENSION_MODULE_EX( i_module, f_injection, s_doc)

/** @ingroup ModuleDefinition
 *  Create an extension module (no documentation).
 *  Convenience macro that wraps PY_EXTENSION_MODULE_EX() with s_doc = nullptr.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_injection Injection function to call during module creation
 */
#define PY_EXTENSION_MODULE( i_module, f_injection )\
	PY_EXTENSION_MODULE_EX( i_module, f_injection, 0)

/** @} */

// --- module variables ----------------------------------------------------------------------------

/** @addtogroup ModuleDefinition
 *  @name Module Constants and Objects Macros
 *
 *  These macros add constants, variables, and arbitrary Python objects to modules.
 *  They provide different approaches for exposing C++ values and objects to Python.
 *
 *  @{
 */

/** @ingroup ModuleDefinition
 *  Add an integer constant to a Python module.
 *
 *  The constant will be added to the module during module creation (at injection time).
 *  This is executed before main(), so the constant is available when the module is created.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param s_name Name of constant as shown in the module (const char* string with static storage duration)
 *  @param s_value Integer value of the constant (long)
 */
#define PY_MODULE_INTEGER_CONSTANT( i_module, s_name, s_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleIntegerConstant_, i_module),\
		i_module.addLong( s_value, s_name); ) 



/** @ingroup ModuleDefinition
 *  Add a string constant to a Python module.
 *
 *  The constant will be added to the module during module creation (at injection time).
 *  This is executed before main(), so the constant is available when the module is created.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param s_name Name of constant as shown in the module (const char* string with static storage duration)
 *  @param s_value String value of the constant (const char* string with static storage duration)
 */
#define PY_MODULE_STRING_CONSTANT( i_module, s_name, s_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleIntegerConstant_, i_module),\
		i_module.addString( s_value, s_name); ) 



/** @ingroup ModuleDefinition
 *  Inject an arbitrary object into an already created module at runtime.
 *
 *  This performs immediate injection into the module namespace, unlike the
 *  PY_MODULE_*_CONSTANT macros which defer injection until module creation.
 *  Must be called after the module has been injected with PY_INJECT_MODULE.
 *
 *  @param o_object Object/variable to be injected (will be converted to Python object)
 *  @param i_module Module identifier of an already injected module
 *  @param s_objectName Name of object as shown in the module (const char* string with static storage duration)
 */
#define PY_INJECT_OBJECT_IN_MODULE_EX( o_object, i_module, s_objectName )\
	{\
		i_module.injectObject( o_object, s_objectName );\
	}

/** @ingroup ModuleDefinition
 *  Inject an object using its C++ identifier as the Python name.
 *  Convenience macro that wraps PY_INJECT_OBJECT_IN_MODULE_EX() with s_objectName derived from o_object.
 *
 *  @param o_object Object/variable to be injected (name will be used as Python name)
 *  @param i_module Module identifier of an already injected module
 */
#define PY_INJECT_OBJECT_IN_MODULE( o_object, i_module )\
	PY_INJECT_OBJECT_IN_MODULE_EX(o_object, i_module, LASS_STRINGIFY(o_object))



/** @ingroup ModuleDefinition
 *  @deprecated Use PY_MODULE_INTEGER_CONSTANT instead for compile-time registration
 *  Inject an integer constant into an already created module at runtime.
 *
 *  This performs immediate injection, unlike PY_MODULE_INTEGER_CONSTANT which
 *  registers the constant for inclusion during module creation.
 *
 *  @param i_module Module identifier of an already injected module
 *  @param s_name Name of constant as shown in the module (const char* string with static storage duration)
 *  @param v_value Integer value of the constant (long)
 */
#define PY_MODULE_ADD_INTEGER_CONSTANT( i_module, s_name, v_value )\
	{\
		i_module.injectLong(s_name, v_value);\
	}



/** @ingroup ModuleDefinition
 *  @deprecated Use PY_MODULE_STRING_CONSTANT instead for compile-time registration
 *  Inject a string constant into an already created module at runtime.
 *
 *  This performs immediate injection, unlike PY_MODULE_STRING_CONSTANT which
 *  registers the constant for inclusion during module creation.
 *
 *  @param i_module Module identifier of an already injected module
 *  @param s_name Name of constant as shown in the module (const char* string with static storage duration)
 *  @param s_value String value of the constant (const char* string with static storage duration)
 */
#define PY_MODULE_ADD_STRING_CONSTANT( i_module, s_name, s_value )\
	{\
		i_module.injectString(s_name, s_value);\
	}

/** @} */

// --- free module functions -----------------------------------------------------------------------

/** @addtogroup ModuleDefinition
 *  @name Basic Function Export Macros
 *
 *  These macros export C++ functions to Python with automatic type deduction and wrapper generation.
 *  Use these for simple function exports where overloading is not an issue and automatic type
 *  deduction is sufficient.
 *
 *  @{
 */

/** @ingroup ModuleDefinition
 *  Export a C++ function to Python without automatic wrapper generation.
 *
 *  Use this macro for backward compatibility when wrapper functions don't
 *  need to be automatically generated or you want specific Python behavior.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export (must already be a PyCFunction)
 *  @param s_functionName Python function name
 *  @param s_doc Function documentation string
 *
 *  @deprecated Use PY_MODULE_FUNCTION_EX instead for automatic wrapper generation
 */
#define PY_MODULE_PY_FUNCTION_EX( i_module, f_cppFunction, s_functionName, s_doc )\
	static PyCFunction LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) = 0;\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModulePyFunction_, i_module, f_cppFunction ),\
		i_module.addFunctionDispatcher( \
			f_cppFunction, s_functionName, s_doc, \
			LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) \
			);\
	)



/** @ingroup ModuleDefinition
 *  Export a C++ free function to Python with full control over overloading.
 *
 *  This is the most flexible function export macro, allowing manual dispatcher naming
 *  for creating overloaded Python functions. Multiple C++ functions can be exported
 *  with the same Python name to create overloaded functions.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration, or nullptr)
 *  @param i_dispatcher Unique name for the generated dispatcher function (must be unscoped identifier for token concatenation)
 *
 *  Use this macro to export functions to Python. You can create overloaded Python functions 
 *  by exporting multiple C++ functions with the same s_functionName.
 *
 *  @note Overload resolution uses first-fit, not best-fit like C++. The first exported
 *        overload that matches the arguments will be called.
 *
 *  @note The documentation of an overloaded Python function will be the s_doc of the
 *        first exported overload.
 *
 *  @par Example:
 *  @code
 *  void barA(int a);
 *  void barB(const std::string& b);
 *
 *  PY_MODULE_FUNCTION_EX(foo_module, barA, "bar", nullptr, foo_bar_a)
 *  PY_MODULE_FUNCTION_EX(foo_module, barB, "bar", nullptr, foo_bar_b)
 *  @endcode
 */
#define PY_MODULE_FUNCTION_EX( i_module, f_cppFunction, s_functionName, s_doc, i_dispatcher )\
	static PyCFunction LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) = 0;\
	extern "C" LASS_DLL_LOCAL PyObject* i_dispatcher( PyObject* iIgnore, PyObject* iArgs )\
	{\
		if (LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ))\
		{\
			PyObject* result = LASS_CONCATENATE( pyOverloadChain_, i_dispatcher )(iIgnore, iArgs);\
			if (!(PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError)))\
			{\
				return result;\
			}\
			PyErr_Clear();\
			Py_XDECREF(result);\
		}\
		return ::lass::python::impl::callFunction( iArgs, &f_cppFunction );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModuleFunction_, i_module, i_dispatcher ), \
		i_module.addFunctionDispatcher( \
			i_dispatcher, s_functionName, s_doc, \
			LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) \
			);\
	)

/** @ingroup ModuleDefinition
 *  Export a C++ free function to Python with custom name and documentation.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_EX() with auto-generated dispatcher name.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export
 *  @param s_name Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration, or nullptr)
 */
#define PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, s_name, s_doc )\
	PY_MODULE_FUNCTION_EX( i_module, f_cppFunction, s_name, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))

/** @ingroup ModuleDefinition
 *  Export a C++ free function to Python with custom name (no documentation).
 *  Convenience macro that wraps PY_MODULE_FUNCTION_NAME_DOC() with s_doc = nullptr.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export
 *  @param s_name Python function name (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_NAME( i_module, f_cppFunction, s_name)\
	PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, s_name, 0)

/** @ingroup ModuleDefinition
 *  Export a C++ free function to Python using the C++ function name with documentation.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_NAME_DOC() with s_name derived from f_cppFunction.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export (name will be used as Python name)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_DOC( i_module, f_cppFunction, s_doc )\
	PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, LASS_STRINGIFY(f_cppFunction), s_doc)

/** @ingroup ModuleDefinition
 *  Export a C++ free function to Python using the C++ function name (no documentation).
 *  Convenience macro that wraps PY_MODULE_FUNCTION_NAME_DOC() with defaults.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export (name will be used as Python name)
 */
#define PY_MODULE_FUNCTION( i_module, f_cppFunction)\
	PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, LASS_STRINGIFY(f_cppFunction), 0)

/** @} */

// --- casting free functions -----------------------------------------------------------

/** @addtogroup ModuleDefinition
 *  @name Function Cast Export Macros (Deprecated)
 *
 *  @deprecated These macros are deprecated. Instead of using casting macros, define explicit
 *  wrapper functions with the desired signatures and export those directly using the basic
 *  function export macros.
 *
 *  These macros export C++ functions to Python with explicit type casting and support for
 *  default parameters. They create wrapper functions that handle type conversions and
 *  allow exporting functions with default parameters by omitting trailing parameters.
 *
 *  @{
 */

/** @ingroup ModuleDefinition
 *  Exports a C++ free functions to Python with on the fly casting on return type and parameters, including omission of default parameters
 *
 *  @param i_module
 *		the module object
 *  @param f_cppFunction
 *      the name of the function in C++
 *  @param t_return
 *      the return type of @a f_cppFunction
 *  @param t_params
 *      a lass::meta::TypeTuple of the parameter types of @a f_cppFunction
 *  @param s_functionName
 *      the name the method will have in Python
 *  @param s_doc
 *      documentation of function as shown in Python (zero terminated C string)
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *
 *  You can use this macro instead of PY_MODULE_FUNCTION_EX if you want to use for instance the default parameters of the C++ definition of
 *	@a f_cppFunction. This macro can also help you to resolve ambiguities althought the PY_MODULE_QUALIFIED_EX is the preferred macro for
 *	doing that.
 *
 *  @code
 *	void bar(int a, int b=555);
 *
 *  PY_MODULE_FUNCTION_CAST_NAME_1(foo_module, bar, void, int, "bar" )			// export the function with as default input for b=555
 *  PY_MODULE_FUNCTION_CAST_NAME_2(foo_module, bar, void, int, int, "bar" )
 *  @endcode
 */


/** @ingroup ModuleDefinition
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit return type casting for 0-parameter functions with full control.
 *  This macro allows exporting functions with default parameters or when explicit type casting is needed.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export (0 parameters)
 *  @param t_return Return type of the function (for explicit casting)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *  @param i_dispatcher Unique identifier for the function dispatcher
 */
#define PY_MODULE_FUNCTION_CAST_EX_0(i_module, f_cppFunction, t_return, s_functionName, s_doc, i_dispatcher) \
	::lass::python::OwnerCaster<t_return>::TCaster::TTarget LASS_CONCATENATE(i_dispatcher, _caster) ()\
	{\
 		return f_cppFunction() ; \
	}\
	PY_MODULE_FUNCTION_EX( i_module, LASS_CONCATENATE(i_dispatcher, _caster), s_functionName, s_doc, i_dispatcher );


 $[
/** @ingroup ModuleDefinition
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit type casting for $x-parameter functions with full control.
 *  This macro allows exporting functions with default parameters or when explicit type casting is needed.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export ($x parameters)
 *  @param t_return Return type of the function (for explicit casting)
 *  @param $(t_P$x)$ Parameter types for the function (for explicit casting)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *  @param i_dispatcher Unique identifier for the function dispatcher
 */
 #define PY_MODULE_FUNCTION_CAST_EX_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc, i_dispatcher )\
	::lass::python::OwnerCaster< t_return >::TCaster::TTarget LASS_CONCATENATE(i_dispatcher, _caster) ( \
	$(::lass::python::OwnerCaster< t_P$x >::TCaster::TTarget iArg$x)$ \
	)\
	{\
 		return f_cppFunction ( $(::lass::python::OwnerCaster< t_P$x >::TCaster::cast(iArg$x))$ );\
	}\
	PY_MODULE_FUNCTION_EX( i_module, LASS_CONCATENATE(i_dispatcher, _caster), s_functionName, s_doc, i_dispatcher );
 ]$

/** @ingroup ModuleDefinition
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit type casting for 0-parameter functions with custom name and documentation.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_CAST_EX_0() with automatically generated dispatcher name.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export (0 parameters)
 *  @param t_return Return type of the function (for explicit casting)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for explicit casting - should be empty TypeTuple<>)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_CAST_NAME_DOC_0( i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_CAST_EX_0(\
		i_module, f_cppFunction, t_return, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))
$[
/** @ingroup ModuleDefinition
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit type casting for $x-parameter functions with custom name and documentation.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_CAST_EX_$x() with automatically generated dispatcher name.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export ($x parameters)
 *  @param t_return Return type of the function (for explicit casting)
 *  @param $(t_P$x)$ Parameter types for the function (for explicit casting)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_CAST_NAME_DOC_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_CAST_EX_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))
]$


/** @ingroup ModuleDefinition
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit type casting for 0-parameter functions with custom name (no documentation).
 *  Convenience macro that wraps PY_MODULE_FUNCTION_CAST_NAME_DOC_0() with s_doc = nullptr.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export (0 parameters)
 *  @param t_return Return type of the function (for explicit casting)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_CAST_NAME_0( i_module, f_cppFunction, t_return, s_functionName )\
	PY_MODULE_FUNCTION_CAST_NAME_DOC_0(\
		i_module, f_cppFunction, t_return, s_functionName, 0 )
$[
/** @ingroup ModuleDefinition
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit type casting for $x-parameter functions with custom name (no documentation).
 *  Convenience macro that wraps PY_MODULE_FUNCTION_CAST_NAME_DOC_$x() with s_doc = nullptr.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export ($x parameters)
 *  @param t_return Return type of the function (for explicit casting)
 *  @param $(t_P$x)$ Parameter types for the function (for explicit casting)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_CAST_NAME_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName )\
	PY_MODULE_FUNCTION_CAST_NAME_DOC_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, 0 )
]$

/** @} */

// --- explicit qualified free functions -----------------------------------------------------------

/** @addtogroup ModuleDefinition
 *  @name Type-Qualified Function Export Macros
 *
 *  These macros export C++ functions to Python with explicit type qualification to resolve
 *  function overload ambiguities. They provide fine-grained control over function signatures
 *  and are essential when exporting overloaded functions that would otherwise be ambiguous.
 *
 *  The macros are organized in layers:
 *  - PY_MODULE_FUNCTION_QUALIFIED_EX(): Full control with custom dispatcher and meta::TypeTuple for parameters
 *  - PY_MODULE_FUNCTION_QUALIFIED_EX_0() through PY_MODULE_FUNCTION_QUALIFIED_EX_15(): Full control with custom dispatcher, for 0-15 parameters
 *  - PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC(): Automatic dispatcher with custom name and documentation, and meta::TypeTuple for parameters
 *  - PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0() through PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_15(): Automatic dispatcher with custom name, for 0-15 parameters
 *  - PY_MODULE_FUNCTION_QUALIFIED_NAME(): Automatic dispatcher with custom name, no documentation, and meta::TypeTuple for parameters
 *  - PY_MODULE_FUNCTION_QUALIFIED_NAME_0() through PY_MODULE_FUNCTION_QUALIFIED_NAME_15(): Automatic dispatcher with custom name, no documentation, for 0-15 parameters
 *  - PY_MODULE_FUNCTION_QUALIFIED_DOC(): Automatic dispatcher with documentation, and meta::TypeTuple for parameters
 *  - PY_MODULE_FUNCTION_QUALIFIED_DOC_0() through PY_MODULE_FUNCTION_QUALIFIED_DOC_15(): Automatic dispatcher with documentation, for 0-15 parameters
 *  - PY_MODULE_FUNCTION_QUALIFIED(): Automatic dispatcher, no documentation, and meta::TypeTuple for parameters
 *  - PY_MODULE_FUNCTION_QUALIFIED_0() through PY_MODULE_FUNCTION_QUALIFIED_15(): Automatic dispatcher, no documentation, for 0-15 parameters
 *
 *  @{
 */


/** @ingroup ModuleDefinition
 *  Export a C++ free function to Python with explicit type qualification to resolve ambiguities.
 *
 *  Use this macro instead of PY_MODULE_FUNCTION_EX when there are overloaded C++ functions
 *  that would create ambiguity. By explicitly specifying return type and parameter types,
 *  you can disambiguate which overload to export.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export (can be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration, or nullptr)
 *  @param i_dispatcher Unique name for the generated dispatcher function (must be unscoped identifier for token concatenation)
 *
 *  This macro helps resolve function overload ambiguities by explicitly specifying
 *  the function signature to export.
 *
 *  @par Example:
 *  @code
 *  void bar(int a);
 *  void bar(const std::string& b);
 *
 *  PY_MODULE_FUNCTION_QUALIFIED_EX(foo_module, bar, void, meta::TypeTuple<int>, "bar", nullptr, foo_bar_a)
 *  PY_MODULE_FUNCTION_QUALIFIED_EX(foo_module, bar, void, meta::TypeTuple<const std::string&>, "bar", nullptr, foo_bar_b)
 *  @endcode
 */

#define PY_MODULE_FUNCTION_QUALIFIED_EX(i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc, i_dispatcher)\
	static PyCFunction LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) = 0;\
	extern "C" LASS_DLL_LOCAL PyObject* i_dispatcher( PyObject* iIgnore, PyObject* iArgs )\
	{\
		if (LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ))\
		{\
			PyObject* result = LASS_CONCATENATE( pyOverloadChain_, i_dispatcher )(iIgnore, iArgs);\
			if (!(PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError)))\
			{\
				return result;\
			}\
			PyErr_Clear();\
			Py_XDECREF(result);\
		}\
		return ::lass::python::impl::ExplicitResolver\
		<\
			lass::meta::NullType,\
			t_return,\
			t_params\
		>\
		::callFunction(iArgs, &f_cppFunction);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModuleFunction_, i_module, i_dispatcher ),\
		i_module.addFunctionDispatcher( \
			i_dispatcher, s_functionName, s_doc, \
			LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) \
		);\
	)

/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification for 0-parameter functions with full control.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_EX() for functions with 0 parameters.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export (0 parameters, can be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *  @param i_dispatcher Unique identifier for the function dispatcher
 */
#define PY_MODULE_FUNCTION_QUALIFIED_EX_0( i_module, f_cppFunction, t_return, s_functionName, s_doc, i_dispatcher )\
	PY_MODULE_FUNCTION_QUALIFIED_EX(\
		i_module, f_cppFunction, t_return, ::lass::meta::TypeTuple<>, s_functionName, s_doc, i_dispatcher )
$[
/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification for $x-parameter functions with full control.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_EX() for functions with exactly $x parameters.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export ($x parameters, can be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the function (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *  @param i_dispatcher Unique identifier for the function dispatcher
 */
#define PY_MODULE_FUNCTION_QUALIFIED_EX_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc, i_dispatcher )\
	typedef ::lass::meta::TypeTuple< $(t_P$x)$ > \
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams));\
	PY_MODULE_FUNCTION_QUALIFIED_EX(\
		i_module, f_cppFunction, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams)),\
		s_functionName, s_doc, i_dispatcher )
]$

/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification and custom name with documentation.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_EX() with automatically generated dispatcher name.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export (can be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC( i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_EX(\
		i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))


/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification for 0-parameter functions with custom name and documentation.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_EX_0() with automatically generated dispatcher name.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export (0 parameters, can be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation - should be empty TypeTuple<>)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0( i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_EX_0(\
		i_module, f_cppFunction, t_return, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))
$[
/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification for $x-parameter functions with custom name and documentation.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_EX_$x() with automatically generated dispatcher name.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export ($x parameters, can be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the function (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_EX_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))
]$

/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification and custom name (no documentation).
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC() with s_doc = nullptr.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export (can be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME( i_module, f_cppFunction, t_return, t_params, s_functionName )\
		PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC(\
			i_module, f_cppFunction, t_return, t_params, s_functionName, 0 )

/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification for 0-parameter functions and custom name (no documentation).
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0() with s_doc = nullptr.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export (0 parameters, can be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_0( i_module, f_cppFunction, t_return, s_functionName )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0(\
		i_module, f_cppFunction, t_return, s_functionName, 0 )
$[
/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification for $x-parameter functions with custom name (no documentation).
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x() with s_doc = nullptr.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export ($x parameters, can be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the function (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, 0 )
]$

/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification using the C++ function name with documentation.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC() with s_functionName derived from f_cppFunction.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export (can be overloaded, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_QUALIFIED_DOC( i_module, f_cppFunction, t_return, t_params, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC( \
		i_module, f_cppFunction, t_return, t_params, LASS_STRINGIFY(f_cppFunction), s_doc)

/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification for 0-parameter functions using the C++ function name with documentation.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0() with s_functionName derived from f_cppFunction.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export (0 parameters, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_QUALIFIED_DOC_0( i_module, f_cppFunction, t_return, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0( \
		i_module, f_cppFunction, t_return, LASS_STRINGIFY(f_cppFunction), s_doc)
$[
/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification for $x-parameter functions using the C++ function name with documentation.
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x() with s_functionName derived from f_cppFunction.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*()
 *  @param f_cppFunction C++ function to export ($x parameters, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the function (for disambiguation)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_QUALIFIED_DOC_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x( \
		i_module, f_cppFunction, t_return, $(t_P$x)$, LASS_STRINGIFY(f_cppFunction), s_doc)
]$

/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification using the C++ function name (no documentation).
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC() with defaults.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export (can be overloaded, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 */
#define PY_MODULE_FUNCTION_QUALIFIED( i_module, f_cppFunction, t_return, t_params )\
	PY_MODULE_FUNCTION_QUALIFIED_DOC( i_module, f_cppFunction, t_return, t_params, 0 )

/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification for 0-parameter functions using the C++ function name (no documentation).
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_DOC_0() with s_doc = nullptr.
 *
 *  @param i_module Module identifier declared by PY_DECLARE_MODULE_*
 *  @param f_cppFunction C++ function to export (0 parameters, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 */
#define PY_MODULE_FUNCTION_QUALIFIED_0( i_module, f_cppFunction, t_return )\
	PY_MODULE_FUNCTION_QUALIFIED_DOC_0( i_module, f_cppFunction, t_return, 0 )
$[
/** @ingroup ModuleDefinition
 *  Export a C++ function with type qualification for $x-parameter functions using the C++ function name (no documentation).
 *  Convenience macro that wraps PY_MODULE_FUNCTION_QUALIFIED_DOC_$x() with s_doc = nullptr.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_$x( i_module, f_cppFunction, t_return, $(t_P$x)$ )\
	PY_MODULE_FUNCTION_QUALIFIED_DOC_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, 0 )
]$

/** @} */



// --- classes -------------------------------------------------------------------------------------

/** @addtogroup ClassDefinition
 *  @name Class Declaration & Setup
 *
 *  Macros to declare and configure Python classes from C++ types.
 *
 *  These macros create the internal class definition objects that aggregate all the information
 *  needed to generate a Python type. Every C++ class that needs to be exposed to Python
 *  must be declared using one of these macros.
 *
 *  LASS supports two approaches for exporting C++ classes to Python:
 *
 *  1. **Native Python classes**: C++ classes that directly inherit from PyObjectPlus
 *     and are designed to be Python-aware from the start.
 *
 *  2. **Shadow classes**: Wrapper classes created for existing native C++ types
 *     using the shadow system (see PY_SHADOW_CLASS macros). The shadow class inherits from
 *     PyObjectPlus and wraps the original C++ type.
 *
 *  In both cases, the class parameter in these declaration macros refers to the 
 *  Python binding class (either PyObjectPlus-derived or shadow wrapper), never
 *  the underlying native C++ type being shadowed.
 *
 *  All Python classes must use the PY_HEADER macro in their declaration and be declared
 *  in source files only, never in headers, to avoid multiple definition errors.
 *
 *  **Native Python Class Usage:**
 *  ```cpp
 *  // In header file (MyClass.h):
 *  class MyClass: public PyObjectPlus
 *  {
 *      PY_HEADER(PyObjectPlus)
 *  public:
 *      MyClass();
 *      void someMethod();
 *  };
 *
 *  // In source file (MyClass.cpp):
 *  PY_DECLARE_CLASS_NAME_DOC(MyClass, "MyClass", "A sample class")
 *  // ... add methods, constructors, properties etc.
 *  ```
 *
 *  **Shadow Class Usage:**
 *  ```cpp
 *  // Existing non-Python-aware C++ class:
 *  class LegacyClass
 *  {
 *  public:
 *      void doSomething();
 *  };
 *
 *  // Shadow wrapper class:
 *  PY_SHADOW_CLASS(LASS_DLL_EXPORT, PyShadowLegacyClass, LegacyClass)
 *
 *  // Declaration (note: we declare the shadow class, not LegacyClass):
 *  PY_DECLARE_CLASS_NAME_DOC(PyShadowLegacyClass, "LegacyClass", "Legacy class wrapper")
 *  ```
 *
 *  @{
 */

/** Declare a Python class with full control over name and documentation.
 *
 *  This is the primary class declaration macro that creates the internal ClassDefinition
 *  object for a C++ class. All other class declaration macros ultimately call this one.
 *  The class definition collects constructors, methods, properties, and other elements
 *  that will be added later using PY_CLASS_* macros.
 *
 *  @param t_cppClass     Python binding class type. This must be either:
 *                        - A class directly inheriting from PyObjectPlus, or  
 *                        - A shadow class created with PY_SHADOW_CLASS macros.
 *                        Never pass the underlying native C++ shadowed type.
 *  @param s_className    Python class name as string literal
 *  @param s_doc          Class documentation string (or nullptr for no doc)
 *
 *  @remark This macro must be used exactly once per class and only in source files, never in headers!
 *
 *  **Native Python Class Example:**
 *  ```cpp
 *  PY_DECLARE_CLASS_NAME_DOC(MyClass, "MyClass", "A sample native Python class")
 *  ```
 *
 *  **Shadow Class Example:**
 *  ```cpp  
 *  PY_DECLARE_CLASS_NAME_DOC(PyShadowLegacy, "LegacyClass", "Wrapper for LegacyClass")
 *  ```
 *
 *  @ingroup ClassDefinition
 */
#define PY_DECLARE_CLASS_NAME_DOC( t_cppClass, s_className, s_doc ) \
	::lass::python::impl::ClassDefinition t_cppClass ::_lassPyClassDef( \
		s_className, s_doc, sizeof(t_cppClass), \
		::lass::python::impl::richCompareDispatcher< t_cppClass >,\
		& t_cppClass ::_lassPyParentType::_lassPyClassDef, \
		& t_cppClass ::_lassPyClassRegisterHook);

/** Declare a Python class with custom name but no documentation.
 *
 *  Convenience wrapper around PY_DECLARE_CLASS_NAME_DOC that omits the documentation string.
 *  Use this when you want to control the Python class name but don't need documentation.
 *
 *  @param t_cppClass     Python binding class type (PyObjectPlus-derived or shadow class)
 *  @param s_className    Python class name as string literal
 *
 *  **Example:**
 *  ```cpp
 *  PY_DECLARE_CLASS_NAME(MyClass, "MyClass")
 *  ```
 *
 *  @ingroup ClassDefinition
 */
#define PY_DECLARE_CLASS_NAME( t_cppClass, s_className )\
	PY_DECLARE_CLASS_NAME_DOC( t_cppClass, s_className, 0 )

/** Declare a Python class with automatic name and custom documentation.
 *
 *  Convenience wrapper that uses the C++ class name as the Python class name
 *  but allows custom documentation. The class name is automatically stringified.
 *
 *  @param i_cppClass     Python-exportable C++ class identifier (unqualified name)
 *  @param s_doc          Class documentation string
 *
 *  **Example:**
 *  ```cpp
 *  PY_DECLARE_CLASS_DOC(MyClass, "A sample class for demonstration")
 *  // Creates Python class named "MyClass"
 *  ```
 *
 *  @ingroup ClassDefinition
 */
#define PY_DECLARE_CLASS_DOC( i_cppClass, s_doc ) \
	PY_DECLARE_CLASS_NAME_DOC( i_cppClass, LASS_STRINGIFY(i_cppClass), s_doc )

/** Declare a Python class with automatic name and no documentation.
 *
 *  The simplest class declaration macro. Uses the C++ class name as the Python class name
 *  and provides no documentation string. Most commonly used for basic class exports.
 *
 *  @param i_cppClass     Python binding class identifier (unqualified name)
 *
 *  **Example:**
 *  ```cpp
 *  PY_DECLARE_CLASS(MyClass)
 *  // Creates Python class named "MyClass" with no documentation
 *  ```
 *
 *  @ingroup ClassDefinition
 */
#define PY_DECLARE_CLASS( i_cppClass ) \
	PY_DECLARE_CLASS_NAME_DOC( i_cppClass, LASS_STRINGIFY(i_cppClass), 0 )

/** Legacy class declaration macro.
 *
 *  @param t_cppClass                Python binding class type
 *  @param s_className               Python class name  
 *  @param i_uniqueClassIdentifier   Unused parameter (legacy)
 *
 *  @deprecated This macro is deprecated and should not be used in new code. Use PY_DECLARE_CLASS_NAME_DOC instead.
 *
 *  @ingroup ClassDefinition
 */
#define PY_DECLARE_CLASS_EX( t_cppClass, s_className, i_uniqueClassIdentifier )\
	PY_DECLARE_CLASS_NAME_DOC( t_cppClass, s_className, 0 )

/** @} */



/** @addtogroup ModuleDefinition
 *  @name Class Integration
 *
 *  Macros to add Python classes to modules.
 *
 *  These macros integrate class definitions with module definitions, making the classes
 *  available as types within the module namespace. Classes must first be declared using
 *  PY_DECLARE_CLASS_* macros before they can be added to modules.
 *
 *  @{
 */

/** Inject a class into a module at runtime (deprecated).
 *
 *  This is the legacy runtime approach for adding classes to modules.
 *  Use PY_MODULE_CLASS instead for compile-time registration.
 *
 *  @param t_cppClass    Python binding class type that has been declared with PY_DECLARE_CLASS_*
 *  @param i_module      Module object identifier to inject the class into
 *  @param s_doc         Optional class documentation string (or nullptr)
 *
 *  @deprecated Use PY_MODULE_CLASS instead, and set class doc with PY_DECLARE_CLASS_DOC
 *
 *  @remark This is executed at runtime, so it must be called from main() or a function called by main().
 *
 *  @ingroup ModuleDefinition
 */
#define PY_INJECT_CLASS_IN_MODULE( t_cppClass, i_module, s_doc ) \
	t_cppClass::_lassPyClassDef.setDocIfNotNull(s_doc);\
	i_module.injectClass(t_cppClass::_lassPyClassDef); 

/** Add a Python class to a module.
 *
 *  Registers a class definition to be included in the module when it is created.
 *  The class must have been declared with PY_DECLARE_CLASS_* macros.
 *  This is executed before main(), so the class is available when the module is created.
 *
 *  @param i_module      Module identifier declared by PY_DECLARE_MODULE_* (must be unscoped identifier for token concatenation)
 *  @param t_cppClass    Python binding class type that has been declared with PY_DECLARE_CLASS_*
 *
 *  **Example:**
 *  ```cpp
 *  PY_DECLARE_CLASS_NAME_DOC(MyClass, "MyClass", "Sample class")
 *  PY_MODULE_CLASS(mymodule, MyClass)
 *  ```
 *
 *  @ingroup ModuleDefinition
 */
#define PY_MODULE_CLASS( i_module, t_cppClass ) \
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleClass_, i_module ),\
		i_module.addClass( t_cppClass ::_lassPyClassDef); \
	)

/** @} */



/** @addtogroup ClassDefinition
 *  @name Static Constants
 *
 *  Macros to export static constant values as class attributes.
 *
 *  These macros allow you to expose compile-time constant values as static attributes
 *  of Python classes. The values are converted to Python objects using PyExportTraits
 *  and become accessible as class-level attributes in Python.
 *
 *  @{
 */

/** Export a static constant value as a class attribute.
 *
 *  Adds a static constant to a Python class that can be accessed as a class attribute.
 *  The constant value is converted to a Python object at module initialization time
 *  and becomes accessible via the class in Python.
 *
 *  @param i_cppClass    Python binding class identifier (must be declared with PY_DECLARE_CLASS_*)
 *  @param s_name        Name of the constant as it will appear in Python (string literal)
 *  @param v_value       The constant value to export (must be convertible via PyExportTraits::build)
 *
 *  **Example:**
 *  ```cpp
 *  class MyClass: public PyObjectPlus { ... };
 *  PY_DECLARE_CLASS(MyClass)
 *  PY_CLASS_STATIC_CONST(MyClass, "PI", 3.14159)
 *  PY_CLASS_STATIC_CONST(MyClass, "MAX_SIZE", 1024)
 *  
 *  // In Python:
 *  // MyClass.PI == 3.14159
 *  // MyClass.MAX_SIZE == 1024
 *  ```
 *
 *  @ingroup ClassDefinition
 */
#define PY_CLASS_STATIC_CONST( i_cppClass, s_name, v_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyClassStaticConst, i_cppClass ),\
		i_cppClass ::_lassPyClassDef.addStaticConst(s_name, v_value);\
	)

/** @} */



// --- inner class ---------------------------------------------------------------------------------



/** @addtogroup ClassDefinition
 *  @name Inner Classes
 *  
 *  Macros for declaring inner classes (nested classes) within Python-exported classes.
 *  These macros establish hierarchical class relationships where inner classes become
 *  attributes of their outer class in Python.
 *
 *  @note Inner classes must be declared with PY_DECLARE_CLASS* macros before using these macros.
 *  
 *  Usage pattern:
 *  ```cpp
 *  // Declare both classes first
 *  PY_DECLARE_CLASS_DOC(Outer, "Outer class")
 *  PY_DECLARE_CLASS_DOC(Inner, "Inner class")
 *  
 *  // Establish inner class relationship
 *  PY_CLASS_INNER_CLASS(Outer, Inner)
 *  ```
 *
 *  @{
 */

/** @ingroup ClassDefinition
 *  @brief Exports an inner class with full customization of name, documentation, and symbol suffix.
 *  
 *  This is the most flexible inner class macro, allowing complete control over all parameters.
 *  The inner class becomes accessible as an attribute of the outer class in Python.
 *  
 *  @param t_outerCppClass C++ class that will contain the inner class
 *  @param t_innerCppClass C++ class to be exported as inner class  
 *  @param s_name Python name for the inner class (null-terminated C string literal)
 *  @param s_doc Python docstring for the inner class (null-terminated C string literal, may be nullptr)
 *  @param i_uniqueSuffix Unique C++ identifier to generate unique symbols for the registration code.
 *                        This prevents symbol collisions when multiple inner class exports exist.
 *  
 *  ```cpp
 *  // Export Inner as nested class of Outer with custom name
 *  PY_CLASS_INNER_CLASS_EX(Outer, Inner, "CustomInner", "Inner class documentation", MyUniqueSuffix)
 *  // Python: outer_instance.CustomInner
 *  ```
 */
#define PY_CLASS_INNER_CLASS_EX( t_outerCppClass, t_innerCppClass, s_name, s_doc, i_uniqueSuffix )\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE(lassPythonImplExecuteBeforeMain_, i_uniqueSuffix),\
		t_innerCppClass::_lassPyClassDef.setDocIfNotNull(s_doc);\
		t_outerCppClass::_lassPyClassDef.addInnerClass(t_innerCppClass::_lassPyClassDef);\
	)

/** @ingroup ClassDefinition
 *  @brief Exports an inner class with custom name and documentation.
 *  
 *  Convenience macro that automatically generates a unique suffix from the class names.
 *  Provides full control over the Python name and documentation string.
 *  
 *  @param i_outerCppClass C++ class that will contain the inner class
 *  @param i_innerCppClass C++ class to be exported as inner class
 *  @param s_name Python name for the inner class (null-terminated C string literal)
 *  @param s_doc Python docstring for the inner class (null-terminated C string literal, may be nullptr)
 *  
 *  @deprecated You should be setting the doc when declaring the innerclass ...
 *  
 *  ```cpp
 *  PY_CLASS_INNER_CLASS_NAME_DOC(Outer, Inner, "NestedClass", "Documentation")
 *  // Python: outer_instance.NestedClass
 *  ```
 */
#define PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, s_name, s_doc )\
	PY_CLASS_INNER_CLASS_EX( i_outerCppClass, i_innerCppClass, s_name, s_doc,\
		LASS_CONCATENATE(i_outerCppClass, i_innerCppClass) )

/** @ingroup ClassDefinition
 *  @brief Exports an inner class with custom name but no documentation.
 *  
 *  Convenience macro for cases where you want to customize the Python name
 *  but don't need to provide additional documentation.
 *  
 *  @param i_outerCppClass C++ class that will contain the inner class
 *  @param i_innerCppClass C++ class to be exported as inner class
 *  @param s_name Python name for the inner class (null-terminated C string literal)
 *  
 *  ```cpp
 *  PY_CLASS_INNER_CLASS_NAME(Outer, Inner, "CustomName")
 *  // Python: outer_instance.CustomName
 *  ```
 */
#define PY_CLASS_INNER_CLASS_NAME( i_outerCppClass, i_innerCppClass, s_name)\
	PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, s_name, 0)

/** @ingroup ClassDefinition
 *  @brief Exports an inner class with default name and custom documentation.
 *  
 *  The inner class will use its C++ class name as the Python name,
 *  but allows you to provide custom documentation.
 *  
 *  @param i_outerCppClass C++ class that will contain the inner class
 *  @param i_innerCppClass C++ class to be exported as inner class
 *  @param s_doc Python docstring for the inner class (null-terminated C string literal)
 *  
 *  @deprecated You should be setting the doc when declaring the innerclass ...
 *  
 *  ```cpp
 *  PY_CLASS_INNER_CLASS_DOC(Outer, Inner, "Custom documentation")
 *  // Python: outer_instance.Inner (with custom doc)
 *  ```
 */
#define PY_CLASS_INNER_CLASS_DOC( i_outerCppClass, i_innerCppClass, s_doc )\
	PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, LASS_STRINGIFY(i_innerCppClass), s_doc)

/** @ingroup ClassDefinition
 *  @brief Exports an inner class with default name and no documentation.
 *  
 *  The simplest inner class export macro. Uses the C++ class name as the Python name
 *  and doesn't provide additional documentation beyond what was set during class declaration.
 *  
 *  @param i_outerCppClass C++ class that will contain the inner class
 *  @param i_innerCppClass C++ class to be exported as inner class
 *  
 *  ```cpp
 *  PY_CLASS_INNER_CLASS(Outer, Inner)
 *  // Python: outer_instance.Inner
 *  ```
 */
#define PY_CLASS_INNER_CLASS( i_outerCppClass, i_innerCppClass)\
	PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, LASS_STRINGIFY(i_innerCppClass), 0)

/** @} */



// --- methods -------------------------------------------------------------------------------------

/** @addtogroup ClassDefinition
 *  @name Methods
 *
 *  Macros for exporting C++ class methods to Python with automatic type deduction 
 *  and wrapper generation. These macros handle member function calls, overloading,
 *  and method documentation.
 *
 *  @note Overload resolution uses first-fit, not best-fit like C++. The first exported
 *        overload that matches the arguments will be called.
 *
 *  @note The documentation of an overloaded Python method will be the s_doc of the
 *        first exported overload.
 *
 *  Usage pattern:
 *  ```cpp
 *  // Declare the class first
 *  PY_DECLARE_CLASS_DOC(Foo, "My class")
 *  
 *  // Export methods - simple case
 *  PY_CLASS_METHOD(Foo, someMethod)
 *  
 *  // Export overloaded methods with same Python name
 *  PY_CLASS_METHOD_EX(Foo, barA, "bar", nullptr, foo_bar_a)
 *  PY_CLASS_METHOD_EX(Foo, barB, "bar", nullptr, foo_bar_b)
 *  ```
 *
 *  @{
 */

/** @ingroup ClassDefinition
 *  @brief Export a C++ method that returns raw PyObject* to Python.
 *  
 *  Use this macro when you need a method that returns Python-specific objects
 *  or handles Python types directly. The C++ method must return PyObject* and
 *  accept PyObject* arguments directly.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (must return PyObject* and take PyObject* args)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @deprecated Use PY_CLASS_METHOD_EX() instead for automatic wrapper generation
 *  
 *  ```cpp
 *  class Foo {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      PyObject* specialPythonMethod(PyObject* args);  // Returns PyObject* directly
 *  };
 *  
 *  PY_CLASS_PY_METHOD_EX(Foo, specialPythonMethod, "special", nullptr)
 *  // Python: foo_instance.special(args)
 *  ```
 */
#define PY_CLASS_PY_METHOD_EX( i_cppClass, i_cppMethod, s_methodName, s_doc  )\
	static ::lass::python::impl::OverloadLink LASS_CONCATENATE_3( staticDispatchOverloadChain, i_cppClass, i_cppMethod);\
	extern "C" LASS_DLL_LOCAL PyObject* LASS_CONCATENATE_3( staticDispatch, i_cppClass, i_cppMethod) ( PyObject* iObject, PyObject* iArgs )\
	{\
		if (!PyType_IsSubtype(iObject->ob_type , i_cppClass::_lassPyClassDef.type() ))\
		{\
			PyErr_Format(PyExc_TypeError,"PyObject not castable to %s", i_cppClass::_lassPyClassDef.name());\
			return 0;\
		}\
		i_cppClass* object = static_cast<i_cppClass*>(iObject);\
		return object->i_cppMethod( iArgs );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyClassPyMethod_, i_cppClass, i_cppMethod ),\
		i_cppClass::_lassPyClassDef.addMethod(\
			s_methodName, s_doc, \
			LASS_CONCATENATE_3( staticDispatch, i_cppClass, i_cppMethod),\
			LASS_CONCATENATE_3( staticDispatchOverloadChain, i_cppClass, i_cppMethod));\
	)


/** @ingroup ClassDefinition
 *  @brief Export a C++ method to Python with full control over overloading.
 *
 *  This is the most flexible method export macro, allowing manual dispatcher naming
 *  for creating overloaded Python methods. Multiple C++ methods can be exported
 *  with the same Python name to create overloaded methods.
 *
 *  @param t_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  Use this macro to export methods to Python. You can create overloaded Python methods 
 *  by exporting multiple C++ methods with the same s_methodName.
 *
 *  ```cpp
 *  class Foo {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      void barA(int a);
 *      void barB(const std::string& b) const;
 *      Foo operator+(const Foo& other) const;
 *  };
 *
 *  PY_DECLARE_CLASS(Foo)
 *  
 *  // Regular method export with custom name
 *  PY_CLASS_METHOD_EX(Foo, barA, "bar", nullptr, foo_bar_a)
 *  PY_CLASS_METHOD_EX(Foo, barB, "bar", nullptr, foo_bar_b)
 *  
 *  // Special method export using lass::python::methods constants
 *  PY_CLASS_METHOD_EX(Foo, operator+, lass::python::methods::_add_, nullptr, foo_add)
 *  
 *  // Python: foo_instance.bar(42) or foo_instance.bar("hello")
 *  // Python: result = foo_a + foo_b  (calls operator+)
 *  ```
 *
 *  @note For most special methods (like `__add__`, `__str__`, etc.), you **must** use the special
 *        method names defined in lass::python::methods namespace to ensure correct behavior.
 *        Regular string names like `"__add__"` will not work for these special methods.
 */
#define PY_CLASS_METHOD_EX(t_cppClass, i_cppMethod, s_methodName, s_doc, i_dispatcher)\
	PY_CLASS_METHOD_IMPL(t_cppClass, &TCppClass::i_cppMethod, s_methodName, s_doc, i_dispatcher,\
		::lass::python::impl::CallMethod<TShadowTraits>::call)

/** @ingroup ClassDefinition
 *  @brief Export a C++ method to Python with custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_EX() with auto-generated dispatcher name.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  ```cpp
 *  PY_CLASS_METHOD_NAME_DOC(Foo, calculate, "compute", "Performs calculation")
 *  // Python: foo_instance.compute()
 *  ```
 *  
 *  @sa PY_CLASS_METHOD_EX
 */
#define PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, s_doc )\
		PY_CLASS_METHOD_EX(\
			i_cppClass, i_cppMethod, s_methodName, s_doc,\
			LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export a C++ method to Python with custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_NAME_DOC() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  
 *  ```cpp
 *  PY_CLASS_METHOD_NAME(Foo, calculate, "compute")
 *  // Python: foo_instance.compute()
 *  ```
 *  
 *  @sa PY_CLASS_METHOD_EX
 */
#define PY_CLASS_METHOD_NAME( i_cppClass, i_cppMethod, s_methodName )\
		PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, 0 )

/** @ingroup ClassDefinition
 *  @brief Export a C++ method to Python using the C++ method name with documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_NAME_DOC() with s_methodName derived from i_cppMethod.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (name will be used as Python name)
 *  @param s_doc Method documentation string (null-terminated C string literal)
 *  
 *  ```cpp
 *  PY_CLASS_METHOD_DOC(Foo, calculate, "Performs calculation")
 *  // Python: foo_instance.calculate()
 *  ```
 *  
 *  @sa PY_CLASS_METHOD_EX
 */
#define PY_CLASS_METHOD_DOC( i_cppClass, i_cppMethod, s_doc )\
		PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, LASS_STRINGIFY(i_cppMethod), s_doc)

/** @ingroup ClassDefinition
 *  @brief Export a C++ method to Python using the C++ method name (no documentation).
 *  
 *  The simplest method export macro. Uses the C++ method name as the Python name
 *  and doesn't provide additional documentation.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (name will be used as Python name)
 *  
 *  ```cpp
 *  PY_CLASS_METHOD(Foo, calculate)
 *  // Python: foo_instance.calculate()
 *  ```
 *  
 *  @sa PY_CLASS_METHOD_EX
 */
#define PY_CLASS_METHOD( i_cppClass, i_cppMethod )\
		PY_CLASS_METHOD_DOC( i_cppClass, i_cppMethod, 0 )

/** @} */

// --- explicit qualified methods ------------------------------------------------------------------

/** @addtogroup ClassDefinition
 *  @name Type-Qualified Method Export Macros
 *
 *  These macros export C++ class methods to Python with explicit type qualification to resolve
 *  method overload ambiguities. They provide fine-grained control over method signatures
 *  and are essential when exporting overloaded methods that would otherwise be ambiguous.
 *
 *  Use these macros when you have overloaded C++ methods and need to specify exactly which
 *  overload to export to Python by providing explicit return and parameter types.
 *
 *  The macros are organized in layers:
 *  - PY_CLASS_METHOD_QUALIFIED_EX(): Full control with custom dispatcher and meta::TypeTuple for parameters
 *  - PY_CLASS_METHOD_QUALIFIED_EX_0() through PY_CLASS_METHOD_QUALIFIED_EX_15(): Full control with custom dispatcher, for 0-15 parameters
 *  - PY_CLASS_METHOD_QUALIFIED_NAME_DOC(): Automatic dispatcher with custom name and documentation, and meta::TypeTuple for parameters
 *  - PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0() through PY_CLASS_METHOD_QUALIFIED_NAME_DOC_15(): Automatic dispatcher with custom name, for 0-15 parameters
 *  - PY_CLASS_METHOD_QUALIFIED_NAME(): Automatic dispatcher with custom name, no documentation, and meta::TypeTuple for parameters
 *  - PY_CLASS_METHOD_QUALIFIED_NAME_0() through PY_CLASS_METHOD_QUALIFIED_NAME_15(): Automatic dispatcher with custom name, no documentation, for 0-15 parameters
 *  - PY_CLASS_METHOD_QUALIFIED_DOC(): Automatic dispatcher with documentation, and meta::TypeTuple for parameters
 *  - PY_CLASS_METHOD_QUALIFIED_DOC_0() through PY_CLASS_METHOD_QUALIFIED_DOC_15(): Automatic dispatcher with documentation, for 0-15 parameters
 *  - PY_CLASS_METHOD_QUALIFIED(): Automatic dispatcher, no documentation, and meta::TypeTuple for parameters
 *  - PY_CLASS_METHOD_QUALIFIED_0() through PY_CLASS_METHOD_QUALIFIED_15(): Automatic dispatcher, no documentation, for 0-15 parameters
 *
 *  @{
 */

/** @ingroup ClassDefinition
 *  @brief Export a C++ method to Python with explicit type qualification to resolve ambiguities.
 *
 *  Use this macro instead of PY_CLASS_METHOD_EX when there are overloaded C++ methods
 *  that would create ambiguity. By explicitly specifying return type and parameter types,
 *  you can disambiguate which overload to export.
 *
 *  @param t_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  This macro helps resolve method overload ambiguities by explicitly specifying
 *  the method signature to export. Overloads can be mixed with PY_CLASS_METHOD_EX methods.
 *
 *  ```cpp
 *  class Foo {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      void bar(int a);
 *      void bar(const std::string& b) const;
 *  };
 *
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_METHOD_QUALIFIED_EX(Foo, bar, void, meta::TypeTuple<int>, "bar", nullptr, foo_bar_a)
 *  PY_CLASS_METHOD_QUALIFIED_EX(Foo, bar, void, meta::TypeTuple<const std::string&>, "bar", nullptr, foo_bar_b)
 *  // Python: foo_instance.bar(42) or foo_instance.bar("hello")
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_EX(t_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc, i_dispatcher)\
	static ::lass::python::impl::OverloadLink LASS_CONCATENATE(i_dispatcher, _overloadChain);\
	extern "C" LASS_DLL_LOCAL PyObject* i_dispatcher(PyObject* iObject, PyObject* iArgs)\
	{\
		PyObject* result = 0;\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain)(iObject, iArgs, result))\
		{\
			return result;\
		}\
		LASS_ASSERT(result == 0);\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		return ::lass::python::impl::ExplicitResolver<TShadowTraits,t_return,t_params>::callMethod(\
			iArgs, iObject, &TCppClass::i_cppMethod); \
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX(LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addMethod(\
			s_methodName, s_doc, \
			::lass::python::impl::FunctionTypeDispatcher< SPECIAL_SLOT_TYPE(s_methodName) , i_dispatcher>::fun,\
			LASS_CONCATENATE(i_dispatcher, _overloadChain));\
	)
/**/

/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification for 0-parameter methods.
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_EX() for methods with 0 parameters.
 *  
 *  @param t_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (0 parameters, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_EX_0( t_cppClass, i_cppMethod, t_return, s_methodName, s_doc, i_dispatcher )\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		t_cppClass, i_cppMethod, t_return, ::lass::meta::TypeTuple<>, s_methodName, s_doc, i_dispatcher )
$[
/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification for $x-parameter methods.
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_EX() for methods with exactly $x parameters.
 *  
 *  @param t_cppClass C++ class containing the method  
 *  @param i_cppMethod C++ method name to export ($x parameters, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the method (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_EX_$x( t_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc, i_dispatcher )\
	typedef ::lass::meta::TypeTuple< $(t_P$x)$ > \
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams));\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		t_cppClass, i_cppMethod, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams)), s_methodName, s_doc,\
		i_dispatcher )
]$

/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification and custom name with documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_EX() with automatically generated dispatcher name.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		i_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification for 0-parameter methods with custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_EX_0() with automatically generated dispatcher name.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (0 parameters, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0( i_cppClass, i_cppMethod, t_return, s_methodName, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_EX_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
$[
/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification for $x-parameter methods with custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_EX_$x() with automatically generated dispatcher name.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export ($x parameters, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the method (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_EX_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
]$

/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification and custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME( i_cppClass, i_cppMethod, t_return, t_params, s_methodName )\
		PY_CLASS_METHOD_QUALIFIED_NAME_DOC(\
			i_cppClass, i_cppMethod, t_return, t_params, s_methodName, 0 )

/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification for 0-parameter methods and custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (0 parameters, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_0( i_cppClass, i_cppMethod, t_return, s_methodName )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, 0 )
$[
/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification for $x-parameter methods and custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export ($x parameters, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the method (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, 0 )
]$

/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification and custom documentation (method name derived from C++ method).
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC() with s_methodName = LASS_STRINGIFY(i_cppMethod).
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (used as Python method name, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC(\
		i_cppClass, i_cppMethod, t_return, t_params, LASS_STRINGIFY(i_cppMethod), s_doc )

/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification for 0-parameter methods and custom documentation (method name derived from C++ method).
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0() with s_methodName = LASS_STRINGIFY(i_cppMethod).
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (0 parameters, used as Python method name, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppMethod, t_return, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, LASS_STRINGIFY(i_cppMethod), s_doc )
$[
/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification for $x-parameter methods and custom documentation (method name derived from C++ method).
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x() with s_methodName = LASS_STRINGIFY(i_cppMethod).
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export ($x parameters, used as Python method name, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the method (for disambiguation)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, LASS_STRINGIFY(i_cppMethod), s_doc )
]$

/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification (method name derived from C++ method, no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_DOC() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (used as Python method name, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED( i_cppClass, i_cppMethod, t_return, t_params )\
	PY_CLASS_METHOD_QUALIFIED_DOC( i_cppClass, i_cppMethod, t_return, t_params, 0 )

/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification for 0-parameter methods (method name derived from C++ method, no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_DOC_0() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export (0 parameters, used as Python method name, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_0( i_cppClass, i_cppMethod, t_return )\
	PY_CLASS_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppMethod, t_return, 0 )
$[
/** @ingroup ClassDefinition
 *  @brief Export a C++ method with type qualification for $x-parameter methods (method name derived from C++ method, no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_METHOD_QUALIFIED_DOC_$x() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class containing the method
 *  @param i_cppMethod C++ method name to export ($x parameters, used as Python method name, can be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the method (for disambiguation)
 *  
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_METHOD_QUALIFIED_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$ )\
	PY_CLASS_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, 0 )
]$

/** @} */

// --- "free" methods ------------------------------------------------------------------------------

/** @addtogroup ClassDefinition
 *  @name Free Method Export Macros
 *
 *  Export C/C++ free functions as Python methods. The first parameter of the free function 
 *  becomes the implicit 'self' parameter and must be a pointer or reference (const or non-const)
 *  to the class being exported. This is particularly useful for shadow classes where adding
 *  methods directly to the class is undesirable or impossible.
 *
 *  @{
 */

/** @ingroup ClassDefinition
 *  @brief Export a C/C++ free function as a Python method with full control over all parameters.
 *  
 *  This macro allows you to export a C/C++ free function as a Python method. The free function
 *  must accept a pointer or reference to the object as first argument (const or non-const).
 *  This is extremely useful when using shadow classes to export a C++ class, because in such
 *  cases, it's often undesirable or impossible to write the function as a method.
 *  
 *  Like PY_CLASS_METHOD_EX(), this macro supports overloading. These overloads may be mixed
 *  with PY_CLASS_METHOD_EX() methods.
 *  
 *  @param t_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *  
 *  ```cpp
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  };
 *
 *  void barA(Foo* foo, int a);
 *  void barB(const Foo&, const std::string& b);
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_FREE_METHOD_EX(Foo, barA, "bar", nullptr, foo_bar_a)
 *  PY_CLASS_FREE_METHOD_EX(Foo, barB, "bar", nullptr, foo_bar_b)
 *  ```
 */
#define PY_CLASS_FREE_METHOD_EX(t_cppClass, f_cppFreeMethod, s_methodName, s_doc, i_dispatcher)\
	PY_CLASS_METHOD_IMPL(t_cppClass, f_cppFreeMethod, s_methodName, s_doc, i_dispatcher,\
		::lass::python::impl::CallMethod<TShadowTraits>::callFree)

/** @ingroup ClassDefinition
 *  @brief Export a C/C++ free function as a Python method with custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_EX() with automatically generated dispatcher name.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_METHOD_EX
 */
#define PY_CLASS_FREE_METHOD_NAME_DOC( i_cppClass, f_cppFreeMethod, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_EX(\
		i_cppClass, f_cppFreeMethod, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export a C/C++ free function as a Python method with custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_NAME_DOC() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  
 *  @sa PY_CLASS_FREE_METHOD_EX
 */
#define PY_CLASS_FREE_METHOD_NAME( i_cppClass, f_cppFreeMethod, s_methodName )\
	PY_CLASS_FREE_METHOD_NAME_DOC( i_cppClass, f_cppFreeMethod, s_methodName, 0 )

/** @ingroup ClassDefinition
 *  @brief Export a C/C++ free function as a Python method with custom documentation (method name derived from function name).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_NAME_DOC() with s_methodName = LASS_STRINGIFY(i_cppFreeMethod).
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier, used as Python method name)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_METHOD_EX
 */
#define PY_CLASS_FREE_METHOD_DOC( i_cppClass, i_cppFreeMethod, s_doc )\
	PY_CLASS_FREE_METHOD_NAME_DOC( i_cppClass, i_cppFreeMethod, LASS_STRINGIFY(i_cppFreeMethod), s_doc)

/** @ingroup ClassDefinition
 *  @brief Export a C/C++ free function as a Python method (method name derived from function name, no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_DOC() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier, used as Python method name)
 *  
 *  @sa PY_CLASS_FREE_METHOD_EX
 */
#define PY_CLASS_FREE_METHOD( i_cppClass, i_cppFreeMethod )\
	PY_CLASS_FREE_METHOD_DOC( i_cppClass, i_cppFreeMethod, 0 )

/** @} */

/** @addtogroup ClassDefinition
 *  @name Type-Qualified Free Method Export Macros
 *
 *  Export C++ free functions as Python methods with explicit type qualification to resolve 
 *  overload ambiguity. These macros require explicit specification of return type and parameter 
 *  types, making them suitable for overloaded free functions. The first parameter of the free 
 *  function becomes the implicit 'self' parameter and must be a pointer or reference (const or 
 *  non-const) to the class being exported.
 *
 *  @{
 */

/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with explicit type qualification and full parameter control.
 *  
 *  This macro allows you to export a C++ free function as a Python method when there's 
 *  ambiguity due to overloading. It explicitly specifies return type and parameter types 
 *  to resolve such ambiguity. The free function must accept a pointer or reference to the 
 *  object as first argument (const or non-const).
 *  
 *  Like PY_CLASS_FREE_METHOD_EX(), this macro supports overloading and these overloads 
 *  may be mixed with PY_CLASS_FREE_METHOD_EX() methods.
 *  
 *  @param t_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *  
 *  ```cpp
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      // ...
 *  };
 *
 *  void bar(Foo& foo, int a);
 *  void bar(const Foo&, const std::string& b);
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_FREE_METHOD_QUALIFIED_EX(Foo, bar, void, meta::TypeTuple<int>, "bar", nullptr, foo_bar_a)
 *  PY_CLASS_FREE_METHOD_QUALIFIED_EX(Foo, bar, void, meta::TypeTuple<const std::string&>, "bar", nullptr, foo_bar_b)
 *  ```
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_EX(t_cppClass, f_cppFreeMethod, t_return, t_params, s_methodName, s_doc, i_dispatcher)\
	static ::lass::python::impl::OverloadLink LASS_CONCATENATE(i_dispatcher, _overloadChain);\
	extern "C" LASS_DLL_LOCAL PyObject* i_dispatcher(PyObject* iObject, PyObject* iArgs)\
	{\
		PyObject* result = 0;\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain)(iObject, iArgs, result))\
		{\
			return result;\
		}\
		LASS_ASSERT(result == 0);\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		return ::lass::python::impl::ExplicitResolver<TShadowTraits,t_return,t_params>::callFreeMethod(\
			iArgs, iObject, f_cppFreeMethod); \
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX(LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addMethod(\
			s_methodName, s_doc, \
			::lass::python::impl::FunctionTypeDispatcher< SPECIAL_SLOT_TYPE(s_methodName) , i_dispatcher>::fun,\
			LASS_CONCATENATE(i_dispatcher, _overloadChain));\
	)

/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification for 0-parameter functions.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX() for free functions with 0 parameters.
 *  
 *  @param t_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function, 0 parameters, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_EX_0( t_cppClass, f_cppFreeMethod, t_return, s_methodName, s_doc, i_dispatcher )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX(\
		t_cppClass, f_cppFreeMethod, t_return, ::lass::meta::TypeTuple<>, s_methodName, s_doc, i_dispatcher )
$[
/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification for $x-parameter functions.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX() for free functions with $x parameters.
 *  
 *  @param t_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function, $x parameters, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the free function (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_EX_$x( t_cppClass, f_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, s_doc, i_dispatcher )\
	typedef ::lass::meta::TypeTuple< $(t_P$x)$ > \
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams));\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX(\
		t_cppClass, f_cppFreeMethod, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams)), s_methodName, s_doc,\
		i_dispatcher )
]$

/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification, custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX() with automatically generated dispatcher name.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC( i_cppClass, f_cppFreeMethod, t_return, t_params, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX(\
		i_cppClass, f_cppFreeMethod, t_return, t_params, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification for 0-parameter functions, custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX_0() with automatically generated dispatcher name.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function, 0 parameters, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0( i_cppClass, f_cppFreeMethod, t_return, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX_0(\
		i_cppClass, f_cppFreeMethod, t_return, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
$[
/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification for $x-parameter functions, custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX_$x() with automatically generated dispatcher name.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function, $x parameters, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the free function (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x( i_cppClass, f_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX_$x(\
		i_cppClass, f_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
]$

/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification and custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME( i_cppClass, f_cppFreeMethod, t_return, t_params, s_methodName )\
		PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC(\
			i_cppClass, f_cppFreeMethod, t_return, t_params, s_methodName, 0 )

/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification for 0-parameter functions and custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function, 0 parameters, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_0( i_cppClass, f_cppFreeMethod, t_return, s_methodName )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, f_cppFreeMethod, t_return, s_methodName, 0 )
$[
/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification for $x-parameter functions and custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (can be function pointer or std::function, $x parameters, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the free function (for disambiguation)
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_$x( i_cppClass, f_cppFreeMethod, t_return, $(t_P$x)$, s_methodName )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, f_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, 0 )
]$

/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification and custom documentation (method name derived from function name).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC() with s_methodName = LASS_STRINGIFY(i_cppFreeMethod).
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier, used as Python method name, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_DOC( i_cppClass, i_cppFreeMethod, t_return, t_params, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC(\
		i_cppClass, i_cppFreeMethod, t_return, t_params, LASS_STRINGIFY(i_cppFreeMethod), s_doc )

/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification for 0-parameter functions and custom documentation (method name derived from function name).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0() with s_methodName = LASS_STRINGIFY(i_cppFreeMethod).
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier, 0 parameters, used as Python method name, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppFreeMethod, t_return, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, i_cppFreeMethod, t_return, LASS_STRINGIFY(i_cppFreeMethod), s_doc )
$[
/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification for $x-parameter functions and custom documentation (method name derived from function name).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x() with s_methodName = LASS_STRINGIFY(i_cppFreeMethod).
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier, $x parameters, used as Python method name, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the free function (disambiguation)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, LASS_STRINGIFY(i_cppFreeMethod), s_doc )
]$

/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification (method name derived from function name, no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_DOC() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier, used as Python method name, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED( i_cppClass, i_cppFreeMethod, t_return, t_params )\
	PY_CLASS_FREE_METHOD_QUALIFIED_DOC( i_cppClass, i_cppFreeMethod, t_return, t_params, 0 )

/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification for 0-parameter functions (method name derived from function name, no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_DOC_0() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier, 0 parameters, used as Python method name, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_0( i_cppClass, i_cppFreeMethod, t_return )\
	PY_CLASS_FREE_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppFreeMethod, t_return, 0 )
$[
/** @ingroup ClassDefinition
 *  @brief Export a C++ free function as a Python method with type qualification for $x-parameter functions (method name derived from function name, no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_METHOD_QUALIFIED_DOC_$x() with s_doc = nullptr.
 *  
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier, $x parameters, used as Python method name, can be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the free function (for disambiguation)
 *  
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$ )\
	PY_CLASS_FREE_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, 0 )
]$

/** @} */

// --- "casting" methods ------------------------------------------------------------------------------

/** @addtogroup ClassDefinition
 *  @name Casting Method Export Macros (Deprecated)
 *
 *  @deprecated These casting method macros are deprecated. Use the regular or free method export macros instead.
 *
 *  Export C++ methods to Python with custom casting policies for type conversion. These macros
 *  allow on-the-fly type conversion using casting operators like PointerCast and CopyCast.
 *  However, these macros are deprecated and should be avoided in new code.
 *
 *  @{
 */

/** @ingroup ClassDefinition
 *  @brief Export a C++ method to Python with custom casting policy and full parameter control.
 *  @deprecated Use PY_CLASS_METHOD_EX() or PY_CLASS_FREE_METHOD_EX() instead.
 *
 *  This macro exports a C++ method to Python with a custom casting policy for type conversion.
 *  It allows on-the-fly conversion using casting operators. However, this macro is deprecated
 *  and should not be used in new code.
 *
 *  Supported casting operators:
 *  - `PointerCast<_type>`: Interprets ownership rules as if a pointer is passed
 *  - `CopyCast<_type>`: Interprets ownership rules as if a copy of the object is passed
 *  - Custom casting operators can be defined following the PointerCast pattern
 *
 *  @param t_cppClass C++ class you're exporting a method of
 *  @param i_cppMethod Name of the method in C++
 *  @param t_return Return type of the method
 *  @param t_params lass::meta::TypeTuple of the parameter types
 *  @param s_methodName Python method name (null-terminated C string literal or special method from lass::python::methods namespace)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *  @param i_typename Type name for casting operations
 *
 *  ```cpp
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      void bar(int a);
 *      void bar(const std::string& b) const;
 *  };
 *
 *  // foo.cpp - DEPRECATED USAGE
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_METHOD_CAST_EX_0(Foo, bar, void, CopyCast<const std::string&>, "bar", nullptr, foo_bar_a)
 *  ```
 */


/** @ingroup ClassDefinition
 *  @brief Export a C++ method with casting policy for 0-parameter methods.
 *  @deprecated Use PY_CLASS_METHOD_EX() or PY_CLASS_FREE_METHOD_EX() instead.
 */
#define PY_CLASS_METHOD_CAST_EX_0(t_cppClass, i_cppMethod, t_return, s_methodName, s_doc, i_dispatcher, i_typename) \
	::lass::python::OwnerCaster<t_return>::TCaster::TTarget LASS_CONCATENATE(i_dispatcher, _caster) ( \
	::lass::python::impl::ShadowTraits< t_cppClass >::TCppClass& iThis\
	)\
	{\
 		return iThis.i_cppMethod () ; \
	}\
	PY_CLASS_FREE_METHOD_EX( t_cppClass, LASS_CONCATENATE(i_dispatcher, _caster), s_methodName, s_doc, i_dispatcher );


 $[
/** @ingroup ClassDefinition
 *  @brief Export a C++ method with casting policy for $x-parameter methods.
 *  @deprecated Use PY_CLASS_METHOD_EX() or PY_CLASS_FREE_METHOD_EX() instead.
 */
 #define PY_CLASS_METHOD_CAST_EX_$x( t_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc, i_dispatcher, i_typename )\
	::lass::python::OwnerCaster< t_return >::TCaster::TTarget LASS_CONCATENATE(i_dispatcher, _caster) ( \
	::lass::python::impl::ShadowTraits< t_cppClass >::TCppClass& iThis,\
	$(::lass::python::OwnerCaster< t_P$x >::TCaster::TTarget iArg$x)$ \
	)\
	{\
 		return iThis.i_cppMethod ( $(::lass::python::OwnerCaster< t_P$x >::TCaster::cast(iArg$x))$ );\
	}\
	PY_CLASS_FREE_METHOD_EX( t_cppClass, LASS_CONCATENATE(i_dispatcher, _caster), s_methodName, s_doc, i_dispatcher );
 ]$

/** @ingroup ClassDefinition
 *  @brief Convenience wrapper for PY_CLASS_METHOD_CAST_EX_0().
 *  @deprecated Use PY_CLASS_METHOD_NAME_DOC() or PY_CLASS_FREE_METHOD_NAME_DOC() instead.
 */
#define PY_CLASS_METHOD_CAST_NAME_DOC_0( i_cppClass, i_cppMethod, t_return, s_methodName, s_doc )\
	PY_CLASS_METHOD_CAST_EX_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)),\
		LASS_UNIQUENAME(LASS_CONCATENATE(TypelassPyImpl_method_, i_cppClass)))
$[
/** @ingroup ClassDefinition
 *  @brief Convenience wrapper for PY_CLASS_METHOD_CAST_EX_$x().
 *  @deprecated Use PY_CLASS_METHOD_NAME_DOC() or PY_CLASS_FREE_METHOD_NAME_DOC() instead.
 */
#define PY_CLASS_METHOD_CAST_NAME_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc )\
	PY_CLASS_METHOD_CAST_EX_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)),\
		LASS_UNIQUENAME(LASS_CONCATENATE(TypelassPyImpl_method_, i_cppClass)))
]$

/** @ingroup ClassDefinition
 *  @brief Convenience wrapper with no documentation.
 *  @deprecated Use PY_CLASS_METHOD_NAME() or PY_CLASS_FREE_METHOD_NAME() instead.
 */
#define PY_CLASS_METHOD_CAST_NAME( i_cppClass, i_cppMethod, t_return, t_params, s_methodName )\
		PY_CLASS_METHOD_CAST_NAME_DOC(\
			i_cppClass, i_cppMethod, t_return, t_params, s_methodName, 0 )

/** @ingroup ClassDefinition
 *  @brief Convenience wrapper with no documentation.
 *  @deprecated Use PY_CLASS_METHOD_NAME() or PY_CLASS_FREE_METHOD_NAME() instead.
 */
#define PY_CLASS_METHOD_CAST_NAME_0( i_cppClass, i_cppMethod, t_return, s_methodName )\
	PY_CLASS_METHOD_CAST_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, 0 )
$[
/** @ingroup ClassDefinition
 *  @brief Convenience wrapper with no documentation.
 *  @deprecated Use PY_CLASS_METHOD_NAME() or PY_CLASS_FREE_METHOD_NAME() instead.
 */
#define PY_CLASS_METHOD_CAST_NAME_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName )\
	PY_CLASS_METHOD_CAST_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, 0 )
]$

/** @ingroup ClassDefinition
 *  @brief Convenience wrapper with method name derived from C++ method.
 *  @deprecated Use PY_CLASS_METHOD_DOC() or PY_CLASS_FREE_METHOD_DOC() instead.
 */
#define PY_CLASS_METHOD_CAST_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_doc )\
	PY_CLASS_METHOD_CAST_NAME_DOC(\
		i_cppClass, i_cppMethod, t_return, t_params, LASS_STRINGIFY(i_cppMethod), s_doc )

/** @ingroup ClassDefinition
 *  @brief Convenience wrapper with method name derived from C++ method.
 *  @deprecated Use PY_CLASS_METHOD_DOC() or PY_CLASS_FREE_METHOD_DOC() instead.
 */
#define PY_CLASS_METHOD_CAST_DOC_0( i_cppClass, i_cppMethod, t_return, s_doc )\
	PY_CLASS_METHOD_CAST_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, LASS_STRINGIFY(i_cppMethod), s_doc )
$[
/** @ingroup ClassDefinition
 *  @brief Convenience wrapper with method name derived from C++ method.
 *  @deprecated Use PY_CLASS_METHOD_DOC() or PY_CLASS_FREE_METHOD_DOC() instead.
 */
#define PY_CLASS_METHOD_CAST_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_doc )\
	PY_CLASS_METHOD_CAST_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, LASS_STRINGIFY(i_cppMethod), s_doc )
]$

/** @ingroup ClassDefinition
 *  @brief Basic convenience wrapper with minimal parameters.
 *  @deprecated Use PY_CLASS_METHOD() or PY_CLASS_FREE_METHOD() instead.
 */
#define PY_CLASS_METHOD_CAST( i_cppClass, i_cppMethod, t_return, t_params )\
	PY_CLASS_METHOD_CAST_DOC( i_cppClass, i_cppMethod, t_return, t_params, 0 )

/** @ingroup ClassDefinition
 *  @brief Basic convenience wrapper with minimal parameters.
 *  @deprecated Use PY_CLASS_METHOD() or PY_CLASS_FREE_METHOD() instead.
 */
#define PY_CLASS_METHOD_CAST_0( i_cppClass, i_cppMethod, t_return )\
	PY_CLASS_METHOD_CAST_DOC_0( i_cppClass, i_cppMethod, t_return, 0 )
$[
/** @ingroup ClassDefinition
 *  @brief Basic convenience wrapper with minimal parameters.
 *  @deprecated Use PY_CLASS_METHOD() or PY_CLASS_FREE_METHOD() instead.
 */
#define PY_CLASS_METHOD_CAST_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$ )\
	PY_CLASS_METHOD_CAST_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, 0 )
]$

/** @} */

// --- static methods ------------------------------------------------------------------------------

/** @addtogroup ClassDefinition
 *  @name Static Method Export Macros
 *
 *  Export C++ static methods or free functions as Python static methods (class methods).
 *  Static methods are called on the class itself rather than on instances, and do not
 *  receive an implicit 'self' parameter. Both C++ static member functions and free
 *  functions can be exported as static methods.
 *
 *  @{
 */


/** @ingroup ClassDefinition
 *  @brief Export a C++ function as a Python static method with full parameter control.
 *
 *  This macro exports a C++ static method or free function as a Python static method (class method).
 *  Static methods are called on the class itself rather than on instances, and can access class
 *  attributes but not instance attributes. Both C++ static member functions and free functions
 *  can be exported as static methods.
 *
 *  @param t_cppClass C++ class to add the static method to
 *  @param f_cppFunction Full name of the C++ function that implements the static method
 *  @param s_methodName Python method name (null-terminated C string literal)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  ```cpp
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      static void bar(int iA);
 *  };
 *
 *  int spam(int iB, int iC);
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_STATIC_METHOD_EX(Foo, Foo::bar, "bar", "a regular C++ static method", foo_bar)
 *  PY_CLASS_STATIC_METHOD_EX(Foo, spam, "spam", "free function as static method", foo_spam)
 *  ```
 */
#define PY_CLASS_STATIC_METHOD_EX( t_cppClass, f_cppFunction, s_methodName, s_doc, i_dispatcher )\
	static PyCFunction LASS_CONCATENATE(i_dispatcher, _overloadChain) = 0;\
	extern "C" LASS_DLL_LOCAL PyObject* i_dispatcher( PyObject* iIgnore, PyObject* iArgs )\
	{\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain))\
		{\
			PyObject* result = LASS_CONCATENATE(i_dispatcher, _overloadChain)(iIgnore, iArgs);\
			if (!(PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError)))\
			{\
				return result;\
			}\
			PyErr_Clear();\
			Py_XDECREF(result);\
		}\
		return ::lass::python::impl::callFunction( iArgs, f_cppFunction );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _excecuteBeforeMain ),\
		t_cppClass ::_lassPyClassDef.addStaticMethod(\
			s_methodName, s_doc, i_dispatcher, LASS_CONCATENATE(i_dispatcher, _overloadChain));\
	)

/** @ingroup ClassDefinition
 *  @brief Export a C++ static method with custom documentation (method name derived from C++ method).
 *  
 *  Convenience macro that wraps PY_CLASS_STATIC_METHOD_EX() for C++ static member functions
 *  with automatically generated dispatcher name and method name derived from C++ method name.
 *  
 *  @param i_cppClass C++ class to add the static method to
 *  @param i_cppMethod Name of the C++ static method to export
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_STATIC_METHOD_EX
 */
#define PY_CLASS_STATIC_METHOD_DOC( i_cppClass, i_cppMethod, s_doc )\
	PY_CLASS_STATIC_METHOD_EX(\
		i_cppClass,\
		&::lass::python::impl::ShadowTraits<i_cppClass>::TCppClass::i_cppMethod,\
		LASS_STRINGIFY(i_cppMethod), s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_staticMethod_, i_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export a C++ static method with custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_STATIC_METHOD_EX() for C++ static member functions
 *  with automatically generated dispatcher name.
 *  
 *  @param i_cppClass C++ class to add the static method to
 *  @param i_cppMethod Name of the C++ static method to export
 *  @param s_methodName Python method name (null-terminated C string literal)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_STATIC_METHOD_EX
 */
#define PY_CLASS_STATIC_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, s_doc )\
	PY_CLASS_STATIC_METHOD_EX(\
		i_cppClass,\
		&::lass::python::impl::ShadowTraits<i_cppClass>::TCppClass::i_cppMethod,\
		s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_staticMethod_, i_cppClass)))


/** @ingroup ClassDefinition
 *  @brief Export a C++ static method with custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_STATIC_METHOD_EX() for C++ static member functions
 *  with automatically generated dispatcher name and no documentation.
 *  
 *  @param i_cppClass C++ class to add the static method to
 *  @param i_cppMethod Name of the C++ static method to export
 *  @param s_methodName Python method name (null-terminated C string literal)
 *  
 *  @sa PY_CLASS_STATIC_METHOD_EX
 */
#define PY_CLASS_STATIC_METHOD_NAME( i_cppClass, i_cppMethod, s_methodName)\
	PY_CLASS_STATIC_METHOD_EX(\
		i_cppClass,\
		&::lass::python::impl::ShadowTraits<i_cppClass>::TCppClass::i_cppMethod,\
		s_methodName, "",\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_staticMethod_, i_cppClass)))


/** @ingroup ClassDefinition
 *  @brief Export a C++ static method (method name derived from C++ method, no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_STATIC_METHOD_DOC() with no documentation.
 *  
 *  @param i_cppClass C++ class to add the static method to
 *  @param i_cppMethod Name of the C++ static method to export (used as Python method name)
 *  
 *  @sa PY_CLASS_STATIC_METHOD_EX
 */
#define PY_CLASS_STATIC_METHOD( i_cppClass, i_cppMethod )\
		PY_CLASS_STATIC_METHOD_DOC( i_cppClass, i_cppMethod, 0 )

/** @} */

// --- data members --------------------------------------------------------------------------------

/** @addtogroup ClassDefinition
 *  @name Data Member Export Macros
 *
 *  Export C++ class data members as Python properties/attributes.
 *  These macros create Python properties that provide access to C++ class
 *  data through various access patterns: getter/setter methods, free functions,
 *  or direct public member access.
 *
 *  | Member Methods (Read/Write)   | Member Methods (Read-only)   | Free Functions (Read/Write)        | Free Functions (Read-only)        | Public Member (Read/Write)        | Public Member (Read-only)           |
 *  |-------------------------------|------------------------------|------------------------------------|-----------------------------------|-----------------------------------|-------------------------------------|
 *  | PY_CLASS_MEMBER_RW_EX()       | PY_CLASS_MEMBER_R_EX()       | PY_CLASS_FREE_MEMBER_RW_EX()       | PY_CLASS_FREE_MEMBER_R_EX()       | PY_CLASS_PUBLIC_MEMBER_EX()       | PY_CLASS_PUBLIC_MEMBER_R_EX()       |
 *  | PY_CLASS_MEMBER_RW_NAME_DOC() | PY_CLASS_MEMBER_R_NAME_DOC() | PY_CLASS_FREE_MEMBER_RW_NAME_DOC() | PY_CLASS_FREE_MEMBER_R_NAME_DOC() | PY_CLASS_PUBLIC_MEMBER_NAME_DOC() | PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC() |
 *  | PY_CLASS_MEMBER_RW_NAME()     | PY_CLASS_MEMBER_R_NAME()     | PY_CLASS_FREE_MEMBER_RW_NAME()     | PY_CLASS_FREE_MEMBER_R_NAME()     | PY_CLASS_PUBLIC_MEMBER_NAME()     | PY_CLASS_PUBLIC_MEMBER_R_NAME()     |
 *  | PY_CLASS_MEMBER_RW_DOC()      | PY_CLASS_MEMBER_R_DOC()      | PY_CLASS_FREE_MEMBER_RW_DOC()      | PY_CLASS_FREE_MEMBER_R_DOC()      | PY_CLASS_PUBLIC_MEMBER_DOC()      | PY_CLASS_PUBLIC_MEMBER_R_DOC()      |
 *  | PY_CLASS_MEMBER_RW()          | PY_CLASS_MEMBER_R()          | PY_CLASS_FREE_MEMBER_RW()          | PY_CLASS_FREE_MEMBER_R()          | PY_CLASS_PUBLIC_MEMBER()          | PY_CLASS_PUBLIC_MEMBER_R()          |
 *
 *  @{
 */


/** @ingroup ClassDefinition
 *  @brief Export getter/setter method pair as read/write Python property with full control.
 *
 *  This is the most flexible read-write member export macro, allowing manual dispatcher naming.
 *  Exports a pair of C++ getter and setter methods as a Python read/write property.
 *
 *  @param t_cppClass C++ class containing the getter and setter methods
 *  @param i_cppGetter C++ method name used to get the attribute value  
 *  @param i_cppSetter C++ method name used to set the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  ```cpp
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      const int getBar() const { return bar_; }
 *      int setBar(int iBar) { bar_ = iBar; }
 *      const std::string& spam() const { return spam_; }
 *      std::string& spam() const { return spam_; }
 *  private:
 *      int bar_;
 *      std::string spam_;
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_MEMBER_RW_EX(Foo, getBar, setBar, "bar", "regular get and setter", foo_bar)
 *  PY_CLASS_MEMBER_RW_EX(Foo, spam, spam, "spam", "cool get and setter", foo_spam)
 *  
 *  // Python: foo_instance.bar = 42; print(foo_instance.bar)
 *  // Python: foo_instance.spam = "hello"; print(foo_instance.spam)
 *  ```
 */
#define PY_CLASS_MEMBER_RW_EX( t_cppClass, i_cppGetter, i_cppSetter, s_memberName, s_doc, i_dispatcher)\
	extern "C" LASS_DLL_LOCAL PyObject* LASS_CONCATENATE(i_dispatcher, _getter)( PyObject* iObject, void* )\
	{\
		try \
		{ \
			typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
			TShadowTraits::TCppClassPtr self; \
			if (TShadowTraits::getObject(iObject, self) == 0)\
			{ \
					return ::lass::python::pyBuildSimpleObject(self->i_cppGetter()); \
			} \
			PyErr_Clear(); \
			TShadowTraits::TConstCppClassPtr constSelf; \
			if (TShadowTraits::getObject(iObject, constSelf) == 0)\
			{ \
					return ::lass::python::pyBuildSimpleObject(constSelf->i_cppGetter()); \
			} \
			return 0; \
		} \
		LASS_PYTHON_CATCH_AND_RETURN \
	}\
	extern "C" LASS_DLL_LOCAL int LASS_CONCATENATE(i_dispatcher, _setter)( PyObject* iObject, PyObject* iArgs, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		return ::lass::python::impl::CallMethod<TShadowTraits>::set( iArgs, iObject, &TCppClass::i_cppSetter );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), LASS_CONCATENATE(i_dispatcher, _setter));\
	)

/** @ingroup ClassDefinition
 *  @brief Export getter/setter method pair as read/write Python property with custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_MEMBER_RW_EX() with auto-generated dispatcher name.
 *  
 *  @param t_cppClass C++ class containing the getter and setter methods
 *  @param i_cppGetter C++ method name used to get the attribute value  
 *  @param i_cppSetter C++ method name used to set the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_MEMBER_RW_EX
 */
#define PY_CLASS_MEMBER_RW_NAME_DOC(t_cppClass, i_cppGetter, i_cppSetter, s_memberName, s_doc)\
	PY_CLASS_MEMBER_RW_EX(t_cppClass, i_cppGetter, i_cppSetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_memberRW, t_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export getter/setter method pair as read/write Python property with custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_MEMBER_RW_NAME_DOC() with s_doc = nullptr.
 *  
 *  @param t_cppClass C++ class containing the getter and setter methods
 *  @param i_cppGetter C++ method name used to get the attribute value  
 *  @param i_cppSetter C++ method name used to set the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  
 *  @sa PY_CLASS_MEMBER_RW_EX
 */
#define PY_CLASS_MEMBER_RW_NAME(t_cppClass, i_cppGetter, i_cppSetter, s_memberName)\
	PY_CLASS_MEMBER_RW_NAME_DOC(t_cppClass, i_cppGetter, i_cppSetter, s_memberName, 0)

/** @ingroup ClassDefinition
 *  @brief Export getter/setter method pair as read/write Python property (name derived from getter, with documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_MEMBER_RW_NAME_DOC() with s_memberName derived from i_cppGetter.
 *  
 *  @param t_cppClass C++ class containing the getter and setter methods
 *  @param i_cppGetter C++ method name used to get the attribute value (also used as Python property name)
 *  @param i_cppSetter C++ method name used to set the attribute value
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_MEMBER_RW_EX
 */
#define PY_CLASS_MEMBER_RW_DOC(t_cppClass, i_cppGetter, i_cppSetter, s_doc)\
	PY_CLASS_MEMBER_RW_NAME_DOC(t_cppClass, i_cppGetter, i_cppSetter, LASS_STRINGIFY(i_cppGetter), s_doc)

/** @ingroup ClassDefinition
 *  @brief Export getter/setter method pair as read/write Python property (name derived from getter, no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_MEMBER_RW_DOC() with s_doc = nullptr.
 *  
 *  @param t_cppClass C++ class containing the getter and setter methods
 *  @param i_cppGetter C++ method name used to get the attribute value (also used as Python property name)
 *  @param i_cppSetter C++ method name used to set the attribute value
 *  
 *  @sa PY_CLASS_MEMBER_RW_EX
 */
#define PY_CLASS_MEMBER_RW(t_cppClass, i_cppGetter, i_cppSetter)\
	PY_CLASS_MEMBER_RW_DOC(t_cppClass, i_cppGetter, i_cppSetter, 0)



/** @ingroup ClassDefinition
 *  @brief Export getter method as read-only Python property with full control.
 *
 *  This is the most flexible read-only member export macro, allowing manual dispatcher naming.
 *  Exports a C++ getter method as a Python read-only property (no setter provided).
 *
 *  @param t_cppClass C++ class containing the getter method
 *  @param i_cppGetter C++ method name used to get the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  ```cpp
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      const int getBar() const { return bar_; }
 *  private:
 *      int bar_;
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_MEMBER_R_EX(Foo, getBar, "bar", "read-only property", foo_bar_getter)
 *  
 *  // Python: print(foo_instance.bar)  # Read-only access
 *  ```
 */
#define PY_CLASS_MEMBER_R_EX( t_cppClass, i_cppGetter, s_memberName, s_doc, i_dispatcher )\
	extern "C" LASS_DLL_LOCAL PyObject* LASS_CONCATENATE(i_dispatcher, _getter)( PyObject* iObject, void* )\
	{\
		try \
		{ \
			typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
			TShadowTraits::TCppClassPtr self; \
			if (TShadowTraits::getObject(iObject, self) == 0)\
			{ \
					return ::lass::python::pyBuildSimpleObject(self->i_cppGetter()); \
			} \
			PyErr_Clear(); \
			TShadowTraits::TConstCppClassPtr constSelf; \
			if (TShadowTraits::getObject(iObject, constSelf) == 0)\
			{ \
					return ::lass::python::pyBuildSimpleObject(constSelf->i_cppGetter()); \
			} \
			return 0; \
		} \
		LASS_PYTHON_CATCH_AND_RETURN \
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), 0);\
	)


/** @ingroup ClassDefinition
 *  @brief Export getter method as read-only Python property with custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_MEMBER_R_EX() with auto-generated dispatcher name.
 *  
 *  @param t_cppClass C++ class containing the getter method
 *  @param i_cppGetter C++ method name used to get the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_MEMBER_R_EX
 */
#define PY_CLASS_MEMBER_R_NAME_DOC(t_cppClass, i_cppGetter, s_memberName, s_doc)\
	PY_CLASS_MEMBER_R_EX(t_cppClass, i_cppGetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_memberR, t_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export getter method as read-only Python property with custom name (no documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_MEMBER_R_NAME_DOC() with s_doc = nullptr.
 *  
 *  @param t_cppClass C++ class containing the getter method
 *  @param i_cppGetter C++ method name used to get the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  
 *  @sa PY_CLASS_MEMBER_R_EX
 */
#define PY_CLASS_MEMBER_R_NAME(t_cppClass, i_cppGetter, s_memberName)\
	PY_CLASS_MEMBER_R_NAME_DOC(t_cppClass, i_cppGetter, s_memberName, 0)

/** @ingroup ClassDefinition
 *  @brief Export getter method as read-only Python property (name derived from getter, with documentation).
 *  
 *  Convenience macro that wraps PY_CLASS_MEMBER_R_NAME_DOC() with s_memberName derived from i_cppGetter.
 *  
 *  @param t_cppClass C++ class containing the getter method
 *  @param i_cppGetter C++ method name used to get the attribute value (also used as Python property name)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_MEMBER_R_EX
 */
#define PY_CLASS_MEMBER_R_DOC(t_cppClass, i_cppGetter, s_doc)\
	PY_CLASS_MEMBER_R_NAME_DOC(t_cppClass, i_cppGetter, LASS_STRINGIFY(i_cppGetter), s_doc)

/** @ingroup ClassDefinition
 *  @brief Export getter method as read-only Python property using method name.
 *  
 *  Convenience macro that wraps PY_CLASS_MEMBER_R_DOC() with method name as property name
 *  and no documentation.
 *  
 *  @param t_cppClass C++ class containing the getter method
 *  @param i_cppGetter C++ method name used to get the attribute value (also used as Python property name)
 *  
 *  @sa PY_CLASS_MEMBER_R_EX
 */
#define PY_CLASS_MEMBER_R(t_cppClass, i_cppGetter)\
	PY_CLASS_MEMBER_R_DOC(t_cppClass, i_cppGetter, 0)

// --- data members --------------------------------------------------------------------------------

/** @ingroup ClassDefinition
 *  @brief Export free function accessors as read-write Python property.
 *  
 *  @deprecated Use member method-based macros like PY_CLASS_MEMBER_RW_EX() instead.
 *  
 *  Exports a pair of free functions as read/write Python property. Unlike member method-based
 *  macros, this uses standalone functions that take the object as their first parameter.
 *  
 *  @param t_cppClass C++ class to add the property to
 *  @param i_cppFreeGetter Free function name used to get the attribute value
 *  @param i_cppFreeSetter Free function name used to set the attribute value  
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      int bar;
 *  };
 *
 *  int getBar(const Foo const* iThis) 
 *	{ 
 *		return iThis->bar;
 *	}
 *  void setBar(Foo* iThis, int iBar) 
 *  {	
 *		iBar->bar = abar;
 *	}
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_FREE_MEMBER_RW_EX(Foo, getBar, setBar, "bar", "regular get and setter")
 *  @endcode
 */
#define PY_CLASS_FREE_MEMBER_RW_EX( t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName, s_doc, i_dispatcher)\
	extern "C" LASS_DLL_LOCAL PyObject* LASS_CONCATENATE(i_dispatcher, _getter)( PyObject* iObject, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		return ::lass::python::impl::CallMethod<TShadowTraits>::freeGet( iObject, i_cppFreeGetter );\
	}\
	extern "C" int LASS_CONCATENATE(i_dispatcher, _setter)( PyObject* iObject, PyObject* iArgs, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		return ::lass::python::impl::CallMethod<TShadowTraits>::freeSet( iArgs, iObject, i_cppFreeSetter );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), LASS_CONCATENATE(i_dispatcher, _setter));\
	)

/** @ingroup ClassDefinition
 *  @brief Export free function accessors as read-write Python property with custom name and documentation.
 *  
 *  @deprecated Use member method-based macros like PY_CLASS_MEMBER_RW_NAME_DOC() instead.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_MEMBER_RW_EX() with auto-generated dispatcher name.
 *  
 *  @param t_cppClass C++ class to add the property to
 *  @param i_cppFreeGetter Free function name used to get the attribute value
 *  @param i_cppFreeSetter Free function name used to set the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_MEMBER_RW_EX
 */
#define PY_CLASS_FREE_MEMBER_RW_NAME_DOC(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName, s_doc)\
	PY_CLASS_FREE_MEMBER_RW_EX(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_freeMemberRW, t_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export free function accessors as read-write Python property with custom name.
 *  
 *  @deprecated Use member method-based macros like PY_CLASS_MEMBER_RW_NAME() instead.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_MEMBER_RW_NAME_DOC() with no documentation.
 *  
 *  @param t_cppClass C++ class to add the property to
 *  @param i_cppFreeGetter Free function name used to get the attribute value
 *  @param i_cppFreeSetter Free function name used to set the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  
 *  @sa PY_CLASS_FREE_MEMBER_RW_EX
 */
#define PY_CLASS_FREE_MEMBER_RW_NAME(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName)\
	PY_CLASS_FREE_MEMBER_RW_NAME_DOC(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName, 0)

/** @ingroup ClassDefinition
 *  @brief Export free function accessors as read-write Python property using function name.
 *  
 *  @deprecated Use member method-based macros like PY_CLASS_MEMBER_RW_DOC() instead.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_MEMBER_RW_NAME_DOC() with function name as property name.
 *  
 *  @param t_cppClass C++ class to add the property to
 *  @param i_cppFreeGetter Free function name used to get the attribute value (also used as Python property name)
 *  @param i_cppFreeSetter Free function name used to set the attribute value
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_MEMBER_RW_EX
 */
#define PY_CLASS_FREE_MEMBER_RW_DOC(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_doc)\
	PY_CLASS_FREE_MEMBER_RW_NAME_DOC(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, LASS_STRINGIFY(i_cppFreeGetter), s_doc)

/** @ingroup ClassDefinition
 *  @brief Export free function accessors as read-write Python property using function name.
 *  
 *  @deprecated Use member method-based macros like PY_CLASS_MEMBER_RW() instead.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_MEMBER_RW_DOC() with function name as property name
 *  and no documentation.
 *  
 *  @param t_cppClass C++ class to add the property to
 *  @param i_cppFreeGetter Free function name used to get the attribute value (also used as Python property name)
 *  @param i_cppFreeSetter Free function name used to set the attribute value
 *  
 *  @sa PY_CLASS_FREE_MEMBER_RW_EX
 */
#define PY_CLASS_FREE_MEMBER_RW(t_cppClass, i_cppFreeGetter, i_cppFreeSetter)\
	PY_CLASS_FREE_MEMBER_RW_DOC(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, 0)



/** @ingroup ClassDefinition
 *  @brief Export free function accessor as read-only Python property.
 *  
 *  @deprecated Use member method-based macros like PY_CLASS_MEMBER_R_EX() instead.
 *  
 *  Exports a free function as read-only Python property. Unlike member method-based
 *  macros, this uses a standalone function that takes the object as its first parameter.
 *  
 *  @param t_cppClass C++ class to add the property to
 *  @param i_freeCppGetter Free function name used to get the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      int bar;
 *  };
 *
 *  int getBar(const Foo const* iThis) 
 *	{ 
 *		return iThis->bar;
 *	}
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_FREE_MEMBER_R_EX(Foo, getBar, "bar", "regular get and setter")
 *  @endcode
 */
#define PY_CLASS_FREE_MEMBER_R_EX( t_cppClass, i_freeCppGetter, s_memberName, s_doc, i_dispatcher )\
	extern "C" LASS_DLL_LOCAL PyObject* LASS_CONCATENATE(i_dispatcher, _getter)( PyObject* iObject, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		return ::lass::python::impl::CallMethod<TShadowTraits>::freeGet( iObject, i_freeCppGetter );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), 0);\
	)

/** @ingroup ClassDefinition
 *  @brief Export free function accessor as read-only Python property with custom name and documentation.
 *  
 *  @deprecated Use member method-based macros like PY_CLASS_MEMBER_R_NAME_DOC() instead.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_MEMBER_R_EX() with auto-generated dispatcher name.
 *  
 *  @param t_cppClass C++ class to add the property to
 *  @param i_freeCppGetter Free function name used to get the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_MEMBER_R_EX
 */
#define PY_CLASS_FREE_MEMBER_R_NAME_DOC(t_cppClass, i_freeCppGetter, s_memberName, s_doc)\
	PY_CLASS_FREE_MEMBER_R_EX(t_cppClass, i_freeCppGetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_freeMemberR, t_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export free function accessor as read-only Python property with custom name.
 *  
 *  @deprecated Use member method-based macros like PY_CLASS_MEMBER_R_NAME() instead.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_MEMBER_R_NAME_DOC() with no documentation.
 *  
 *  @param t_cppClass C++ class to add the property to
 *  @param i_freeCppGetter Free function name used to get the attribute value
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  
 *  @sa PY_CLASS_FREE_MEMBER_R_EX
 */
#define PY_CLASS_FREE_MEMBER_R_NAME(t_cppClass, i_freeCppGetter, s_memberName)\
	PY_CLASS_FREE_MEMBER_R_NAME_DOC(t_cppClass, i_freeCppGetter, s_memberName, 0)

/** @ingroup ClassDefinition
 *  @brief Export free function accessor as read-only Python property using function name.
 *  
 *  @deprecated Use member method-based macros like PY_CLASS_MEMBER_R_DOC() instead.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_MEMBER_R_NAME_DOC() with function name as property name.
 *  
 *  @param t_cppClass C++ class to add the property to
 *  @param i_freeCppGetter Free function name used to get the attribute value (also used as Python property name)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_FREE_MEMBER_R_EX
 */
#define PY_CLASS_FREE_MEMBER_R_DOC(t_cppClass, i_freeCppGetter, s_doc)\
	PY_CLASS_FREE_MEMBER_R_NAME_DOC(t_cppClass, i_freeCppGetter, LASS_STRINGIFY(i_freeCppGetter), s_doc)

/** @ingroup ClassDefinition
 *  @brief Export free function accessor as read-only Python property using function name.
 *  
 *  @deprecated Use member method-based macros like PY_CLASS_MEMBER_R() instead.
 *  
 *  Convenience macro that wraps PY_CLASS_FREE_MEMBER_R_DOC() with function name as property name
 *  and no documentation.
 *  
 *  @param t_cppClass C++ class to add the property to
 *  @param i_freeCppGetter Free function name used to get the attribute value (also used as Python property name)
 *  
 *  @sa PY_CLASS_FREE_MEMBER_R_EX
 */
#define PY_CLASS_FREE_MEMBER_R(t_cppClass, i_freeCppGetter)\
	PY_CLASS_FREE_MEMBER_R_DOC(t_cppClass, i_freeCppGetter, 0)

/** @ingroup ClassDefinition
 *  @brief Export public data member as read-write Python property.
 *  
 *  Exports a public data member directly as a Python property, allowing both read and write access.
 *  This provides direct access to the C++ member variable without requiring getter/setter methods.
 *  
 *  @param t_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export  
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      int bar;
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_PUBLIC_MEMBER_EX(Foo, bar, "bar", "blablabla")
 *  @endcode
 */
#define PY_CLASS_PUBLIC_MEMBER_EX(t_cppClass, i_cppMember, s_memberName, s_doc, i_dispatcher)\
	extern "C" LASS_DLL_LOCAL PyObject* LASS_CONCATENATE(i_dispatcher, _getter)(PyObject* obj, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		TShadowTraits::TConstCppClassPtr self;\
		if (TShadowTraits::getObject(obj, self) != 0)\
		{\
			return 0;\
		}\
		return lass::python::pyBuildSimpleObject(self->i_cppMember);\
	}\
	extern "C" LASS_DLL_LOCAL int LASS_CONCATENATE(i_dispatcher, _setter)(PyObject* obj,PyObject* args, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		TShadowTraits::TCppClassPtr self;\
		if (TShadowTraits::getObject(obj, self) != 0)\
		{\
			return -1;\
		}\
		return -::lass::python::pyGetSimpleObject(args, self->i_cppMember);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), LASS_CONCATENATE(i_dispatcher, _setter));\
	)

/** @ingroup ClassDefinition
 *  @brief Export public data member as read-write Python property with custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_PUBLIC_MEMBER_EX() with auto-generated dispatcher name.
 *  
 *  @param i_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_PUBLIC_MEMBER_EX
 */
#define PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, s_memberName, s_doc )\
	PY_CLASS_PUBLIC_MEMBER_EX( i_cppClass, i_cppMember, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_publicMember_, i_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export public data member as read-write Python property with custom name.
 *  
 *  Convenience macro that wraps PY_CLASS_PUBLIC_MEMBER_NAME_DOC() with no documentation.
 *  
 *  @param i_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  
 *  @sa PY_CLASS_PUBLIC_MEMBER_EX
 */
#define PY_CLASS_PUBLIC_MEMBER_NAME( i_cppClass, i_cppMember, s_memberName )\
	PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, s_memberName, 0 )

/** @ingroup ClassDefinition
 *  @brief Export public data member as read-write Python property using member name.
 *  
 *  Convenience macro that wraps PY_CLASS_PUBLIC_MEMBER_NAME_DOC() with member name as property name.
 *  
 *  @param i_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export (also used as Python property name)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_PUBLIC_MEMBER_EX
 */
#define PY_CLASS_PUBLIC_MEMBER_DOC( i_cppClass, i_cppMember , s_doc)\
	PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember), s_doc )

/** @ingroup ClassDefinition
 *  @brief Export public data member as read-write Python property using member name.
 *  
 *  Convenience macro that wraps PY_CLASS_PUBLIC_MEMBER_NAME_DOC() with member name as property name
 *  and no documentation.
 *  
 *  @param i_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export (also used as Python property name)
 *  
 *  @sa PY_CLASS_PUBLIC_MEMBER_EX
 */
#define PY_CLASS_PUBLIC_MEMBER( i_cppClass, i_cppMember )\
	PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember), 0 )




/** @ingroup ClassDefinition
 *  @brief Export public data member as read-only Python property.
 *  
 *  Exports a public data member directly as a read-only Python property, allowing only read access.
 *  This provides direct read access to the C++ member variable without requiring a getter method.
 *  Useful for const members or when write access should be restricted.
 *  
 *  @param t_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      const int bar;
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_PUBLIC_MEMBER_R_EX(Foo, bar, "bar", "read-only member")
 *  @endcode
 */
#define PY_CLASS_PUBLIC_MEMBER_R_EX( t_cppClass, i_cppMember, s_memberName, s_doc, i_dispatcher )\
	extern "C" LASS_DLL_LOCAL PyObject* LASS_CONCATENATE(i_dispatcher, _getter)(PyObject* obj, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		TShadowTraits::TConstCppClassPtr self;\
		if (TShadowTraits::getObject(obj, self) != 0)\
		{\
			return 0;\
		}\
		return lass::python::pyBuildSimpleObject(self->i_cppMember);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), nullptr);\
	)

/** @ingroup ClassDefinition
 *  @brief Export public data member as read-only Python property with custom name and documentation.
 *  
 *  Convenience macro that wraps PY_CLASS_PUBLIC_MEMBER_R_EX() with auto-generated dispatcher name.
 *  
 *  @param i_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_PUBLIC_MEMBER_R_EX
 */
#define PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, s_memberName, s_doc)\
	PY_CLASS_PUBLIC_MEMBER_R_EX(i_cppClass, i_cppMember, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_publicMemberR_, i_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export public data member as read-only Python property with custom name.
 *  
 *  Convenience macro that wraps PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC() with no documentation.
 *  
 *  @param i_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  
 *  @sa PY_CLASS_PUBLIC_MEMBER_R_EX
 */
#define PY_CLASS_PUBLIC_MEMBER_R_NAME(i_cppClass, i_cppMember, s_memberName)\
	PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, s_memberName, 0 )

/** @ingroup ClassDefinition
 *  @brief Export public data member as read-only Python property using member name.
 *  
 *  Convenience macro that wraps PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC() with member name as property name.
 *  
 *  @param i_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export (also used as Python property name)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  
 *  @sa PY_CLASS_PUBLIC_MEMBER_R_EX
 */
#define PY_CLASS_PUBLIC_MEMBER_R_DOC(i_cppClass, i_cppMember , s_doc)\
	PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember),  s_doc)

/** @ingroup ClassDefinition
 *  @brief Export public data member as read-only Python property using member name.
 *  
 *  Convenience macro that wraps PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC() with member name as property name
 *  and no documentation.
 *  
 *  @param i_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export (also used as Python property name)
 *  
 *  @sa PY_CLASS_PUBLIC_MEMBER_R_EX
 */
#define PY_CLASS_PUBLIC_MEMBER_R(i_cppClass, i_cppMember)\
	PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember), 0)

/** @} */

// --- constructors --------------------------------------------------------------------------------

/** @addtogroup ClassDefinition
 *  @name Constructor Export Macros
 *
 *  Export C++ constructors directly as Python class constructors.
 *  These macros make abstract Python classes concrete by adding constructors
 *  that can create instances from Python code using the actual C++ constructors.
 *  
 *  **Standard Constructor Export:**
 *  - PY_CLASS_CONSTRUCTOR_EX() - Full control with manual dispatcher naming
 *  - PY_CLASS_CONSTRUCTOR() - Auto-generated dispatcher name
 *  
 *  **Convenience Macros by Parameter Count:**
 *  - PY_CLASS_CONSTRUCTOR_0() - Constructor with no parameters
 *  - PY_CLASS_CONSTRUCTOR_1() - Constructor with 1 parameter
 *  - PY_CLASS_CONSTRUCTOR_2() - Constructor with 2 parameters
 *  - ... (up to PY_CLASS_CONSTRUCTOR_15()) - Constructor with up to 15 parameters
 *
 *  @{
 */


/** @ingroup ClassDefinition
 *  @brief Export C++ constructor as Python class constructor with full control.
 *
 *  Makes an abstract Python class concrete by adding a constructor. All Python classes 
 *  are abstract by default with no constructors to create instances. This macro provides
 *  the most flexible constructor export with manual dispatcher naming.
 *
 *  @param t_cppClass C++ class to add the constructor to
 *  @param t_params lass::meta::TypeTuple of constructor parameter types (use meta::TypeTuple<> for no parameters)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      Foo();
 *      Foo(int iA, const std::string& iB);
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_CONSTRUCTOR(Foo, meta::TypeTuple<>) // constructor without arguments.
 *  typedef meta::TypeTuple<int, std::string> TArguments;
 *  PY_CLASS_CONSTRUCTOR(Foo, TArguments) // constructor with some arguments. *
 *  @endcode
 */
#define PY_CLASS_CONSTRUCTOR_EX( t_cppClass, t_params, i_dispatcher )\
	static newfunc LASS_CONCATENATE(i_dispatcher, _overloadChain) = 0;\
	extern "C" LASS_DLL_LOCAL PyObject* i_dispatcher(PyTypeObject *iSubtype, PyObject *iArgs, PyObject *iKwds)\
	{\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain))\
		{\
			PyObject* result = LASS_CONCATENATE(i_dispatcher, _overloadChain)(\
				iSubtype, iArgs, iKwds);\
			if (!(PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError)))\
			{\
				return result;\
			}\
			PyErr_Clear();\
			Py_XDECREF(result);\
		}\
		return ::lass::python::impl::ExplicitResolver\
		<\
			::lass::python::impl::ShadowTraits< t_cppClass >,\
			::lass::meta::NullType,\
			t_params\
		>\
		::callConstructor(iSubtype, iArgs);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass::_lassPyClassDef.addConstructor( \
			i_dispatcher, \
			LASS_CONCATENATE(i_dispatcher, _overloadChain) \
		); \
	)

/** @ingroup ClassDefinition
 *  @brief Export C++ constructor as Python class constructor.
 *  
 *  Convenience macro that wraps PY_CLASS_CONSTRUCTOR_EX() with auto-generated dispatcher name.
 *  Makes an abstract Python class concrete by adding a constructor.
 *  
 *  @param i_cppClass C++ class to add the constructor to
 *  @param t_params lass::meta::TypeTuple of constructor parameter types
 *  
 *  @sa PY_CLASS_CONSTRUCTOR_EX
 */
#define PY_CLASS_CONSTRUCTOR( i_cppClass, t_params )\
	PY_CLASS_CONSTRUCTOR_EX(i_cppClass, t_params,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_constructor_, i_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export C++ default constructor as Python class constructor.
 *  
 *  Convenience macro for exporting a parameterless C++ constructor.
 *  Equivalent to PY_CLASS_CONSTRUCTOR(t_cppClass, meta::TypeTuple<>).
 *  
 *  @param t_cppClass C++ class to add the default constructor to
 *  
 *  @sa PY_CLASS_CONSTRUCTOR_EX
 */
#define PY_CLASS_CONSTRUCTOR_0( t_cppClass )\
	PY_CLASS_CONSTRUCTOR( t_cppClass, ::lass::meta::TypeTuple<> )
$[
/** @ingroup ClassDefinition
 *  @brief Export C++ constructor as Python class constructor with $x parameters.
 *  
 *  Convenience macro for exporting a C++ constructor with exactly $x parameters.
 *  Automatically creates the required meta::TypeTuple from the parameter types.
 *  
 *  @param t_cppClass C++ class to add the constructor to
 *  $(  @param t_P$x Type of parameter $x)$
 *  
 *  @sa PY_CLASS_CONSTRUCTOR_EX
 */
#define PY_CLASS_CONSTRUCTOR_$x( t_cppClass, $(t_P$x)$ )\
	typedef ::lass::meta::TypeTuple< $(t_P$x)$ > \
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, t_cppClass));\
	PY_CLASS_CONSTRUCTOR(\
		t_cppClass, LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, t_cppClass)))
]$

/** @} */


/** @addtogroup ClassDefinition
 *  @name Free Function Constructor Export Macros
 *
 *  Export factory functions as Python class constructors. These macros allow C++ 
 *  functions that return instances of a class to be used as Python constructors.
 *  This is useful when you need special construction logic or when the actual 
 *  constructor is not accessible from Python.
 *  
 *  These factory function constructors appear as regular constructors to Python 
 *  code, but are implemented using C++ functions rather than actual constructors.
 *  The functions must return an instance of the target class.
 *
 *  @{
 */

/** @ingroup ClassDefinition
 *  @brief Export C++ factory function as Python constructor with full control.
 *  
 *  The most detailed macro for exporting factory functions as constructors. Allows
 *  manual specification of all parameters including the dispatcher name for 
 *  maximum control over the export process.
 *
 *  @param t_cppClass
 *      the C++ class you want to add the constructor method to.
 *  @param f_cppFunction
 *      the full name of the C++ function that implements the constructor method
 *		if the return argument is not of type t_cppClass the compiler will flag this as an error.
 *	    This is a design decision to protect against runtime surprises.
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *      per exported C++ class/method pair.
 *  @remark No documentation can be provided for constructors, this is a limitation of Python.  Although
 *		the automatic function prototype matching could resolve without specifying the number of arguments
 *		of the constructor like function for the sake of uniformity with PY_CLASS_CONSTRUCTOR the same
 *		format of macro is employed.  This also means that there is no need for a QUALIFIED version.
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *  public:
 *  };
 *
 *  Foo spam(int iB);
 *  Foo spamming(int iB, int iC);
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_FREE_CONSTRUCTOR_1(Foo, spam, int  )
 *  PY_CLASS_FREE_CONSTRUCTOR_2(Foo, spamming, int, int  )
 *  @endcode
 */
#define PY_CLASS_FREE_CONSTRUCTOR_EX( t_cppClass, f_cppFunction, t_params, i_dispatcher )\
	static newfunc LASS_CONCATENATE(i_dispatcher, _overloadChain) = 0;\
	extern "C" LASS_DLL_LOCAL PyObject* i_dispatcher( PyTypeObject *iSubtype, PyObject *iArgs, PyObject *iKwds )\
	{\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain))\
		{\
			PyObject* result = LASS_CONCATENATE(i_dispatcher, _overloadChain)(iSubtype, iArgs, iKwds);\
			if (!(PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError)))\
			{\
				return result;\
			}\
			PyErr_Clear();\
			Py_XDECREF(result);\
		}\
		return ::lass::python::impl::callFunction( iArgs, f_cppFunction );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX(LASS_CONCATENATE(i_dispatcher, _executeBeforeMain), \
		t_cppClass::_lassPyClassDef.addConstructor(\
			i_dispatcher, \
			LASS_CONCATENATE(i_dispatcher, _overloadChain) \
		); \
	)
/** @ingroup ClassDefinition
 *  @brief Convenience wrapper for PY_CLASS_FREE_CONSTRUCTOR_EX.
 *  
 *  Automatically generates a unique dispatcher name.
 *  
 *  @param i_cppClass C++ class to add the constructor to
 *  @param f_cppFunction Factory function that creates the class instance
 *  @param t_params meta::TypeTuple containing parameter types
 *  
 *  @sa PY_CLASS_FREE_CONSTRUCTOR_EX
 */
#define PY_CLASS_FREE_CONSTRUCTOR( i_cppClass, f_cppFunction, t_params )\
	PY_CLASS_FREE_CONSTRUCTOR_EX(i_cppClass, f_cppFunction, t_params,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_constructor_, i_cppClass)))

/** @ingroup ClassDefinition
 *  @brief Export C++ factory function as Python constructor with 0 parameters.
 *  
 *  Convenience macro for exporting a factory function that takes no parameters.
 *  
 *  @param t_cppClass C++ class to add the constructor to
 *  @param f_cppFunction Factory function that creates the class instance
 *  
 *  @sa PY_CLASS_FREE_CONSTRUCTOR_EX
 */
#define PY_CLASS_FREE_CONSTRUCTOR_0( t_cppClass, f_cppFunction)\
	PY_CLASS_FREE_CONSTRUCTOR( t_cppClass, f_cppFunction, ::lass::meta::TypeTuple<> )
$[
/** @ingroup ClassDefinition
 *  @brief Export C++ factory function as Python constructor with $x parameters.
 *  
 *  Convenience macro for exporting a factory function with exactly $x parameters.
 *  Automatically creates the required meta::TypeTuple from the parameter types.
 *  
 *  @param t_cppClass C++ class to add the constructor to
 *  @param f_cppFunction Factory function that creates the class instance
 *  $(  @param t_P$x Type of parameter $x)$
 *  
 *  @sa PY_CLASS_FREE_CONSTRUCTOR_EX
 */
#define PY_CLASS_FREE_CONSTRUCTOR_$x( t_cppClass, f_cppFunction, $(t_P$x)$ )\
	typedef ::lass::meta::TypeTuple< $(t_P$x)$ > \
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, t_cppClass));\
	PY_CLASS_FREE_CONSTRUCTOR(\
		t_cppClass, f_cppFunction, LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, t_cppClass)))
]$

/** @} */



// --- internals -----------------------------------------------------------------------------------

/** @internal
 */
#define PY_CLASS_METHOD_IMPL(t_cppClass, i_cppMethod, s_methodName, s_doc, i_dispatcher, i_caller)\
	static ::lass::python::impl::OverloadLink LASS_CONCATENATE(i_dispatcher, _overloadChain);\
	extern "C" LASS_DLL_LOCAL PyObject* i_dispatcher(PyObject* iSelf, PyObject* iArgs)\
	{\
		PyObject* result = 0;\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain)(iSelf, iArgs, result))\
		{\
			return result;\
		}\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > LASS_UNUSED(TShadowTraits);\
		typedef TShadowTraits::TCppClass LASS_UNUSED(TCppClass);\
		LASS_ASSERT(result == 0);\
		return i_caller(iArgs, iSelf, i_cppMethod);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX(LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addMethod(\
			s_methodName, s_doc, \
			::lass::python::impl::FunctionTypeDispatcher< SPECIAL_SLOT_TYPE(s_methodName) , i_dispatcher>::fun,\
			LASS_CONCATENATE(i_dispatcher, _overloadChain));\
	)
#endif

// EOF
