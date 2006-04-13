/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#define LASS_PYTHON_ERR_MSG_NO_NEW_OPERATOR "Can not create object"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_BOOL  "not bool"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_INT  "not int"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_LONG  "not long"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_FLOAT  "not float"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_DOUBLE  "not double"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_STRING  "not string"

#ifndef PySequence_ITEM
#	define PySequence_ITEM(o, i) PySequence_GetItem(o, i)
#endif

/** @internal
 *  This macro is only used in the PyObjectPlus class and is for internal LASS
 *  use.  Do not use in custom objects... DON'T!
 */
#define PY_HEADER_INTERNAL \
	public: \
		static PyTypeObject   Type; \
		static ::std::vector<PyMethodDef>    Methods; \
		static ::std::vector<PyGetSetDef>    GetSetters; \
		virtual PyTypeObject *GetType(void) const {return &Type;};

/** @ingroup Python
 *  Place as first line of your Pythonized class.    
 *
 *  For @a t_parentClass use the C++ class from which you wish the python object inherits.  
 *	@a t_parentClass must also be a Pythonized class or use lass::python::PyObjectPlus as default.
 *
 *   @remark Any declarations coming after this macro are public!
 */
#define PY_HEADER( t_parentClass ) \
	public: \
		typedef t_parentClass TPyParent;\
		static PyTypeObject   Type; \
		static ::std::vector<PyMethodDef>    Methods; \
		static ::std::vector<PyGetSetDef>    GetSetters; \
		static ::std::vector< ::lass::python::impl::StaticMember >	Statics; \
		static PyTypeObject* GetParentType(void)\
		{\
			return &TPyParent::Type != &::lass::python::PyObjectPlus::Type ? &TPyParent::Type : &PyBaseObject_Type;\
		}\
		virtual PyTypeObject *GetType(void) const {return &Type;};



/** @ingroup Python
*
*	@deprecated You should use protected constructors in base classes to pass the python type
*	from the most derived class to PyObjectPluse
*
*	Use this macro to make objects derived from PyObjectPlus have
*   a type in Python too.  If you don't use this macro you can still derive from the PyObjectPlus
*   parent class with the type as argument for the constructor.  For single inheritance this last
*   method is preferred.  For classes with a multitude of constructors the PYTHONIZE macro can
*   be considered.
*/
#define PY_PYTHONIZE \
	{this->ob_type = &this->Type;}



// --- modules -------------------------------------------------------------------------------------

/** @ingroup Python
 *  Declare and define an object respresenting a python module.
 *
 *  @param i_module 
 *		the identifier of the module to be used in C++
 */
#define PY_DECLARE_MODULE( i_module ) \
	PyObject* LASS_CONCATENATE( lassPythonModule, i_module ) = 0;\
	std::vector<PyMethodDef> LASS_CONCATENATE_3( lassPythonModule, Methods, i_module ); \
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyDeclareModule_, i_module ), \
		LASS_CONCATENATE_3( lassPythonModule, Methods, i_module ).push_back( \
			::lass::python::impl::createPyMethodDef( 0, 0, 0, 0 ) ) ;\
	)



/** @ingroup Python
 *	Inject a python module so Python is aware of it.
 *
 *  @remark This is to be done at @e runtime!  So, it has to be somewhere in your main or any
 *	function called by main.
 *
 *  @param i_module
 *		the identifier of a module declared by PY_DECLARE_MODULE
 *	@param s_moduleName
 *		name of module as shown in Python (zero terminated C string)
 *	@param s_doc
 *      documentation of module as shown in Python (zero terminated C string)
 */
#define PY_INJECT_MODULE_EX( i_module, s_moduleName, s_doc ) \
	{\
		Py_Initialize(); \
		LASS_CONCATENATE( lassPythonModule, i_module ) = Py_InitModule3(\
			s_moduleName, \
			&LASS_CONCATENATE_3( lassPythonModule, Methods, i_module )[0], s_doc ); \
	}

/** @ingroup Python
 *  convenience macro, wraps PY_INJECT_MODULE_EX with
 *  @a s_doc = 0
 */
#define PY_INJECT_MODULE_NAME( i_module, s_moduleName )\
	PY_INJECT_MODULE_EX( i_module, s_moduleName, 0)

/** @ingroup Python
 *  convenience macro, wraps PY_INJECT_MODULE_EX with
 *  @a s_moduleName = # @a i_module
 */
#define PY_INJECT_MODULE_DOC( i_module, s_doc )\
	PY_INJECT_MODULE_EX( i_module, const_cast<char*>( LASS_STRINGIFY(i_module) ), s_doc)

/** @ingroup Python
 *  convenience macro, wraps PY_INJECT_MODULE_EX with
 *  @a s_moduleName = # @a i_module and s_doc = 0
 */
#define PY_INJECT_MODULE( i_module )\
	PY_INJECT_MODULE_EX( i_module, const_cast<char*>( LASS_STRINGIFY(i_module) ), 0)



// --- module variables ----------------------------------------------------------------------------

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
		PyModule_AddObject(\
			LASS_CONCATENATE( lassPythonModule, i_module ), s_objectName,\
			::lass::python::pyBuildSimpleObject(o_object) );\
	}

/** @ingroup Python
 *  convenience macro, wraps PY_INJECT_OBJECT_IN_MODULE_EX with
 *  @a s_objectName = # @a o_object
 */
#define PY_INJECT_OBJECT_IN_MODULE( o_object, i_module )\
	PY_INJECT_OBJECT_IN_MODULE_EX(o_object, i_module, LASS_STRINGIFY(o_object))



/** @ingroup 
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
		PyModule_AddIntConstant(\
			LASS_CONCATENATE( lassPythonModule, i_module ),	s_name, s_value);\
	}



/** @ingroup 
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
		PyModule_AddStringConstant(\
			LASS_CONCATENATE( lassPythonModule, i_module ),	s_name, v_value);\
	}

// --- free module functions -----------------------------------------------------------------------

/* @ingroup Python
 * @depracated
 * Use this macro for backward compatibility when wrapper functions don't 
 * need to be automatically generated or you want specific Python behaviour.
 */
#define PY_MODULE_PY_FUNCTION_EX( i_module, f_cppFunction, s_functionName, s_doc )\
	extern std::vector< PyMethodDef > LASS_CONCATENATE_3( lassPythonModule, Methods, i_module );\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModulePyFunction_, i_module, f_cppFunction ),\
		LASS_CONCATENATE_3( lassPythonModule, Methods , i_module ).insert(\
			LASS_CONCATENATE_3( lassPythonModule, Methods , i_module ).begin(),\
				::lass::python::impl::createPyMethodDef( s_functionName, f_cppFunction , METH_VARARGS  , s_doc ));\
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
	extern std::vector< PyMethodDef > LASS_CONCATENATE( lassPythonModuleMethods, i_module );\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModuleFunction_, i_module, i_dispatcher ),\
		lass::python::impl::addModuleFunction(\
			LASS_CONCATENATE( lassPythonModuleMethods, i_module ),\
			s_functionName, s_doc, i_dispatcher,\
			LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) );\
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




// --- explicit qualified free functions -----------------------------------------------------------

/** @ingroup Python
 *  Exports a C++ free functions to Python will fully qualified return type and parameters
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
 *  PY_MODULE_FUNCTION_QUALIFIED_EX(foo_module, bar, void, LASS_TYPE_LIST_1(int), "bar", 0, foo_bar_a)
 *  PY_MODULE_FUNCTION_QUALIFIED_EX(foo_module, bar, void, LASS_TYPE_LIST_1(const std::string&), "bar", 0, foo_bar_b)
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
	extern std::vector< PyMethodDef > LASS_CONCATENATE_3( lassPythonModule, Methods, i_module );\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModuleFunction_, i_module, i_dispatcher ),\
		lass::python::impl::addModuleFunction(\
			LASS_CONCATENATE( lassPythonModuleMethods, i_module ),\
			s_functionName, s_doc, i_dispatcher,\
			LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) );\
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
	typedef LASS_TYPE_LIST_$x( $(t_P$x)$ )\
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
 *	@param i_uniqueClassIdentifier
 *		a unique identifier for household stuff
 *
 *  @remark put each PY_DECLARE_CLASS_EX in only one translation unit.  So not in headers!
 */
#define PY_DECLARE_CLASS_EX( t_cppClass, s_className, i_uniqueClassIdentifier ) \
	PyTypeObject t_cppClass::Type = {\
		PY_STATIC_FUNCTION_FORWARD_PLUS( t_cppClass, ::lass::meta::NullType, s_className ) };\
	std::vector<PyMethodDef> t_cppClass::Methods;\
	std::vector<PyGetSetDef> t_cppClass::GetSetters;\
	std::vector< ::lass::python::impl::StaticMember > t_cppClass::Statics;\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE( lassPythonImplExecutePyDeclareClass_, i_uniqueClassIdentifier ),\
		t_cppClass::Methods.push_back( ::lass::python::impl::createPyMethodDef( 0, 0, 0, 0 ) ) ; \
		t_cppClass::GetSetters.push_back( ::lass::python::impl::createPyGetSetDef( 0, 0, 0, 0, 0 ) ) ; \
)

/** @ingroup Python
 *  convenience macro, wraps PY_DECLARE_CLASS_EX for with
 *  @a i_uniqueClassIdentifier = @a i_cppClass
 */
#define PY_DECLARE_CLASS_NAME( i_cppClass, s_className )\
	PY_DECLARE_CLASS_EX( i_cppClass, s_className, i_cppClass )

/** @ingroup Python
 *  convenience macro, wraps PY_DECLARE_CLASS_EX for with
 *  @a i_uniqueClassIdentifier = @a i_cppClass and @a s_className = # @a i_cppClass
 */
#define PY_DECLARE_CLASS( i_cppClass ) \
	PY_DECLARE_CLASS_EX( i_cppClass, LASS_STRINGIFY(i_cppClass), i_cppClass );



#define PY_DECLARE_CLASS_PLUS_NAME( i_cppClass, s_className ) \
	PY_DECLARE_CLASS_PLUS_EX( i_cppClass, s_className, i_cppClass )

#define PY_DECLARE_CLASS_PLUS( i_cppClass ) \
	PY_DECLARE_CLASS_PLUS_EX( i_cppClass, LASS_STRINGIFY( i_cppClass ) i_cppClass )



/** @ingroup Python
 *  Inject a class in a module
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
	{\
		::lass::python::impl::injectClassInModule< t_cppClass >(\
			LASS_CONCATENATE(lassPythonModule, i_module), s_doc);\
	}

/** @ingroup Python
 *  @deprecated
 */
#define PY_INJECT_CLASS_IN_MODULE_BEFORE_MAIN( i_cppClass, i_module, s_doc ) \
	extern PyObject* LASS_CONCATENATE( lassPythonModule, i_module );\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyInjectClassInModule_, i_cppClass, i_module ),\
		PY_INJECT_CLASS_IN_MODULE_AT_RUNTIME( i_cppClass, i_module, s_doc );\
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
 *		python::pyBuildSimpleObject functions
 */
#define PY_CLASS_STATIC_CONST( i_cppClass, s_name, v_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyClassStaticConst, i_cppClass ),\
		::lass::python::impl::addClassStaticConst<i_cppClass>(s_name, v_value);\
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
		::lass::python::impl::addClassInnerClass<t_innerCppClass>(t_outerCppClass::Statics, s_name, s_doc);\
	)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_INNER_CLASS_EX with @a i_uniqueId = @a i_outerCppClass ## @a i_innerCppClass.
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
*   is no pyBuildSimpleObject defined for.
*/
#define PY_CLASS_PY_METHOD_EX( i_cppClass, i_cppMethod, s_methodName, s_doc  )\
	inline PyObject* LASS_CONCATENATE_3( staticDispatch, i_cppClass, i_cppMethod) ( PyObject* iObject, PyObject* iArgs )\
	{\
		if (!PyType_IsSubtype(iObject->ob_type , &i_cppClass::Type ))\
		{\
			PyErr_Format(PyExc_TypeError,"PyObject not castable to %s", i_cppClass::Type.tp_name);\
			return 0;\
		}\
		i_cppClass* object = static_cast<i_cppClass*>(iObject);\
		return object->i_cppMethod( iArgs );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyClassPyMethod_, i_cppClass, i_cppMethod ),\
		i_cppClass::Methods.insert(\
			i_cppClass::Methods.begin(),\
			::lass::python::impl::createPyMethodDef(\
				s_methodName, (PyCFunction) LASS_CONCATENATE_3( staticDispatch, i_cppClass, i_cppMethod),\
				METH_VARARGS, s_doc));\
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
		::lass::python::impl::CallMethod<TCppClass>::call)

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
		::lass::python::impl::CallMethod<TCppClass>::callFree)

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
 *  PY_CLASS_METHOD_QUALIFIED_EX(Foo, bar, void, LASS_TYPE_LIST_1(int), "bar", 0, foo_bar_a)
 *  PY_CLASS_METHOD_QUALIFIED_EX(Foo, bar, void, LASS_TYPE_LIST_1(const std::string&), "bar", 0, foo_bar_b)
 *  @endcode
 */
/*
#define PY_CLASS_METHOD_QUALIFIED_EX(t_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc, i_dispatcher)\
	PY_CLASS_METHOD_IMPL(t_cppClass, &TCppClass::i_cppMethod, s_methodName, s_doc, i_dispatcher,\
		(&::lass::python::impl::ExplicitResolver<TCppClass,t_return,t_params>::TImpl::callMethod))
/*/
#define PY_CLASS_METHOD_QUALIFIED_EX(t_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc, i_dispatcher)\
	static PyCFunction LASS_CONCATENATE(i_dispatcher, _overloadChain) = 0;\
	PyObject* i_dispatcher(PyObject* iObject, PyObject* iArgs)\
	{\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain))\
		{\
			PyObject* result = LASS_CONCATENATE(i_dispatcher, _overloadChain)(iObject, iArgs);\
			if (!(PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError)))\
			{\
				return result;\
			}\
			PyErr_Clear();\
			Py_XDECREF(result);\
		}\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* const cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return ::lass::python::impl::ExplicitResolver<TCppClass,t_return,t_params>::TImpl::callMethod(\
			iArgs, cppObject, &TCppClass::i_cppMethod); \
	}\
	static ternaryfunc LASS_CONCATENATE(i_dispatcher, _ternary_overloadChain) = 0;\
	PyObject* LASS_CONCATENATE(i_dispatcher, _ternary)(PyObject* iObject, PyObject* iArgs, PyObject* iKw)\
	{\
		if (LASS_CONCATENATE(i_dispatcher, _ternary_overloadChain))\
		{\
			PyObject* result = LASS_CONCATENATE(i_dispatcher, _ternary_overloadChain)(iObject, iArgs, iKw);\
			if (!(PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError)))\
			{\
				return result;\
			}\
			PyErr_Clear();\
			Py_XDECREF(result);\
		}\
		if (iKw)\
		{\
			PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");\
			return 0;\
		}\
		return i_dispatcher(iObject, iArgs);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX(LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		::lass::python::impl::addClassMethod< t_cppClass >(\
			s_methodName, s_doc, \
			i_dispatcher, LASS_CONCATENATE(i_dispatcher, _overloadChain),\
			LASS_CONCATENATE(i_dispatcher, _ternary), LASS_CONCATENATE(i_dispatcher, _ternary_overloadChain));\
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
	typedef LASS_TYPE_LIST_$x( $(t_P$x)$ )\
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
		i_cppClass, i_cppMethod, t_return, s_methodName )
$[
/** @ingroup Python
 *  @sa PY_CLASS_METHOD_QUALIFIED_EX
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x with @a s_doc = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x(\
		i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName )
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
		::lass::python::impl::addClassStaticMethod< t_cppClass >(\
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
 *  PY_CLASS_MEMBER_RW_EX(Foo, "bar", getBar, setBar, "regular get and setter")
 *  PY_CLASS_MEMBER_RW_EX(Foo, "spam", spam, spam, "cool get and setter")
 *  @endcode
 */
#define PY_CLASS_MEMBER_RW_EX( t_cppClass, s_memberName, i_cppGetter, i_cppSetter, s_doc, i_dispatcher)\
	PyObject* LASS_CONCATENATE(i_dispatcher, _getter)( PyObject* iObject, void* iClosure)\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return ::lass::python::impl::CallMethod<TCppClass>::get(\
			cppObject, &TCppClass::i_cppGetter );\
	}\
	int LASS_CONCATENATE(i_dispatcher, _setter)( PyObject* iObject, PyObject* iArgs, void* iClosure )\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		int r = ::lass::python::impl::CallMethod<TCppClass>::set(\
			iArgs, cppObject, &TCppClass::i_cppSetter );\
		return r;\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass::GetSetters.insert(\
			t_cppClass::GetSetters.begin(),\
			::lass::python::impl::createPyGetSetDef(\
				s_memberName,\
				LASS_CONCATENATE(i_dispatcher, _getter),\
				LASS_CONCATENATE(i_dispatcher, _setter),\
				s_doc, 0));\
	)

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_RW_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_RW_EX with
 *  @a i_dispatcher = lassPyImpl_memberR_ @a t_cppClass __LINE__
 */
#define PY_CLASS_MEMBER_RW_DOC(t_cppClass, s_memberName, i_cppGetter, i_cppSetter, s_doc)\
	PY_CLASS_MEMBER_RW_EX(t_cppClass, s_memberName, i_cppGetter, i_cppSetter, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_memberRW, t_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_RW_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_RW_DOC with @a s_doc = 0.
 */
#define PY_CLASS_MEMBER_RW(t_cppClass, s_memberName, i_cppGetter, i_cppSetter)\
	PY_CLASS_MEMBER_RW_DOC(t_cppClass, s_memberName, i_cppGetter, i_cppSetter, 0)



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
 *  PY_CLASS_MEMBER_R_EX(Foo, "bar", getBar, "regular get and setter")
 *  @endcode
 */
#define PY_CLASS_MEMBER_R_EX( t_cppClass, s_memberName, i_cppGetter, s_doc, i_dispatcher )\
	PyObject* LASS_CONCATENATE(i_dispatcher, _getter) ( PyObject* iObject, void* iClosure)\
	{\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return ::lass::python::impl::CallMethod<TCppClass>::get(\
			cppObject, &TCppClass::i_cppGetter );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		t_cppClass::GetSetters.insert(\
			t_cppClass::GetSetters.begin(),\
			::lass::python::impl::createPyGetSetDef(\
				s_memberName, LASS_CONCATENATE(i_dispatcher, _getter), 0, \
				s_doc, 0));\
	)

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_R_EX
 *  convenience macro, wraps PY_CLASS_MEMBER_R_EX with
 *  @a i_dispatcher = lassPyImpl_memberR_ ## @a t_cppClass ## __LINE__
 */
#define PY_CLASS_MEMBER_R_DOC(t_cppClass, s_memberName, i_cppGetter , s_doc)\
	PY_CLASS_MEMBER_R_EX(t_cppClass, s_memberName, i_cppGetter, s_doc,\
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_memberR, t_cppClass)))

/** @ingroup Python
 *  @sa PY_CLASS_MEMBER_R_DOC
 *  convenience macro, wraps PY_CLASS_MEMBER_R_EX with @a s_doc = 0.
 */
#define PY_CLASS_MEMBER_R(t_cppClass, s_memberName, i_cppGetter)\
	PY_CLASS_MEMBER_R_DOC(t_cppClass, s_memberName, i_cppGetter, 0)



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
 *  PY_CLASS_PUBLIC_MEMBER_EX(Foo, bar, "bar", "regular get and setter")
 *  @endcode
 */
#define PY_CLASS_PUBLIC_MEMBER_EX(i_cppClass, i_cppMember, s_memberName, s_doc, i_dispatcher)\
	PyObject* LASS_CONCATENATE(i_dispatcher, _getter)(PyObject* iObject, void* iClosure)\
	{\
		typedef ::lass::python::impl::ShadowTraits<i_cppClass> TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return ::lass::python::pyBuildSimpleObject(cppObject->i_cppMember);\
	}\
	int LASS_CONCATENATE(i_dispatcher, _setter)(PyObject* iObject,PyObject* iArgs, void* iClosure)\
	{\
		typedef ::lass::python::impl::ShadowTraits<i_cppClass> TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return -1;\
		}\
		return ::lass::python::pyGetSimpleObject(iArgs, cppObject->i_cppMember);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		i_cppClass::GetSetters.insert(\
			i_cppClass::GetSetters.begin(),\
			::lass::python::impl::createPyGetSetDef(\
				s_memberName,\
				LASS_CONCATENATE(i_dispatcher, _getter),\
				LASS_CONCATENATE(i_dispatcher, _setter),\
				s_doc, 0));\
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
#define PY_CLASS_PUBLIC_MEMBER_R_EX( i_cppClass, i_cppMember, s_memberName, s_doc, i_dispatcher )\
	PyObject* LASS_CONCATENATE(i_dispatcher, _getter)(PyObject* iObject, void* iClosure)\
	{\
		typedef ::lass::python::impl::ShadowTraits<i_cppClass> TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return ::lass::python::pyBuildSimpleObject( cppObject->i_cppMember);\
	}\
	int LASS_CONCATENATE(i_dispatcher, _setter)( PyObject* iObject,PyObject* iArgs, void* iClosure )\
	{\
		std::ostringstream buffer;\
		buffer << "Object/reference " << s_memberName << " is read-only.";\
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());\
		return -1;\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		i_cppClass::GetSetters.insert(\
			i_cppClass::GetSetters.begin(),\
			::lass::python::impl::createPyGetSetDef(\
				s_memberName,\
				LASS_CONCATENATE(i_dispatcher, _getter),\
				LASS_CONCATENATE(i_dispatcher, _setter),\
				s_doc, 0));\
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
			t_cppClass,\
			::lass::meta::NullType,\
			t_params\
		>\
		::TImpl::callConstructor(iArgs);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX(\
		LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		LASS_CONCATENATE(i_dispatcher, _overloadChain) = t_cppClass::Type.tp_new;\
		t_cppClass::Type.tp_new = i_dispatcher; \
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
#define PY_CLASS_CONSTRUCTOR_$x( t_cppClass, $(P$x__)$ )\
	typedef LASS_TYPE_LIST_$x( $(P$x__)$ ) \
		LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, t_cppClass));\
	PY_CLASS_CONSTRUCTOR(\
		t_cppClass, LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_TParams_, t_cppClass)))
]$



// --- internals -----------------------------------------------------------------------------------

/** @internal
 */
#define PY_STATIC_FUNCTION_FORWARD( t_cppClass, s_className )   \
	PyObject_HEAD_INIT(&PyType_Type)\
	0,	/*ob_size*/\
	(char*)( s_className ),	/*tp_name*/\
	sizeof( t_cppClass ),	/*tp_basicsize*/\
	0,	/*tp_itemsize*/\
	PyObjectPlus::__dealloc,	/*tp_dealloc*/\
	0,	/*tp_print*/\
	0,	/*tp_getattr*/\
	0,	/*tp_setattr*/\
	0,	/*tp_compare*/\
	0,	/*tp_repr*/\
	0,	/*tp_as_number*/\
	0,	/*tp_as_sequence*/\
	0,	/*tp_as_mapping*/\
	0,	/*tp_hash*/\
	0,	/*tp_call */\
	0,	/*tp_str */\
	0,/*PyObject_GenericGetAttr ,*/	/*tp_getattro */\
	0,/*PyObject_GenericSetAttr,*/	/*tp_setattro */\
	0,	/*tp_as_buffer*/\
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_CLASS ,	/*tp_flags*/\
	0,	/*tp_doc*/\
	0,	/*tp_traverse*/\
	0,	/*tp_clear*/\
	0,	/*tp_richcompare*/\
	0,	/*tp_weaklistoffset*/\
	0,	/*tp_iter*/\
	0,	/*tp_iternext*/\
	0,	/*tp_methods*/\
	0,	/*tp_members*/\
	0,	/*tp_getset*/\
	0,	/*tp_base*/\
	0,	/*tp_dict*/\
	0,	/*tp_descr_get*/\
	0,	/*tp_descr_set*/\
	0,	/*tp_dictoffset*/\
	0,	/*tp_init*/\
	0,	/*tp_alloc*/\
	0,	/*tp_new*/\
	0,	/*tp_free*/\
	0,	/*tp_is_gc*/\
	0,	/*tp_bases*/\
	0,	/*tp_mro*/\
	0,	/*tp_cache*/\
	0,	/*tp_subclasses*/\
	0,	/*tp_weaklist*/

/** @internal
 */
#define PY_STATIC_FUNCTION_FORWARD_PLUS( t_cppClass, t_cppParentClass, s_className )    \
	PyObject_HEAD_INIT(&PyType_Type)\
	0,	/*ob_size*/\
	(char*)( s_className ), /*tp_name*/\
	sizeof( t_cppClass ),	/*tp_basicsize*/\
	0,	/*tp_itemsize*/\
	PyObjectPlus::__dealloc,	/*tp_dealloc*/\
	0,	/*tp_print*/\
	0,	/*tp_getattr*/\
	0,	/*tp_setattr*/\
	0,	/*tp_compare*/\
	PyObjectPlus::__repr,	/*tp_repr*/\
	0,	/*tp_as_number*/\
	0,	/*tp_as_sequence*/\
	0,	/*tp_as_mapping*/\
	0,	/*tp_hash*/\
	0,	/*tp_call */\
	PyObjectPlus::__str,	/*tp_str */\
	0,/*PyObject_GenericGetAttr ,*/	/*tp_getattro */\
	0,/*PyObject_GenericSetAttr,*/	/*tp_setattro */\
	0,	/*tp_as_buffer*/\
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_CLASS ,	/*tp_flags*/\
	0,	/*tp_doc*/\
	0,	/*tp_traverse*/\
	0,	/*tp_clear*/\
	0,	/*tp_richcompare*/\
	0,	/*tp_weaklistoffset*/\
	0,	/*tp_iter*/\
	0,	/*tp_iternext*/\
	0,	/*tp_methods*/\
	0,	/*tp_members*/\
	0,	/*tp_getset*/\
	0,	/*tp_base*/\
	0,	/*tp_dict*/\
	0,	/*tp_descr_get*/\
	0,	/*tp_descr_set*/\
	0,	/*tp_dictoffset*/\
	0,	/*tp_init*/\
	0,	/*tp_alloc*/\
	0,	/*tp_new*/\
	0,	/*tp_free*/\
	0,	/*tp_is_gc*/\
	0,	/*tp_bases*/\
	0,	/*tp_mro*/\
	0,	/*tp_cache*/\
	0,	/*tp_subclasses*/\
	0,	/*tp_weaklist*/

/** @internal
 */
#define PY_CLASS_METHOD_IMPL(t_cppClass, i_cppMethod, s_methodName, s_doc, i_dispatcher, i_caller)\
	static PyCFunction LASS_CONCATENATE(i_dispatcher, _overloadChain) = 0;\
	PyObject* i_dispatcher(PyObject* iObject, PyObject* iArgs)\
	{\
		if (LASS_CONCATENATE(i_dispatcher, _overloadChain))\
		{\
			PyObject* result = LASS_CONCATENATE(i_dispatcher, _overloadChain)(iObject, iArgs);\
			if (!(PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError)))\
			{\
				return result;\
			}\
			PyErr_Clear();\
			Py_XDECREF(result);\
		}\
		typedef ::lass::python::impl::ShadowTraits< t_cppClass > TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* const cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return i_caller(iArgs, cppObject, i_cppMethod);\
	}\
	static ternaryfunc LASS_CONCATENATE(i_dispatcher, _ternary_overloadChain) = 0;\
	PyObject* LASS_CONCATENATE(i_dispatcher, _ternary)(PyObject* iObject, PyObject* iArgs, PyObject* iKw)\
	{\
		if (LASS_CONCATENATE(i_dispatcher, _ternary_overloadChain))\
		{\
			PyObject* result = LASS_CONCATENATE(i_dispatcher, _ternary_overloadChain)(iObject, iArgs, iKw);\
			if (!(PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError)))\
			{\
				return result;\
			}\
			PyErr_Clear();\
			Py_XDECREF(result);\
		}\
		if (iKw)\
		{\
			PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");\
			return 0;\
		}\
		return i_dispatcher(iObject, iArgs);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX(LASS_CONCATENATE(i_dispatcher, _executeBeforeMain),\
		::lass::python::impl::addClassMethod< t_cppClass >(\
			s_methodName, s_doc, \
			i_dispatcher, LASS_CONCATENATE(i_dispatcher, _overloadChain),\
			LASS_CONCATENATE(i_dispatcher, _ternary), LASS_CONCATENATE(i_dispatcher, _ternary_overloadChain));\
	)

#endif

// EOF
