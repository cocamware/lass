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
 *	Copyright (C) 2004-2026 the Initial Developer.
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

/** @defgroup PythonMacroName Python Macro Name Conventions
 *  @ingroup Python
 *
 *  Every export macro is composed as:
 *
 *  ```
 *  PY_<scope>_[FREE_]<kind>[_R|_RW][_QUALIFIED|_CAST][_NAME][_DOC][_<N>]
 *  PY_<scope>_[FREE_]<kind>[_R|_RW][_QUALIFIED|_CAST]_EX[_<N>]
 *  ```
 *
 *  where `<scope>` is `MODULE` or `CLASS`, and `<kind>` is `FUNCTION`, `METHOD`, `STATIC_METHOD`
 *  `MEMBER`, `PUBLIC_MEMBER`, `CONSTRUCTOR`, `INNER_CLASS`, ...
 *
 *  Every element is optional and they always appear in this order. So
 *  `PY_CLASS_FREE_MEMBER_RW_NAME_DOC` parses as *class scope, free function form, property,
 *  read/write, custom name, with docstring.*.
 *
 *  @par Macro Parameter Prefixes
 *
 *  The macro parameters use Hungarian prefixes to tell you what is expected of the argument:
 *
 *  - `t_`: The argument must be a type, and may be qualified with namespaces, like `std::string`.
 *  - `i_`: The argument must be a valid identifier. It cannot be qualified. This is often required
 *          when the identifier must be concatenated to create a unique name.
 *
 *          - If this argument must also name a type, you must invoke the macro in the correct
 *            namespace, or use a typedef or alias to name the type.
 *          - If this argument must name a class method, just name the method. The class will be
 *            prepended.
 *  - `f_`: The argument must be a function pointer, and may be qualified. In some cases, it may
 *          even be a std::function, a lambda expression, a variable holding a lambda, or any other
 *          callable.
 *  - `s_`: A null-terminated string literal. `"spam"` is a string literal, `spam` is not.
 *          If `nullptr` is allowed, this will be documented.
 *  - `v_`: some value like an int, float, ...
 *  - `o_`: some existing object.
 *
 *  @par Name, Docstring, and Dispatcher Name
 *
 *  Nearly all export macros take common suffixes to add an optional custom Python name or
 *  docstring. They all forward to the `_EX` variant that provides full control over the parameters
 *  and the dispatcher name.
 *
 *  At the class scope, the `_EX` form is unique in that it allows you to pass fully qualified
 *  class names, which the other forms cannot since they build the dispatcher name from it.
 *
 *  Normally, you will not be using this `_EX` variant, but one of the top four:
 *
 *  | Suffix      | Adds parameters                   | Use for ...                         | Example                       |
 *  |-------------|-----------------------------------|-------------------------------------|-------------------------------|
 *  | -           | -                                 | Python name = C++ name              | `PY_MODULE_FUNCTION`          |
 *  | `_NAME`     | `s_name`                          | Custom Python name                  | `PY_MODULE_FUNCTION_NAME`     |
 *  | `_DOC`      | `s_doc`                           | With docstring                      | `PY_MODULE_FUNCTION_DOC`      |
 *  | `_NAME_DOC` | `s_name`, `s_doc`                 | Custom Python name + Docstring      | `PY_MODULE_FUNCTION_NAME_DOC` |
 *  | `_EX`       | `s_name`, `s_doc`, `i_dispatcher` | Custom dispatcher / Qualified class | `PY_MODULE_FUNCTION_EX`       |
 *
 *  Examples:
 *
 *  ```cpp
 *  void spam(int a);
 *
 *  PY_MODULE_FUNCTION         (foo, spam)                        // foo.spam
 *  PY_MODULE_FUNCTION_DOC     (foo, spam, "eat it")              // foo.spam, documented
 *  PY_MODULE_FUNCTION_NAME    (foo, spam, "eggs")                // foo.eggs
 *  PY_MODULE_FUNCTION_NAME_DOC(foo, spam, "eggs", "eat it")      // foo.eggs, documented
 *  PY_MODULE_FUNCTION_EX      (foo, spam, "eggs", "eat it", dsp) // ... + explicit dispatcher
 *  ```
 *
 *  @par Qualifying Overloaded Signatures
 *
 *  Macros for exporting functions also come in variants to fully qualify the function signature to
 *  disambiguate overloaded functions, by adding the return type (except for constructors) and all
 *  parameter types.
 *
 *  The list of parameter types can be passed as a single lass::meta::TypeTuple, or as individual
 *  arguments. For the latter, the `_<N>` tells the number of arguments.
 *
 *  The `_<N>` form is the most often used one, and simply packs its types into a `TypeTuple`
 *
 *  | Form               | Adds parameters             | Example                              |
 *  |--------------------|-----------------------------|--------------------------------------|
 *  | `_QUALIFIED`       | one lass::meta::TypeTuple   | `PY_MODULE_FUNCTION_QUALIFIED`       |
 *  | `_QUALIFIED_<N>`   | `N` loose types, `N` = 0…15 | `PY_MODULE_FUNCTION_QUALIFIED_2`     |
 *
 *  Examples:
 *
 *  ```cpp
 *  double spam(int a, float b);
 *  void spam(const std::string& c);
 *
 *  PY_MODULE_FUNCTION_QUALIFIED_2(foo, spam, double, int, float)
 *  PY_MODULE_FUNCTION_QUALIFIED_1(foo, spam, void, const std::string&)
 *  ```
 *
 *  They combine with the `_NAME` and `_DOC` suffixes, with the `s_name`, `s_doc`, `i_dispatcher`
 *  arguments following the function return and parameter types:
 *
 *  | Suffix                    | Use for ...                    | Example                                   |
 *  |---------------------------|--------------------------------|-------------------------------------------|
 *  | `_QUALIFIED_<N>`          | Python name = C++ name         | `PY_MODULE_FUNCTION_QUALIFIED_2`          |
 *  | `_QUALIFIED_NAME_<N>`     | Custom Python name             | `PY_MODULE_FUNCTION_QUALIFIED_NAME_2`     |
 *  | `_QUALIFIED_DOC_<N>`      | With docstring                 | `PY_MODULE_FUNCTION_QUALIFIED_DOC_2`      |
 *  | `_QUALIFIED_NAME_DOC_<N>` | Custom Python name + Docstring | `PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_2` |
 *  | `_QUALIFIED_EX_<N>`       | Full control                   | `PY_MODULE_FUNCTION_QUALIFIED_EX_2`       |
 */

// --- modules -------------------------------------------------------------------------------------

/** @defgroup PyModuleDeclaration Module Declaration Macros
 *  @ingroup ModuleDefinition
 *
 *  These macros declare and define ModuleDefinition objects that represent Python modules.
 *  They create the fundamental module object that serves as the container for all exported
 *  functions, classes, and constants.
 */

/** @brief Declare and define a ModuleDefinition object representing a Python module.
 *  @ingroup PyModuleDeclaration
 *
 *  Creates a ModuleDefinition instance that can be used to build a Python module
 *  with the specified name and documentation.
 *
 *  @param i_module Unscoped identifier of the module, used as C++ variable name
 *  @param s_name Python module name (const char* string, will be copied)
 *  @param s_doc module docstring (const char* string, will be copied), or nullptr
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE_NAME_DOC(mod_mymodule, "mymodule", "My module with awesome things")
 *  ```
 */
#define PY_DECLARE_MODULE_NAME_DOC( i_module, s_name, s_doc ) \
	::lass::python::ModuleDefinition i_module( s_name, s_doc );

/** @brief Declare a module with name only (no documentation).
 *  @ingroup PyModuleDeclaration
 *
 *  Wraps PY_DECLARE_MODULE_NAME_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_module Unscoped identifier of the module, used as C++ variable name
 *  @param s_name Python module name (const char* string, will be copied)
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE_NAME(mod_mymodule, "mymodule")
 *  ```
 */
#define PY_DECLARE_MODULE_NAME( i_module, s_name ) \
	PY_DECLARE_MODULE_NAME_DOC( i_module, s_name, 0)

/** @brief Declare a module with documentation, using the identifier as the module name.
 *  @ingroup PyModuleDeclaration
 *
 *  Wraps PY_DECLARE_MODULE_NAME_DOC() @a with s_name derived from @a i_module.
 *
 *  @param i_module Unscoped identifier of the module, used as C++ variable and Python module name
 *  @param s_doc Module documentation string (const char* string, will be copied), or nullptr
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE_DOC(mymodule, "My module with awesome things")
 *  ```
 */
#define PY_DECLARE_MODULE_DOC( i_module, s_doc ) \
	PY_DECLARE_MODULE_NAME_DOC( i_module, LASS_STRINGIFY(i_module), s_doc)

/** @brief Declare a module with minimal setup (name derived from identifier, no documentation).
 *  @ingroup PyModuleDeclaration
 *
 *  Wraps PY_DECLARE_MODULE_NAME_DOC() with defaults.
 *
 *  @param i_module Unscoped identifier of the module, used as C++ variable and Python module name
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE(mymodule)
 *  ```
 */
#define PY_DECLARE_MODULE( i_module ) \
	PY_DECLARE_MODULE_NAME_DOC( i_module, LASS_STRINGIFY(i_module), 0)



/** @defgroup PyModuleEntrypoint Module Entrypoint and Injection Macros
 *  @ingroup ModuleDefinition
 *
 *  These macros handle module initialization, injection, and extension module creation. They manage
 *  the Python module lifecycle from creation to registration with the Python interpreter.
 *
 *  If you're building a Python extension module (a .pyd or .so file), you will typically use
 *  the entrypoint macros to create the required `PyInit_*` function that Python calls:
 *  ```cpp
 *  // Declare module
 *  PY_DECLARE_MODULE_DOC(mymodule, "My example module")
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
 */

/** @brief Create a `PyInit_*` Python module initialization function with custom name.
 *  @ingroup PyModuleEntrypoint
 *
 *  Generates the `PyInit_*` function required for Python extension modules. This function will be
 *  called by Python when the module is imported.
 *
 *  @param i_module Module identifier declared with `PY_DECLARE_MODULE_*`
 *  @param i_name Name for the initialization function (`PyInit_<i_name>` will be generated)
 *                Must be the same as your modoule name
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE_NAME(mod_mymodule, "mymodule")
 *  // ...
 *  PY_MODULE_ENTRYPOINT_NAME(mod_mymodule, mymodule) // generates PyInit_mymodule()
 *  ```
 */
#define PY_MODULE_ENTRYPOINT_NAME( i_module, i_name ) \
	PyMODINIT_FUNC LASS_CONCATENATE(PyInit_, i_name)() { return i_module.inject(); }

/** @brief Create a Python module initialization function using the module identifier as the name.
 *  @ingroup PyModuleEntrypoint
 *
 *  Wraps PY_MODULE_ENTRYPOINT_NAME() with @a i_name = @a i_module.
 *
 *  @param i_module Module identifier (used for both module reference and function name)
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE(mymodule)
 *  // ...
 *  PY_MODULE_ENTRYPOINT(mymodule) // generates PyInit_mymodule()
 *  ```
 */
#define PY_MODULE_ENTRYPOINT( i_module ) PY_MODULE_ENTRYPOINT_NAME( i_module, i_module )

/** @brief Inject a Python module so Python becomes aware of it.
 *  @ingroup PyModuleEntrypoint
 *
 *  Creates the actual Python module object with all accumulated definitions (functions, classes,
 *  enums, constants). This should be done at runtime, typically in your `main()` function or
 *  during Python initialization.
 *
 *  @note This is a specialist function for embedding modules instead of writing extension modules.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @return A new reference to the Python module object, or `nullptr` on error (with Python
 *          exception set)
 *
 *  @par Example
 *  ```cpp
 *  LASS_ASSERT(Py_IsInitialized());
 *  LockGIL lock;
 *  TPyObjPtr mod(PY_INJECT_MODULE(mymodule));
 *  if (mod)
 *  {
 *      PyObject* sysmodules = PyImport_GetModuleDict();
 *      PyDict_SetItemString(sysmodules, "mymodule", mod);
 *  }
 *  // now you can run `import mymodule`
 *  ```
 */
#define PY_INJECT_MODULE( i_module )\
	i_module.inject();

/** @brief Inject a module with name and documentation override.
 *  @ingroup PyModuleEntrypoint
 *
 *  @deprecated Use PY_DECLARE_MODULE_NAME_DOC and PY_INJECT_MODULE instead
 */
#define PY_INJECT_MODULE_EX( i_module, s_moduleName, s_doc ) \
	i_module.setName(s_moduleName); \
	i_module.setDoc(s_doc); \
	i_module.inject();

/** @brief Inject a module with name override.
 *  @ingroup PyModuleEntrypoint
 *
 *  @deprecated Use PY_DECLARE_MODULE_NAME and PY_INJECT_MODULE instead
 */
#define PY_INJECT_MODULE_NAME( i_module, s_moduleName )\
	i_module.setName(s_moduleName); \
	i_module.inject();

/** @brief Inject a module with documentation override.
 *  @ingroup PyModuleEntrypoint
 *
 *  @deprecated Use PY_DECLARE_MODULE_DOC and PY_INJECT_MODULE instead
 */
#define PY_INJECT_MODULE_DOC( i_module, s_doc )\
	i_module.setDoc(s_doc);\
	i_module.inject();



/** @ingroup PyModuleEntrypoint
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

/** @ingroup PyModuleEntrypoint
 *  Create an extension module with documentation.
 *  Wraps PY_EXTENSION_MODULE_EX() with @a s_doc parameter.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_injection Injection function to call during module creation
 *  @param s_doc Module documentation string (const char* string with static storage duration)
 */
#define PY_EXTENSION_MODULE_DOC( i_module, f_injection, s_doc )\
	PY_EXTENSION_MODULE_EX( i_module, f_injection, s_doc)

/** @ingroup PyModuleEntrypoint
 *  Create an extension module (no documentation).
 *  Wraps PY_EXTENSION_MODULE_EX() with @a s_doc = `nullptr`.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_injection Injection function to call during module creation
 */
#define PY_EXTENSION_MODULE( i_module, f_injection )\
	PY_EXTENSION_MODULE_EX( i_module, f_injection, 0)


// --- module variables ----------------------------------------------------------------------------

/** @defgroup ModuleMembers Module Constants and Objects Macros
 *  @ingroup ModuleDefinition
 *
 *  These macros add constants, variables, and arbitrary Python objects to modules.
 *  They provide different approaches for exposing C++ values and objects to Python.
 */

/** @brief Add an integer constant to a Python module.
 *  @ingroup ModuleMembers
 *
 *  The constant will be added to the module during module creation (at injection time).
 *  This is executed before main(), so the constant is available when the module is created.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param s_name Python name of constant (const char* string with static storage duration)
 *  @param s_value Integer value of the constant (long)
 *
 *  @par Example
 *  ```cpp
 *  PY_MODULE_INTEGER_CONSTANT(mymodule, "ANSWER", 42) // mymodule.ANSWER == 42
 *  ```
 */
#define PY_MODULE_INTEGER_CONSTANT( i_module, s_name, s_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleIntegerConstant_, i_module),\
		i_module.addLong( s_value, s_name); )



/** @brief Add a string constant to a Python module.
 *  @ingroup ModuleMembers
 *
 *  The constant will be added to the module during module creation (at injection time).
 *  This is executed before main(), so the constant is available when the module is created.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param s_name Python name of constant (const char* string with static storage duration)
 *  @param s_value String value of the constant (const char* string with static storage duration)
 *
 *  @par Example
 *  ```cpp
 *  PY_MODULE_STRING_CONSTANT(mymodule, "GREETING", "hello") // mymodule.GREETING == "hello"
 *  ```
 */
#define PY_MODULE_STRING_CONSTANT( i_module, s_name, s_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleIntegerConstant_, i_module),\
		i_module.addString( s_value, s_name); )



/** @brief Inject an arbitrary object into an already created module at runtime.
 *  @ingroup ModuleMembers
 *
 *  This performs immediate injection into the module namespace, unlike the `PY_MODULE_*_CONSTANT`
 *  macros which defer injection until module creation.
 *
 *  Must be called after the module has been injected with PY_INJECT_MODULE.
 *
 *  @param o_object Object/variable to be injected (will be converted to Python object)
 *  @param i_module Module identifier of an already injected module
 *  @param s_objectName Python name of object (const char* string with static storage duration)
 *
 *  @note This macro must be invoked after the module has been created. It's best to place this in
 *        a postInject function as shown in the example below. The postInject function gets the
 *        module object as a parameter, but don't use that if you want the Lass stubgen tool to
 *        work correctly. Work on the module definition instead (the one you created with
 *        PY_DECLARE_MODULE()).
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE( mymodule )
 *  void mymodule_postinject(PyObject*)
 *  {
 *      PY_INJECT_OBJECT_IN_MODULE_EX(spam, mymodule, "SPAM") // mymodule.SPAM
 *  }
 *  LASS_EXECUTE_BEFORE_MAIN( mymodule.setPostInject(mymodule_postinject); )
 *  PY_MODULE_ENTRYPOINT( mymodule)
 *  ```
 */
#define PY_INJECT_OBJECT_IN_MODULE_EX( o_object, i_module, s_objectName )\
	{\
		i_module.injectObject( o_object, s_objectName );\
	}

/** @brief Inject an object using its C++ identifier as the Python name.
 *  @ingroup ModuleMembers
 *
 *  Wraps PY_INJECT_OBJECT_IN_MODULE_EX() with @a s_objectName derived from @a o_object.
 *
 *  @param o_object Object/variable to be injected (name will be used as Python name)
 *  @param i_module Module identifier of an already injected module
 *
 *  @note This macro must be invoked after the module has been created. It's best to place this in
 *        a postInject function as shown in the example below. The postInject function gets the
 *        module object as a parameter, but don't use that if you want the Lass stubgen tool to
 *        work correctly. Work on the module definition instead (the one you created with
 *        PY_DECLARE_MODULE()).
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE( mymodule )
 *  void mymodule_postinject(PyObject*)
 *  {
 *      PY_INJECT_OBJECT_IN_MODULE(spam, mymodule) // mymodule.spam
 *  }
 *  LASS_EXECUTE_BEFORE_MAIN( mymodule.setPostInject(mymodule_postinject); )
 *  PY_MODULE_ENTRYPOINT( mymodule)
 *  ```
 */
#define PY_INJECT_OBJECT_IN_MODULE( o_object, i_module )\
	PY_INJECT_OBJECT_IN_MODULE_EX(o_object, i_module, LASS_STRINGIFY(o_object))



/** @brief Inject an integer constant into an already created module at runtime.
 *  @ingroup ModuleMembers
 *
 *  This performs immediate injection, unlike PY_MODULE_INTEGER_CONSTANT which
 *  registers the constant for inclusion during module creation.
 *
 *  @param i_module Module identifier of an already injected module
 *  @param s_name Python name of constant (const char* string with static storage duration)
 *  @param v_value Integer value of the constant (long)
 *
 *  @note This macro must be invoked after the module has been created. It's best to place this in
 *        a postInject function as shown in the example below. The postInject function gets the
 *        module object as a parameter, but don't use that if you want the Lass stubgen tool to
 *        work correctly. Work on the module definition instead (the one you created with
 *        PY_DECLARE_MODULE()).
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE( mymodule )
 *  void mymodule_postinject(PyObject*)
 *  {
 *      PY_MODULE_ADD_INTEGER_CONSTANT(mymodule, "ANSWER", 42) // mymodule.ANSWER == 42
 *  }
 *  LASS_EXECUTE_BEFORE_MAIN( mymodule.setPostInject(mymodule_postinject); )
 *  PY_MODULE_ENTRYPOINT( mymodule)
 *  ```
 */
#define PY_MODULE_ADD_INTEGER_CONSTANT( i_module, s_name, v_value )\
	{\
		i_module.injectLong(s_name, v_value);\
	}



/** @brief Inject a string constant into an already created module at runtime.
 *  @ingroup ModuleMembers
 *
 *  This performs immediate injection, unlike PY_MODULE_STRING_CONSTANT which
 *  registers the constant for inclusion during module creation.
 *
 *  @param i_module Module identifier of an already injected module
 *  @param s_name Python name of constant (const char* string with static storage duration)
 *  @param s_value String value of the constant (const char* string with static storage duration)
 *
 *  @note This macro must be invoked after the module has been created. It's best to place this in
 *        a postInject function as shown in the example below. The postInject function gets the
 *        module object as a parameter, but don't use that if you want the Lass stubgen tool to
 *        work correctly. Work on the module definition instead (the one you created with
 *        PY_DECLARE_MODULE()).
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE( mymodule )
 *  void mymodule_postinject(PyObject*)
 *  {
 *      PY_MODULE_ADD_STRING_CONSTANT(mymodule, "GREETING", "hello") // mymodule.GREETING == "hello"
 *  }
 *  LASS_EXECUTE_BEFORE_MAIN( mymodule.setPostInject(mymodule_postinject); )
 *  PY_MODULE_ENTRYPOINT( mymodule)
 *  ```
 */
#define PY_MODULE_ADD_STRING_CONSTANT( i_module, s_name, s_value )\
	{\
		i_module.injectString(s_name, s_value);\
	}



// --- free module functions -----------------------------------------------------------------------

/** @defgroup ModuleFunctions Module Function Macros
 *  @ingroup ModuleDefinition
 *
 *  These macros export C++ functions to a Python and add them to a Python module.
 *  All macros take a ModuleDefinition as first argument.
 *
 *  @note For stubgen to work correctly, these macros should be called in the same translation unit
 *        (source file) that declares the module with PY_DECLARE_MODULE_NAME_DOC or similar.
 *
 *  There are two sets of macros:
 *  - Simple macros in case there's no ambiguity what C++ function is being exported
 *  - Qualified macros that help to disambigue an overloaded C++ function.
 */


/** @ingroup ModuleFunctions
 *  Export a C++ function to Python without automatic wrapper generation (deprecated)
 *
 *  Use this macro for backward compatibility when wrapper functions don't need to be automatically
 *  generated or you want specific Python behavior.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
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

/** @addtogroup ModuleFunctions
 *  @name Simple Function Export Macros
 *
 *  These macros export C++ functions to Python with automatic type deduction and wrapper
 *  generation. Use these for simple function exports where overloading is not an issue and
 *  automatic type deduction is sufficient.
 *
 *  The basic form is:
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION( i_module, f_cppFunction )
 *  ```
 *
 *  with:
 *  - @a i_module : Module identifier declared by `PY_DECLARE_MODULE_*`
 *  - @a f_cppFunction : C++ function, `std::function`, lambda, or other callable to export
 *
 *  @par Example
 *
 *  ```cpp
 *  void spam(int a);
 *  PY_DECLARE_MODULE(foo)
 *  PY_MODULE_FUNCTION(foo, spam) // foo.spam(42)
 *  ```
 *
 *  @par Common suffixes
 *
 *  The `_NAME`, `_DOC` and `_EX` suffixes allow you to specify a custom Python name, docstring, or
 *  (in rare cases) a custom dispatcher name.
 *
 *  | Macro                         | Adds parameters                   | Use for ...                    |
 *  |-------------------------------|-----------------------------------|--------------------------------|
 *  | `PY_MODULE_FUNCTION_NAME`     | `s_name`                          | Custom Python name             |
 *  | `PY_MODULE_FUNCTION_DOC`      | `s_doc`                           | With docstring                 |
 *  | `PY_MODULE_FUNCTION_NAME_DOC` | `s_name`, `s_doc`                 | Custom Python name + Docstring |
 *  | `PY_MODULE_FUNCTION_EX`       | `s_name`, `s_doc`, `i_dispatcher` | Custom dispatcher name         |
 *
 *  @par Example
 *
 *  ```cpp
 *  void spam(int a);
 *
 *  PY_MODULE_FUNCTION         (foo, spam)                        // foo.spam
 *  PY_MODULE_FUNCTION_DOC     (foo, spam, "eat it")              // foo.spam, documented
 *  PY_MODULE_FUNCTION_NAME    (foo, spam, "eggs")                // foo.eggs
 *  PY_MODULE_FUNCTION_NAME_DOC(foo, spam, "eggs", "eat it")      // foo.eggs, documented
 *  PY_MODULE_FUNCTION_EX      (foo, spam, "eggs", "eat it", dsp) // ... + explicit dispatcher
 *  ```
 *
 *  @par Overloading Python functions
 *
 *  Multiple C++ functions can be exported to the same Python name, to create a Python function that
 *  is overloaded on the parameter types.
 *
 *  @note Overload resolution uses first-fit, not best-fit like C++. The first exported
 *        overload that matches the arguments will be called.
 *
 *  @par Example
 *
 *  ```cpp
 *  void barA(int a);
 *  void barB(const std::string& b);
 *
 *  PY_MODULE_FUNCTION_NAME(foo, barA, "bar") // foo.bar(123)
 *  PY_MODULE_FUNCTION_NAME(foo, barB, "bar") // foo.bar("123")
 *  ```
 *
 *  @par `std::function`, Lambda Expressions, and Other Callables
 *
 *  Besides normal functions, you can also export `std::function` objects, lambda expressions, and
 *  other callables as Python functions. Overloading is fully supported.
 *
 *  @note `std::function` objects do not have parameter names, so you will end up with generic names
 *        like `_1`, `_2` as parameter names in the generated Python stubs. Lambda expressions do
 *        carry over proper parameter names, however.
 *
 *  @note Inline lambda expressions that contain commas must be wrapped in (parentheses), and you
 *        must explicitly provide a function name with one of the `_NAME` export macros.
 *
 *  @note Callables with overloaded `operator()` are not supported, and neither are callables with
 *        template `operator()` such as generic lambdas like `[](auto a, auto b) { return a + b; }`.
 *
 *  @par Example
 *
 *  ```cpp
 *  std::function<int(int, int)> adderStdFunction = [](int a, int b) { return a + b; };
 *  auto adderLambda = [](int a, int b) { return a + b; };
 *
 *  PY_MODULE_FUNCTION(foo, adderStdFunction)
 *  PY_MODULE_FUNCTION(foo, adderLambda)
 *  PY_MODULE_FUNCTION_NAME(foo, ([](int a, int b) { return a * b; }), "multiplierLambda")
 *  ```
 *
 *  @{
 */

/** @brief Export a C++ function to Python, with full control.
 *  @ingroup ModuleFunctions
 *
 *  This is the most flexible function export macro, allowing manual dispatcher naming for creating
 *  overloaded Python functions. It should rarely be used in practice, use one of the wrappers that
 *  automatically fill in some of the parameters.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function, `std::function`, lambda, or other callable to export
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function docstring (const char* string with static storage duration), or nullptr
 *  @param i_dispatcher Unique name for the generated dispatcher (unscoped, it is concatenated)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_NAME_DOC(foo, doBar, "do_bar", "Do Bar", foo_do_bar) // foo.do_bar(...)
 *  ```
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
		return ::lass::python::impl::callFunction( iArgs, f_cppFunction );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModuleFunction_, i_module, i_dispatcher ), \
		i_module.addFunctionDispatcher( \
			i_dispatcher, s_functionName, s_doc, \
			LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) \
			);\
	)

/** @brief Export a C++ function to Python, with custom name and docstring.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_EX() with auto-generated dispatcher name.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function, `std::function`, lambda, or other callable to export
 *  @param s_name Python function name (const char* string with static storage duration)
 *  @param s_doc Function docstring (const char* string with static storage duration, or nullptr)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_NAME_DOC(foo, doBar, "do_bar", "Do Bar") // foo.do_bar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, s_name, s_doc )\
	PY_MODULE_FUNCTION_EX( i_module, f_cppFunction, s_name, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))

/** @brief Export a C++ function to Python, with custom Python name.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_NAME_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function, `std::function`, lambda, or other callable to export
 *  @param s_name Python function name (const char* string with static storage duration)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_NAME(foo, doBar, "do_bar") // foo.do_bar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_NAME( i_module, f_cppFunction, s_name)\
	PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, s_name, 0)

/** @brief Export a C++ function to Python, with docstring.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_NAME_DOC() with @a s_name derived from @a f_cppFunction.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function, `std::function`, lambda, or other callable to export
 *                       (name will be used as Python name)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_DOC(foo, doBar, "Do Bar") // foo.doBar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_DOC( i_module, f_cppFunction, s_doc )\
	PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, LASS_STRINGIFY(f_cppFunction), s_doc)

/** @brief Export a C++ function to Python.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_NAME_DOC() with defaults.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function, `std::function`, lambda, or other callable to export
 *                       (name will be used as Python name)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION(foo, doBar) // foo.doBar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION( i_module, f_cppFunction)\
	PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, LASS_STRINGIFY(f_cppFunction), 0)

/** @} */

// --- casting free functions -----------------------------------------------------------

/** @defgroup ModuleFunctionCast Function Cast Export Macros (Deprecated)
 *  @ingroup ModuleDefinition
 *
 *  @deprecated These macros are deprecated. Instead of using casting macros, define explicit
 *              wrapper functions with the desired signatures and export those directly using the
 *              basic function export macros.
 *
 *  These macros export C++ functions to Python with explicit type casting and support for
 *  default parameters. They create wrapper functions that handle type conversions and
 *  allow exporting functions with default parameters by omitting trailing parameters.
 */

/** @ingroup ModuleFunctionCast
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
 *  PY_MODULE_FUNCTION_CAST_NAME_1(foo, bar, void, int, "bar" )			// export the function with as default input for b=555
 *  PY_MODULE_FUNCTION_CAST_NAME_2(foo, bar, void, int, int, "bar" )
 *  @endcode
 */


/** @ingroup ModuleFunctionCast
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit return type casting for 0-parameter functions with full control.
 *  This macro allows exporting functions with default parameters or when explicit type casting is needed.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
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
/** @ingroup ModuleFunctionCast
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit type casting for $x-parameter functions with full control.
 *  This macro allows exporting functions with default parameters or when explicit type casting is needed.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
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

/** @ingroup ModuleFunctionCast
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit type casting for 0-parameter functions with custom name and documentation.
 *  Wraps PY_MODULE_FUNCTION_CAST_EX_0() with automatically generated dispatcher name.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
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
/** @ingroup ModuleFunctionCast
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit type casting for $x-parameter functions with custom name and documentation.
 *  Wraps PY_MODULE_FUNCTION_CAST_EX_$x() with automatically generated dispatcher name.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
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


/** @ingroup ModuleFunctionCast
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit type casting for 0-parameter functions with custom name (no documentation).
 *  Wraps PY_MODULE_FUNCTION_CAST_NAME_DOC_0() with @a s_doc = `nullptr`.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_cppFunction C++ function to export (0 parameters)
 *  @param t_return Return type of the function (for explicit casting)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_CAST_NAME_0( i_module, f_cppFunction, t_return, s_functionName )\
	PY_MODULE_FUNCTION_CAST_NAME_DOC_0(\
		i_module, f_cppFunction, t_return, s_functionName, 0 )
$[
/** @ingroup ModuleFunctionCast
 *  @deprecated Define an explicit wrapper function instead of using casting macros
 *  Export a C++ function with explicit type casting for $x-parameter functions with custom name (no documentation).
 *  Wraps PY_MODULE_FUNCTION_CAST_NAME_DOC_$x() with @a s_doc = `nullptr`.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_cppFunction C++ function to export ($x parameters)
 *  @param t_return Return type of the function (for explicit casting)
 *  @param $(t_P$x)$ Parameter types for the function (for explicit casting)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 */
#define PY_MODULE_FUNCTION_CAST_NAME_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName )\
	PY_MODULE_FUNCTION_CAST_NAME_DOC_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, 0 )
]$



// --- explicit qualified free functions -----------------------------------------------------------

/** @addtogroup ModuleFunctions
 *  @name Qualified Free Functions
 *
 *  If the C++ function that you want to export is overloaded, then you need to be able to
 *  disambiquate what function exactly you want to export.
 *
 *  These macros export C++ functions to Python with explicit type qualification to resolve
 *  function overload ambiguities. They provide fine-grained control over function signatures to
 *  disambiguate overloaded functions, by adding the return type (except for constructors) and all
 *  parameter types.
 *
 *  The list of parameter types can be passed as a single lass::meta::TypeTuple, or as individual
 *  arguments. For the latter, the `_<N>` tells the number of arguments. The `_<N>` form is the most
 *  often used one, and simply packs its types into a `TypeTuple`
 *
 *  | Form                                 | Adds parameters                                 |
 *  |--------------------------------------|-------------------------------------------------|
 *  | `PY_MODULE_FUNCTION_QUALIFIED`       | `t_return`, `t_params` as lass::meta::TypeTuple |
 *  | `PY_MODULE_FUNCTION_QUALIFIED_<N>`   | `t_return`, `t_P1`, `t_P2`, ... ``t_P<N>`       |
 *
 *  @par Example
 *
 *  ```cpp
 *  double spam(int a, float b);
 *  void spam(const std::string& c);
 *
 *  PY_MODULE_FUNCTION_QUALIFIED_2(foo, spam, double, int, float)        // foo.spam(2, 3.14)
 *  PY_MODULE_FUNCTION_QUALIFIED_1(foo, spam, void, const std::string&)  // foo.spam("baz")
 *  ```
 *
 *  They combine with the `_NAME` and `_DOC` suffixes, with the `s_name`, `s_doc`, `i_dispatcher`
 *  arguments following the function return and parameter types.
 *
 *  @par Example
 *
 *  ```cpp
 *  void spam(const std::vector<int>& x);
 *  double do_spam(int a, float b);
 *  void do_spam(const std::string& c);
 *
 *  PY_MODULE_FUNCTION_DOC(foo, spam, "Does Spam!!!")                                   // foo.spam([1,2,3])
 *  PY_MODULE_FUNCTION_QUALIFIED_NAME_2(foo, do_spam, double, int, float, "spam")       // foo.spam(2, 3.14)
 *  PY_MODULE_FUNCTION_QUALIFIED_NAME_1(foo, do_spam, void, const std::string&, "spam") // foo.spam("baz")
 *  ```
 *
 *  @{
 */


/** @brief Export an overloaded C++ function to Python, with full control.
 *  @ingroup ModuleFunctions
 *
 *  Use this macro instead of PY_MODULE_FUNCTION_EX when there are overloaded C++ functions that
 *  would create ambiguity. By explicitly specifying return type and parameter types, you can
 *  disambiguate which overload to export.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function to export (may be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function docstring (const char* string with static storage duration, or nullptr)
 *  @param i_dispatcher Unique name for the generated dispatcher function (unscoped identifier for
 *                      token concatenation)
 *
 *  This macro helps resolve function overload ambiguities by explicitly specifying the function
 *  signature to export.
 *
 *  @par Example
 *
 *  ```cpp
 *  void bar(int a);
 *  void bar(const std::string& b);
 *
 *  PY_MODULE_FUNCTION_QUALIFIED_EX(foo, bar, void, meta::TypeTuple<int>, "bar", nullptr, foo_bar_a)
 *  PY_MODULE_FUNCTION_QUALIFIED_EX(foo, bar, void, meta::TypeTuple<const std::string&>, "bar", nullptr, foo_bar_b)
 *  ```
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
		::callFunction(iArgs, f_cppFunction);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModuleFunction_, i_module, i_dispatcher ),\
		i_module.addFunctionDispatcher( \
			i_dispatcher, s_functionName, s_doc, \
			LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) \
		);\
	)

/** @brief Export an overloaded 0-ary C++ function to Python, with full control.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_EX() for functions with 0 parameters.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_cppFunction C++ function to export (may be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *  @param i_dispatcher Unique identifier for the function dispatcher
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_EX_0(foo, doBar, R, "do_bar", "Do Bar", foo_do_bar) // foo.do_bar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_EX_0( i_module, f_cppFunction, t_return, s_functionName, s_doc, i_dispatcher )\
	PY_MODULE_FUNCTION_QUALIFIED_EX(\
		i_module, f_cppFunction, t_return, ::lass::meta::TypeTuple<>, s_functionName, s_doc, i_dispatcher )
$[
/** @brief Export an overloaded $x-ary C++ function to Python, with full control.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_EX() for functions with exactly $x parameters.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_cppFunction C++ function to export (may be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the function (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *  @param i_dispatcher Unique identifier for the function dispatcher
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_EX_$x(foo, doBar, R, $(P$x)$, "do_bar", "Do Bar", foo_do_bar) // foo.do_bar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_EX_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc, i_dispatcher )\
	typedef ::lass::meta::TypeTuple< $(t_P$x)$ > \
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams));\
	PY_MODULE_FUNCTION_QUALIFIED_EX(\
		i_module, f_cppFunction, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams)),\
		s_functionName, s_doc, i_dispatcher )
]$

/** @brief Export an overloaded C++ function to Python, with custom name and docstring.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_EX() with automatically generated dispatcher name.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_cppFunction C++ function to export (may be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC(foo, doBar, R, (TypeTuple<P1, P2, P3>), "do_bar", "Do Bar") // foo.do_bar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC( i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_EX(\
		i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))


/** @brief Export an overloaded 0-ary C++ function to Python, with custom name and docstring.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_EX_0() with automatically generated dispatcher name.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_cppFunction C++ function to export (may be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Unused, you can set it to `void`
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *
 *  @note `t_params` is a stray parameter that deviates from the pattern. It's here for historical
 *        backward compatibility.
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0$x(foo, doBar, R, void, "do_bar", "Do Bar") // foo.do_bar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0( i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_EX_0(\
		i_module, f_cppFunction, t_return, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))
$[
/** @brief Export an overloaded $x-ary C++ function to Python, with custom name and docstring.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_EX_$x() with automatically generated dispatcher name.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_cppFunction C++ function to export (may be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the function (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x(foo, doBar, R, $(P$x)$, "do_bar", "Do Bar") // foo.do_bar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_EX_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))
]$

/** @brief Export an overloaded C++ function to Python, with custom Python name.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function to export (may be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC(foo, doBar, R, (TypeTuple<P1, P2, P3>), "do_bar") // foo.do_bar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME( i_module, f_cppFunction, t_return, t_params, s_functionName )\
		PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC(\
			i_module, f_cppFunction, t_return, t_params, s_functionName, 0 )

/** @brief Export an overloaded 0-ary C++ function to Python, with custom Python name.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0() with @a s_doc = `nullptr`.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function to export (may be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_NAME_0(foo, doBar, R, "do_bar") // foo.do_bar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_0( i_module, f_cppFunction, t_return, s_functionName )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0(\
		i_module, f_cppFunction, t_return, s_functionName, 0 )
$[
/** @brief Export an overloaded $x-ary C++ function to Python, with custom Python name.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x() with @a s_doc = `nullptr`.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_cppFunction C++ function to export (may be overloaded)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the function (for disambiguation)
 *  @param s_functionName Python function name (const char* string with static storage duration)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_NAME_$x(foo, doBar, R, $(P$x)$, "do_bar") // foo.do_bar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, 0 )
]$

/** @brief Export an overloaded C++ function to Python, with docstring.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC() with @a s_functionName derived from
 *  @a f_cppFunction.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function to export (may be overloaded, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC(foo, doBar, R, (TypeTuple<P1, P2, P3>), "Do Bar") // foo.doBar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_DOC( i_module, f_cppFunction, t_return, t_params, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC( \
		i_module, f_cppFunction, t_return, t_params, LASS_STRINGIFY(f_cppFunction), s_doc)

/** @brief Export an overloaded 0-ary C++ function to Python, with docstring.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0() with @a s_functionName derived from
 *  @a f_cppFunction.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function to export (0 parameters, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_DOC_0(foo, doBar, R, "Do Bar") // foo.doBar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_DOC_0( i_module, f_cppFunction, t_return, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0( \
		i_module, f_cppFunction, t_return, LASS_STRINGIFY(f_cppFunction), s_doc)
$[
/** @brief Export an overloaded $x-ary C++ function to Python, with docstring.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x() with @a s_functionName derived from
 *  @a f_cppFunction.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_cppFunction C++ function to export ($x parameters, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the function (for disambiguation)
 *  @param s_doc Function documentation string (const char* string with static storage duration)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_DOC_$x(foo, doBar, R, $(P$x)$, "Do Bar") // foo.doBar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_DOC_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x( \
		i_module, f_cppFunction, t_return, $(t_P$x)$, LASS_STRINGIFY(f_cppFunction), s_doc)
]$

/** @brief Export an overloaded C++ function to Python.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC() with defaults.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function to export (may be overloaded, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC(foo, doBar, R, (TypeTuple<P1, P2, P3>)) // foo.doBar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED( i_module, f_cppFunction, t_return, t_params )\
	PY_MODULE_FUNCTION_QUALIFIED_DOC( i_module, f_cppFunction, t_return, t_params, 0 )

/** @brief Export an overloaded 0-ary C++ function to Python.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_DOC_0() with @a s_doc = `nullptr`.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*`
 *  @param f_cppFunction C++ function to export (0 parameters, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_0(foo, doBar, R) // foo.doBar(...)
 *  ```
 */
#define PY_MODULE_FUNCTION_QUALIFIED_0( i_module, f_cppFunction, t_return )\
	PY_MODULE_FUNCTION_QUALIFIED_DOC_0( i_module, f_cppFunction, t_return, 0 )
$[
/** @brief Export an overloaded $x-ary C++ function to Python.
 *  @ingroup ModuleFunctions
 *
 *  Wraps PY_MODULE_FUNCTION_QUALIFIED_DOC_$x() with @a s_doc = `nullptr`.
 *
 *  @param i_module Module identifier declared by `PY_DECLARE_MODULE_*()`
 *  @param f_cppFunction C++ function to export ($x parameters, name will be used as Python name)
 *  @param t_return Return type of the function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the function (for disambiguation)
 *
 *  @par Example
 *
 *  ```cpp
 *  PY_MODULE_FUNCTION_QUALIFIED_$x(foo, doBar, R, $(P$x)$) // foo.doBar(...)
 *  ```
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
 *  needed to generate a Python type. Every C++ class that needs to be exposed to Python must be
 *  declared using one of these macros.
 *
 *  LASS supports two approaches for exporting C++ classes to Python:
 *
 *  1. **Direct Python classes**: C++ classes that directly inherit from lass::python::PyObjectPlus
 *                                and are designed to be Python-aware from the start.
 *
 *  2. **@ref ShadowClasses**: Wrapper classes created for existing native C++ types using the
 *                             shadow system.
 *
 *  @note In both cases, the class parameter in these declaration macros refers to the Python
 *        binding class (either PyObjectPlus-derived or shadow wrapper), never the underlying native
 *        C++ type being shadowed.
 *
 *  The basic form is
 *
 *  ```cpp
 *  PY_DECLARE_CLASS( i_cppClass )
 *  ```
 *
 *  with:
 *  - @a i_cppClass : Python binding class, either a @ref ShadowClasses "shadow class" or a class
 *                    deriving from lass::python::PyObjectPlus
 *
 *  @par Common suffixes
 *
 *  The `_NAME` and `_DOC` suffixes allow you to specify a custom Python name and docstring.
 *
 *  | Macro                       | Fixed parameters | Adds parameters                     | Use for ...                    |
 *  |-----------------------------|------------------|-------------------------------------|--------------------------------|
 *  | `PY_DECLARE_CLASS_NAME`     | `t_cppClass`     | `s_name`                            | Custom Python name             |
 *  | `PY_DECLARE_CLASS_DOC`      | `i_cppClass`     | `s_doc`                             | With docstring                 |
 *  | `PY_DECLARE_CLASS_NAME_DOC` | `t_cppClass`     | `s_name`, `s_doc`                   | Custom Python name + Docstring |
 *  | `PY_DECLARE_CLASS_EX`       | `t_cppClass`     | `s_name`, `i_uniqueClassIdentifier` | (deprecated)                   |
 *
 *  @par Direct Python Class Example:
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
 *  @par Shadow Class Example:
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

/** @brief Declare a Python class with full control over name and documentation.
 *
 *  This is the primary class declaration macro that creates the internal ClassDefinition object for
 *  a C++ class. All other class declaration macros ultimately call this one. The class definition
 *  collects constructors, methods, properties, and other elements that will be added later using
 *  `PY_CLASS_*` macros.
 *
 *  @param t_cppClass     Python binding class type. This must be either:
 *                        - A class directly inheriting from PyObjectPlus, or
 *                        - A shadow class created with PY_SHADOW_CLASS macros.
 *                        Never pass the underlying native C++ shadowed type.
 *  @param s_name         Python class name as string literal
 *  @param s_doc          Class documentation string (or nullptr for no doc)
 *
 *  @note This macro must be used exactly once per class and only in source files, never in headers!
 *
 *  @par Direct Python Class Example:
 *  ```cpp
 *  PY_DECLARE_CLASS_NAME_DOC(MyClass, "MyClass", "A sample direct Python class")
 *  ```
 *
 *  @par Shadow Class Example:
 *  ```cpp
 *  PY_DECLARE_CLASS_NAME_DOC(PyShadowLegacy, "LegacyClass", "Wrapper for LegacyClass")
 *  ```
 *
 *  @ingroup ClassDefinition
 */
#define PY_DECLARE_CLASS_NAME_DOC( t_cppClass, s_name, s_doc ) \
	::lass::python::impl::ClassDefinition t_cppClass ::_lassPyClassDef( \
		s_name, s_doc, sizeof(t_cppClass), \
		::lass::python::impl::richCompareDispatcher< t_cppClass >,\
		& t_cppClass ::_lassPyParentType::_lassPyClassDef, \
		& t_cppClass ::_lassPyClassRegisterHook);

/** @brief Declare a Python class with custom name but no documentation.
 *
 *  Convenience wrapper around PY_DECLARE_CLASS_NAME_DOC that omits the documentation string.
 *  Use this when you want to control the Python class name but don't need documentation.
 *
 *  @param t_cppClass     Python binding class type (PyObjectPlus-derived or shadow class)
 *  @param s_name         Python class name as string literal
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_CLASS_NAME(MyClass, "MyClass")
 *  ```
 *
 *  @ingroup ClassDefinition
 */
#define PY_DECLARE_CLASS_NAME( t_cppClass, s_name )\
	PY_DECLARE_CLASS_NAME_DOC( t_cppClass, s_name, 0 )

/** @brief Declare a Python class with automatic name and custom documentation.
 *
 *  Convenience wrapper that uses the C++ class name as the Python class name but allows custom
 *  documentation. The class name is automatically stringified.
 *
 *  @param i_cppClass     Python-exportable C++ class identifier (unqualified name)
 *  @param s_doc          Class documentation string
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_CLASS_DOC(MyClass, "A sample class for demonstration") // Creates Python class named "MyClass"
 *  ```
 *
 *  @ingroup ClassDefinition
 */
#define PY_DECLARE_CLASS_DOC( i_cppClass, s_doc ) \
	PY_DECLARE_CLASS_NAME_DOC( i_cppClass, LASS_STRINGIFY(i_cppClass), s_doc )

/** @brief Declare a Python class with automatic name and no documentation.
 *
 *  The simplest class declaration macro. Uses the C++ class name as the Python class name and
 *  provides no documentation string. Most commonly used for basic class exports.
 *
 *  @param i_cppClass     Python binding class identifier (unqualified name)
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_CLASS(MyClass) // Creates Python class named "MyClass" with no documentation
 *  ```
 *
 *  @ingroup ClassDefinition
 */
#define PY_DECLARE_CLASS( i_cppClass ) \
	PY_DECLARE_CLASS_NAME_DOC( i_cppClass, LASS_STRINGIFY(i_cppClass), 0 )

/** @brief Legacy class declaration macro.
 *
 *  @param t_cppClass                Python binding class type
 *  @param s_name                    Python class name
 *  @param i_uniqueClassIdentifier   Unused parameter (legacy)
 *
 *  @deprecated This macro is deprecated and should not be used in new code. Use
 *              PY_DECLARE_CLASS_NAME_DOC() instead.
 *
 *  @ingroup ClassDefinition
 */
#define PY_DECLARE_CLASS_EX( t_cppClass, s_name, i_uniqueClassIdentifier )\
	PY_DECLARE_CLASS_NAME_DOC( t_cppClass, s_name, 0 )

/** @} */



/** @addtogroup ModuleDefinition
 *  @name Class Integration
 *
 *  Macros to add Python classes to modules.
 *
 *  These macros integrate class definitions with module definitions, making the classes available
 *  as types within the module namespace. Classes must first be declared using `PY_DECLARE_CLASS_*`
 *  macros before they can be added to modules.
 *
 *  @{
 */

/** @brief Inject a class into a module at runtime
 *  @ingroup ModuleDefinition
 *
 *  This is the legacy runtime approach for adding classes to modules. Use PY_MODULE_CLASS() instead
 *  for compile-time registration.
 *
 *  @param t_cppClass    Python binding class type that has been declared with PY_DECLARE_CLASS_*
 *  @param i_module      Module object identifier to inject the class into
 *  @param s_doc         Optional class documentation string (or nullptr)
 *
 *  @note This macro must be invoked after the module has been created. It's best to place this in
 *        a postInject function as shown in the example below. The postInject function gets the
 *        module object as a parameter, but don't use that if you want the Lass stubgen tool to
 *        work correctly. Work on the module definition instead (the one you created with
 *        PY_DECLARE_MODULE()).
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_MODULE( mymodule )
 *  PY_DECLARE_CLASS(MyClass)
 *  void mymodule_postinject(PyObject*)
 *  {
 *      PY_INJECT_CLASS_IN_MODULE(MyClass, mymodule, "Sample class") // mymodule.MyClass
 *  }
 *  LASS_EXECUTE_BEFORE_MAIN( mymodule.setPostInject(mymodule_postinject); )
 *  PY_MODULE_ENTRYPOINT( mymodule)
 *  ```
 */
#define PY_INJECT_CLASS_IN_MODULE( t_cppClass, i_module, s_doc ) \
	t_cppClass::_lassPyClassDef.setDocIfNotNull(s_doc);\
	i_module.injectClass(t_cppClass::_lassPyClassDef);

/** @brief Add a Python class to a module.
 *  @ingroup ModuleDefinition
 *
 *  Registers a class definition to be included in the module when it is created.
 *  The class must have been declared with `PY_DECLARE_CLASS_*` macros.
 *  This is executed before `main()`, so the class is available when the module is created.
 *
 *  @param i_module      Module identifier declared by `PY_DECLARE_MODULE_*`
 *                       (must be unscoped identifier for token concatenation)
 *  @param t_cppClass    Python binding class type that has been declared with `PY_DECLARE_CLASS_*`
 *
 *  @par Example
 *  ```cpp
 *  PY_DECLARE_CLASS_NAME_DOC(MyClass, "MyClass", "Sample class")
 *  PY_MODULE_CLASS(mymodule, MyClass)
 *  ```
 */
#define PY_MODULE_CLASS( i_module, t_cppClass ) \
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleClass_, i_module ),\
		i_module.addClass( t_cppClass ::_lassPyClassDef); \
	)

/** @} */



/** @defgroup ClassAttributes Class Attribute Export Macros
 *  @ingroup ClassDefinition
 *
 *  Export values and nested types as attributes on the Python class itself.
 *
 *  Unlike @ref ClassMembers "properties" which install descriptors that get or set member data from
 *  or on instances, these attributes are set directly on the Python type object when the class is
 *  frozen and are immutable. In Python, they are reached as `Foo.CONSTANT` or `Outer.Inner`,
 *  without an instance.
 *
 *  Nested enums work the same way, but are exported with PY_CLASS_ENUM(), which is documented in
 *  @ref EnumDefinition.
 */

/** @addtogroup ClassAttributes
 *  @name Static Constants
 *
 *  These macros allow you to expose compile-time constant values as static attributes of Python
 *  classes. The values are converted to Python objects using PyExportTraits and become accessible
 *  as class-level attributes in Python.
 *
 *  @{
 */

/** @brief Export a static constant value as a class attribute.
 *  @ingroup ClassAttributes
 *
 *  Adds a static constant to a Python class that can be accessed as a class attribute. The constant
 *  value is converted to a Python object at module initialization time and becomes accessible via
 *  the class in Python.
 *
 *  @param i_cppClass    Python binding class identifier (must be declared with `PY_DECLARE_CLASS_*`)
 *  @param s_name        Name of the constant as it will appear in Python (string literal)
 *  @param v_value       The constant value to export (must be convertible via `PyExportTraits::build`)
 *
 *  @par Example
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
 */
#define PY_CLASS_STATIC_CONST( i_cppClass, s_name, v_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyClassStaticConst, i_cppClass ),\
		i_cppClass ::_lassPyClassDef.addStaticConst(s_name, v_value);\
	)

/** @} */



// --- inner class ---------------------------------------------------------------------------------



/** @addtogroup ClassAttributes
 *  @name Inner Classes
 *
 *  Macros for adding inner classes (nested classes) to Python-exported classes.
 *
 *  These macros establish class relationships where inner classes become attributes of their outer
 *  class in Python.
 *
 *  The basic form is
 *
 *  ```cpp
 *  PY_CLASS_INNER_CLASS( i_outerCppClass, i_innerCppClass )
 *  ```
 *
 *  with:
 *  - @a i_outerCppClass : C++ class that will contain the inner class
 *  - @a i_innerCppClass : C++ class to be exported as inner class
 *
 *  @par Common suffixes
 *
 *  The `_NAME`, `_DOC` and `_EX` suffixes allow you to specify a custom Python name, docstring, or
 *  (in rare cases) fully qualified typenames.
 *
 *  | Macro                           | Fixed parameters                     | Adds parameters                     | Use for ...                                 |
 *  |---------------------------------|--------------------------------------|-------------------------------------|---------------------------------------------|
 *  | `PY_CLASS_INNER_CLASS_NAME`     | `i_outerCppClass`, `i_innerCppClass` | `s_name`                            | Custom Python name                          |
 *  | `PY_CLASS_INNER_CLASS_DOC`      | `i_outerCppClass`, `i_innerCppClass` | `s_doc`                             | With docstring (deprecated)                 |
 *  | `PY_CLASS_INNER_CLASS_NAME_DOC` | `i_outerCppClass`, `i_innerCppClass` | `s_name`, `s_doc`                   | Custom Python name + Docstring (deprecated) |
 *  | `PY_CLASS_INNER_CLASS_EX`       | `t_outerCppClass`, `t_innerCppClass` | `s_name`, `s_doc`, `i_uniqueSuffix` | Fully qualified typenames                   |
 *
 *  @note Inner classes must be declared with `PY_DECLARE_CLASS*` macros before using these macros.
 *
 *  @note The outer class may also be the parent class of the inner class.
 *
 *  @note The two `*_DOC` forms are deprecated as you should set the docstring when declaring
 *        the innerclass using PY_DECLARE_CLASS_DOC() or PY_DECLARE_CLASS_NAME_DOC()
 *
 *  @par Example
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

/** @ingroup ClassAttributes
 *  @brief Exports an inner class with full customization of name, documentation, and symbol suffix.
 *
 *  This is the most flexible inner class macro, allowing complete control over all parameters.
 *  The inner class becomes accessible as an attribute of the outer class in Python.
 *  In contrast to the convenience macros, here you can provide fully qualified typenames, at the
 *  cost of having to provide a unique suffix.
 *
 *  @param t_outerCppClass C++ class that will contain the inner class
 *  @param t_innerCppClass C++ class to be exported as inner class
 *  @param s_name Python name for the inner class (null-terminated C string literal)
 *  @param s_doc Python docstring for the inner class (null-terminated C string literal, deprecated:
 *               should be nullptr)
 *  @param i_uniqueSuffix Unique C++ identifier to generate unique symbols for the registration code.
 *                        This prevents symbol collisions when multiple inner class exports exist.
 *
 *  @note Setting a docstring on the inner class using this macro is deprecated. You should be
 *        setting the doc when declaring the innerclass using PY_DECLARE_CLASS_DOC() or
 *        PY_DECLARE_CLASS_NAME_DOC().
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_INNER_CLASS_EX(Outer, Inner, "CustomInner", nullptr, MyUniqueSuffix) // Outer.CustomInner
 *  ```
 */
#define PY_CLASS_INNER_CLASS_EX( t_outerCppClass, t_innerCppClass, s_name, s_doc, i_uniqueSuffix )\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE(lassPythonImplExecuteBeforeMain_, i_uniqueSuffix),\
		t_innerCppClass::_lassPyClassDef.setDocIfNotNull(s_doc);\
		t_outerCppClass::_lassPyClassDef.addInnerClass(t_innerCppClass::_lassPyClassDef);\
	)

/** @ingroup ClassAttributes
 *  @brief Exports an inner class with custom name and documentation.
 *
 *  Convenience macro that automatically generates a unique suffix from the class names.
 *  Provides full control over the Python name and documentation string.
 *
 *  @param i_outerCppClass C++ class that will contain the inner class (unqualified name)
 *  @param i_innerCppClass C++ class to be exported as inner class (unqualified name)
 *  @param s_name Python name for the inner class (null-terminated C string literal)
 *  @param s_doc Python docstring for the inner class (null-terminated C string literal, may be nullptr)
 *
 *  @deprecated You should be setting the doc when declaring the innerclass using
 *              PY_DECLARE_CLASS_DOC() or PY_DECLARE_CLASS_NAME_DOC()
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_INNER_CLASS_NAME_DOC(Outer, Inner, "NestedClass", "Documentation") // Outer.NestedClass
 *  ```
 */
#define PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, s_name, s_doc )\
	PY_CLASS_INNER_CLASS_EX( i_outerCppClass, i_innerCppClass, s_name, s_doc,\
		LASS_CONCATENATE(i_outerCppClass, i_innerCppClass) )

/** @ingroup ClassAttributes
 *  @brief Exports an inner class with custom name but no documentation.
 *
 *  Convenience macro for cases where you want to customize the Python name, but don't need to
 *  provide additional documentation.
 *
 *  @param i_outerCppClass C++ class that will contain the inner class (unqualified name)
 *  @param i_innerCppClass C++ class to be exported as inner class (unqualified name)
 *  @param s_name Python name for the inner class (null-terminated C string literal)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_INNER_CLASS_NAME(Outer, Inner, "CustomName") // Outer.CustomName
 *  ```
 */
#define PY_CLASS_INNER_CLASS_NAME( i_outerCppClass, i_innerCppClass, s_name)\
	PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, s_name, 0)

/** @ingroup ClassAttributes
 *  @brief Exports an inner class with default name and custom documentation.
 *
 *  The inner class will use its C++ class name as the Python name, but allows you to provide custom
 *  documentation.
 *
 *  @param i_outerCppClass C++ class that will contain the inner class (unqualified name)
 *  @param i_innerCppClass C++ class to be exported as inner class (unqualified name)
 *  @param s_doc Python docstring for the inner class (null-terminated C string literal)
 *
 *  @deprecated You should be setting the doc when declaring the innerclass using
 *              PY_DECLARE_CLASS_DOC() or PY_DECLARE_CLASS_NAME_DOC()
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_INNER_CLASS_DOC(Outer, Inner, "Custom documentation") // Outer.Inner
 *  ```
 */
#define PY_CLASS_INNER_CLASS_DOC( i_outerCppClass, i_innerCppClass, s_doc )\
	PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, LASS_STRINGIFY(i_innerCppClass), s_doc)

/** @ingroup ClassAttributes
 *  @brief Exports an inner class with default name and no documentation.
 *
 *  The simplest inner class export macro. Uses the C++ class name as the Python name and doesn't
 *  provide additional documentation beyond what was set during class declaration.
 *
 *  @param i_outerCppClass C++ class that will contain the inner class (unqualified name)
 *  @param i_innerCppClass C++ class to be exported as inner class (unqualified name)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_INNER_CLASS(Outer, Inner) // Outer.Inner
 *  ```
 */
#define PY_CLASS_INNER_CLASS( i_outerCppClass, i_innerCppClass)\
	PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, LASS_STRINGIFY(i_innerCppClass), 0)

/** @} */



// --- methods -------------------------------------------------------------------------------------

/** @defgroup ClassMethods Method Export Macros
 *  @ingroup ClassDefinition
 *
 *  @brief These macros export C++ methods on a class to Python.
 *
 *  All macros take either the C++ class (in case it derives from lass::python::PyObjectPlus) or its
 *  @ref ShadowClasses "shadow class" as first argument.
 *
 *  @par Class methods
 *
 *  There are two sets of macros to directly export C++ methods defined on the class itself:
 *
 *  - Simple macros in case there's no ambiguity what C++ method is being exported
 *  - Qualified macros that help to disambiguate overloaded C++ methods.
 *
 *  @par Free Methods
 *
 *  Additionally, you can export a free function that takes an instance of a class as a method on
 *  the Python class. This is extremely useful using @ref ShadowClasses to export existing classes,
 *  and you want to add a Python method that doesn't exist on the original C++ class. Or if you need
 *  to add a wrapper to an existing method.
 *
 *  There's again two sets of macros for C++ free methods:
 *
 *  - Simple macros in case there's no ambiguity what C++ free method is being exported
 *  - Qualified macros that help to disambiguate overloaded C++ free methods.
 *
 *  @par `std::function`, Lambda Expressions, and Other Callables as Free Methods
 *
 *  Besides normal free functions, you can also export `std::function` objects, lambda expressions
 *  or any other callable as free methods. This *only* applies to the simple free method macros, not
 *  the qualified ones.
 *
 *  @note `std::function` objects do not have parameter names, so you will end up with generic names
 *        like `_1`, `_2` as parameter names in the generated Python stubs. Lambda expressions do
 *        carry over proper parameter names, however.
 *
 *  @note Inline lambda expressions that contain commas must be wrapped in (parentheses), and you
 *        must explicitly provide a method name with one of the `_NAME` export macros.
 *
 *  @note Callables with overloaded `operator()` are not supported, and neither are callables with
 *        template `operator()` such as generic lambdas like `[](auto a, auto b) { return a + b; }`.
 *
 *  @par Overloading
 *
 *  By exporting multiple (free) methods to the same Python method name, you will effectively
 *  overload the Python method on the parameter types. You can mix simple method exports, qualified
 *  exports and free method exports: they can all overload on the same Python method name.
 *
 *  @note Overload resolution uses first-fit, not best-fit like C++. The first exported overload
 *        that matches the arguments will be called.
 *
 *  @note The documentation of an overloaded Python method will be the s_doc of the first exported
 *        overload.
 *
 *  @par Operator and special methods
 *
 *  @note For most special methods (like `__add__`, `__str__`, etc.), you **must** use the special
 *        method names defined in lass::python::methods namespace to ensure correct behavior.
 *        Regular string names like `"__add__"` will not work for these special methods.
 *
 *  @par Example
 *
 *  ```cpp
 *  class Menu {
 *      PY_HEADER(lass::python::PyObjectPlus)
 *  public:
 *      void spam(int a);
 *      void baconA(double a);
 *      void baconB(const std::string& b);
 *      long eggs(int a, int b) const;
 *      std::vector<std::string> eggs(const std::string& a, const std::string& b) const;
 *      Menu operator+(const Menu& other) const;
 *      void operator()(int number);
 *      int getInt() const;
 *  };
 *  using TMenuPtr = PyObjectPtr<Menu>::Type;
 *
 *  void sausage(const Menu& self, int a);
 *  void moreBacon(Menu* self, std::complex<double> c);
 *  long bakedBeans(const TMenuPtr& self, int a, int b);
 *  std::vector<std::string> bakedBeans(TMenuPtr self, const std::string& a, const std::string& b);
 *
 *  PY_DECLARE_CLASS(Menu)
 *
 *  // Simple method export
 *  PY_CLASS_METHOD(Menu, spam) // menu.spam(42)
 *
 *  // Overloaded method export with custom name and docstring
 *  PY_CLASS_METHOD_NAME_DOC(Menu, baconA, "bacon", "Add bacon") // menu.bacon(3.14)
 *  PY_CLASS_METHOD_NAME(Menu, baconB, "bacon")                  // menu.bacon("pi")
 *
 *  // Special method export using lass::python::methods constants
 *  PY_CLASS_METHOD_NAME(Menu, operator+, lass::python::methods::_add_)   // menu3 = menu1 + menu2
 *  PY_CLASS_METHOD_NAME(Menu, operator(), lass::python::methods::_call_) // menu(4)
 *
 *  // Type-qualified overloads
 *  PY_CLASS_METHOD_QUALIFIED_2(Menu, eggs, long, int, int)                                                   // i = menu.eggs(1, 2)
 *  PY_CLASS_METHOD_QUALIFIED_2(Menu, eggs, std::vector<std::string>, const std::string&, const std::string&) // s = menu.eggs("a", "b")
 *
 *  // Free methods
 *  PY_CLASS_FREE_METHOD(Menu, sausage) // menu.sausage(42)
 *  PY_CLASS_FREE_METHOD_NAME(Menu, moreBacon, "bacon")          // menu.bacon(3+4j)
 *
 *  // Type-qualified free function overloads (note we need to state the self parameter too)
 *  PY_CLASS_FREE_METHOD_QUALIFIED_3(Menu, bakedBeans, long, const TMenuPtr&, int, int)                                            // i = menu.bakedBeans(1, 2)
 *  PY_CLASS_FREE_METHOD_QUALIFIED_3(Menu, bakedBeans, std::vector<std::string>, TMenuPtr, const std::string&, const std::string&) // s = menu.bakedBeans("a", "b")
 *
 *  // std::function and lambda expressions as free methods
 *  std::function<int(const Menu&, int)> stdFunctionAdder = [](const Menu& self, int x) { return self.getInt() + x; };
 *  PY_CLASS_FREE_METHOD(Menu, stdFunctionAdder)
 *  auto lambdaMultiplier = [](const Menu& self, int x) { return self.getInt() * x; };
 *  PY_CLASS_FREE_METHOD(Menu, lambdaMultiplier)
 *  PY_CLASS_FREE_METHOD_NAME(Menu, ([](const Menu& self, int x) { return self.getInt() / x; }), "lambdaDivider")
 *  ```
 */


/** @ingroup ClassMethods
 *  @brief Export a C++ method that returns raw PyObject* to Python (deprecated)
 *
 *  Use this macro when you need a method that returns Python-specific objects or handles Python
 *  types directly. The C++ method must return `PyObject*` and accept `PyObject*` arguments.
 *
 *  @param i_cppClass C++ class containing the method (unqualified name)
 *  @param i_cppMethod C++ method name to export (must return PyObject* and take PyObject* args)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
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



/** @addtogroup ClassMethods
 *  @name Simple Class Method Export Macros
 *
 *  These macros export C++ methods to Python with automatic type deduction and wrapper generation.
 *
 *  The basic form is:
 *
 *  ```cpp
 *  PY_CLASS_METHOD( i_cppClass, i_cppMethod )
 *  ```
 *
 *  with:
 *  - @a  i_cppClass : C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  - @a  i_cppMethod : C++ method name to export
 *
 *  @par Common suffixes
 *
 *  The `_NAME`, `_DOC` and `_EX` suffixes allow you to specify a custom Python name, docstring, or
 *  (in rare cases) a custom dispatcher name.
 *
 *  | Macro                      | Fixed parameters            | Adds parameters                   | Use for ...                    |
 *  |----------------------------|-----------------------------|-----------------------------------|--------------------------------|
 *  | `PY_CLASS_METHOD_NAME`     | `i_cppClass`, `i_cppMethod` | `s_name`                          | Custom Python name             |
 *  | `PY_CLASS_METHOD_DOC`      | `i_cppClass`, `i_cppMethod` | `s_doc`                           | With docstring                 |
 *  | `PY_CLASS_METHOD_NAME_DOC` | `i_cppClass`, `i_cppMethod` | `s_name`, `s_doc`                 | Custom Python name + Docstring |
 *  | `PY_CLASS_METHOD_EX`       | `t_cppClass`, `i_cppMethod` | `s_name`, `s_doc`, `i_dispatcher` | Custom dispatcher name         |
 *
 *  See above in @ref ClassMethods for more details about overloading and special operators.
 *
 *  @{
 */

/** @brief Export a C++ method to Python, with full control.
 *  @ingroup ClassMethods
 *
 *  This is the most flexible method export macro. In contrast to the convenience macros, here you
 *  can provide the fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_EX(Menu, spam, "do_spam", "Do Spam", menu_spam)  // menu.do_spam(42)
 *  ```
 */
#define PY_CLASS_METHOD_EX(t_cppClass, i_cppMethod, s_methodName, s_doc, i_dispatcher)\
	PY_CLASS_METHOD_IMPL(t_cppClass, &TCppClass::i_cppMethod, s_methodName, s_doc, i_dispatcher,\
		::lass::python::impl::CallMethod<TShadowTraits>::call)

/** @brief Export a C++ method to Python, with custom name and docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_EX() with auto-generated dispatcher name.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_NAME_DOC(Menu, spam, "do_spam", "Do Spam")  // menu.do_spam(42)
 *  ```
 */
#define PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, s_doc )\
		PY_CLASS_METHOD_EX(\
			i_cppClass, i_cppMethod, s_methodName, s_doc,\
			LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @brief Export a C++ method to Python, with custom Python name.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_NAME_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_NAME(Menu, spam, "do_spam")  // menu.do_spam(42)
 *  ```
 */
#define PY_CLASS_METHOD_NAME( i_cppClass, i_cppMethod, s_methodName )\
		PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, 0 )

/** @brief Export a C++ method to Python, with docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_NAME_DOC() with @a s_methodName derived from @a i_cppMethod.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (name will be used as Python name)
 *  @param s_doc Method documentation string (null-terminated C string literal)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_DOC(Menu, spam, "Do Spam")  // menu.spam(42)
 *  ```
 */
#define PY_CLASS_METHOD_DOC( i_cppClass, i_cppMethod, s_doc )\
		PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, LASS_STRINGIFY(i_cppMethod), s_doc)

/** @brief Export a C++ method to Python
 *  @ingroup ClassMethods
 *
 *  The simplest method export macro. Uses the C++ method name as the Python name and doesn't
 *  provide additional documentation.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (name will be used as Python name)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD(Menu, spam)  // menu.spam(42)
 *  ```
 */
#define PY_CLASS_METHOD( i_cppClass, i_cppMethod )\
		PY_CLASS_METHOD_DOC( i_cppClass, i_cppMethod, 0 )

/** @} */

// --- explicit qualified methods ------------------------------------------------------------------

/** @addtogroup ClassMethods
 *  @name Type-Qualified Method Export Macros
 *
 *  These macros export C++ class methods to Python with explicit type qualification to resolve
 *  method overload ambiguities. They provide fine-grained control over method signatures and are
 *  essential when exporting overloaded methods that would otherwise be ambiguous.
 *
 *  Use these macros when you have overloaded C++ methods and need to specify exactly which overload
 *  to export to Python by providing explicit return and parameter types.
 *
 *  The list of parameter types can be passed as a single lass::meta::TypeTuple, or as individual
 *  arguments. For the latter, the `_<N>` tells the number of arguments. The `_<N>` form is the most
 *  often used one, and simply packs its types into a `TypeTuple`
 *
 *  | Form                              | Adds parameters                                 |
 *  |-----------------------------------|-------------------------------------------------|
 *  | `PY_CLASS_METHOD_QUALIFIED`       | `t_return`, `t_params` as lass::meta::TypeTuple |
 *  | `PY_CLASS_METHOD_QUALIFIED_<N>`   | `t_return`, `t_P1`, `t_P2`, ... ``t_P<N>`       |
 *
 *  They combine with the `_NAME` and `_DOC` suffixes, with the `s_name`, `s_doc`, `i_dispatcher`
 *  arguments following the function return and parameter types.
 *
 *  See above in @ref ClassMethods for more details about overloading and special operators.
 *
 *  @{
 */

/** @brief Export an overloaded C++ method to Python, with full control.
 *  @ingroup ClassMethods
 *
 *  Use this macro instead of PY_CLASS_METHOD_EX when there are overloaded C++ methods that would
 *  create ambiguity. By explicitly specifying return type and parameter types, you can disambiguate
 *  which overload to export.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  This macro helps resolve method overload ambiguities by explicitly specifying
 *  the method signature to export. Overloads can be mixed with PY_CLASS_METHOD_EX methods.
 *
 *  @par Example
 *  ```cpp
 *  using TArgs1 = lass::meta::TypeTuple<int, int>;
 *  PY_CLASS_METHOD_QUALIFIED_EX(Menu, eggs, long, TArgs1, "add_eggs", "Add eggs", menu_eggs1) // i = menu.add_eggs(1, 2)
 *  using TArgs2 = lass::meta::TypeTuple<const std::string&, const std::string&>;
 *  PY_CLASS_METHOD_QUALIFIED_EX(Menu, eggs, std::vector<std::string>, TArgs2, "add_eggs", "Add eggs", menu_eggs2) // s = menu.add_eggs("a", "b")
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

/** @brief Export an overloaded 0-ary C++ method to Python, with full control.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_EX() for methods with 0 parameters.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_QUALIFIED_EX_0(Menu, eggs, long, "add_eggs", "Add eggs", foo_bar_a)
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_EX_0( t_cppClass, i_cppMethod, t_return, s_methodName, s_doc, i_dispatcher )\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		t_cppClass, i_cppMethod, t_return, ::lass::meta::TypeTuple<>, s_methodName, s_doc, i_dispatcher )
$[
/** @brief Export an overloaded $x-ary C++ method to Python, with full control.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_EX() for methods with exactly $x parameters.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the method (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_QUALIFIED_EX_$x(Menu, eggs, long, $(T$x)$, "add_eggs", "Add eggs", foo_bar_a)
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_EX_$x( t_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc, i_dispatcher )\
	typedef ::lass::meta::TypeTuple< $(t_P$x)$ > \
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams));\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		t_cppClass, i_cppMethod, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams)), s_methodName, s_doc,\
		i_dispatcher )
]$

/** @brief Export an overloaded C++ method to Python, with custom name and docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_EX() with automatically generated dispatcher name.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  using TArgs = lass::meta::TypeTuple<int, int>;
 *  PY_CLASS_METHOD_QUALIFIED_NAME_DOC(Menu, eggs, long, TArgs1, "add_eggs", "Add eggs") // i = menu.add_eggs(1, 2)
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		i_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @brief Export an overloaded 0-ary C++ method to Python, with custom name and docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_EX_0() with automatically generated dispatcher name.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0(Menu, eggs, long, "add_eggs", "Add eggs")
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0( i_cppClass, i_cppMethod, t_return, s_methodName, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_EX_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
$[
/** @brief Export an overloaded $x-ary C++ method to Python, with custom name and docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_EX_$x() with automatically generated dispatcher name.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the method (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x(Menu, eggs, long, $(T$x)$, "add_eggs", "Add eggs")
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_EX_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
]$

/** @brief Export an overloaded C++ method to Python, with custom Python name.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *
 *  @par Example
 *  ```cpp
 *  using TArgs = lass::meta::TypeTuple<int, int>;
 *  PY_CLASS_METHOD_QUALIFIED_NAME(Menu, eggs, long, TArgs1, "add_eggs") // i = menu.add_eggs(1, 2)
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME( i_cppClass, i_cppMethod, t_return, t_params, s_methodName )\
		PY_CLASS_METHOD_QUALIFIED_NAME_DOC(\
			i_cppClass, i_cppMethod, t_return, t_params, s_methodName, 0 )

/** @brief Export an overloaded 0-ary C++ method to Python, with custom Python name.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_QUALIFIED_NAME_0(Menu, eggs, long, "add_eggs")
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_0( i_cppClass, i_cppMethod, t_return, s_methodName )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, 0 )
$[
/** @brief Export an overloaded $x-ary C++ method to Python, with custom Python name.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the method (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_QUALIFIED_NAME_$x(Menu, eggs, long, "add_eggs")
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, 0 )
]$

/** @brief Export an overloaded C++ method to Python, with docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC() with @a s_methodName = `LASS_STRINGIFY(i_cppMethod)`.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (used as Python method name, may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  using TArgs = lass::meta::TypeTuple<int, int>;
 *  PY_CLASS_METHOD_QUALIFIED_DOC(Menu, eggs, long, TArgs1, "Add eggs") // i = menu.eggs(1, 2)
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC(\
		i_cppClass, i_cppMethod, t_return, t_params, LASS_STRINGIFY(i_cppMethod), s_doc )

/** @brief Export an overloaded 0-ary C++ method to Python, with docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0() with
 *  @a s_methodName = `LASS_STRINGIFY(i_cppMethod)`.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (used as Python method name, may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_QUALIFIED_DOC_0(Menu, eggs, long, "Add eggs")
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppMethod, t_return, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, LASS_STRINGIFY(i_cppMethod), s_doc )
$[
/** @brief Export an overloaded $x-ary C++ method to Python, with docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x() with
 *  @a s_methodName = `LASS_STRINGIFY(i_cppMethod)`.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (used as Python method name, may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the method (for disambiguation)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_QUALIFIED_DOC_$x(Menu, eggs, long, $(T$x)$, "Add eggs")
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, LASS_STRINGIFY(i_cppMethod), s_doc )
]$

/** @brief Export an overloaded C++ method to Python
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (used as Python method name, may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation)
 *
 *  @par Example
 *  ```cpp
 *  using TArgs = lass::meta::TypeTuple<int, int>;
 *  PY_CLASS_METHOD_QUALIFIED(Menu, eggs, long, TArgs1) // i = menu.eggs(1, 2)
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED( i_cppClass, i_cppMethod, t_return, t_params )\
	PY_CLASS_METHOD_QUALIFIED_DOC( i_cppClass, i_cppMethod, t_return, t_params, 0 )

/** @brief Export an overloaded 0-ary C++ method to Python
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_DOC_0() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (used as Python method name, may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_QUALIFIED_DOC_0(Menu, eggs, long)
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_0( i_cppClass, i_cppMethod, t_return )\
	PY_CLASS_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppMethod, t_return, 0 )
$[
/** @brief Export an overloaded $x-ary C++ method to Python
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_METHOD_QUALIFIED_DOC_$x() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  @param i_cppMethod C++ method name to export (used as Python method name, may be overloaded)
 *  @param t_return Return type of the method (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the method (for disambiguation)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_METHOD_QUALIFIED_DOC_$x(Menu, eggs, long, $(T$x)$)
 *  ```
 */
#define PY_CLASS_METHOD_QUALIFIED_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$ )\
	PY_CLASS_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, 0 )
]$

/** @} */

// --- "free" methods ------------------------------------------------------------------------------

/** @addtogroup ClassMethods
 *  @name Free Method Export Macros
 *
 *  Export C/C++ free functions as Python methods. The first parameter of the free function
 *  becomes the implicit 'self' parameter and must be a pointer or reference (const or non-const)
 *  to the class being exported. This is particularly useful for @ref ShadowClasses where adding
 *  methods directly to the class is undesirable or impossible.
 *
 *  The basic form is:
 *
 *  ```cpp
 *  PY_CLASS_FREE_METHOD( i_cppClass, i_cppFreeMethod )
 *  ```
 *
 *  with:
 *  - @a  i_cppClass : C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  - @a  i_cppFreeMethod : C++ function, `std::function`, lambda, or other callable to export as method
 *                          (name will be used as Python name)
 *
 *  @par Common suffixes
 *
 *  The `_NAME`, `_DOC` and `_EX` suffixes allow you to specify a custom Python name, docstring, or
 *  (in rare cases) a custom dispatcher name.
 *
 *  | Macro                           | Fixed parameters                | Adds parameters                   | Use for ...                    |
 *  |---------------------------------|---------------------------------|-----------------------------------|--------------------------------|
 *  | `PY_CLASS_FREE_METHOD_NAME`     | `i_cppClass`, `f_cppFreeMethod` | `s_methodName`                          | Custom Python name             |
 *  | `PY_CLASS_FREE_METHOD_DOC`      | `i_cppClass`, `i_cppFreeMethod` | `s_doc`                           | With docstring                 |
 *  | `PY_CLASS_FREE_METHOD_NAME_DOC` | `i_cppClass`, `f_cppFreeMethod` | `s_methodName`, `s_doc`                 | Custom Python name + Docstring |
 *  | `PY_CLASS_FREE_METHOD_EX`       | `t_cppClass`, `f_cppFreeMethod` | `s_methodName`, `s_doc`, `i_dispatcher` | Custom dispatcher name         |
 *
 *  Besides normal C++ functions, you can also use these macros to export std::function objects,
 *  lambda expressions, or any other callable as free methods.
 * 
 *  Overloading and special operators are also supported.
 * 
 *  See above in @ref ClassMethods for more details.
 *  @{
 */

/** @brief Export a free function as a Python method, with full control.
 *  @ingroup ClassMethods
 *
 *  This macro allows you to export a C/C++ free function as a Python method. The free function
 *  must accept a pointer or reference to the object as first argument (const or non-const).
 *  This is extremely useful when using shadow classes to export a C++ class, because in such cases,
 *  it's often undesirable or impossible to write the function as a method.
 *
 *  Like PY_CLASS_METHOD_EX(), this macro supports overloading. These overloads may be mixed with
 *  PY_CLASS_METHOD_EX() methods.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function, `std::function`, lambda, or other callable to export as method
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_METHOD_EX(Menu, sausage, "add_sausage", "Add sausage", menu_sausage) // menu.add_sausage()
 *  ```
 */
#define PY_CLASS_FREE_METHOD_EX(t_cppClass, f_cppFreeMethod, s_methodName, s_doc, i_dispatcher)\
	PY_CLASS_METHOD_IMPL(t_cppClass, f_cppFreeMethod, s_methodName, s_doc, i_dispatcher,\
		::lass::python::impl::CallMethod<TShadowTraits>::callFree)

/** @brief Export a free function as a Python method, with custom name and docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_EX() with automatically generated dispatcher name.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function, `std::function`, lambda, or other callable to export as method
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_METHOD_NAME_DOC(Menu, sausage, "add_sausage", "Add sausage") // menu.add_sausage()
 *  ```
 */
#define PY_CLASS_FREE_METHOD_NAME_DOC( i_cppClass, f_cppFreeMethod, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_EX(\
		i_cppClass, f_cppFreeMethod, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @brief Export a free function as a Python method, with custom Python name.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_NAME_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function, `std::function`, lambda, or other callable to export as method
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_METHOD_NAME(Menu, sausage, "add_sausage") // menu.add_sausage()
 *  ```
 */
#define PY_CLASS_FREE_METHOD_NAME( i_cppClass, f_cppFreeMethod, s_methodName )\
	PY_CLASS_FREE_METHOD_NAME_DOC( i_cppClass, f_cppFreeMethod, s_methodName, 0 )

/** @brief Export a free function as a Python method, with docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_NAME_DOC() with @ s_methodName = `LASS_STRINGIFY(i_cppFreeMethod)`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function, `std::function`, lambda, or other callable to export as method
 *                         (must be a valid identifier, used as Python name)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_METHOD_DOC(Menu, sausage, "Add sausage") // menu.sausage()
 *  ```
 */
#define PY_CLASS_FREE_METHOD_DOC( i_cppClass, i_cppFreeMethod, s_doc )\
	PY_CLASS_FREE_METHOD_NAME_DOC( i_cppClass, i_cppFreeMethod, LASS_STRINGIFY(i_cppFreeMethod), s_doc)

/** @brief Export a free function as a Python method
 *  @ingroup ClassMethods.
 *
 *  Wraps PY_CLASS_FREE_METHOD_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function, `std::function`, lambda, or other callable to export as method
 *                         (must be a valid identifier, used as Python name)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_METHOD(Menu, sausage) // menu.sausage()
 *  ```
 */
#define PY_CLASS_FREE_METHOD( i_cppClass, i_cppFreeMethod )\
	PY_CLASS_FREE_METHOD_DOC( i_cppClass, i_cppFreeMethod, 0 )

/** @} */

/** @addtogroup ClassMethods
 *  @name Type-Qualified Free Method Export Macros
 *
 *  Export C++ free functions as Python methods with explicit type qualification to resolve
 *  overload ambiguity. These macros require explicit specification of return type and parameter
 *  types, making them suitable for overloaded free functions.
 *
 *  The first parameter of the free function becomes the implicit 'self' parameter and must be a
 *  pointer or reference (const or non-const) to the class being exported.
 *
 *  The list of parameter types can be passed as a single lass::meta::TypeTuple, or as individual
 *  arguments. For the latter, the `_<N>` tells the number of arguments. The `_<N>` form is the most
 *  often used one, and simply packs its types into a `TypeTuple`
 *
 *  | Form                                 | Adds parameters                                 |
 *  |--------------------------------------|-------------------------------------------------|
 *  | `PY_CLASS_FREE_METHOD_QUALIFIED`     | `t_return`, `t_params` as lass::meta::TypeTuple |
 *  | `PY_CLASS_FREE_METHOD_QUALIFIED_<N>` | `t_return`, `t_P1`, `t_P2`, ... ``t_P<N>`       |
 *
 *  They combine with the `_NAME` and `_DOC` suffixes, with the `s_name`, `s_doc`, `i_dispatcher`
 *  arguments following the function return and parameter types.
 *
 *  See above in @ref ClassMethods for more details about overloading and special operators.
 *
 *  @note There's a special case for binary functions that implement reflected operators like
 *        `__radd__`. Because they are mapped on the same slot like the regular operator (e.g. both
 *        `__add__` and `__radd__` are mapped on lass::python::methods::_add_), they differ in the
 *        order of parameters: for the @b reversed operators, the `self` argument comes @b second.
 *
 *  @{
 */

/** @brief Export an overloaded free function as a Python method, with full control.
 *  @ingroup ClassMethods
 *
 *  This macro allows you to export a C++ free function as a Python method when there's
 *  ambiguity due to overloading. It explicitly specifies return type and parameter types
 *  to resolve such ambiguity. The free function must accept a pointer or reference to the
 *  object as first argument (const or non-const).
 *
 *  Like PY_CLASS_FREE_METHOD_EX(), this macro supports overloading and these overloads
 *  may be mixed with PY_CLASS_FREE_METHOD_EX() methods.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation), first is `self`
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  @par Example
 *  ```cpp
 *  using TArgs = lass::meta::TypeTuple<TMenuPtr, const std::string&, const std::string&>;
 *  PY_CLASS_FREE_METHOD_QUALIFIED_EX(Menu, bakedBeans, std::vector<std::string>, TArgs, "baked_beans", "Add baked beans", menu_baked_beans) // menu.baked_beans("a", "b")
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

/** @brief Export an overloaded 0-ary free function as a Python method, with full control.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX() for free functions with 0 parameters.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  @deprecated Doesn't work
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_EX_0( t_cppClass, f_cppFreeMethod, t_return, s_methodName, s_doc, i_dispatcher )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX(\
		t_cppClass, f_cppFreeMethod, t_return, ::lass::meta::TypeTuple<>, s_methodName, s_doc, i_dispatcher )
$[
/** @brief Export an overloaded $x-ary free function as a Python method, with full control.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX() for free functions with $x parameters.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the free function (for disambiguation), @a t_P1 is `self`
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_METHOD_QUALIFIED_EX_$x(Menu, bakedBeans, std::vector<std::string>, $(T$x)$, "baked_beans", "Add baked beans", menu_baked_beans) // menu.baked_beans("a", "b")
 *  ```
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_EX_$x( t_cppClass, f_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, s_doc, i_dispatcher )\
	typedef ::lass::meta::TypeTuple< $(t_P$x)$ > \
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams));\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX(\
		t_cppClass, f_cppFreeMethod, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams)), s_methodName, s_doc,\
		i_dispatcher )
]$

/** @brief Export an overloaded free function as a Python method, with custom name and docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX() with automatically generated dispatcher name.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation), first is `self`
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  using TArgs = lass::meta::TypeTuple<TMenuPtr, const std::string&, const std::string&>;
 *  PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC(Menu, bakedBeans, std::vector<std::string>, TArgs, "baked_beans", "Add baked beans") // menu.baked_beans("a", "b")
 *  ```
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC( i_cppClass, f_cppFreeMethod, t_return, t_params, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX(\
		i_cppClass, f_cppFreeMethod, t_return, t_params, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @brief Export an overloaded 0-ary free function as a Python method, with custom name and docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX_0() with automatically generated dispatcher name.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @deprecated Doesn't work
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0( i_cppClass, f_cppFreeMethod, t_return, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX_0(\
		i_cppClass, f_cppFreeMethod, t_return, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
$[
/** @brief Export an overloaded $x-ary free function as a Python method, with custom name and docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX_$x() with automatically generated dispatcher name.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the free function (for disambiguation), @a t_P1 is `self`
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x(Menu, bakedBeans, std::vector<std::string>, $(T$x)$, "baked_beans", "Add baked beans") // menu.baked_beans("a", "b")
 *  ```
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x( i_cppClass, f_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX_$x(\
		i_cppClass, f_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
]$

/** @brief Export an overloaded free function as a Python method, with custom Python name.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation), first is `self`
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *
 *  @par Example
 *  ```cpp
 *  using TArgs = lass::meta::TypeTuple<TMenuPtr, const std::string&, const std::string&>;
 *  PY_CLASS_FREE_METHOD_QUALIFIED_NAME(Menu, bakedBeans, std::vector<std::string>, TArgs, "baked_beans") // menu.baked_beans("a", "b")
 *  ```
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME( i_cppClass, f_cppFreeMethod, t_return, t_params, s_methodName )\
		PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC(\
			i_cppClass, f_cppFreeMethod, t_return, t_params, s_methodName, 0 )

/** @brief Export an overloaded 0-ary free function as a Python method, with custom Python name.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *
 *  @deprecated Doesn't work
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_0( i_cppClass, f_cppFreeMethod, t_return, s_methodName )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, f_cppFreeMethod, t_return, s_methodName, 0 )
$[
/** @brief Export an overloaded $x-ary free function as a Python method, with custom Python name.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param f_cppFreeMethod C++ function to export (may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the free function (for disambiguation), @a t_P1 is `self`
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_METHOD_QUALIFIED_NAME_$x(Menu, bakedBeans, std::vector<std::string>, $(T$x)$, "baked_beans") // menu.baked_beans("a", "b")
 *  ```
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_$x( i_cppClass, f_cppFreeMethod, t_return, $(t_P$x)$, s_methodName )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, f_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, 0 )
]$

/** @brief Export an overloaded free function as a Python method, with docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC() with
 *  @a s_methodName = `LASS_STRINGIFY(i_cppFreeMethod)`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function to export (must be valid identifier, used as Python name,
 *                         may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation), first is `self`
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  using TArgs = lass::meta::TypeTuple<TMenuPtr, const std::string&, const std::string&>;
 *  PY_CLASS_FREE_METHOD_QUALIFIED_DOC(Menu, bakedBeans, std::vector<std::string>, TArgs, "Add baked beans") // menu.bakedBeans("a", "b")
 *  ```
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_DOC( i_cppClass, i_cppFreeMethod, t_return, t_params, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC(\
		i_cppClass, i_cppFreeMethod, t_return, t_params, LASS_STRINGIFY(i_cppFreeMethod), s_doc )

/** @brief Export an overloaded 0-ary free function as a Python method, with docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0() with
 *  @a s_methodName = `LASS_STRINGIFY(i_cppFreeMethod)`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function to export (must be valid identifier, used as Python name,
 *                         may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @deprecated Doesn't work
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppFreeMethod, t_return, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, i_cppFreeMethod, t_return, LASS_STRINGIFY(i_cppFreeMethod), s_doc )
$[
/** @brief Export an overloaded $x-ary free function as a Python method, with docstring.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x() with
 *  @a s_methodName = `LASS_STRINGIFY(i_cppFreeMethod)`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function to export (must be valid identifier, used as Python name,
 *                         may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the free function (for disambiguation), @a t_P1 is `self`
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_METHOD_QUALIFIED_DOC_$x(Menu, bakedBeans, std::vector<std::string>, $(T$x)$, "Add baked beans") // menu.bakedBeans("a", "b")
 *  ```
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, LASS_STRINGIFY(i_cppFreeMethod), s_doc )
]$

/** @brief Export an overloaded free function as a Python method.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier, used as
 *                         Python method name, may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param t_params Parameter types as lass::meta::TypeTuple (for disambiguation), first is `self`
 *
 *  @par Example
 *  ```cpp
 *  using TArgs = lass::meta::TypeTuple<TMenuPtr, const std::string&, const std::string&>;
 *  PY_CLASS_FREE_METHOD_QUALIFIED(Menu, bakedBeans, std::vector<std::string>, TArgs) // menu.bakedBeans("a", "b")
 *  ```
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED( i_cppClass, i_cppFreeMethod, t_return, t_params )\
	PY_CLASS_FREE_METHOD_QUALIFIED_DOC( i_cppClass, i_cppFreeMethod, t_return, t_params, 0 )

/** @brief Export an overloaded 0-ary free function as a Python method.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_DOC_0() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier,
 *                         0 parameters, used as Python method name, may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *
 *  @deprecated Doesn't work
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_0( i_cppClass, i_cppFreeMethod, t_return )\
	PY_CLASS_FREE_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppFreeMethod, t_return, 0 )
$[
/** @brief Export an overloaded $x-ary free function as a Python method.
 *  @ingroup ClassMethods
 *
 *  Wraps PY_CLASS_FREE_METHOD_QUALIFIED_DOC_$x() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class you're exporting the method for
 *  @param i_cppFreeMethod C++ function identifier to export (must be valid identifier,
 *                         $x parameters, used as Python method name, may be overloaded)
 *  @param t_return Return type of the free function (for disambiguation)
 *  @param $(t_P$x)$ Parameter types for the free function (for disambiguation), @a t_P1 is `self`
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_METHOD_QUALIFIED_$x(Menu, bakedBeans, std::vector<std::string>, $(T$x)$) // menu.bakedBeans("a", "b")
 *  ```
 *
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$ )\
	PY_CLASS_FREE_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, 0 )
]$

/** @} */

// --- "casting" methods ------------------------------------------------------------------------------

/** @defgroup ClassMethodsCast Casting Method Export Macros (Deprecated)
 *  @ingroup ClassDefinition
 *
 *  @deprecated These casting method macros are deprecated. Use the regular or free method export macros instead.
 *
 *  Export C++ methods to Python with custom casting policies for type conversion. These macros
 *  allow on-the-fly type conversion using casting operators like PointerCast and CopyCast.
 *  However, these macros are deprecated and should be avoided in new code.
 *
 */

/** @ingroup ClassMethodsCast
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
 *  @param s_methodName Python method name (string literal), or special method from
 *                      lass::python::methods
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


/** @ingroup ClassMethodsCast
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
/** @ingroup ClassMethodsCast
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

/** @ingroup ClassMethodsCast
 *  @brief Convenience wrapper for PY_CLASS_METHOD_CAST_EX_0().
 *  @deprecated Use PY_CLASS_METHOD_NAME_DOC() or PY_CLASS_FREE_METHOD_NAME_DOC() instead.
 */
#define PY_CLASS_METHOD_CAST_NAME_DOC_0( i_cppClass, i_cppMethod, t_return, s_methodName, s_doc )\
	PY_CLASS_METHOD_CAST_EX_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)),\
		LASS_UNIQUENAME(LASS_CONCATENATE(TypelassPyImpl_method_, i_cppClass)))
$[
/** @ingroup ClassMethodsCast
 *  @brief Convenience wrapper for PY_CLASS_METHOD_CAST_EX_$x().
 *  @deprecated Use PY_CLASS_METHOD_NAME_DOC() or PY_CLASS_FREE_METHOD_NAME_DOC() instead.
 */
#define PY_CLASS_METHOD_CAST_NAME_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc )\
	PY_CLASS_METHOD_CAST_EX_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)),\
		LASS_UNIQUENAME(LASS_CONCATENATE(TypelassPyImpl_method_, i_cppClass)))
]$

/** @ingroup ClassMethodsCast
 *  @brief Convenience wrapper with no documentation.
 *  @deprecated Use PY_CLASS_METHOD_NAME() or PY_CLASS_FREE_METHOD_NAME() instead.
 */
#define PY_CLASS_METHOD_CAST_NAME( i_cppClass, i_cppMethod, t_return, t_params, s_methodName )\
		PY_CLASS_METHOD_CAST_NAME_DOC(\
			i_cppClass, i_cppMethod, t_return, t_params, s_methodName, 0 )

/** @ingroup ClassMethodsCast
 *  @brief Convenience wrapper with no documentation.
 *  @deprecated Use PY_CLASS_METHOD_NAME() or PY_CLASS_FREE_METHOD_NAME() instead.
 */
#define PY_CLASS_METHOD_CAST_NAME_0( i_cppClass, i_cppMethod, t_return, s_methodName )\
	PY_CLASS_METHOD_CAST_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, 0 )
$[
/** @ingroup ClassMethodsCast
 *  @brief Convenience wrapper with no documentation.
 *  @deprecated Use PY_CLASS_METHOD_NAME() or PY_CLASS_FREE_METHOD_NAME() instead.
 */
#define PY_CLASS_METHOD_CAST_NAME_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName )\
	PY_CLASS_METHOD_CAST_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, 0 )
]$

/** @ingroup ClassMethodsCast
 *  @brief Convenience wrapper with method name derived from C++ method.
 *  @deprecated Use PY_CLASS_METHOD_DOC() or PY_CLASS_FREE_METHOD_DOC() instead.
 */
#define PY_CLASS_METHOD_CAST_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_doc )\
	PY_CLASS_METHOD_CAST_NAME_DOC(\
		i_cppClass, i_cppMethod, t_return, t_params, LASS_STRINGIFY(i_cppMethod), s_doc )

/** @ingroup ClassMethodsCast
 *  @brief Convenience wrapper with method name derived from C++ method.
 *  @deprecated Use PY_CLASS_METHOD_DOC() or PY_CLASS_FREE_METHOD_DOC() instead.
 */
#define PY_CLASS_METHOD_CAST_DOC_0( i_cppClass, i_cppMethod, t_return, s_doc )\
	PY_CLASS_METHOD_CAST_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, LASS_STRINGIFY(i_cppMethod), s_doc )
$[
/** @ingroup ClassMethodsCast
 *  @brief Convenience wrapper with method name derived from C++ method.
 *  @deprecated Use PY_CLASS_METHOD_DOC() or PY_CLASS_FREE_METHOD_DOC() instead.
 */
#define PY_CLASS_METHOD_CAST_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_doc )\
	PY_CLASS_METHOD_CAST_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, LASS_STRINGIFY(i_cppMethod), s_doc )
]$

/** @ingroup ClassMethodsCast
 *  @brief Basic convenience wrapper with minimal parameters.
 *  @deprecated Use PY_CLASS_METHOD() or PY_CLASS_FREE_METHOD() instead.
 */
#define PY_CLASS_METHOD_CAST( i_cppClass, i_cppMethod, t_return, t_params )\
	PY_CLASS_METHOD_CAST_DOC( i_cppClass, i_cppMethod, t_return, t_params, 0 )

/** @ingroup ClassMethodsCast
 *  @brief Basic convenience wrapper with minimal parameters.
 *  @deprecated Use PY_CLASS_METHOD() or PY_CLASS_FREE_METHOD() instead.
 */
#define PY_CLASS_METHOD_CAST_0( i_cppClass, i_cppMethod, t_return )\
	PY_CLASS_METHOD_CAST_DOC_0( i_cppClass, i_cppMethod, t_return, 0 )
$[
/** @ingroup ClassMethodsCast
 *  @brief Basic convenience wrapper with minimal parameters.
 *  @deprecated Use PY_CLASS_METHOD() or PY_CLASS_FREE_METHOD() instead.
 */
#define PY_CLASS_METHOD_CAST_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$ )\
	PY_CLASS_METHOD_CAST_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, 0 )
]$


// --- static methods ------------------------------------------------------------------------------

/** @defgroup ClassStaticMethods Static Method Export Macros
 *  @ingroup ClassDefinition
 *
 *  @brief Export C++ static methods or free functions as Python static methods.
 *
 *  Static methods are called on the class itself rather than on instances, and do not receive an
 *  implicit `self` parameter.
 *
 *  The basic form is:
 *
 *  ```cpp
 *  PY_CLASS_STATIC_METHOD( i_cppClass, i_cppMethod )
 *  ```
 *
 *  with:
 *  - @a  i_cppClass : C++ class containing the method, or its @ref ShadowClasses "shadow class"
 *  - @a  i_cppMethod : C++ static method name to export
 *
 *  @par Common suffixes
 *
 *  The `_NAME`, `_DOC` and `_EX` suffixes allow you to specify a custom Python name, docstring, or
 *  (in rare cases) a custom dispatcher name.
 *
 *  | Macro                             | Fixed parameters              | Adds parameters                   | Use for ...                                                |
 *  |-----------------------------------|-------------------------------|-----------------------------------|------------------------------------------------------------|
 *  | `PY_CLASS_STATIC_METHOD_NAME`     | `i_cppClass`, `i_cppMethod`   | `s_name`                          | Custom Python name                                         |
 *  | `PY_CLASS_STATIC_METHOD_DOC`      | `i_cppClass`, `i_cppMethod`   | `s_doc`                           | With docstring                                             |
 *  | `PY_CLASS_STATIC_METHOD_NAME_DOC` | `i_cppClass`, `i_cppMethod`   | `s_name`, `s_doc`                 | Custom Python name + Docstring                             |
 *  | `PY_CLASS_STATIC_METHOD_EX`       | `t_cppClass`, `f_cppFunction` | `s_name`, `s_doc`, `i_dispatcher` | Free functions as static method, or custom dispatcher name |
 *
 *  There are no `_QUALIFIED` versions of these macros.
 * 
 *  @par Free functions, `std::function`, lambdas, and other callables as static methods.
 * 
 *  Also free functions, `std::function`, lambda expressions, and other callables can be exported as
 *  Python static methods, but you need to *only* use the `PY_CLASS_STATIC_METHOD_EX()`.
 * 
 *  @note Callables with overloaded `operator()` are not supported, and neither are callables with
 *        template `operator()` such as generic lambdas like `[](auto a, auto b) { return a + b; }`.
 *
 *  @par Overloading Python static methods
 *
 *  Multiple static methods can be exported to the same Python name, to create a Python static
 *  method that is overloaded on the parameter types.
 *
 *  @note Overload resolution uses first-fit, not best-fit like C++. The first exported overload
 *        that matches the arguments will be called.
 *
 *  @par Example
 *
 *  ```cpp
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      static void bar(int a);
 *      static void baz(const std::string& s);
 *  };
 *
 *  Foo spam(int b, int c);
 *
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_STATIC_METHOD_DOC(Foo, bar, "a regular C++ static method")
 *  PY_CLASS_STATIC_METHOD_EX(Foo, Foo::baz, "baz", "another C++ static method", foo_baz)
 *  PY_CLASS_STATIC_METHOD_EX(Foo, spam, "spam", "free function as static method", foo_spam)
 *  ```
 */


/** @brief Export a C++ static method to Python, with full control.
 *  @ingroup ClassStaticMethods
 *
 *  This macro exports a C++ static method or free function as a Python static method (class method).
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @note Unlike the other convenience macros, you must use the full name of the C++ static method,
 *        including the class name.
 *
 *  @note This is the only macro that allows you to export a free function as static method.
 *
 *  @param t_cppClass C++ class to add the static method to
 *  @param f_cppFunction C++ static method, C++ function, `std::function`, lambda, or other callable
 *                       that implements the static method
 *  @param s_methodName Python method name (null-terminated C string literal)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  ```cpp
 *  PY_CLASS_STATIC_METHOD_EX(Foo, Foo::bar, "bar", "a regular C++ static method", foo_bar)
 *  PY_CLASS_STATIC_METHOD_EX(Foo, spam, "spam", "free function as static method", foo_spam)
 *  PY_CLASS_STATIC_METHOD_EX(Foo, ([](int a, int b) { return a * b; }), "adder", "lambda as static method", foo_adder)
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
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain ),\
		t_cppClass ::_lassPyClassDef.addStaticMethod(\
			s_methodName, s_doc, i_dispatcher, LASS_CONCATENATE(i_dispatcher, _overloadChain));\
	)

/** @brief Export a C++ static method to Python, with docstring.
 *  @ingroup ClassStaticMethods
 *
 *  Wraps PY_CLASS_STATIC_METHOD_EX() for C++ static member functions with automatically generated
 *  dispatcher name and method name derived from C++ method name.
 *
 *  @param i_cppClass C++ class to add the static method to (unqualified name)
 *  @param i_cppMethod Name of the C++ static method to export
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_STATIC_METHOD_DOC(Foo, bar, "Do some bar")
 *  ```
 */
#define PY_CLASS_STATIC_METHOD_DOC( i_cppClass, i_cppMethod, s_doc )\
	PY_CLASS_STATIC_METHOD_EX(\
		i_cppClass,\
		&::lass::python::impl::ShadowTraits<i_cppClass>::TCppClass::i_cppMethod,\
		LASS_STRINGIFY(i_cppMethod), s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_staticMethod_, i_cppClass)))

/** @brief Export a C++ static method to Python, with custom name and docstring.
 *  @ingroup ClassStaticMethods
 *
 *  Wraps PY_CLASS_STATIC_METHOD_EX() for C++ static member functions with automatically generated
 *  dispatcher name.
 *
 *  @param i_cppClass C++ class to add the static method to (unqualified name)
 *  @param i_cppMethod Name of the C++ static method to export
 *  @param s_methodName Python method name (null-terminated C string literal)
 *  @param s_doc Method documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_STATIC_METHOD_NAME_DOC(Foo, bar, "do_bar", "Do some bar")
 *  ```
 */
#define PY_CLASS_STATIC_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, s_doc )\
	PY_CLASS_STATIC_METHOD_EX(\
		i_cppClass,\
		&::lass::python::impl::ShadowTraits<i_cppClass>::TCppClass::i_cppMethod,\
		s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_staticMethod_, i_cppClass)))


/** @brief Export a C++ static method to Python, with custom Python name.
 *  @ingroup ClassStaticMethods
 *
 *  Wraps PY_CLASS_STATIC_METHOD_EX() for C++ static member functions with automatically generated
 *  dispatcher name and no documentation.
 *
 *  @param i_cppClass C++ class to add the static method to (unqualified name)
 *  @param i_cppMethod Name of the C++ static method to export
 *  @param s_methodName Python method name (null-terminated C string literal)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_STATIC_METHOD_NAME(Foo, bar, "baz")
 *  ```
 */
#define PY_CLASS_STATIC_METHOD_NAME( i_cppClass, i_cppMethod, s_methodName)\
	PY_CLASS_STATIC_METHOD_EX(\
		i_cppClass,\
		&::lass::python::impl::ShadowTraits<i_cppClass>::TCppClass::i_cppMethod,\
		s_methodName, "",\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_staticMethod_, i_cppClass)))


/** @brief Export a C++ static method to Python.
 *  @ingroup ClassStaticMethods
 *
 *  Wraps PY_CLASS_STATIC_METHOD_DOC() with no documentation.
 *
 *  @param i_cppClass C++ class to add the static method to (unqualified name)
 *  @param i_cppMethod Name of the C++ static method to export (used as Python method name)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_STATIC_METHOD(Foo, bar)
 *  ```
 */
#define PY_CLASS_STATIC_METHOD( i_cppClass, i_cppMethod )\
		PY_CLASS_STATIC_METHOD_DOC( i_cppClass, i_cppMethod, 0 )



// --- data members --------------------------------------------------------------------------------

/** @defgroup ClassMembers Data Member Export Macros
 *  @ingroup ClassDefinition
 *
 *  Export C++ class data members as Python properties.
 *
 *  These macros create Python properties that provide access to C++ class data through various
 *  access patterns: getter/setter methods, free functions, or direct public member access.
 *
 *  Macro names follow the regular grammar (see @ref PythonMacroName), but there are no
 *  `_QUALIFIED` versions.
 *
 *  @par Read/Write
 *
 *  | Member Methods                | Free Functions                     | Public Member                     |
 *  |-------------------------------|------------------------------------|-----------------------------------|
 *  | PY_CLASS_MEMBER_RW()          | PY_CLASS_FREE_MEMBER_RW()          | PY_CLASS_PUBLIC_MEMBER()          |
 *  | PY_CLASS_MEMBER_RW_NAME()     | PY_CLASS_FREE_MEMBER_RW_NAME()     | PY_CLASS_PUBLIC_MEMBER_NAME()     |
 *  | PY_CLASS_MEMBER_RW_DOC()      | PY_CLASS_FREE_MEMBER_RW_DOC()      | PY_CLASS_PUBLIC_MEMBER_DOC()      |
 *  | PY_CLASS_MEMBER_RW_NAME_DOC() | PY_CLASS_FREE_MEMBER_RW_NAME_DOC() | PY_CLASS_PUBLIC_MEMBER_NAME_DOC() |
 *  | PY_CLASS_MEMBER_RW_EX()       | PY_CLASS_FREE_MEMBER_RW_EX()       | PY_CLASS_PUBLIC_MEMBER_EX()       |
 *
 *  @par Read-only
 *
 *  | Member Methods               | Free Functions                    | Public Member                       |
 *  |------------------------------|-----------------------------------|-------------------------------------|
 *  | PY_CLASS_MEMBER_R()          | PY_CLASS_FREE_MEMBER_R()          | PY_CLASS_PUBLIC_MEMBER_R()          |
 *  | PY_CLASS_MEMBER_R_NAME()     | PY_CLASS_FREE_MEMBER_R_NAME()     | PY_CLASS_PUBLIC_MEMBER_R_NAME()     |
 *  | PY_CLASS_MEMBER_R_DOC()      | PY_CLASS_FREE_MEMBER_R_DOC()      | PY_CLASS_PUBLIC_MEMBER_R_DOC()      |
 *  | PY_CLASS_MEMBER_R_NAME_DOC() | PY_CLASS_FREE_MEMBER_R_NAME_DOC() | PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC() |
 *  | PY_CLASS_MEMBER_R_EX()       | PY_CLASS_FREE_MEMBER_R_EX()       | PY_CLASS_PUBLIC_MEMBER_R_EX()       |

 */

/** @addtogroup ClassMembers
 *  @name Member Method Properties
 *
 *  Use C++ methods as getter/setter to define a Python property
 *
 *  @par Non-const and Const Getter
 *
 *  Although Python doesn't know the concept of constness, @ref ShadowClasses may contain a
 *  reference to a constant shadowee instance. Getters may be overloaded on this constness and the
 *  `PY_CLASS_MEMBER_*` macros will take this into account. This allows you to overload a getter and
 *  return a non-const pointer to a member if the shadowee isn't const.
 *
 *  ```cpp
 *  class Foo
 *  {
 *  public:
 *      lass::util::SharedPtr<const Bar> bar() const;
 *      lass::util::SharedPtr<Bar> bar();
 *  };
 *
 *  PY_SHADOW_CLASS(DLL_EXPORT, PyFoo, Foo)
 *  PY_SHADOW_CASTERS(PyFoo)
 *  PY_DECLARE_CLASS_NAME(PyFoo, "Foo")
 *  PY_CLASS_MEMBER_R(PyFoo, bar) // will return SharedPtr<const Bar> or SharedPtr<Bar> depending on Foo constness
 *  ```
 *
 *  This also works in combination with a setter, using the `PY_CLASS_MEMBER_RW*` macros.
 *
 *  @par Two Forms of Setters
 *
 *  Setters may have one of following signatures:
 *
 *  - take the new value as parameter, and have void as return type:
 *    ```cpp
 *    void setName(const std::string& name);
 *    ```
 *  - have no paramater, but return a non-const reference to the member so it can be assigned to.
 *    ```cpp
 *    float& price();
 *    ```
 *
 *  See example below.
 *
 *
 *  @par Example
 *
 *  ```cpp
 *  class Parrot: public lass::python::PyObjectPlus
 *  {
 *      PY_HEADER(lass::python::PyObjectPlus)
 *  public:
 *      bool isAlive() const;
 *
 *      const std::string& name() const;
 *      void setName(const std::string& name); // setter with parameter
 *
 *      float price() const;
 *      float& price(); // setter through non-const reference
 *  };
 *
 *  PY_DECLARE_CLASS(Parrot)
 *  PY_CLASS_MEMBER_R_NAME_DOC(Parrot, isAlive, "alive", "is True if parrot isn't dead yet") // p.alive
 *  PY_CLASS_MEMBER_RW(Parrot, name, setName)                                                // p.name
 *  PY_CLASS_MEMBER_RW(Parrot, price, price)                                                 // p.price
 *  ```
 *
 *  @note Member exports cannot be overloaded on types, and there are no qualified versions of these
 *        macros.
 *
 *  @{
 */

/** @brief Export a getter/setter pair as a Python property, with full control.
 *  @ingroup ClassMembers
 *
 *  This is the most flexible read-write member export macro, allowing manual dispatcher naming.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  Exports a pair of C++ getter and setter methods as a Python read/write property.
 *
 *  @param t_cppClass C++ class containing the getter and setter methods
 *  @param i_cppGetter C++ method name of getter
 *  @param i_cppSetter C++ method name of setter
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_MEMBER_RW_NAME_DOC(Parrot, getName, setName, "name", "the name of the parrot", parrot_name)
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

/** @brief Export a getter/setter pair as a Python property, with custom name and docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_MEMBER_RW_EX() with auto-generated dispatcher name.
 *
 *  @param i_cppClass C++ class containing the getter and setter methods (unqualified name)
 *  @param i_cppGetter C++ method name of getter
 *  @param i_cppSetter C++ method name of setter
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_MEMBER_RW_NAME_DOC(Parrot, getName, setName, "name", "the name of the parrot")
 *  ```
 */
#define PY_CLASS_MEMBER_RW_NAME_DOC(i_cppClass, i_cppGetter, i_cppSetter, s_memberName, s_doc)\
	PY_CLASS_MEMBER_RW_EX(i_cppClass, i_cppGetter, i_cppSetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_memberRW, i_cppClass)))

/** @brief Export a getter/setter pair as a Python property, with custom Python name.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_MEMBER_RW_NAME_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class containing the getter and setter methods (unqualified name)
 *  @param i_cppGetter C++ method name of getter
 *  @param i_cppSetter C++ method name of setter
 *  @param s_memberName Python property name (null-terminated C string literal)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_MEMBER_RW_NAME(Parrot, getName, setName, "name")
 *  ```
 */
#define PY_CLASS_MEMBER_RW_NAME(i_cppClass, i_cppGetter, i_cppSetter, s_memberName)\
	PY_CLASS_MEMBER_RW_NAME_DOC(i_cppClass, i_cppGetter, i_cppSetter, s_memberName, 0)

/** @brief Export a getter/setter pair as a Python property, with docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_MEMBER_RW_NAME_DOC() with s_memberName derived from i_cppGetter.
 *
 *  @param i_cppClass C++ class containing the getter and setter methods (unqualified name)
 *  @param i_cppGetter C++ method name of getter (also used as Python property name)
 *  @param i_cppSetter C++ method name of setter
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_MEMBER_RW_DOC(Parrot, name, setName, "the name of the parrot")
 *  ```
 */
#define PY_CLASS_MEMBER_RW_DOC(i_cppClass, i_cppGetter, i_cppSetter, s_doc)\
	PY_CLASS_MEMBER_RW_NAME_DOC(i_cppClass, i_cppGetter, i_cppSetter, LASS_STRINGIFY(i_cppGetter), s_doc)

/** @brief Export a getter/setter pair as a Python property.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_MEMBER_RW_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class containing the getter and setter methods (unqualified name)
 *  @param i_cppGetter C++ method name of getter (also used as Python property name)
 *  @param i_cppSetter C++ method name of setter
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_MEMBER_RW_DOC(Parrot, name, setName)
 *  ```
 */
#define PY_CLASS_MEMBER_RW(i_cppClass, i_cppGetter, i_cppSetter)\
	PY_CLASS_MEMBER_RW_DOC(i_cppClass, i_cppGetter, i_cppSetter, 0)



/** @brief Export a getter as a read-only Python property, with full control.
 *  @ingroup ClassMembers
 *
 *  This is the most flexible read-only member export macro, allowing manual dispatcher naming.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  Exports a C++ getter method as a Python read-only property (no setter provided).
 *
 *  @param t_cppClass C++ class containing the getter method
 *  @param i_cppGetter C++ method name of getter
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher function
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_MEMBER_R_EX(Parrot, isAlive, "alive", "True if parrot isn't dead", parrot_alive)
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


/** @brief Export a getter as a read-only Python property, with custom name and docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_MEMBER_R_EX() with auto-generated dispatcher name.
 *
 *  @param i_cppClass C++ class containing the getter method (unqualified name)
 *  @param i_cppGetter C++ method name of getter
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_MEMBER_R_DOC(Parrot, isAlive, "alive", "True if the parrot is not dead")
 *  ```
 */
#define PY_CLASS_MEMBER_R_NAME_DOC(i_cppClass, i_cppGetter, s_memberName, s_doc)\
	PY_CLASS_MEMBER_R_EX(i_cppClass, i_cppGetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_memberR, i_cppClass)))

/** @brief Export a getter as a read-only Python property, with custom Python name.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_MEMBER_R_NAME_DOC() with @a s_doc = `nullptr`.
 *
 *  @param i_cppClass C++ class containing the getter method (unqualified name)
 *  @param i_cppGetter C++ method name of getter
 *  @param s_memberName Python property name (null-terminated C string literal)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_MEMBER_R_DOC(Parrot, isAlive, "alive")
 *  ```
 */
#define PY_CLASS_MEMBER_R_NAME(i_cppClass, i_cppGetter, s_memberName)\
	PY_CLASS_MEMBER_R_NAME_DOC(i_cppClass, i_cppGetter, s_memberName, 0)

/** @brief Export a getter as a read-only Python property, with docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_MEMBER_R_NAME_DOC() with s_memberName derived from i_cppGetter.
 *
 *  @param i_cppClass C++ class containing the getter method (unqualified name)
 *  @param i_cppGetter C++ method name of getter (also used as Python property name)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_MEMBER_R_DOC(Parrot, alive, "True if the parrot is not dead")
 *  ```
 */
#define PY_CLASS_MEMBER_R_DOC(i_cppClass, i_cppGetter, s_doc)\
	PY_CLASS_MEMBER_R_NAME_DOC(i_cppClass, i_cppGetter, LASS_STRINGIFY(i_cppGetter), s_doc)

/** @brief Export a getter as a read-only Python property.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_MEMBER_R_DOC() with method name as property name and no documentation.
 *
 *  @param i_cppClass C++ class containing the getter method (unqualified name)
 *  @param i_cppGetter C++ method name of getter (also used as Python property name)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_MEMBER_R_DOC(Parrot, alive)
 *  ```
 */
#define PY_CLASS_MEMBER_R(i_cppClass, i_cppGetter)\
	PY_CLASS_MEMBER_R_DOC(i_cppClass, i_cppGetter, 0)

/** @} */



/** @addtogroup ClassMembers
 *  @name Free Function Properties
 *
 *  Export C/C++ free functions as Python properties.
 *
 *  Use these when the getter/setter can't be a member. This is particularly useful for
 *  @ref ShadowClasses where adding methods directly to the class is undesirable or impossible.
 *
 *  @par Example
 *  ```cpp
 *  class Parrot
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      std::string name;
 *  };
 *
 *  int getName(const Parrot& self)
 *  {
 *      return self.name;
 *	}
 *  void setName(Parrot& self, const std::string& name)
 *  {
 *      self.name = name;
 *  }
 *
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_FREE_MEMBER_RW_NAME(Parrot, getName, setName, "parrot")
 *  ```
 *
 *  @note Member exports cannot be overloaded on types, and there are no qualified versions of these
 *        macros.
 *
 *  @{
 */

/** @brief Export free accessors as a read-write Python property, with full control.
 *  @ingroup ClassMembers
 *
 *  Exports a pair of free functions as read/write Python property. Unlike member method-based
 *  macros, this uses standalone functions that take the object as their first parameter.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class to add the property to
 *  @param i_cppFreeGetter Free getter function
 *  @param i_cppFreeSetter Free setter function
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_MEMBER_RW_EX(Parrot, getName, setName, "name", "Parrot's name", parrot_name)
 *  ```
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

/** @brief Export free accessors as a read-write Python property, with custom name and docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_FREE_MEMBER_RW_EX() with auto-generated dispatcher name.
 *
 *  @param i_cppClass C++ class to add the property to (unqualified name)
 *  @param i_cppFreeGetter Free getter function
 *  @param i_cppFreeSetter Free setter function
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_MEMBER_RW_NAME_DOC(Parrot, getName, setName, "name", "Parrot's name")
 *  ```
 */
#define PY_CLASS_FREE_MEMBER_RW_NAME_DOC(i_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName, s_doc)\
	PY_CLASS_FREE_MEMBER_RW_EX(i_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_freeMemberRW, i_cppClass)))

/** @brief Export free accessors as a read-write Python property, with custom Python name.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_FREE_MEMBER_RW_NAME_DOC() with no documentation.
 *
 *  @param i_cppClass C++ class to add the property to (unqualified name)
 *  @param i_cppFreeGetter Free getter function
 *  @param i_cppFreeSetter Free setter function
 *  @param s_memberName Python property name (null-terminated C string literal)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_MEMBER_RW_NAME_DOC(Parrot, getName, setName, "name")
 *  ```
 */
#define PY_CLASS_FREE_MEMBER_RW_NAME(i_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName)\
	PY_CLASS_FREE_MEMBER_RW_NAME_DOC(i_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName, 0)

/** @brief Export free accessors as a read-write Python property, with docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_FREE_MEMBER_RW_NAME_DOC() with function name as property name.
 *
 *  @param i_cppClass C++ class to add the property to (unqualified name)
 *  @param i_cppFreeGetter Free getter function (also used as Python property name)
 *  @param i_cppFreeSetter Free setter function
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_MEMBER_RW_DOC(Parrot, name, setName, "Parrot's name")
 *  ```
 */
#define PY_CLASS_FREE_MEMBER_RW_DOC(i_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_doc)\
	PY_CLASS_FREE_MEMBER_RW_NAME_DOC(i_cppClass, i_cppFreeGetter, i_cppFreeSetter, LASS_STRINGIFY(i_cppFreeGetter), s_doc)

/** @brief Export free accessors as a read-write Python property.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_FREE_MEMBER_RW_DOC() with function name as property name and no documentation.
 *
 *  @param i_cppClass C++ class to add the property to (unqualified name)
 *  @param i_cppFreeGetter Free getter function (also used as Python property name)
 *  @param i_cppFreeSetter Free setter function
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_MEMBER_RW_DOC(Parrot, name, setName)
 *  ```
 */
#define PY_CLASS_FREE_MEMBER_RW(i_cppClass, i_cppFreeGetter, i_cppFreeSetter)\
	PY_CLASS_FREE_MEMBER_RW_DOC(i_cppClass, i_cppFreeGetter, i_cppFreeSetter, 0)



/** @brief Export a free accessor as a read-only Python property, with full control.
 *  @ingroup ClassMembers
 *
 *  Exports a free function as read-only Python property. Unlike member method-based macros, this
 *  uses a standalone function that takes the object as its first parameter.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class to add the property to
 *  @param i_cppFreeGetter Free getter function
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_MEMBER_R_EX(Parrot, getColor, "color", "Parrot's color", parrot_color)
 *  ```
 */
#define PY_CLASS_FREE_MEMBER_R_EX( t_cppClass, i_cppFreeGetter, s_memberName, s_doc, i_dispatcher )\
	extern "C" LASS_DLL_LOCAL PyObject* LASS_CONCATENATE(i_dispatcher, _getter)( PyObject* iObject, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		return ::lass::python::impl::CallMethod<TShadowTraits>::freeGet( iObject, i_cppFreeGetter );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), 0);\
	)

/** @brief Export a free accessor as a read-only Python property, with custom name and docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_FREE_MEMBER_R_EX() with auto-generated dispatcher name.
 *
 *  @param i_cppClass C++ class to add the property to (unqualified name)
 *  @param i_cppFreeGetter Free getter function
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_MEMBER_R_NAME_DOC(Parrot, getColor, "color", "Parrot's color")
 *  ```
 */
#define PY_CLASS_FREE_MEMBER_R_NAME_DOC(i_cppClass, i_cppFreeGetter, s_memberName, s_doc)\
	PY_CLASS_FREE_MEMBER_R_EX(i_cppClass, i_cppFreeGetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_freeMemberR, i_cppClass)))

/** @brief Export a free accessor as a read-only Python property, with custom Python name.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_FREE_MEMBER_R_NAME_DOC() with no documentation.
 *
 *  @param i_cppClass C++ class to add the property to (unqualified name)
 *  @param i_cppFreeGetter Free getter function
 *  @param s_memberName Python property name (null-terminated C string literal)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_MEMBER_R_NAME(Parrot, getColor, "color")
 *  ```
 */
#define PY_CLASS_FREE_MEMBER_R_NAME(i_cppClass, i_cppFreeGetter, s_memberName)\
	PY_CLASS_FREE_MEMBER_R_NAME_DOC(i_cppClass, i_cppFreeGetter, s_memberName, 0)

/** @brief Export a free accessor as a read-only Python property, with docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_FREE_MEMBER_R_NAME_DOC() with function name as property name.
 *
 *  @param i_cppClass C++ class to add the property to (unqualified name)
 *  @param i_cppFreeGetter Free getter function (also used as Python property name)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_MEMBER_R_DOC(Parrot, color, "Parrot's color")
 *  ```
 */
#define PY_CLASS_FREE_MEMBER_R_DOC(i_cppClass, i_cppFreeGetter, s_doc)\
	PY_CLASS_FREE_MEMBER_R_NAME_DOC(i_cppClass, i_cppFreeGetter, LASS_STRINGIFY(i_cppFreeGetter), s_doc)

/** @brief Export a free accessor as a read-only Python property.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_FREE_MEMBER_R_DOC() with function name as property name and no documentation.
 *
 *  @param i_cppClass C++ class to add the property to (unqualified name)
 *  @param i_cppFreeGetter Free getter function (also used as Python property name)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_FREE_MEMBER_R(Parrot, color)
 *  ```
 */
#define PY_CLASS_FREE_MEMBER_R(i_cppClass, i_cppFreeGetter)\
	PY_CLASS_FREE_MEMBER_R_DOC(i_cppClass, i_cppFreeGetter, 0)

/** @} */



/** @addtogroup ClassMembers
 *  @name Public Member Properties
 *
 *  Exports a public data member directly as a Python property, allowing both read and write access.
 *  This provides direct access to the C++ member variable without requiring getter/setter methods.
 *
 *  @par Example
 *  ```cpp
 *  class Parrot
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      std::string name;
 *      const Color color;
 *  };
 *
 *  PY_DECLARE_CLASS(Parrot)
 *  PY_CLASS_PUBLIC_MEMBER_DOC(Parrot, name, "Parrot's name")
 *  PY_CLASS_PUBLIC_MEMBER_R(Parrot, color)
 *  ```
 *
 *  @note Member exports cannot be overloaded on types, and there are no qualified versions of these
 *        macros.
 *
 *  @{
 */

/** @brief Export a public member as a read-write Python property, with full control.
 *  @ingroup ClassMembers
 *
 *  Exports a public data member directly as a Python property, allowing both read and write access.
 *  This provides direct access to the C++ member variable without requiring getter/setter methods.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_PUBLIC_MEMBER_EX(Parrot, name, "name", "Parrot's name", parrot_name)
 *  ```
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

/** @brief Export a public member as a read-write Python property, with custom name and docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_PUBLIC_MEMBER_EX() with auto-generated dispatcher name.
 *
 *  @param i_cppClass C++ class containing the public member (unqualified name)
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_PUBLIC_MEMBER_NAME_DOC(Parrot, name, "name", "Parrot's name")
 *  ```
 */
#define PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, s_memberName, s_doc )\
	PY_CLASS_PUBLIC_MEMBER_EX( i_cppClass, i_cppMember, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_publicMember_, i_cppClass)))

/** @brief Export a public member as a read-write Python property, with custom Python name.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_PUBLIC_MEMBER_NAME_DOC() with no documentation.
 *
 *  @param i_cppClass C++ class containing the public member (unqualified name)
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_PUBLIC_MEMBER_NAME(Parrot, name, "name")
 *  ```
 */
#define PY_CLASS_PUBLIC_MEMBER_NAME( i_cppClass, i_cppMember, s_memberName )\
	PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, s_memberName, 0 )

/** @brief Export a public member as a read-write Python property, with docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_PUBLIC_MEMBER_NAME_DOC() with member name as property name.
 *
 *  @param i_cppClass C++ class containing the public member (unqualified name)
 *  @param i_cppMember C++ public data member name to export (also used as Python property name)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_PUBLIC_MEMBER_NAME_DOC(Parrot, name, "name")
 *  ```
 */
#define PY_CLASS_PUBLIC_MEMBER_DOC( i_cppClass, i_cppMember , s_doc)\
	PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember), s_doc )

/** @brief Export a public member as a read-write Python property.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_PUBLIC_MEMBER_NAME_DOC() with member name as property name and no documentation.
 *
 *  @param i_cppClass C++ class containing the public member (unqualified name)
 *  @param i_cppMember C++ public data member name to export (also used as Python property name)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_PUBLIC_MEMBER_NAME_DOC(Parrot, name)
 *  ```
 */
#define PY_CLASS_PUBLIC_MEMBER( i_cppClass, i_cppMember )\
	PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember), 0 )




/** @brief Export a public member as a read-only Python property, with full control.
 *  @ingroup ClassMembers
 *
 *  Exports a public data member directly as a read-only Python property, allowing only read access.
 *  This provides direct read access to the C++ member variable without requiring a getter method.
 *  Useful for const members or when write access should be restricted.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class containing the public member
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_PUBLIC_MEMBER_R_EX(Parrot, color, "color", "Parrot's color", parrot_color)
 *  ```
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

/** @brief Export a public member as a read-only Python property, with custom name and docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_PUBLIC_MEMBER_R_EX() with auto-generated dispatcher name.
 *
 *  @param i_cppClass C++ class containing the public member (unqualified name)
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(Parrot, color, "color", "Parrot's color")
 *  ```
 */
#define PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, s_memberName, s_doc)\
	PY_CLASS_PUBLIC_MEMBER_R_EX(i_cppClass, i_cppMember, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_publicMemberR_, i_cppClass)))

/** @brief Export a public member as a read-only Python property, with custom Python name.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC() with no documentation.
 *
 *  @param i_cppClass C++ class containing the public member (unqualified name)
 *  @param i_cppMember C++ public data member name to export
 *  @param s_memberName Python property name (null-terminated C string literal)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_PUBLIC_MEMBER_R_NAME(Parrot, color, "color")
 *  ```
 */
#define PY_CLASS_PUBLIC_MEMBER_R_NAME(i_cppClass, i_cppMember, s_memberName)\
	PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, s_memberName, 0 )

/** @brief Export a public member as a read-only Python property, with docstring.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC() with member name as property name.
 *
 *  @param i_cppClass C++ class containing the public member (unqualified name)
 *  @param i_cppMember C++ public data member name to export (also used as Python property name)
 *  @param s_doc Property documentation string (null-terminated C string literal, may be nullptr)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_PUBLIC_MEMBER_R_DOC(Parrot, color, "Parrot's color")
 *  ```
 */
#define PY_CLASS_PUBLIC_MEMBER_R_DOC(i_cppClass, i_cppMember , s_doc)\
	PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember),  s_doc)

/** @brief Export a public member as a read-only Python property.
 *  @ingroup ClassMembers
 *
 *  Wraps PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC() with member name as property name and no documentation.
 *
 *  @param i_cppClass C++ class containing the public member (unqualified name)
 *  @param i_cppMember C++ public data member name to export (also used as Python property name)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_PUBLIC_MEMBER_R_DOC(Parrot, color)
 *  ```
 */
#define PY_CLASS_PUBLIC_MEMBER_R(i_cppClass, i_cppMember)\
	PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember), 0)

/** @} */


// --- constructors --------------------------------------------------------------------------------

/** @defgroup ClassConstructors Constructor Export Macros
 *  @ingroup ClassDefinition
 *
 *  Export C++ constructors directly as Python class constructors.
 *  These macros make abstract Python classes concrete by adding constructors
 *  that can create instances from Python code using the actual C++ constructors.
 *
 *  Besides exporting constructors from the C++ class directly as Python class constructors, you can
 *  also export free or static factory functions that return a new instance of that class as a
 *  constructor.
 *
 *  @par Overloading
 *
 *  Just like other functions, multiple constructors can be exported on the same class, to overload
 *  them on the parameter types.
 *
 *  @note Overload resolution uses first-fit, not best-fit like C++. The first exported constructor
 *        that matches the arguments will be called.
 *
 *  @par Naming and Documentation
 *
 *  @note None of the constructor export macros allow you to specify a name or a docstring. The name
 *        is fixed to `__init__`, and constructor documentation has to be added to the class'
 *        docstring.
 *
 *  @par Example
 *
 *  ```cpp
 *  class Parrot;
 *  using TParrotPtr = lass::python::PyObjectPtr<Parrot>::Type;
 *
 *  class Parrot
 *  {
 *      PY_HEADER(lass::python::PyObjectPlus)
 *  public:
 *      Parrot();
 *      Parrot(const std::string& name);
 *      Parrot(const std::string& name, bool alive);
 *      static TParrotPtr loadParrot(int id);
 *  };
 *
 *  TParrotPtr makeParrot(bool alive);
 *
 *  PY_DECLARE_CLASS(Parrot)
 *
 *  PY_CLASS_CONSTRUCTOR_0(Parrot)                           // p = Parrot()
 *  PY_CLASS_CONSTRUCTOR_1(Parrot, const std::string&)       // p = Parrot("bird")
 *  PY_CLASS_CONSTRUCTOR_2(Parrot, const std::string&, bool) // p = Parrot("bird", False)
 *
 *  PY_CLASS_FREE_CONSTRUCTOR_1(Parrot, makeParrot, bool)    // p = Parrot(True)
 *  PY_CLASS_FREE_CONSTRUCTOR_1(Parrot, &Parrot::loadParrot, int)  // p = Parrot(123)
 *  PY_CLASS_FREE_CONSTRUCTOR_1(Parrot, ([](bool alive) { return TParrotPtr(new Parrot("Polly", alive)); }), bool)
 *  ```
 */

/** @addtogroup ClassConstructors
 *  @name Class Constructor Export Macros
 *
 *  Constructors need to be fully qualified to be exported. You always need to provide the full list
 *  of parameter types to disambiguate overloaded constructors. This is why there are no
 *  `_QUALIFIED` versions of these macros, they already are fully qualified.
 *
 *  The list of parameter types can be passed as a single lass::meta::TypeTuple, or as individual
 *  arguments. For the latter, the `_<N>` tells the number of arguments. The `_<N>` form is the most
 *  often used one, and simply packs its types into a `TypeTuple`.
 *
 *  There's also the `_EX` form in case you need a custom dispatcher name, but that should be rarely
 *  used.
 *
 *  | Form                       | Fixed parameters | Adds parameters                     |
 *  |----------------------------|------------------|-------------------------------------|
 *  | `PY_CLASS_CONSTRUCTOR`     | `i_cppClass`     | `t_params` as lass::meta::TypeTuple |
 *  | `PY_CLASS_CONSTRUCTOR_EX`  | `t_cppClass`     | `t_params`, `i_dispatcher`          |
 *  | `PY_CLASS_CONSTRUCTOR_<N>` | `i_cppClass`     | `t_P1`, `t_P2`, ... `t_P<N>`        |
 *
 *  @{
 */

/** @brief Export C++ constructor as Python class constructor with full control.
 *  @ingroup ClassConstructors
 *
 *  This macro provides the most flexible constructor export with manual dispatcher naming.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class to add the constructor to
 *  @param t_params Constructor parameter types as lass::meta::TypeTuple (empty for none)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @par Example
 *  ```cpp
 *  using TParrotArgs = meta::TypeTuple<const std::string&, bool>;
 *  PY_CLASS_CONSTRUCTOR_EX(Parrot, TParrotArgs, parrot_constructor)
 *  ```
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

/** @brief Export C++ constructor as Python class constructor.
 *  @ingroup ClassConstructors
 *
 *  Wraps PY_CLASS_CONSTRUCTOR_EX() with auto-generated dispatcher name.
 *
 *  @param i_cppClass C++ class to add the constructor to (unqualified name)
 *  @param t_params lass::meta::TypeTuple of constructor parameter types
 *
 *  @par Example
 *  ```cpp
 *  using TParrotArgs = meta::TypeTuple<const std::string&, bool>;
 *  PY_CLASS_CONSTRUCTOR(Parrot, TParrotArgs)
 *  ```
 */
#define PY_CLASS_CONSTRUCTOR( i_cppClass, t_params )\
	PY_CLASS_CONSTRUCTOR_EX(i_cppClass, t_params,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_constructor_, i_cppClass)))

/** @brief Export C++ default constructor as Python class constructor.
 *  @ingroup ClassConstructors
 *
 *  Convenience macro for exporting a parameterless C++ constructor.
 *  Equivalent to PY_CLASS_CONSTRUCTOR(i_cppClass, meta::TypeTuple<>).
 *
 *  @param i_cppClass C++ class to add the default constructor to (unqualified name)
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_CONSTRUCTOR_0(Parrot)
 *  ```
 */
#define PY_CLASS_CONSTRUCTOR_0( i_cppClass )\
	PY_CLASS_CONSTRUCTOR( i_cppClass, ::lass::meta::TypeTuple<> )
$[
/** @brief Export C++ constructor as Python class constructor with $x parameters.
 *  @ingroup ClassConstructors
 *
 *  Wraps PY_CLASS_CONSTRUCTOR() for exporting a C++ constructor with exactly $x parameters.
 *  Automatically creates the required lass::meta::TypeTuple from the parameter types.
 *
 *  @param i_cppClass C++ class to add the constructor to (unqualified name)
 *  @param $(t_P$x)$ Parameter types for the constructor
 *
 *  @par Example
 *  ```cpp
 *  PY_CLASS_CONSTRUCTOR_$x(Parrot, $(T$x)$)
 *  ```
 */
#define PY_CLASS_CONSTRUCTOR_$x( i_cppClass, $(t_P$x)$ )\
	typedef ::lass::meta::TypeTuple< $(t_P$x)$ > \
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, i_cppClass));\
	PY_CLASS_CONSTRUCTOR(\
		i_cppClass, LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, i_cppClass)))
]$

/** @} */


/** @addtogroup ClassConstructors
 *  @name Free Function Constructor Export Macros
 *
 *  Export factory functions as Python class constructors. These macros allow C++ functions that
 *  return instances of a class to be used as Python constructors. This is useful when you need
 *  special construction logic or when the actual constructor is not accessible from Python. The
 *  functions must return an instance of the target class, preferably as a PyObjectPtr for Python-
 *  aware classes, or as a ShadoweePtr for shadow classes.
 *
 *  | Form                            | Fixed parameters              | Adds parameters                     |
 *  |---------------------------------|-------------------------------|-------------------------------------|
 *  | `PY_CLASS_FREE_CONSTRUCTOR`     | `i_cppClass`, `f_cppFunction` | `t_params` as lass::meta::TypeTuple |
 *  | `PY_CLASS_FREE_CONSTRUCTOR_EX`  | `t_cppClass`, `f_cppFunction` | `t_params`, `i_dispatcher`          |
 *  | `PY_CLASS_FREE_CONSTRUCTOR_<N>` | `i_cppClass`, `f_cppFunction` | `t_P1`, `t_P2`, ... `t_P<N>`        |
 *
 *  @par `std::function`, Lambda Expressions, and Other Callables as Free Constructors
 *
 *  Besides normal C++ function, @a f_cppFunction may also be a C++ static method, `std::function`,
 *  a lambda expression, or any other callable.
 *
 *  @note Callables with overloaded `operator()` are not supported, and neither are callables with
 *        template `operator()` such as generic lambdas like `[](auto a, auto b) { return a + b; }`.
 *
 *  @{
 */

/** @brief Export free/static C++ factory function as Python constructor with full control.
 *  @ingroup ClassConstructors
 *
 *  The most detailed macro for exporting factory functions as constructors. Allows manual
 *  specification of all parameters including the dispatcher name for maximum control over the
 *  export process.
 *
 *  Here you can use a fully qualified class name, at the cost of having to provide a unique suffix.
 *
 *  @param t_cppClass C++ class to add the constructor to
 *  @param f_cppFunction Free or static factory function, std::function, lambda expression, or
 *                       other callable that creates the class instance (preferably on the heap)
 *  @param t_params Constructor parameter types as lass::meta::TypeTuple (empty for none)
 *  @param i_dispatcher Unique identifier for the generated dispatcher functions
 *
 *  @par Example
 *  ```cpp
 *  Parrot makeParrot(const std::string&, bool);
 *  using TParrotArgs = meta::TypeTuple<const std::string&, bool>;
 *  PY_CLASS_FREE_CONSTRUCTOR_EX(Parrot, makeParrot, TParrotArgs, parrot_free_constructor)
 *  ```
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

/** @brief Export free/static C++ factory function as Python class constructor.
 *  @ingroup ClassConstructors
 *
 *  Wraps PY_CLASS_FREE_CONSTRUCTOR_EX() with auto-generated dispatcher name.
 *
 *  @param i_cppClass C++ class to add the constructor to (unqualified name)
 *  @param f_cppFunction Free or static factory function, std::function, lambda expression, or
 *                       other callable that creates the class instance (preferably on the heap)
 *  @param t_params Constructor parameter types as lass::meta::TypeTuple (empty for none)
 *
 *  @par Example
 *  ```cpp
 *  Parrot makeParrot(const std::string&, bool);
 *  using TParrotArgs = meta::TypeTuple<const std::string&, bool>;
 *  PY_CLASS_FREE_CONSTRUCTOR(Parrot, makeParrot, TParrotArgs)
 *  ```
 */
#define PY_CLASS_FREE_CONSTRUCTOR( i_cppClass, f_cppFunction, t_params )\
	PY_CLASS_FREE_CONSTRUCTOR_EX(i_cppClass, f_cppFunction, t_params,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_constructor_, i_cppClass)))

/** @brief Export free/static C++ factory function as Python constructor with 0 parameters.
 *  @ingroup ClassConstructors
 *
 *  Wraps PY_CLASS_FREE_CONSTRUCTOR() for exporting a factory function that takes no parameters.
 *
 *  @param i_cppClass C++ class to add the constructor to (unqualified name)
 *  @param f_cppFunction Free or static factory function, std::function, lambda expression, or
 *                       other callable that creates the class instance (preferably on the heap)
 *
 *  @par Example
 *  ```cpp
 *  Parrot makeParrot();
 *  PY_CLASS_FREE_CONSTRUCTOR_0(Parrot, makeParrot)
 *  ```
 */
#define PY_CLASS_FREE_CONSTRUCTOR_0( i_cppClass, f_cppFunction)\
	PY_CLASS_FREE_CONSTRUCTOR( i_cppClass, f_cppFunction, ::lass::meta::TypeTuple<> )
$[
/** @brief Export free/static C++ factory function as Python constructor with $x parameters.
 *  @ingroup ClassConstructors
 *
 *  Wraps PY_CLASS_FREE_CONSTRUCTOR() for exporting a factory function that takes exactly $x
 *  parameters. Automatically creates the required lass::meta::TypeTuple from the parameter types.
 *
 *  @param i_cppClass C++ class to add the constructor to (unqualified name)
 *  @param f_cppFunction Free or static factory function, std::function, lambda expression, or
 *                       other callable that creates the class instance (preferably on the heap)
 *  @param $(t_P$x)$ Parameter types for the constructor
 *
 *  @par Example
 *  ```cpp
 *  Parrot makeParrot($(T$x)$);
 *  PY_CLASS_FREE_CONSTRUCTOR_$x(Parrot, makeParrot, $(T$x)$)
 *  ```
 */
#define PY_CLASS_FREE_CONSTRUCTOR_$x( i_cppClass, f_cppFunction, $(t_P$x)$ )\
	typedef ::lass::meta::TypeTuple< $(t_P$x)$ > \
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, i_cppClass));\
	PY_CLASS_FREE_CONSTRUCTOR(\
		i_cppClass, f_cppFunction, LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, i_cppClass)))
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
		[[maybe_unused]] typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		[[maybe_unused]] typedef TShadowTraits::TCppClass TCppClass;\
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
