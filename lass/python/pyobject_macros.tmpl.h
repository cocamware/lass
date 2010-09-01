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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

/** @ingroup Python
 *  Declare and define an object respresenting a python module.
 *
 *  @param i_module 
 *		the identifier of the module to be used in C++
 */
#define PY_DECLARE_MODULE_NAME_DOC( i_module, s_name, s_doc ) \
	::lass::python::ModuleDefinition i_module( s_name, s_doc );

/** @ingroup Python
 *  convenience macro, wraps PY_DECLARE_MODULE_NAME_DOC with @a s_doc = 0
 */
#define PY_DECLARE_MODULE_NAME( i_module, s_name ) \
	PY_DECLARE_MODULE_NAME_DOC( i_module, s_name, 0)

/** @ingroup Python
 *  convenience macro, wraps PY_DECLARE_MODULE_NAME_DOC with @a s_moduleName = # @a i_module
 */
#define PY_DECLARE_MODULE_DOC( i_module, s_doc ) \
	PY_DECLARE_MODULE_NAME_DOC( i_module, LASS_STRINGIFY(i_module), s_doc)

/** @ingroup Python
 *  convenience macro, wraps PY_DECLARE_MODULE_NAME_DOC with @a s_moduleName = # @a i_module and @a s_doc = 0
 */
#define PY_DECLARE_MODULE( i_module ) \
	PY_DECLARE_MODULE_NAME_DOC( i_module, LASS_STRINGIFY(i_module), 0)

/** @def PY_INIT_MODULE
 */
#if PY_MAJOR_VERSION < 3
#	define PY_MODULE_ENTRYPOINT_NAME( i_module, i_name ) \
		PyMODINIT_FUNC LASS_CONCATENATE(init, i_name)() { i_module.inject(); }
#else
#	define PY_MODULE_ENTRYPOINT_NAME( i_module, i_name ) \
		PyMODINIT_FUNC LASS_CONCATENATE(PyInit_, i_name)() { return i_module.inject(); }
#endif

/** @ingroup Python
 */
#define PY_MODULE_ENTRYPOINT( i_module ) PY_MODULE_ENTRYPOINT_NAME( i_module, i_module )

/** @ingroup Python
 *	Inject a python module so Python is aware of it.
 *
 *  @remark This is to be done at @e runtime!  So, it has to be somewhere in your main or any
 *	function called by main.
 *
 *  @param i_module
 *		the identifier of a module declared by PY_DECLARE_MODULE
 */
#define PY_INJECT_MODULE( i_module )\
	i_module.inject();

/** @ingroup Python
 *  @deprecated
 */
#define PY_INJECT_MODULE_EX( i_module, s_moduleName, s_doc ) \
	i_module.setName(s_moduleName); \
	i_module.setDoc(s_doc); \
	i_module.inject();

/** @ingroup Python
 *  @deprecated
 */
#define PY_INJECT_MODULE_NAME( i_module, s_moduleName )\
	i_module.setName(s_moduleName); \
	i_module.inject();

/** @ingroup Python
 *  @deprecated
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

/** @ingroup Python
 *  convenience macro, wraps PY_EXTENSION_MODULE_EX with
 *  @a s_moduleName = # @a i_module
 */
#define PY_EXTENSION_MODULE_DOC( i_module, f_injection, s_doc )\
	PY_EXTENSION_MODULE_EX( i_module, f_injection, s_doc)

/** @ingroup Python
 *  convenience macro, wraps PY_EXTENSION_MODULE_EX with
 *  @a s_moduleName = # @a i_module and s_doc = 0
 */
#define PY_EXTENSION_MODULE( i_module, f_injection )\
	PY_EXTENSION_MODULE_EX( i_module, f_injection, 0)


// --- module variables ----------------------------------------------------------------------------


/** @ingroup
 *	Inject a integer constant in a python module
 *
 *	@param i_module
 *		the identifier of the module to inject the object in
 *	@param s_name
 *		name of constant as shown in the module (zero terminated C string)
 *  @param s_value
 *		value of the constant (zero terminated C string)
 */
#define PY_MODULE_INTEGER_CONSTANT( i_module, s_name, s_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleIntegerConstant_, i_module),\
		i_module.addLong( s_value, s_name); ) 



/** @ingroup 
 *	Inject a string constant in a python module
 *
 *	@param i_module
 *		the identifier of the module to inject the object in
 *	@param s_name
 *		name of constant as shown in the module (zero terminated C string)
 *  @param v_value
 *		value of the constant (long)
 */
#define PY_MODULE_STRING_CONSTANT( i_module, s_name, s_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleIntegerConstant_, i_module),\
		i_module.addString( s_value, s_name); ) 



/** @ingroup Python
 *	Inject a variable in a python module
 *
 *  @remark This is to be done at @e runtime!  So, it has to be somewhere in your main or any
 *	function called by main.
 *
 *  @param o_object
 *		the object/variable to be injected
 *	@param i_module
 *		the identifier of the module to inject the object in
 *	@param s_objectName
 *		name of object as shown in the module (zero terminated C string)
 */
#define PY_INJECT_OBJECT_IN_MODULE_EX( o_object, i_module, s_objectName )\
	{\
		PyModule_AddObject(i_module.module(), s_objectName, lass::python::pyBuildSimpleObject(o_object) );\
	}

/** @ingroup Python
 *  convenience macro, wraps PY_INJECT_OBJECT_IN_MODULE_EX with
 *  @a s_objectName = # @a o_object
 */
#define PY_INJECT_OBJECT_IN_MODULE( o_object, i_module )\
	PY_INJECT_OBJECT_IN_MODULE_EX(o_object, i_module, LASS_STRINGIFY(o_object))



/** @ingroup
 *  @deprecated
 *	Inject a integer constant in a python module
 *
 *  @remark This is to be done at @e runtime!  So, it has to be somewhere in your main or any
 *	function called by main.
 *
 *	@param i_module
 *		the identifier of the module to inject the object in
 *	@param s_name
 *		name of constant as shown in the module (zero terminated C string)
 *  @param s_value
 *		value of the constant (zero terminated C string)
 */
#define PY_MODULE_ADD_INTEGER_CONSTANT( i_module, s_name, s_value )\
	{\
		PyModule_AddIntConstant(i_module.module(), s_name, s_value);\
	}



/** @ingroup 
 *  @deprecated
 *	Inject a string constant in a python module
 *
 *  @remark This is to be done at @e runtime!  So, it has to be somewhere in your main or any
 *	function called by main.
 *
 *	@param i_module
 *		the identifier of the module to inject the object in
 *	@param s_name
 *		name of constant as shown in the module (zero terminated C string)
 *  @param v_value
 *		value of the constant (long)
 */
#define PY_MODULE_ADD_STRING_CONSTANT( i_module, s_name, v_value )\
	{\
		PyModule_AddStringConstant(i_module.module(), s_name, const_cast<char*>(v_value));\
	}

// --- free module functions -----------------------------------------------------------------------

/* @ingroup Python
 * @deprecated
 * Use this macro for backward compatibility when wrapper functions don't 
 * need to be automatically generated or you want specific Python behaviour.
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



/** @ingroup Python
 *  Exports a C++ free function to Python
 *
 *  @param i_module
 *		the module object
 *  @param f_cppFunction
 *      the name of the function in C++
 *  @param s_functionName
 *      the name the method will have in Python
 *  @param s_doc
 *      documentation of function as shown in Python (zero terminated C string)
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *      per exported C++ class/method pair.
 *
 *  Invoke this macro to export a function to python.  You can use this to generate overloaded
 *  Python functions by exporting multiple functions with the same @a s_methodName name.
 *
 *  @note
 *      unlike in C++ overload issues will be not be solved by best fit, but by first fit.
 *      If such an overloaded Python functions is called, the different overloads are called in
 *      the same order of export.  The first one that fits the arguments will be called.
 *
 *  @note
 *      the documentation of the Python method will be the @a s_doc of the first exported
 *      overload.
 *
 *  @code
 *  void barA(int a);
 *  void barB(const std::string& b);
 *
 *  PY_MODULE_FUNCTION_EX(foo_module, barA, "bar", 0, foo_bar_a)
 *  PY_MODULE_FUNCTION_EX(foo_module, barB, "bar", 0, foo_bar_b)
 *  @endcode
 */
#define PY_MODULE_FUNCTION_EX( i_module, f_cppFunction, s_functionName, s_doc, i_dispatcher )\
	static PyCFunction LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) = 0;\
	inline PyObject* i_dispatcher( PyObject* iIgnore, PyObject* iArgs )\
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

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_EX with 
 *	@a i_dispatcher = lassPyImpl_function_ ## @a i_module ## __LINE__
 */
#define PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, s_name, s_doc )\
	PY_MODULE_FUNCTION_EX( i_module, f_cppFunction, s_name, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_NAME_DOC with @a s_doc = 0.
 */
#define PY_MODULE_FUNCTION_NAME( i_module, f_cppFunction, s_name)\
	PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, s_name, 0)

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_NAME_DOC with @a s_name = "@a i_innerCppClass".
 */
#define PY_MODULE_FUNCTION_DOC( i_module, f_cppFunction, s_doc )\
	PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, LASS_STRINGIFY(f_cppFunction), s_doc)

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_NAME_DOC with @a s_name = "@a i_innerCppClass" and s_doc = 0.
 */
#define PY_MODULE_FUNCTION( i_module, f_cppFunction)\
	PY_MODULE_FUNCTION_NAME_DOC( i_module, f_cppFunction, LASS_STRINGIFY(f_cppFunction), 0)


// --- casting free functions -----------------------------------------------------------

/** @ingroup Python
 *  Exports a C++ free functions to Python with on the fly casting on return type and parameters, including omission of default parameters
 *
 *  @param i_module
 *		the module object
 *  @param f_cppFunction
 *      the name of the function in C++
 *  @param t_return
 *      the return type of @a f_cppFunction
 *  @param t_params
 *      a lass::meta::TypeList of the parameter types of @a f_cppFunction
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


#define PY_MODULE_FUNCTION_CAST_EX_0(i_module, f_cppFunction, t_return, s_functionName, s_doc, i_dispatcher) \
	::lass::python::OwnerCaster<t_return>::TCaster::TTarget LASS_CONCATENATE(i_dispatcher, _caster) ()\
	{\
 		return f_cppFunction() ; \
	}\
	PY_MODULE_FUNCTION_EX( i_module, LASS_CONCATENATE(i_dispatcher, _caster), s_functionName, s_doc, i_dispatcher );


 $[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX for $x arguments
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

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_CAST_EX_0 with
 *  @a i_dispatcher = lassPyImpl_function_ ## @a i_module ## __LINE__.
 */
#define PY_MODULE_FUNCTION_CAST_NAME_DOC_0( i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_CAST_EX_0(\
		i_module, f_cppFunction, t_return, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))
$[
/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_CAST_EX_$x with
 *  @a i_dispatcher = lassPyImpl_function_ ## @a i_module ## __LINE__.
 */
#define PY_MODULE_FUNCTION_CAST_NAME_DOC_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_CAST_EX_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))
]$


/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_CAST_NAME_DOC_0 with @a s_doc = 0.
 */
#define PY_MODULE_FUNCTION_CAST_NAME_0( i_module, f_cppFunction, t_return, s_functionName )\
	PY_MODULE_FUNCTION_CAST_NAME_DOC_0(\
		i_module, f_cppFunction, t_return, s_functionName, 0 )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_CAST_NAME_DOC_$x with @a s_doc = 0.
 */
#define PY_MODULE_FUNCTION_CAST_NAME_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName )\
	PY_MODULE_FUNCTION_CAST_NAME_DOC_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, 0 )
]$


// --- explicit qualified free functions -----------------------------------------------------------

/** @ingroup Python
 *  Exports a C++ free functions to Python with fully qualified return type and parameters
 *
 *  @param i_module
 *		the module object
 *  @param f_cppFunction
 *      the name of the function in C++
 *  @param t_return
 *      the return type of @a f_cppFunction
 *  @param t_params
 *      a lass::meta::TypeList of the parameter types of @a f_cppFunction
 *  @param s_functionName
 *      the name the method will have in Python
 *  @param s_doc
 *      documentation of function as shown in Python (zero terminated C string)
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *
 *  You can use this macro instead of PY_MODULE_FUNCTION_EX if there's an ambiguity on 
 *	@a f_cppFunction. This macro will help you to solve this ambiguity by explicitely 
 *	specifying return type and parameter types.
 *
 *  @code
 *	void bar(int a);
 *  void bar(const std::string& b);
 *
 *  PY_MODULE_FUNCTION_QUALIFIED_EX(foo_module, bar, void, meta::type_list::Make<int>::Type, "bar", 0, foo_bar_a)
 *  PY_MODULE_FUNCTION_QUALIFIED_EX(foo_module, bar, void, meta::type_list::Make<const std::string&>::Type, "bar", 0, foo_bar_b)
 *  @endcode
 */

#define PY_MODULE_FUNCTION_QUALIFIED_EX(i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc, i_dispatcher)\
	static PyCFunction LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) = 0;\
	inline PyObject* i_dispatcher( PyObject* iIgnore, PyObject* iArgs )\
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
		::TImpl::callFunction(iArgs, &f_cppFunction);\
	}\
	extern ::std::vector< PyMethodDef > LASS_CONCATENATE( lassPythonModuleMethods_, i_module );\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModuleFunction_, i_module, i_dispatcher ),\
		i_module.addFunctionDispatcher( \
			i_dispatcher, s_functionName, s_doc, \
			LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) \
		);\
	)

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_EX for 0 arguments
 */
#define PY_MODULE_FUNCTION_QUALIFIED_EX_0( i_module, f_cppFunction, t_return, s_functionName, s_doc, i_dispatcher )\
	PY_MODULE_FUNCTION_QUALIFIED_EX(\
		i_module, f_cppFunction, t_return, ::lass::meta::NullType, s_functionName, s_doc, i_dispatcher )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX for $x arguments
 */
#define PY_MODULE_FUNCTION_QUALIFIED_EX_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc, i_dispatcher )\
	typedef ::lass::meta::type_list::Make< $(t_P$x)$ >::Type \
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams));\
	PY_MODULE_FUNCTION_QUALIFIED_EX(\
		i_module, f_cppFunction, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams)),\
		s_functionName, s_doc, i_dispatcher )
]$

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_EX with
 *  @a i_dispatcher = lassPyImpl_function_ ## @a i_module ## __LINE__.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC( i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_EX(\
		i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))


/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_EX_0 with
 *  @a i_dispatcher = lassPyImpl_function_ ## @a i_module ## __LINE__.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0( i_module, f_cppFunction, t_return, t_params, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_EX_0(\
		i_module, f_cppFunction, t_return, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))
$[
/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_EX_$x with
 *  @a i_dispatcher = lassPyImpl_function_ ## @a i_module ## __LINE__.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_EX_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_function_, i_module)))
]$

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC with @a s_doc = 0.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME( i_module, f_cppFunction, t_return, t_params, s_functionName )\
		PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC(\
			i_module, f_cppFunction, t_return, t_params, s_functionName, 0 )

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0 with @a s_doc = 0.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_0( i_module, f_cppFunction, t_return, s_functionName )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0(\
		i_module, f_cppFunction, t_return, s_functionName, 0 )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x with @a s_doc = 0.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_NAME_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x(\
		i_module, f_cppFunction, t_return, $(t_P$x)$, s_functionName, 0 )
]$

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC
 *  with @a s_functionName = # @a f_cppFunction.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_DOC( i_module, f_cppFunction, t_return, t_params, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC( \
		i_module, f_cppFunction, t_return, t_params, LASS_STRINGIFY(f_cppFunction), s_doc)

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0
 *  with @a s_functionName = # @a f_cppFunction.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_DOC_0( i_module, f_cppFunction, t_return, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0( \
		i_module, f_cppFunction, t_return, LASS_STRINGIFY(f_cppFunction), s_doc)
$[
/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x
 *  with @a s_functionName = # @a f_cppFunction.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_DOC_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, s_doc )\
	PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x( \
		i_module, f_cppFunction, t_return, $(t_P$x)$, LASS_STRINGIFY(f_cppFunction), s_doc)
]$

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC
 *  with @a s_functionName = # @a f_cppFunction and @a s_doc = 0.
 */
#define PY_MODULE_FUNCTION_QUALIFIED( i_module, f_cppFunction, t_return, t_params )\
	PY_MODULE_FUNCTION_QUALIFIED_DOC( i_module, f_cppFunction, t_return, t_params, 0 )

/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_0
 *  with @a s_functionName = # @a f_cppFunction and @a s_doc = 0.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_0( i_module, f_cppFunction, t_return )\
	PY_MODULE_FUNCTION_QUALIFIED_DOC_0( i_module, f_cppFunction, t_return, 0 )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_MODULE_FUNCTION_QUALIFIED_NAME_DOC_$x
 *  with @a s_functionName = # @a f_cppFunction and @a s_doc = 0.
 */
#define PY_MODULE_FUNCTION_QUALIFIED_$x( i_module, f_cppFunction, t_return, $(t_P$x)$ )\
	PY_MODULE_FUNCTION_QUALIFIED_DOC_$x( i_module, f_cppFunction, t_return, $(t_P$x)$, 0 )
]$




// --- classes -------------------------------------------------------------------------------------

/** @ingroup Python
 *  Bring alive one derived pupil of PyObjectPlus
 *
 *  @param t_cppClass
 *		the type of the class to be brought alive
 *	@param s_className
 *		the name of class as shown in Python (zero terminated C string)
 *  @param s_doc
 *		doc string of class (zero terminated C string)
 *
 *  @remark put each PY_DECLARE_CLASS_NAME_DOC in only one translation unit.  So not in headers!
 */
#define PY_DECLARE_CLASS_NAME_DOC( t_cppClass, s_className, s_doc ) \
	::lass::python::impl::ClassDefinition t_cppClass ::_lassPyClassDef( \
		s_className, s_doc, sizeof(t_cppClass), \
		::lass::python::impl::richCompareDispatcher< t_cppClass >,\
		& t_cppClass ::_lassPyParentType::_lassPyClassDef, \
		& t_cppClass ::_lassPyClassRegisterHook);

/** @ingroup Python
 *  convenience macro, wraps PY_DECLARE_CLASS_NAME_DOC for with
 *  @a s_doc = 0
 */
#define PY_DECLARE_CLASS_NAME( t_cppClass, s_className )\
	PY_DECLARE_CLASS_NAME_DOC( t_cppClass, s_className, 0 )

/** @ingroup Python
 *  convenience macro, wraps PY_DECLARE_CLASS_NAME_DOC for with
 *  @a s_className = # @a i_cppClass
 */
#define PY_DECLARE_CLASS_DOC( i_cppClass, s_doc ) \
	PY_DECLARE_CLASS_NAME_DOC( i_cppClass, LASS_STRINGIFY(i_cppClass), s_doc )

/** @ingroup Python
 *  convenience macro, wraps PY_DECLARE_CLASS_NAME_DOC for with
 *  @a s_className = # @a i_cppClass, @a s_doc = 0
 */
#define PY_DECLARE_CLASS( i_cppClass ) \
	PY_DECLARE_CLASS_NAME_DOC( i_cppClass, LASS_STRINGIFY(i_cppClass), 0 )

/** @ingroup Python
 *  @deprecated use PY_DECLARE_CLASS_NAME_DOC instead
 */
#define PY_DECLARE_CLASS_EX( t_cppClass, s_className, i_uniqueClassIdentifier )\
	PY_DECLARE_CLASS_NAME_DOC( t_cppClass, s_className, 0 )




/** @ingroup Python
 *  Inject a class in a module, old style
 *
 *  @deprecated Use PY_MODULE_CLASS instead, and set class doc with PY_DECLARE_CLASS_DOC
 *
 *  @remark This is to be done at @e runtime!  So, it has to be somewhere in your main or any
 *	function called by main.
 *
 *  @param t_cppClass
 *		the type of the class to be injected
 *	@param i_module
 *		the identifier of the module object to inject the class in.
 *	@param s_doc
 *		documentation of class as shown in Python (zero terminated C string)
 */
#define PY_INJECT_CLASS_IN_MODULE( t_cppClass, i_module, s_doc ) \
	if (s_doc) t_cppClass::_lassPyClassDef.setDoc(s_doc);\
	i_module.injectClass(t_cppClass::_lassPyClassDef); 

/** @ingroup Python
 *  add a class to a module, new style
 *
 *  @param t_cppClass
 *		the type of the class to be injected
 *	@param i_module
 *		the identifier of the module object to inject the class in.
 */
#define PY_MODULE_CLASS( i_module, t_cppClass ) \
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleClass_, i_module ),\
		i_module.addClass( t_cppClass ::_lassPyClassDef); \
	)




/** @ingroup Python
 *  export some value as a static constant of a class
 *
 *  @param i_cppClass
 *		the class to add the constant to.
 *	@param s_name
 *		name of static constant as shown in python (zero terminated C string)
 *	@param v_value
 *		the value of the constant.  it should be convertible to a PyObject by using the
 *		python::PyExportTraits::build functions
 */
#define PY_CLASS_STATIC_CONST( i_cppClass, s_name, v_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyClassStaticConst, i_cppClass ),\
		i_cppClass ::_lassPyClassDef.addStaticConst(s_name, v_value);\
	)



// --- inner class ---------------------------------------------------------------------------------

/** @ingroup Python
 *  Adds an inner class.
 *
 *  @param t_outerCppClass
 *      the C++ class you want to add an inner class to.
 *  @param t_innerCppClass
 *		the C++ class you want to add as inner class.
 *  @param s_name
 *      the name the inner class will have in python (zero terminated C string)
 *  @param s_doc
 *      documentation of inner class as shown in Python (zero terminated C string)
 *  @param i_uniqueId
 *		some identifier that is unique to this innerclass/outerclass combination
 *
 *  Invoke this macro to add an inner class to a class in Python.  Both classes (inner and outer)
 *	should be derived from PyObjectPlus.
 *
 *  @code
 *  // inner_and_outer.h
 *  class Inner
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *		...
 *  };
 *
 *  class Outer
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *		...
 *  };
 *
 *  // inner_and_outer.cpp
 *  PY_DECLARE_CLASS(Inner)
 *  PY_DECLARE_CLASS(Outer)
 *  PY_CLASS_INNCER_CLASS_EX( Outer, Inner, "Inner", "Inner class" )
 *  @endcode
 */
#define PY_CLASS_INNER_CLASS_EX( t_outerCppClass, t_innerCppClass, s_name, s_doc, i_uniqueClassId )\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE(lassPythonImplExecuteBeforeMain_, i_uniqueClassId),\
		if (s_doc) { t_innerCppClass::_lassPyClassDef.setDoc(s_doc); } \
		t_outerCppClass::_lassPyClassDef.addInnerClass(t_innerCppClass::_lassPyClassDef);\
	)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_INNER_CLASS_EX with @a i_uniqueId = @a i_outerCppClass ## @a i_innerCppClass.
 *  @deprecated You should be setting the doc when declaring the innerclass ...
 */
#define PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, s_name, s_doc )\
	PY_CLASS_INNER_CLASS_EX( i_outerCppClass, i_innerCppClass, s_name, s_doc,\
		LASS_CONCATENATE(i_outerCppClass, i_innerCppClass) )

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_INNER_CLASS_NAME_DOC with @a s_doc = 0.
 */
#define PY_CLASS_INNER_CLASS_NAME( i_outerCppClass, i_innerCppClass, s_name)\
	PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, s_name, 0)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_INNER_CLASS_NAME_DOC with @a s_name = "@a i_innerCppClass".
 *  @deprecated You should be setting the doc when declaring the innerclass ...
 */
#define PY_CLASS_INNER_CLASS_DOC( i_outerCppClass, i_innerCppClass, s_doc )\
	PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, LASS_STRINGIFY(i_innerCppClass), s_doc)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_INNER_CLASS_NAME_DOC with @a s_name = "@a i_innerCppClass" and s_doc = 0.
 */
#define PY_CLASS_INNER_CLASS( i_outerCppClass, i_innerCppClass)\
	PY_CLASS_INNER_CLASS_NAME_DOC( i_outerCppClass, i_innerCppClass, LASS_STRINGIFY(i_innerCppClass), 0)


// --- methods -------------------------------------------------------------------------------------
/** @ingroup Python
*   @deprecated 
*   This macro is provided when there is need for a function in Python where there is no
*   direct equivalent in C++.  An example is when you would need a true polymorphic python list returned
*   from a C++ function.  Or when you need sth very Python specific returning from your function where there
*   is no PyExportTraits::build defined for.
*/
#define PY_CLASS_PY_METHOD_EX( i_cppClass, i_cppMethod, s_methodName, s_doc  )\
	static ::lass::python::impl::OverloadLink LASS_CONCATENATE_3( staticDispatchOverloadChain, i_cppClass, i_cppMethod);\
	inline PyObject* LASS_CONCATENATE_3( staticDispatch, i_cppClass, i_cppMethod) ( PyObject* iObject, PyObject* iArgs )\
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


// --- methods -------------------------------------------------------------------------------------

/** @ingroup Python
 *  Exports a C++ method to Python
 *
 *  @param t_cppClass
 *      the C++ class you're exporting a method of
 *  @param i_cppMethod
 *      the name of the method in C++
 *  @param s_methodName
 *      the name the method will have in Python
 *  @param s_doc
 *      documentation of method as shown in Python (zero terminated C string)
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *      per exported C++ class/method pair.
 *
 *  Invoke this macro to export a method to python.  You can use this to generate overloaded
 *  Python methods by exporting multiple methods with the same @a s_methodName name.
 *
 *  @note
 *      unlike in C++ overload issues will be not be solved by best fit, but by first fit.
 *      If such an overloaded Python method is called, the different overloads are called in
 *      the same order of export.  The first one that fits the arguments will be called.
 *
 *  @note
 *      the documentation of the Python method will be the @a s_doc of the first exported
 *      overload.
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      void barA(int a);
 *      void barB(const std::string& b) const;
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_METHOD_EX(Foo, barA, "bar", 0, foo_bar_a)
 *  PY_CLASS_METHOD_EX(Foo, barB, "bar", 0, foo_bar_b)
 *  @endcode
 */
#define PY_CLASS_METHOD_EX(t_cppClass, i_cppMethod, s_methodName, s_doc, i_dispatcher)\
	PY_CLASS_METHOD_IMPL(t_cppClass, &TCppClass::i_cppMethod, s_methodName, s_doc, i_dispatcher,\
		::lass::python::impl::CallMethod<TShadowTraits>::call)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_EX with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a i_cppClass ## __LINE__.
 */
#define PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, s_doc )\
		PY_CLASS_METHOD_EX(\
			i_cppClass, i_cppMethod, s_methodName, s_doc,\
			LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_NAME_DOC with @a s_doc = 0.
 */
#define PY_CLASS_METHOD_NAME( i_cppClass, i_cppMethod, s_methodName )\
		PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, 0 )

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_NAME_DOC with @a s_methodName = # @a i_cppMethod.
 */
#define PY_CLASS_METHOD_DOC( i_cppClass, i_cppMethod, s_doc )\
		PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, LASS_STRINGIFY(i_cppMethod), s_doc)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_NAME_DOC  with @a s_methodName = # @a i_cppMethod
 *  and @a s_doc = 0.
 */
#define PY_CLASS_METHOD( i_cppClass, i_cppMethod )\
		PY_CLASS_METHOD_DOC( i_cppClass, i_cppMethod, 0 )



// --- explicit qualified methods ------------------------------------------------------------------

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_EX
 *  @brief Exports a C++ method to Python will fully qualified return type and parameters
 *
 *  @param t_cppClass
 *      the C++ class you're exporting a method of
 *  @param i_cppMethod
 *      the name of the method in C++
 *  @param t_return
 *      the return type of @a i_cppMethod
 *  @param t_params
 *      a lass::meta::TypeList of the parameter types of @a i_cppMethod
 *  @param s_methodName
 *      the name the method will have in Python
 *  @param s_doc
 *      documentation of method as shown in Python (zero terminated C string)
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *      per exported C++ class/method pair.
 *
 *  You can use this macro instead of PY_CLASS_METHOD_EX if there's an ambiguity on @a i_cppMethod.
 *  This macro will help you to solve this ambiguity by explicitely specifying return type
 *  and parameter types.
 *
 *  Just like PY_CLASS_METHOD_EX, PY_CLASS_METHOD_QUALIFIED_EX allows for overloading.  These
 *  overloads may be mixed with PY_CLASS_METHOD_EX methods.
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      void bar(int a);
 *      void bar(const std::string& b) const;
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_METHOD_QUALIFIED_EX(Foo, bar, void, meta::type_list::Make<int>::Type, "bar", 0, foo_bar_a)
 *  PY_CLASS_METHOD_QUALIFIED_EX(Foo, bar, void, meta::type_list::Make<const std::string&>::Type, "bar", 0, foo_bar_b)
 *  @endcode
 */
/*
#define PY_CLASS_METHOD_QUALIFIED_EX(t_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc, i_dispatcher)\
	PY_CLASS_METHOD_IMPL(t_cppClass, &TCppClass::i_cppMethod, s_methodName, s_doc, i_dispatcher,\
		(&::lass::python::impl::ExplicitResolver<TCppClass,t_return,t_params>::TImpl::callMethod))
/*/
#define PY_CLASS_METHOD_QUALIFIED_EX(t_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc, i_dispatcher)\
	static ::lass::python::impl::OverloadLink LASS_CONCATENATE(i_dispatcher, _overloadChain);\
	PyObject* i_dispatcher(PyObject* iObject, PyObject* iArgs)\
	{\
		PyObject* result = 0;\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain)(iObject, iArgs, result))\
		{\
			return result;\
		}\
		LASS_ASSERT(result == 0);\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		return ::lass::python::impl::ExplicitResolver<TShadowTraits,t_return,t_params>::TImpl::callMethod(\
			iArgs, iObject, &TCppClass::i_cppMethod); \
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX(LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addMethod(\
			s_methodName, s_doc, \
			::lass::python::impl::FunctionTypeDispatcher< SPECIAL_SLOT_TYPE(s_methodName) , i_dispatcher>::fun,\
			LASS_CONCATENATE(i_dispatcher, _overloadChain));\
	)
/**/

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_EX_0( t_cppClass, i_cppMethod, t_return, s_methodName, s_doc, i_dispatcher )\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		t_cppClass, i_cppMethod, t_return, ::lass::meta::NullType, s_methodName, s_doc, i_dispatcher )
$[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_EX_$x( t_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc, i_dispatcher )\
	typedef ::lass::meta::type_list::Make< $(t_P$x)$ >::Type \
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams));\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		t_cppClass, i_cppMethod, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams)), s_methodName, s_doc,\
		i_dispatcher )
]$

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a t_cppClass ## __LINE__.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		i_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX_0 with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a t_cppClass ## __LINE__.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0( i_cppClass, i_cppMethod, t_return, s_methodName, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_EX_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
$[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX_$x with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a t_cppClass ## __LINE__.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_EX_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
]$

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC with @a s_doc = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME( i_cppClass, i_cppMethod, t_return, t_params, s_methodName )\
		PY_CLASS_METHOD_QUALIFIED_NAME_DOC(\
			i_cppClass, i_cppMethod, t_return, t_params, s_methodName, 0 )

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0 with @a s_doc = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_0( i_cppClass, i_cppMethod, t_return, s_methodName )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, 0 )
$[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x with @a s_doc = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, 0 )
]$

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod.
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC(\
		i_cppClass, i_cppMethod, t_return, t_params, LASS_STRINGIFY(i_cppMethod), s_doc )

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod.
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppMethod, t_return, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, LASS_STRINGIFY(i_cppMethod), s_doc )
$[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod.
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, LASS_STRINGIFY(i_cppMethod), s_doc )
]$

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod and @a s_doc = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED( i_cppClass, i_cppMethod, t_return, t_params )\
	PY_CLASS_METHOD_QUALIFIED_DOC( i_cppClass, i_cppMethod, t_return, t_params, 0 )

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod and @a s_doc = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED_0( i_cppClass, i_cppMethod, t_return )\
	PY_CLASS_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppMethod, t_return, 0 )
$[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod and @a s_doc = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$ )\
	PY_CLASS_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, 0 )
]$



// --- "free" methods ------------------------------------------------------------------------------

/** @ingroup Python
 *  @brief Exports a C/C++ free function as Python method
 *  @sa PY_CLASS_METHOD_EX
 *
 *  @param t_cppClass
 *      the C++ class you're exporting the method for
 *  @param i_cppFreeMethod
 *      the name of the C++ function you're exporting as free method
 *  @param s_methodName
 *      the name the method will have in Python
 *  @param s_doc
 *      documentation of method as shown in Python (zero terminated C string)
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *      per exported C++ class/method pair.
 *
 *  This macro allows you to export a C/C++ free function as a Python method.  This allows you to
 *  export additional functions that are written outside the class definition as methods in the 
 *  Python class.  This is extremely usefull when using shadow classes to export a C++ class,
 *  because in such case, it's often undesirable or impossible to write the function as a method.
 *
 *  The free function has to accept a pointer or reference to the object as first argument.
 *  This may be a const or non-const pointer/reference.
 *
 *  Just like PY_CLASS_METHOD_EX, PY_CLASS_FREE_METHOD_EX allows for overloading.  These
 *  overloads may be mixed with PY_CLASS_METHOD_EX methods.
 *
 *  @code
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
 *  PY_CLASS_FREE_METHOD_EX(Foo, barA, "bar", 0, foo_bar_a)
 *  PY_CLASS_FREE_METHOD_EX(Foo, barB, "bar", 0, foo_bar_b)
 *  @endcode
 */
#define PY_CLASS_FREE_METHOD_EX(t_cppClass, i_cppFreeMethod, s_methodName, s_doc, i_dispatcher)\
	PY_CLASS_METHOD_IMPL(t_cppClass, i_cppFreeMethod, s_methodName, s_doc, i_dispatcher,\
		::lass::python::impl::CallMethod<TShadowTraits>::callFree)

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_EX 
 *  with @a i_dispatcher = lassPyImpl_method_ ## @a i_cppClass ## __LINE__.
 */
#define PY_CLASS_FREE_METHOD_NAME_DOC( i_cppClass, i_cppFreeMethod, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_EX(\
		i_cppClass, i_cppFreeMethod, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_NAME_DOC with @a s_doc = 0.
 */
#define PY_CLASS_FREE_METHOD_NAME( i_cppClass, i_cppFreeMethod, s_methodName )\
	PY_CLASS_FREE_METHOD_NAME_DOC( i_cppClass, i_cppFreeMethod, s_methodName, 0 )

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_NAME_DOC with @a s_methodName = "@a i_cppFreeMethod".
 */
#define PY_CLASS_FREE_METHOD_DOC( i_cppClass, i_cppFreeMethod, s_doc )\
	PY_CLASS_FREE_METHOD_NAME_DOC( i_cppClass, i_cppFreeMethod, LASS_STRINGIFY(i_cppFreeMethod), s_doc)

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_NAME_DOC  with @a s_methodName = "@a i_cppFreeMethod"
 *  and @a s_doc = 0.
 */
#define PY_CLASS_FREE_METHOD( i_cppClass, i_cppFreeMethod )\
	PY_CLASS_FREE_METHOD_DOC( i_cppClass, i_cppFreeMethod, 0 )



// --- explicit qualified "free" methods -----------------------------------------------------------

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_EX
 *  @brief Exports a C++ "free" method to Python will fully qualified return type and parameters
 *
 *  @param t_cppClass
 *      the C++ class you're exporting a method of
 *  @param i_cppFreeMethod
 *      the name of the "free" method in C++
 *  @param t_return
 *      the return type of @a i_cppFreeMethod
 *  @param t_params
 *      a lass::meta::TypeList of the parameter types of @a i_cppFreeMethod
 *  @param s_methodName
 *      the name the method will have in Python
 *  @param s_doc
 *      documentation of method as shown in Python (zero terminated C string)
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *      per exported C++ class/method pair.
 *
 *  You can use this macro instead of PY_CLASS_FREE_METHOD_EX if there's an ambiguity on @a i_cppFreeMethod.
 *  This macro will help you to solve this ambiguity by explicitely specifying return type
 *  and parameter types.
 *
 *  Just like PY_CLASS_FREE_METHOD_EX, PY_CLASS_FREE_METHOD_QUALIFIED_EX allows for overloading.  These
 *  overloads may be mixed with PY_CLASS_FREE_METHOD_EX methods.
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *		// ...
 *  };
 *
 *  void bar(Foo& foo, int a);
 *  void bar(const Foo&, const std::string& b);

 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_FREE_METHOD_QUALIFIED_EX(Foo, bar, void, meta::type_list::Make<int>::Type, "bar", 0, foo_bar_a)
 *  PY_CLASS_FREE_METHOD_QUALIFIED_EX(Foo, bar, void, meta::type_list::Make<const std::string&>::Type), "bar", 0, foo_bar_b)
 *  @endcode
 */
/*
#define PY_CLASS_FREE_METHOD_QUALIFIED_EX(t_cppClass, i_cppFreeMethod, t_return, t_params, s_methodName, s_doc, i_dispatcher)\
	PY_CLASS_FREE_METHOD_IMPL(t_cppClass, &TCppClass::i_cppFreeMethod, s_methodName, s_doc, i_dispatcher,\
		(&::lass::python::impl::ExplicitResolver<TCppClass,t_return,t_params>::TImpl::callMethod))
/*/
#define PY_CLASS_FREE_METHOD_QUALIFIED_EX(t_cppClass, i_cppFreeMethod, t_return, t_params, s_methodName, s_doc, i_dispatcher)\
	static ::lass::python::impl::OverloadLink LASS_CONCATENATE(i_dispatcher, _overloadChain);\
	PyObject* i_dispatcher(PyObject* iObject, PyObject* iArgs)\
	{\
		PyObject* result = 0;\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain)(iObject, iArgs, result))\
		{\
			return result;\
		}\
		LASS_ASSERT(result == 0);\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		return ::lass::python::impl::ExplicitResolver<TShadowTraits,t_return,t_params>::TImpl::callFreeMethod(\
			iArgs, iObject, i_cppFreeMethod); \
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX(LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addMethod(\
			s_methodName, s_doc, \
			::lass::python::impl::FunctionTypeDispatcher< SPECIAL_SLOT_TYPE(s_methodName) , i_dispatcher>::fun,\
			LASS_CONCATENATE(i_dispatcher, _overloadChain));\
	)
/**/

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX for 0 arguments
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_EX_0( t_cppClass, i_cppFreeMethod, t_return, s_methodName, s_doc, i_dispatcher )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX(\
		t_cppClass, i_cppFreeMethod, t_return, ::lass::meta::NullType, s_methodName, s_doc, i_dispatcher )
$[
/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX for $x arguments
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_EX_$x( t_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, s_doc, i_dispatcher )\
	typedef ::lass::meta::type_list::Make< $(t_P$x)$ >::Type\
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams));\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX(\
		t_cppClass, i_cppFreeMethod, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(i_dispatcher, _TParams)), s_methodName, s_doc,\
		i_dispatcher )
]$

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a t_cppClass ## __LINE__.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC( i_cppClass, i_cppFreeMethod, t_return, t_params, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX(\
		i_cppClass, i_cppFreeMethod, t_return, t_params, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX_0 with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a t_cppClass ## __LINE__.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0( i_cppClass, i_cppFreeMethod, t_return, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX_0(\
		i_cppClass, i_cppFreeMethod, t_return, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
$[
/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_EX_$x with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a t_cppClass ## __LINE__.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_EX_$x(\
		i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
]$

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC with @a s_doc = 0.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME( i_cppClass, i_cppFreeMethod, t_return, t_params, s_methodName )\
		PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC(\
			i_cppClass, i_cppFreeMethod, t_return, t_params, s_methodName, 0 )

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0 with @a s_doc = 0.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_0( i_cppClass, i_cppFreeMethod, t_return, s_methodName )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, i_cppFreeMethod, t_return, s_methodName, 0 )
$[
/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x with @a s_doc = 0.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_NAME_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, s_methodName )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, s_methodName, 0 )
]$

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppFreeMethod.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_DOC( i_cppClass, i_cppFreeMethod, t_return, t_params, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC(\
		i_cppClass, i_cppFreeMethod, t_return, t_params, LASS_STRINGIFY(i_cppFreeMethod), s_doc )

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppFreeMethod.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppFreeMethod, t_return, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_0(\
		i_cppClass, i_cppFreeMethod, t_return, LASS_STRINGIFY(i_cppFreeMethod), s_doc )
$[
/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppFreeMethod.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, s_doc )\
	PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, LASS_STRINGIFY(i_cppFreeMethod), s_doc )
]$

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppFreeMethod and @a s_doc = 0.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED( i_cppClass, i_cppFreeMethod, t_return, t_params )\
	PY_CLASS_FREE_METHOD_QUALIFIED_DOC( i_cppClass, i_cppFreeMethod, t_return, t_params, 0 )

/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppFreeMethod and @a s_doc = 0.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_0( i_cppClass, i_cppFreeMethod, t_return )\
	PY_CLASS_FREE_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppFreeMethod, t_return, 0 )
$[
/** @ingroup Python
 *  @sa PY_CLASS_FREE_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_FREE_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = # @a i_cppFreeMethod and @a s_doc = 0.
 */
#define PY_CLASS_FREE_METHOD_QUALIFIED_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$ )\
	PY_CLASS_FREE_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppFreeMethod, t_return, $(t_P$x)$, 0 )
]$


// --- "casting" methods ------------------------------------------------------------------------------

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  @brief Exports a C++ method to Python with fully qualified return type and parameters and with a casting policy
 *		   on how to pass arguments back and forth.
 *
 *  @param t_cppClass
 *      the C++ class you're exporting a method of
 *  @param i_cppMethod
 *      the name of the method in C++
 *  @param t_return
 *      the return type of @a i_cppMethod
 *  @param t_params
 *      a lass::meta::TypeList of the parameter types of @a i_cppMethod
 *  @param s_methodName
 *      the name the method will have in Python
 *  @param s_doc
 *      documentation of method as shown in Python (zero terminated C string)
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *      per exported C++ class/method pair.
 *
 *  You can use this macro instead of PY_CLASS_METHOD_EX if there's a need to convert any types on the fly.
 *
 *  Just like PY_CLASS_METHOD_EX, PY_CLASS_METHOD_QUALIFIED_EX allows for overloading.  These
 *  overloads may be mixed with PY_CLASS_METHOD_EX methods.
 *  By default two casting operators are supported:
 *	* PointerCast<_type> : will interpret the ownership rules as if a pointer is passed along
 *	* CopyCast<_type> : will interpret the ownership rules as if a copy of the object is passed along
 *
 *	It is also possible to define own casting operators in a similar and easy way: look for the PointerCast code.
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      void bar(int a);
 *      void bar(const std::string& b) const;
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_METHOD_CAST_EX_0(Foo, bar, void, CopyCast<const std::string&>, "bar", 0, foo_bar_a)
 *  @endcode
 */


/** @ingroup Python
 *  @sa PY_CLASS_CAST_QUALIFIED_EX_0
 *  convenience macro, wraps PY_CLASS_CAST_QUALIFIED_EX for 0 arguments
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
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX for $x arguments
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

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_EX_0 with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a t_cppClass ## __LINE__.
 */
#define PY_CLASS_METHOD_CAST_NAME_DOC_0( i_cppClass, i_cppMethod, t_return, s_methodName, s_doc )\
	PY_CLASS_METHOD_CAST_EX_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)),\
		LASS_UNIQUENAME(LASS_CONCATENATE(TypelassPyImpl_method_, i_cppClass)))
$[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_EX_$x with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a t_cppClass ## __LINE__.
 */
#define PY_CLASS_METHOD_CAST_NAME_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc )\
	PY_CLASS_METHOD_CAST_EX_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)),\
		LASS_UNIQUENAME(LASS_CONCATENATE(TypelassPyImpl_method_, i_cppClass)))
]$

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_NAME_DOC with @a s_doc = 0.
 */
#define PY_CLASS_METHOD_CAST_NAME( i_cppClass, i_cppMethod, t_return, t_params, s_methodName )\
		PY_CLASS_METHOD_CAST_NAME_DOC(\
			i_cppClass, i_cppMethod, t_return, t_params, s_methodName, 0 )

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_NAME_DOC_0 with @a s_doc = 0.
 */
#define PY_CLASS_METHOD_CAST_NAME_0( i_cppClass, i_cppMethod, t_return, s_methodName )\
	PY_CLASS_METHOD_CAST_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, s_methodName, 0 )
$[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_NAME_DOC_$x with @a s_doc = 0.
 */
#define PY_CLASS_METHOD_CAST_NAME_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName )\
	PY_CLASS_METHOD_CAST_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, 0 )
]$

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod.
 */
#define PY_CLASS_METHOD_CAST_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_doc )\
	PY_CLASS_METHOD_CAST_NAME_DOC(\
		i_cppClass, i_cppMethod, t_return, t_params, LASS_STRINGIFY(i_cppMethod), s_doc )

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod.
 */
#define PY_CLASS_METHOD_CAST_DOC_0( i_cppClass, i_cppMethod, t_return, s_doc )\
	PY_CLASS_METHOD_CAST_NAME_DOC_0(\
		i_cppClass, i_cppMethod, t_return, LASS_STRINGIFY(i_cppMethod), s_doc )
$[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod.
 */
#define PY_CLASS_METHOD_CAST_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_doc )\
	PY_CLASS_METHOD_CAST_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, LASS_STRINGIFY(i_cppMethod), s_doc )
]$

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod and @a s_doc = 0.
 */
#define PY_CLASS_METHOD_CAST( i_cppClass, i_cppMethod, t_return, t_params )\
	PY_CLASS_METHOD_CAST_DOC( i_cppClass, i_cppMethod, t_return, t_params, 0 )

/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod and @a s_doc = 0.
 */
#define PY_CLASS_METHOD_CAST_0( i_cppClass, i_cppMethod, t_return )\
	PY_CLASS_METHOD_CAST_DOC_0( i_cppClass, i_cppMethod, t_return, 0 )
$[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_CAST_EX
 *  convenience macro, wraps PY_CLASS_METHOD_CAST_NAME_DOC
 *  with @a s_methodName = # @a i_cppMethod and @a s_doc = 0.
 */
#define PY_CLASS_METHOD_CAST_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$ )\
	PY_CLASS_METHOD_CAST_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, 0 )
]$



// --- static methods ------------------------------------------------------------------------------

/** @ingroup Python
 *  @brief Exports a function to Python as a static method (aka class method)
 *  @sa PY_CLASS_METHOD_EX
 *
 *  @param t_cppClass
 *      the C++ class you want to add the static method to.
 *  @param f_cppFunction
 *      the full name of the C++ function that implements the static method
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *      per exported C++ class/method pair.
 *  @param s_doc
 *      documentation of method as shown in Python (zero terminated C string)
 *  @param s_methodName
 *      the name of the static method in Python (zerp terminated C string)
 *
 *  @code
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
 *  PY_CLASS_STATIC_METHOD_EX(Foo, Foo::bar, foo_bar, "bar", "a regular C++ static method")
 *  PY_CLASS_STATIC_METHOD_EX(Foo, spam, foo_spam, "spam", "you can export free C++ functions as")
 *  @endcode
 */
#define PY_CLASS_STATIC_METHOD_EX( t_cppClass, f_cppFunction, s_methodName, s_doc, i_dispatcher )\
	PyCFunction LASS_CONCATENATE(i_dispatcher, _overloadChain) = 0;\
	inline PyObject* i_dispatcher( PyObject* iIgnore, PyObject* iArgs )\
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

/** @ingroup Python
 *  @sa PY_CLASS_STATIC_METHOD_EX
 *  convenience macro, wraps PY_CLASS_STATIC_METHOD_EX
 *  with @a f_cppFunction = @a i_cppClass :: @a i_cppMethod
 *  and @a i_dispatcher = lassPyImpl_StaticMethod_ @a i_cppClass __LINE__
 */
#define PY_CLASS_STATIC_METHOD_DOC( i_cppClass, i_cppMethod, s_doc )\
	PY_CLASS_STATIC_METHOD_EX(\
		i_cppClass,\
		&::lass::python::impl::ShadowTraits<i_cppClass>::TCppClass::i_cppMethod,\
		LASS_STRINGIFY(i_cppMethod), s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_staticMethod_, i_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_STATIC_METHOD_EX
 *  convenience macro, wraps PY_CLASS_STATIC_METHOD_EX
 *  with @a f_cppFunction = @a i_cppClass :: @a i_cppMethod
 *  and @a i_dispatcher = lassPyImpl_StaticMethod_ @a i_cppClass __LINE__
 */
#define PY_CLASS_STATIC_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, s_doc )\
	PY_CLASS_STATIC_METHOD_EX(\
		i_cppClass,\
		&::lass::python::impl::ShadowTraits<i_cppClass>::TCppClass::i_cppMethod,\
		s_methodName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_staticMethod_, i_cppClass)))


/** @ingroup Python
 *  @sa PY_CLASS_STATIC_METHOD_EX
 *  convenience macro, wraps PY_CLASS_STATIC_METHOD_EX
 *  with @a f_cppFunction = @a i_cppClass :: @a i_cppMethod
 *  and @a i_dispatcher = lassPyImpl_StaticMethod_ @a i_cppClass __LINE__
 */
#define PY_CLASS_STATIC_METHOD_NAME( i_cppClass, i_cppMethod, s_methodName)\
	PY_CLASS_STATIC_METHOD_EX(\
		i_cppClass,\
		&::lass::python::impl::ShadowTraits<i_cppClass>::TCppClass::i_cppMethod,\
		s_methodName, "",\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_staticMethod_, i_cppClass)))


/** @ingroup Python
 *  @sa PY_CLASS_STATIC_METHOD_EX
 *  convenience macro, wraps PY_CLASS_STATIC_METHOD_EX
 *  with f_cppFunction = @a i_cppClass :: @a i_cppMethod
 */
#define PY_CLASS_STATIC_METHOD( i_cppClass, i_cppMethod )\
		PY_CLASS_STATIC_METHOD_DOC( i_cppClass, i_cppMethod, 0 )



// --- data members --------------------------------------------------------------------------------

/** @ingroup Python
 *  exports a pair of get and set accessors as an read/write attribute in Python
 *
 *  @param t_cppClass
 *      the C++ class you want to add the static method to.
 *  @param s_memberName
 *      the name of the attribute in python (zero terminated C string)
 *  @param a_cppGetter
 *      the method in C++ used to get the attribute
 *  @param a_cppSetter
 *      the method in C++ used to set the attribute
 *  @param s_doc
 *      documentation of member as shown in Python (zero terminated C string)
 *
 *  @code
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
 *  PY_CLASS_MEMBER_RW_EX(Foo, getBar, setBar, "bar", "regular get and setter")
 *  PY_CLASS_MEMBER_RW_EX(Foo, spam, spam, "spam", "cool get and setter")
 *  @endcode
 */
#define PY_CLASS_MEMBER_RW_EX( t_cppClass, i_cppGetter, i_cppSetter, s_memberName, s_doc, i_dispatcher)\
        PyObject* LASS_CONCATENATE(i_dispatcher, _getter)( PyObject* iObject, void* )\
        {\
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
        }\
	int LASS_CONCATENATE(i_dispatcher, _setter)( PyObject* iObject, PyObject* iArgs, void* )\
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

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_RW_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_RW_EX with
 *  @a i_dispatcher = lassPyImpl_memberR_ @a t_cppClass __LINE__
 */
#define PY_CLASS_MEMBER_RW_NAME_DOC(t_cppClass, i_cppGetter, i_cppSetter, s_memberName, s_doc)\
	PY_CLASS_MEMBER_RW_EX(t_cppClass, i_cppGetter, i_cppSetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_memberRW, t_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_RW_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_RW_NAME_DOC with @a s_doc = 0
 */
#define PY_CLASS_MEMBER_RW_NAME(t_cppClass, i_cppGetter, i_cppSetter, s_memberName)\
	PY_CLASS_MEMBER_RW_NAME_DOC(t_cppClass, i_cppGetter, i_cppSetter, s_memberName, 0)

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_RW_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_RW_NAME_DOC with @a s_name = "s_cppGetter"
 */
#define PY_CLASS_MEMBER_RW_DOC(t_cppClass, i_cppGetter, i_cppSetter, s_doc)\
	PY_CLASS_MEMBER_RW_NAME_DOC(t_cppClass, i_cppGetter, i_cppSetter, LASS_STRINGIFY(i_cppGetter), s_doc)

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_RW_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_RW_NAME_DOC with @a s_name = "s_cppGetter" and @a s_doc = 0
 */
#define PY_CLASS_MEMBER_RW(t_cppClass, i_cppGetter, i_cppSetter)\
	PY_CLASS_MEMBER_RW_DOC(t_cppClass, i_cppGetter, i_cppSetter, 0)



/** @ingroup Python
 *  exports a get accessor as an read-only attribute in Python
 *
 *  @param t_cppClass
 *      the C++ class you want to add the static method to.
 *  @param s_memberName
 *      the name of the attribute in python (zero terminated C string)
 *  @param a_cppGetter
 *      the method in C++ used to get the attribute
 *  @param s_doc
 *      documentation of member as shown in Python (zero terminated C string)
 *
 *  @code
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
 *  PY_CLASS_MEMBER_R_EX(Foo, getBar, "bar", "regular get and setter")
 *  @endcode
 */
#define PY_CLASS_MEMBER_R_EX( t_cppClass, i_cppGetter, s_memberName, s_doc, i_dispatcher )\
        PyObject* LASS_CONCATENATE(i_dispatcher, _getter)( PyObject* iObject, void* )\
        {\
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
        }\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), 0);\
	)


/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_R_EX with
 *  @a i_dispatcher = lassPyImpl_memberR_ ## @a t_cppClass ## __LINE__
 */
#define PY_CLASS_MEMBER_R_NAME_DOC(t_cppClass, i_cppGetter, s_memberName, s_doc)\
	PY_CLASS_MEMBER_R_EX(t_cppClass, i_cppGetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_memberR, t_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_R_NAME_DOC with @a s_doc = 0
 */
#define PY_CLASS_MEMBER_R_NAME(t_cppClass, i_cppGetter, s_memberName)\
	PY_CLASS_MEMBER_R_NAME_DOC(t_cppClass, i_cppGetter, s_memberName, 0)

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_R_NAME_DOC with @a s_memberName = "i_cppGetter"
 */
#define PY_CLASS_MEMBER_R_DOC(t_cppClass, i_cppGetter, s_doc)\
	PY_CLASS_MEMBER_R_NAME_DOC(t_cppClass, i_cppGetter, LASS_STRINGIFY(i_cppGetter), s_doc)

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_R_DOC
 *  convenience macro, wraps PY_CLASS_MEMBER_R_NAME_DOC with @a s_memberName = "i_cppGetter" and @a s_doc = 0
 */
#define PY_CLASS_MEMBER_R(t_cppClass, i_cppGetter)\
	PY_CLASS_MEMBER_R_DOC(t_cppClass, i_cppGetter, 0)

// --- data members --------------------------------------------------------------------------------

/** @ingroup Python
 *  exports a pair of get and set accessors as an read/write attribute in Python by using free functions
 *
 *  @param t_cppClass
 *      the C++ class you want to add the static method to.
 *  @param s_memberName
 *      the name of the attribute in python (zero terminated C string)
 *  @param a_freeCppGetter
 *      the free method in C++ used to get the attribute
 *  @param a_freeCppSetter
 *      the free method in C++ used to set the attribute
 *  @param s_doc
 *      documentation of member as shown in Python (zero terminated C string)
 *  @deprecated
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
	PyObject* LASS_CONCATENATE(i_dispatcher, _getter)( PyObject* iObject, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		return ::lass::python::impl::CallMethod<TShadowTraits>::freeGet( iObject, i_cppFreeGetter );\
	}\
	int LASS_CONCATENATE(i_dispatcher, _setter)( PyObject* iObject, PyObject* iArgs, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		return ::lass::python::impl::CallMethod<TShadowTraits>::freeSet( iArgs, iObject, i_cppFreeSetter );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), LASS_CONCATENATE(i_dispatcher, _setter));\
	)

/** @ingroup Python
 *  @sa PY_CLASS_FREE_MEMBER_RW_EX
 *  convenience macro, wraps PY_CLASS_FREE_MEMBER_RW_EX with
 *  @a i_dispatcher = lassPyImpl_memberR_ @a t_cppClass __LINE__
 */
#define PY_CLASS_FREE_MEMBER_RW_NAME_DOC(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName, s_doc)\
	PY_CLASS_FREE_MEMBER_RW_EX(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_freeMemberRW, t_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_FREE_MEMBER_RW_EX
 *  convenience macro, wraps PY_CLASS_FREE_MEMBER_RW_NAME_DOC with @a s_doc = 0
 */
#define PY_CLASS_FREE_MEMBER_RW_NAME(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName)\
	PY_CLASS_FREE_MEMBER_RW_NAME_DOC(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_memberName, 0)

/** @ingroup Python
 *  @sa PY_CLASS_FREE_MEMBER_RW_EX
 *  convenience macro, wraps PY_CLASS_FREE_MEMBER_RW_NAME_DOC with @a s_name = "s_cppFreeGetter"
 */
#define PY_CLASS_FREE_MEMBER_RW_DOC(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, s_doc)\
	PY_CLASS_FREE_MEMBER_RW_NAME_DOC(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, LASS_STRINGIFY(i_cppFreeGetter), s_doc)

/** @ingroup Python
 *  @sa PY_CLASS_FREE_MEMBER_RW_EX
 *  convenience macro, wraps PY_CLASS_FREE_MEMBER_RW_NAME_DOC with @a s_name = "s_freeCppGetter" and @a s_doc = 0
 */
#define PY_CLASS_FREE_MEMBER_RW(t_cppClass, i_cppFreeGetter, i_cppFreeSetter)\
	PY_CLASS_FREE_MEMBER_RW_DOC(t_cppClass, i_cppFreeGetter, i_cppFreeSetter, 0)



/** @ingroup Python
 *  exports a get accessor as an read-only attribute in Python as a free function
 *
 *  @param t_cppClass
 *      the C++ class you want to add the static method to.
 *  @param s_memberName
 *      the name of the attribute in python (zero terminated C string)
 *  @param a_freeCppGetter
 *      a free function in C++ used to get the attribute
 *  @param s_doc
 *      documentation of member as shown in Python (zero terminated C string)
 *  @deprecated
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
	PyObject* LASS_CONCATENATE(i_dispatcher, _getter)( PyObject* iObject, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		return ::lass::python::impl::CallMethod<TShadowTraits>::freeGet( iObject, i_freeCppGetter );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), 0);\
	)

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_R_EX with
 *  @a i_dispatcher = lassPyImpl_memberR_ ## @a t_cppClass ## __LINE__
 */
#define PY_CLASS_FREE_MEMBER_R_NAME_DOC(t_cppClass, i_freeCppGetter, s_memberName, s_doc)\
	PY_CLASS_FREE_MEMBER_R_EX(t_cppClass, i_freeCppGetter, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_freeMemberR, t_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_R_NAME_DOC with @a s_doc = 0
 */
#define PY_CLASS_FREE_MEMBER_R_NAME(t_cppClass, i_freeCppGetter, s_memberName)\
	PY_CLASS_FREE_MEMBER_R_NAME_DOC(t_cppClass, i_freeCppGetter, s_memberName, 0)

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_R_NAME_DOC with @a s_memberName = "i_freeCppGetter"
 */
#define PY_CLASS_FREE_MEMBER_R_DOC(t_cppClass, i_freeCppGetter, s_doc)\
	PY_CLASS_FREE_MEMBER_R_NAME_DOC(t_cppClass, i_freeCppGetter, LASS_STRINGIFY(i_freeCppGetter), s_doc)

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_R_DOC
 *  convenience macro, wraps PY_CLASS_MEMBER_R_NAME_DOC with @a s_memberName = "i_cppGetter" and @a s_doc = 0
 */
#define PY_CLASS_FREE_MEMBER_R(t_cppClass, i_freeCppGetter)\
	PY_CLASS_FREE_MEMBER_R_DOC(t_cppClass, i_freeCppGetter, 0)

/** @ingroup Python
 *  exports a public data member as a read/write attribute in Python
 *
 *  @param t_cppClass
 *      the C++ class you want to add the static method to.
 *  @param i_cppMember
 *      the C++ data meber to be exported
 *  @param s_memberName
 *      the name of the attribute in python (zero terminated C string)
 *  @param s_doc
 *      documentation of member as shown in Python (zero terminated C string)
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
	PyObject* LASS_CONCATENATE(i_dispatcher, _getter)(PyObject* obj, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		TShadowTraits::TConstCppClassPtr self;\
		if (TShadowTraits::getObject(obj, self) != 0)\
		{\
			return 0;\
		}\
		return lass::python::pyBuildSimpleObject(self->i_cppMember);\
	}\
	int LASS_CONCATENATE(i_dispatcher, _setter)(PyObject* obj,PyObject* args, void* )\
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

/** @ingroup Python
 *  @sa PY_CLASS_PUBLIC_MEMBER_EX
 *  convenience macro, wraps PY_CLASS_PUBLIC_MEMBER_EX with
 *  @a dispatcher = lassPyImpl_publicMember_ ## @a i_cppClass ## __LINE__.
 */
#define PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, s_memberName, s_doc )\
	PY_CLASS_PUBLIC_MEMBER_EX( i_cppClass, i_cppMember, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_publicMember_, i_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_PUBLIC_MEMBER_EX
 *  convenience macro, wraps PY_CLASS_PUBLIC_MEMBER_NAME_DOC with @a s_doc = 0.
 */
#define PY_CLASS_PUBLIC_MEMBER_NAME( i_cppClass, i_cppMember, s_memberName )\
	PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, s_memberName, 0 )

/** @ingroup Python
 *  @sa PY_CLASS_PUBLIC_MEMBER_EX
 *  convenience macro, wraps PY_CLASS_PUBLIC_MEMBER_NAME_DOC with @a s_memberName = "@a i_cppMember".
 */
#define PY_CLASS_PUBLIC_MEMBER_DOC( i_cppClass, i_cppMember , s_doc)\
	PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember), s_doc )

/** @ingroup Python
 *  @sa PY_CLASS_PUBLIC_MEMBER_EX
 *  convenience macro, wraps PY_CLASS_PUBLIC_MEMBER_NAME_DOC with @a s_memberName = "@a i_cppMember"
 *  and @a s_doc = 0.
 */
#define PY_CLASS_PUBLIC_MEMBER( i_cppClass, i_cppMember )\
	PY_CLASS_PUBLIC_MEMBER_NAME_DOC( i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember), 0 )




/** @ingroup Python
 *  exports a public data member as a read-only attribute in Python
 *
 *  @param t_cppClass
 *      the C++ class you want to add the static method to.
 *  @param i_cppMember
 *      the C++ data meber to be exported
 *  @param s_memberName
 *      the name of the attribute in python (zero terminated C string)
 *  @param s_doc
 *      documentation of member as shown in Python (zero terminated C string)
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
	PyObject* LASS_CONCATENATE(i_dispatcher, _getter)(PyObject* obj, void* )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		TShadowTraits::TConstCppClassPtr self;\
		if (TShadowTraits::getObject(obj, self) != 0)\
		{\
			return 0;\
		}\
		return lass::python::pyBuildSimpleObject(self->i_cppMember);\
	}\
	int LASS_CONCATENATE(i_dispatcher, _setter)( PyObject*, PyObject*, void* )\
	{\
		std::ostringstream buffer;\
		buffer << "Object/reference " << s_memberName << " is read-only.";\
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());\
		return -1;\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass ::_lassPyClassDef.addGetSetter(\
				s_memberName, s_doc,\
				LASS_CONCATENATE(i_dispatcher, _getter), LASS_CONCATENATE(i_dispatcher, _setter));\
	)

/** @ingroup Python
 *  @sa PY_CLASS_PUBLIC_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_PUBLIC_MEMBER_R_EX with
 *  @a i_dispatcher = lassPyImpl_publicMemberR_ ## @a i_cppClass ## __LINE__.
 */
#define PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, s_memberName, s_doc)\
	PY_CLASS_PUBLIC_MEMBER_R_EX(i_cppClass, i_cppMember, s_memberName, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_publicMemberR_, i_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_PUBLIC_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC with @a s_doc = 0.
 */
#define PY_CLASS_PUBLIC_MEMBER_R_NAME(i_cppClass, i_cppMember, s_memberName)\
	PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, s_memberName, 0 )

/** @ingroup Python
 *  @sa PY_CLASS_PUBLIC_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC with @a s_memberName = "@a i_cppMember"
 */
#define PY_CLASS_PUBLIC_MEMBER_R_DOC(i_cppClass, i_cppMember , s_doc)\
	PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember),  s_doc)

/** @ingroup Python
 *  @sa PY_CLASS_PUBLIC_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC with @a s_memberName = "@a i_cppMember"
 *  and @a s_doc = 0.
 */
#define PY_CLASS_PUBLIC_MEMBER_R(i_cppClass, i_cppMember)\
	PY_CLASS_PUBLIC_MEMBER_R_NAME_DOC(i_cppClass, i_cppMember, LASS_STRINGIFY(i_cppMember), 0)



// --- constructors --------------------------------------------------------------------------------

/** @ingroup Python
 *  makes a abstract python class concreet by adding a constructor.
 *
 *  All python classes are abstract by default, you don't have any constructors to create
 *  instances.  To make a class concrete, you must provide a constructor by using this macro.
 *  You can overload this constructor by the same way you can overload class methods.
 *
 *  @param t_cppClass
 *      the C++ class to generate the constructor
 *  @param t_params
 *      a meta::TypeList of the parameters needed by the constructor.
 *      Use meta::NullType if the constructor doesn't take any parameters.
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *      per exported C++ class/constructor pair.
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
 *   TRUCTOR(Foo, meta::NullType) // constructor without arguments.
 *  typedef meta::type_list::Make<int, std::string>::Type TArguments;
 *  PY_CLASS_CONSTRUCTOR(Foo, TArguments) // constructor with some arguments. *
 *  @endcode
 */
#define PY_CLASS_CONSTRUCTOR_EX( t_cppClass, t_params, i_dispatcher )\
	static newfunc LASS_CONCATENATE(i_dispatcher, _overloadChain) = 0;\
	PyObject* i_dispatcher(PyTypeObject *iSubtype, PyObject *iArgs, PyObject *iKwds)\
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
		::TImpl::callConstructor(iSubtype, iArgs);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX(\
		LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		LASS_CONCATENATE(i_dispatcher, _overloadChain) = t_cppClass::_lassPyClassDef.type()->tp_new;\
		t_cppClass::_lassPyClassDef.type()->tp_new = i_dispatcher; \
	)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_CONSTRUCTOR_EX with
 *  @a i_dispatcher = lassPyImpl_constructor_ ## @a i_cppClass ## __LINE__
 */
#define PY_CLASS_CONSTRUCTOR( i_cppClass, t_params )\
	PY_CLASS_CONSTRUCTOR_EX(i_cppClass, t_params,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_constructor_, i_cppClass)))

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_CONSTRUCTOR for 0 arguments
 */
#define PY_CLASS_CONSTRUCTOR_0( t_cppClass )\
	PY_CLASS_CONSTRUCTOR( t_cppClass, ::lass::meta::NullType )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_CONSTRUCTOR for $x arguments
 */
#define PY_CLASS_CONSTRUCTOR_$x( t_cppClass, $(t_P$x)$ )\
	typedef ::lass::meta::type_list::Make< $(t_P$x)$ >::Type \
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, t_cppClass));\
	PY_CLASS_CONSTRUCTOR(\
		t_cppClass, LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, t_cppClass)))
]$


/** @ingroup Python
 *  @brief Exports a function to Python as a constructor method
 *  @sa PY_CLASS_FREE_CONSTRUCTOR
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
	inline PyObject* i_dispatcher( PyTypeObject *iSubtype, PyObject *iArgs, PyObject *iKwds )\
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
	LASS_EXECUTE_BEFORE_MAIN_EX(\
		LASS_CONCATENATE(i_dispatcher, _excecuteBeforeMain ),\
		LASS_CONCATENATE(i_dispatcher, _overloadChain) = t_cppClass::_lassPyClassDef.type()->tp_new;\
		t_cppClass::_lassPyClassDef.type()->tp_new = i_dispatcher; \
	)
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_CONSTRUCTOR_EX with
 *  @a i_dispatcher = lassPyImpl_constructor_ ## @a i_cppClass ## __LINE__
 */
#define PY_CLASS_FREE_CONSTRUCTOR( i_cppClass, f_cppFunction, t_params )\
	PY_CLASS_FREE_CONSTRUCTOR_EX(i_cppClass, f_cppFunction, t_params,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_constructor_, i_cppClass)))

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_CONSTRUCTOR for 0 arguments
 */
#define PY_CLASS_FREE_CONSTRUCTOR_0( t_cppClass, f_cppFunction)\
	PY_CLASS_FREE_CONSTRUCTOR( t_cppClass, f_cppFunction, ::lass::meta::NullType )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_CONSTRUCTOR for $x arguments
 */
#define PY_CLASS_FREE_CONSTRUCTOR_$x( t_cppClass, f_cppFunction, $(t_P$x)$ )\
	typedef ::lass::meta::type_list::Make< $(t_P$x)$ >::Type \
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, t_cppClass));\
	PY_CLASS_FREE_CONSTRUCTOR(\
		t_cppClass, f_cppFunction, LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, t_cppClass)))
]$



// --- internals -----------------------------------------------------------------------------------

/** @internal
 */
#define PY_CLASS_METHOD_IMPL(t_cppClass, i_cppMethod, s_methodName, s_doc, i_dispatcher, i_caller)\
	static ::lass::python::impl::OverloadLink LASS_CONCATENATE(i_dispatcher, _overloadChain);\
	PyObject* i_dispatcher(PyObject* iSelf, PyObject* iArgs)\
	{\
		PyObject* result = 0;\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain)(iSelf, iArgs, result))\
		{\
			return result;\
		}\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
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
