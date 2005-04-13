/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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


/** @internal
 *  This macro is only used in the PyObjectPlus class and is for internal LASS
 *  use.  Do not use in custom objects... DON'T!
 */
#define PY_HEADER_INTERNAL \
	public: \
		static PyTypeObject   Type; \
		static ::std::vector<PyMethodDef>    Methods; \
		static ::std::vector<PyGetSetDef>    GetSetters; \
		virtual PyTypeObject *GetType(void) {return &Type;}; \
		/*static PyObject* __forbidden_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) \
		{ PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_NO_NEW_OPERATOR); return 0;}*/

/** Place as first line of your Pythonized class.  For ParentClass use the
*  C++ class from which you wish the python object inherits.  ParentClass
*  must also be a Pythonized class or use lass::python::PyObjectPlus as default.
*  @remark Any declarations coming after this macro are public!
*/
#define PY_HEADER( t_parentClass ) \
	public: \
		static PyTypeObject   Type; \
		static ::std::vector<PyMethodDef>    Methods; \
		static ::std::vector<PyGetSetDef>    GetSetters; \
		static ::std::vector<::lass::python::impl::StaticMember>	Statics; \
		static PyTypeObject* GetParentType(void) { return &t_parentClass::Type; }\
		virtual PyTypeObject *GetType(void) {return &Type;}; \
		/*static PyObject* __forbidden_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) \
		{ PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_NO_NEW_OPERATOR); return 0;}*/

/** PY_PYTHONIZE.  Use this macro to make objects derived from PyObjectPlus have
*   a type in Python too.  If you don't use this macro you can still derive from the PyObjectPlus
*   parent class with the type as argument for the constructor.  For single inheritance this last
*   method is preferred.  For classes with a multitude of constructors the PYTHONIZE macro can
*   be considered.
*/
#define PY_PYTHONIZE \
	{this->ob_type = &this->Type;}


#define PY_DECLARE_MODULE( i_module ) \
	PyObject* LASS_CONCATENATE( lassPythonModule, i_module ) = 0;\
	std::vector<PyMethodDef> LASS_CONCATENATE_3( lassPythonModule, Methods, i_module ); \
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyDeclareModule_, i_module ), \
		LASS_CONCATENATE_3( lassPythonModule, Methods, i_module ).push_back( \
			::lass::python::createPyMethodDef( 0, 0, 0, 0 ) ) ;\
	)

#define PY_INJECT_MODULE_EX_AT_RUNTIME( i_module, s_moduleName, s_doc ) \
	{\
		Py_Initialize(); \
		LASS_CONCATENATE( lassPythonModule, i_module ) = Py_InitModule3(\
			s_moduleName, \
			&LASS_CONCATENATE_3( lassPythonModule, Methods, i_module )[0], s_doc ); \
	}

#define PY_INJECT_MODULE_AT_RUNTIME( i_module, s_doc ) \
	{ \
		Py_Initialize(); \
		LASS_CONCATENATE( lassPythonModule, i_module ) = Py_InitModule3(\
			const_cast<char*>( LASS_STRINGIFY(i_module) ), \
			&LASS_CONCATENATE_3( lassPythonModule, Methods, i_module )[0], s_doc ); \
	}

#define PY_INJECT_MODULE_EX( i_module, s_doc ) \
	extern "C" { \
		void LASS_CONCATENATE( lassPythonInjectModule, i_module ) () \
		{ \
			PY_INJECT_MODULE_AT_RUNTIME( i_module, s_doc )\
		}\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyInjectModule_, i_module ), \
		LASS_CONCATENATE( lassPythonInjectModule, i_module ) ();\
	)

#define PY_INJECT_MODULE( i_module ) PY_INJECT_MODULE_EX( i_module, 0 )

/* Use this macro for backward compatibility when wrapper functions don't
*  need to be automatically generated or you want specific Python behaviour.
*/
#define PY_MODULE_PY_FUNCTION_EX( i_module, i_cppFunction, s_functionName, s_doc )\
	extern std::vector< PyMethodDef > LASS_CONCATENATE_3( lassPythonModule, Methods, i_module );\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModulePyFunction_, i_module, i_cppFunction ),\
		LASS_CONCATENATE_3( lassPythonModule, Methods , i_module ).insert(\
			LASS_CONCATENATE_3( lassPythonModule, Methods , i_module ).begin(),\
			::lass::python::createPyMethodDef( s_functionName, i_cppFunction , METH_VARARGS  , s_doc ));\
	)



#define PY_MODULE_FUNCTION_EX( i_module, f_cppFunction, i_dispatcher, s_functionName, s_doc )\
	inline PyObject* i_dispatcher( PyObject* iIgnore, PyObject* iArgs )\
	{\
		return ::lass::python::impl::callFunction( iArgs, f_cppFunction );\
	}\
	extern std::vector< PyMethodDef > LASS_CONCATENATE_3( lassPythonModule, Methods, i_module );\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyModuleFunction_, i_module, i_dispatcher ),\
		LASS_CONCATENATE_3( lassPythonModule, Methods, i_module ).insert(\
			LASS_CONCATENATE_3( lassPythonModule, Methods, i_module ).begin(),\
			::lass::python::createPyMethodDef(\
				s_functionName, i_dispatcher, METH_VARARGS, s_doc ));\
	)

#define PY_MODULE_FUNCTION_DOC( i_module, i_cppFunction, s_doc )\
	PY_MODULE_FUNCTION_EX(\
		i_module, i_cppFunction, LASS_CONCATENATE(py, i_cppFunction), LASS_STRINGIFY(i_cppFunction),\
		s_doc )


#define PY_MODULE_FUNCTION( i_module, i_cppFunction )\
	PY_MODULE_FUNCTION_DOC( i_module, i_cppFunction, 0 )

#define PY_STATIC_FUNCTION_FORWARD( t_cppClass, s_className )   \
	PyObject_HEAD_INIT(&PyType_Type)\
	0,      \
	(char*)( s_className ), \
	sizeof( t_cppClass ),           \
	0,                          \
	\
	PyObjectPlus::__dealloc,    \
	0,                          \
	0,                      /*tp_getattr*/\
	0,                      /*tp_setattr*/\
	0,                          \
	0,      \
	0,              /*tp_as_number*/\
	0,              /*tp_as_sequence*/\
	0,              /*tp_as_mapping*/\
	0,              /*tp_hash*/\
	0,              /*tp_call */\
	0,              /*tp_str */\
	PyObject_GenericGetAttr ,               /*tp_getattro */\
	PyObject_GenericSetAttr,                /*tp_setattro */\
	0,              /*tp_as_buffer*/\
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_CLASS ,      /*tp_flags*/\
	0,              /*tp_doc*/\
	0,              /*tp_traverse*/\
	0,              /*tp_clear*/\
	0,              /*tp_richcompare*/\
	0,              /*tp_weaklistoffset*/\
	0,              /*tp_iter*/\
	0,              /*tp_iternext*/\
	0,              /*tp_methods*/\
	0,              /*tp_members*/\
	0,              /*tp_getset*/\
	0,              /*tp_base*/\
	0,              /*tp_dict*/\
	0,              /*tp_descr_get*/\
	0,              /*tp_descr_set*/\
	0,              /*tp_dictoffset*/\
	0,              /*tp_init*/\
	0,/*tp_alloc*/\
	0/*t_cppClass ::__forbidden_new*/,/*tp_new*/\
	0,              /*tp_free*/\
	0,              /*tp_is_gc*/\
	0,              /*tp_bases*/\
	0,              /*tp_mro*/\
	0,              /*tp_cache*/\
	0,              /*tp_subclasses*/\
	0,              /*tp_weaklist*/

#define PY_STATIC_FUNCTION_FORWARD_PLUS( t_cppClass, t_cppParentClass, s_className )    \
	PyObject_HEAD_INIT(&PyType_Type)\
	0,      \
	(char*)( s_className ), \
	sizeof( t_cppClass ),           \
	0,                          \
	\
	PyObjectPlus::__dealloc,    \
	0,                          \
	0,                      /*tp_getattr*/\
	0,                      /*tp_setattr*/\
	0,                          \
	PyObjectPlus::__repr,       \
	0,              /*tp_as_number*/\
	0,              /*tp_as_sequence*/\
	0,              /*tp_as_mapping*/\
	0,              /*tp_hash*/\
	0,              /*tp_call */\
	PyObjectPlus::__str,                /*tp_str */\
	PyObject_GenericGetAttr ,               /*tp_getattro */\
	PyObject_GenericSetAttr,                /*tp_setattro */\
	0,              /*tp_as_buffer*/\
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_CLASS ,      /*tp_flags*/\
	0,              /*tp_doc*/\
	0,              /*tp_traverse*/\
	0,              /*tp_clear*/\
	0,              /*tp_richcompare*/\
	0,              /*tp_weaklistoffset*/\
	0,              /*tp_iter*/\
	0,              /*tp_iternext*/\
	0,              /*tp_methods*/\
	0,              /*tp_members*/\
	0,              /*tp_getset*/\
	0,              /*tp_base*/\
	0,              /*tp_dict*/\
	0,              /*tp_descr_get*/\
	0,              /*tp_descr_set*/\
	0,              /*tp_dictoffset*/\
	0,              /*tp_init*/\
	0,/*tp_alloc*/\
	0/*lass::python::PyObjectPlusPlus< t_cppClass , t_cppParentClass >::__forbidden_new*/,/*tp_new*/\
	0,              /*tp_free*/\
	0,              /*tp_is_gc*/\
	0,              /*tp_bases*/\
	0,              /*tp_mro*/\
	0,              /*tp_cache*/\
	0,              /*tp_subclasses*/\
	0,              /*tp_weaklist*/



#define PY_DECLARE_CLASS_EX( t_cppClass, s_className, i_uniqueClassIdentifier ) \
	PyTypeObject t_cppClass::Type = {\
		PY_STATIC_FUNCTION_FORWARD_PLUS( t_cppClass, ::lass::meta::NullType, s_className ) };\
	std::vector<PyMethodDef> t_cppClass::Methods;\
	std::vector<PyGetSetDef> t_cppClass::GetSetters;\
	std::vector<::lass::python::impl::StaticMember> t_cppClass::Statics;\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE( lassPythonImplExecutePyDeclareClass_, i_uniqueClassIdentifier ),\
		t_cppClass::Methods.push_back( ::lass::python::createPyMethodDef( 0, 0, 0, 0 ) ) ; \
		t_cppClass::GetSetters.push_back( ::lass::python::createPyGetSetDef( 0, 0, 0, 0 ) ) ; \
)

#define PY_DECLARE_CLASS_NAME( i_cppClass, s_className )\
	PY_DECLARE_CLASS_EX( i_cppClass, s_className, i_cppClass )

#define PY_DECLARE_CLASS( i_cppClass ) \
	PY_DECLARE_CLASS_EX( i_cppClass, LASS_STRINGIFY(i_cppClass), i_cppClass );



#define PY_DECLARE_CLASS_PLUS_EX( i_cppClass, s_className, i_uniqueClassIdentifier ) \
	PyTypeObject i_cppClass::Type = {\
		PY_STATIC_FUNCTION_FORWARD_PLUS( i_cppClass::TPyClassSelf, i_cppClass::TPyClassParent, s_className ) };\
	std::vector<PyMethodDef> i_cppClass::Methods;\
	std::vector<PyGetSetDef> i_cppClass::GetSetters;\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE( lassExecutePyDeclareClassPlus_, i_uniqueClassIdentifier ),\
		i_cppClass::Methods.push_back( lass::python::createPyMethodDef( 0, 0, 0, 0 ) ) ; \
		i_cppClass::GetSetters.push_back( lass::python::createPyGetSetDef( 0, 0, 0, 0 ) ) ; \
)

#define PY_DECLARE_CLASS_PLUS_NAME( i_cppClass, s_className ) \
	PY_DECLARE_CLASS_PLUS_EX( i_cppClass, s_className, i_cppClass )

#define PY_DECLARE_CLASS_PLUS( i_cppClass ) \
	PY_DECLARE_CLASS_PLUS_EX( i_cppClass, LASS_STRINGIFY( i_cppClass ) i_cppClass )



#define PY_INJECT_CLASS_IN_MODULE_AT_RUNTIME( t_cppClass, i_module, s_doc ) \
	{\
		::lass::python::impl::injectClassInModule<t_cppClass>(\
			LASS_CONCATENATE(lassPythonModule, i_module), s_doc);\
	}

#define PY_INJECT_CLASS_IN_MODULE( i_cppClass, i_module, s_doc ) \
	extern PyObject* LASS_CONCATENATE( lassPythonModule, i_module );\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyInjectClassInModule_, i_cppClass, i_module ),\
		PY_INJECT_CLASS_IN_MODULE_AT_RUNTIME( i_cppClass, i_module, s_doc );\
	)



#define PY_INJECT_OBJECT_IN_MODULE_AT_RUNTIME( o_object, i_module, s_objectName )\
	{\
		PyModule_AddObject(\
			LASS_CONCATENATE( lassPythonModule, i_module ), s_objectName,\
			::lass::python::pyBuildSimpleObject(o_object) );\
	}



#define PY_CLASS_STATIC_CONST( i_cppClass, s_name, v_value )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyClassStaticConst, i_cppClass ),\
		::lass::python::impl::classStaticConst( i_cppClass::Statics, s_name, v_value );\
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
	LASS_EXECUTE_BEFORE_MAIN_EX(LASS_CONCATENATE(lassPythonImplExecutePyClassInnerClass, i_uniqueClassId),\
		::lass::python::impl::classInnerClass<t_innerCppClass>(t_outerCppClass::Statics, s_name, s_doc);\
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
/**
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
			::lass::python::createPyMethodDef(\
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
 *      void barB(const std::string& b);
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_METHOD_EX(Foo, barA, "bar", 0, foo_bar_a)
 *  PY_CLASS_METHOD_EX(Foo, barB, "bar", 0, foo_bar_b)
 *  @endcode
 */
#define PY_CLASS_METHOD_EX( t_cppClass, i_cppMethod, s_methodName, s_doc, i_dispatcher)\
	static PyCFunction LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) = 0;\
	inline PyObject* i_dispatcher( PyObject* iObject, PyObject* iArgs )\
	{\
		if (LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ))\
		{\
			PyObject* result = LASS_CONCATENATE( pyOverloadChain_, i_dispatcher )(iObject, iArgs);\
			if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError))\
			{\
				PyErr_Clear();\
			}\
			else\
			{\
				return result;\
			}\
		}\
		typedef ::lass::python::impl::ShadowTraits<t_cppClass> TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* const cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return ::lass::python::impl::CallMethod<TCppClass>::call(\
			iArgs, cppObject, &TCppClass::i_cppMethod );\
	}\
	PY_IMPL_SUBSCRIBE_CLASS_METHOD( t_cppClass, s_methodName, s_doc, i_dispatcher )

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_EX with
 *  @a i_dispatcher = py @a i_cppClass @a i_cppMethod __LINE__.
 */
#define PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, s_doc )\
		PY_CLASS_METHOD_EX(\
			i_cppClass, i_cppMethod, s_methodName, s_doc,\
			LASS_UNIQUENAME(LASS_CONCATENATE_3(py, i_cppClass, i_cppMethod)))

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_NAME_DOC with @a s_doc = 0.
 */
#define PY_CLASS_METHOD_NAME( i_cppClass, i_cppMethod, s_methodName )\
		PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, s_methodName, 0 )

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_NAME_DOC with @a s_methodName = "@a i_cppMethod".
 */
#define PY_CLASS_METHOD_DOC( i_cppClass, i_cppMethod, s_doc )\
		PY_CLASS_METHOD_NAME_DOC( i_cppClass, i_cppMethod, LASS_STRINGIFY(i_cppMethod), s_doc)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_NAME_DOC  with @a s_methodName = "@a i_cppMethod"
 *  and @a s_doc = 0.
 */
#define PY_CLASS_METHOD( i_cppClass, i_cppMethod )\
		PY_CLASS_METHOD_DOC( i_cppClass, i_cppMethod, 0 )



// --- explicit qualified methods ------------------------------------------------------------------

/** @ingroup Python
 *  Exports a C++ method to Python will fully qualified return type and parameters
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
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      void bar(int a);
 *      void bar(const std::string& b);
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_METHOD_QUALIFIED_EX(Foo, bar, void, LASS_TYPE_LIST_1(int), "bar", 0, foo_bar_a)
 *  PY_CLASS_METHOD_QUALIFIED_EX(Foo, bar, void, LASS_TYPE_LIST_1(const std::string&), "bar", 0, foo_bar_b)
 *  @endcode
 */
#define PY_CLASS_METHOD_QUALIFIED_EX(t_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc, i_dispatcher)\
	static PyCFunction LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) = 0;\
	inline PyObject* i_dispatcher( PyObject* iObject, PyObject* iArgs )\
	{\
		if (LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ))\
		{\
			PyObject* result = LASS_CONCATENATE( pyOverloadChain_, i_dispatcher )(iObject, iArgs);\
			if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError))\
			{\
				PyErr_Clear();\
			}\
			else\
			{\
				return result;\
			}\
		}\
		typedef ::lass::python::impl::ShadowTraits<t_cppClass> TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return ::lass::python::impl::ExplicitResolver\
		<\
			TCppClass,\
			t_return,\
			t_params\
		>\
		::TImpl::callMethod(iArgs, cppObject, &TCppClass::i_cppMethod);\
	}\
	PY_IMPL_SUBSCRIBE_CLASS_METHOD( t_cppClass, s_methodName, s_doc, i_dispatcher )

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_EX_0( t_cppClass, i_cppMethod, t_return, s_methodName, s_doc, i_dispatcher )\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		t_cppClass, i_cppMethod, t_return, ::lass::meta::NullType, s_methodName, s_doc, i_dispatcher )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_EX_$x( t_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc, i_dispatcher )\
	typedef LASS_TYPE_LIST_$x( $(t_P$x)$ )\
		LASS_UNIQUENAME(LASS_CONCATENATE(pyClassMethodParams_, i_dispatcher));\
	PY_CLASS_METHOD_QUALIFIED_EX(\
		t_cppClass, i_cppMethod, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(pyClassMethodParams_, i_dispatcher), s_methodName, s_doc,\
		i_dispatcher )
]$

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX with
 *  @a i_dispatcher = py @a t_cppClass @a i_cppMethod __LINE__.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc )\
		PY_CLASS_METHOD_QUALIFIED_EX(\
			i_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc,\
			LASS_UNIQUENAME(LASS_CONCATENATE_3(py, i_cppClass, i_cppMethod)))

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0( i_cppClass, i_cppMethod, t_return, t_params, s_methodName, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC(\
		i_cppClass, i_cppMethod, t_return, ::lass::meta::NullType, s_methodName, s_doc )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName, s_doc )\
	typedef LASS_TYPE_LIST_$x( $(t_P$x)$ ) \
		LASS_UNIQUENAME(LASS_CONCATENATE(pyClassMethodParams_, i_cppClass));\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC(\
		i_cppClass, i_cppMethod, t_return,\
		LASS_UNIQUENAME(LASS_CONCATENATE(pyClassMethodParams_, i_cppClass)), s_methodName, s_doc )
]$

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC with @a s_doc = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME( i_cppClass, i_cppMethod, t_return, t_params, s_methodName )\
		PY_CLASS_METHOD_QUALIFIED_NAME_DOC(\
			i_cppClass, i_cppMethod, t_return, t_params, s_methodName, 0 )

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_0( i_cppClass, i_cppMethod, t_return, s_methodName )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC(\
		i_cppClass, i_cppMethod, t_return, ::lass::meta::NullType, s_methodName )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_methodName )\
	typedef LASS_TYPE_LIST_$x( $(t_P$x)$ ) \
		LASS_UNIQUENAME(LASS_CONCATENATE(pyClassMethodParams_, i_cppClass));\
	PY_CLASS_METHOD_QUALIFIED_NAME( \
		i_cppClass, i_cppMethod, t_return, \
		LASS_UNIQUENAME(LASS_CONCATENATE(pyClassMethodParams_, i_cppClass)), s_methodName )
]$

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = "@a i_cppMethod".
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC( i_cppClass, i_cppMethod, t_return, t_params, s_doc )\
		PY_CLASS_METHOD_QUALIFIED_NAME_DOC( \
			i_cppClass, i_cppMethod, t_return, t_params, LASS_STRINGIFY(i_cppMethod), s_doc)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_DOC for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC_0( i_cppClass, i_cppMethod, t_return, s_doc )\
	PY_CLASS_METHOD_QUALIFIED_DOC( \
		i_cppClass, i_cppMethod, t_return, ::lass::meta::NullType, s_doc )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_DOC for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$, s_doc )\
	typedef LASS_TYPE_LIST_$x( $(t_P$x)$ ) \
		LASS_UNIQUENAME(LASS_CONCATENATE(pyClassMethodParams_, i_cppClass));\
	PY_CLASS_METHOD_QUALIFIED_DOC( \
		i_cppClass, i_cppMethod, t_return, \
		LASS_UNIQUENAME(LASS_CONCATENATE(pyClassMethodParams_, i_cppClass)), s_doc )
]$

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC
 *  with @a s_methodName = "@a i_cppMethod" and @a s_doc = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED( i_cppClass, i_cppMethod, t_return, t_params )\
		PY_CLASS_METHOD_QUALIFIED_DOC( i_cppClass, i_cppMethod, t_return, t_params, 0 )

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_DOC for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_0( i_cppClass, i_cppMethod, t_return )\
	PY_CLASS_METHOD_QUALIFIED( i_cppClass, i_cppMethod, t_return, ::lass::meta::NullType )
$[
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_DOC for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_$x( i_cppClass, i_cppMethod, t_return, $(t_P$x)$ )\
	typedef LASS_TYPE_LIST_$x( $(t_P$x)$ ) \
		LASS_UNIQUENAME(LASS_CONCATENATE(pyClassMethodParams_, i_cppClass));\
	PY_CLASS_METHOD_QUALIFIED( \
		i_cppClass, i_cppMethod, t_return, \
		LASS_UNIQUENAME(LASS_CONCATENATE(pyClassMethodParams_, i_cppClass)) )
]$



// --- static methods ------------------------------------------------------------------------------

/** @ingroup Python
 *  Exports a function to Python as a static method (aka class method)
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
#define PY_CLASS_STATIC_METHOD_EX( t_cppClass, f_cppFunction, i_dispatcher, s_methodName, s_doc )\
	inline PyObject* i_dispatcher( PyObject* iIgnore, PyObject* iArgs )\
	{\
		return ::lass::python::impl::callFunction( iArgs, f_cppFunction );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyClassStaticMethod_, i_dispatcher ),\
		t_cppClass::Methods.insert(\
			t_cppClass::Methods.begin(),\
			::lass::python::createPyMethodDef(\
				s_methodName, (PyCFunction) i_dispatcher, METH_VARARGS  | METH_CLASS , s_doc));\
	)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_STATIC_METHOD_EX
 *  with f_cppFunction = @a i_cppClass :: @a i_cppMethod
 */
#define PY_CLASS_STATIC_METHOD_DOC( i_cppClass, i_cppMethod, s_doc )\
	PY_CLASS_STATIC_METHOD_EX(\
		i_cppClass,\
		&::lass::python::impl::ShadowTraits<i_cppClass>::TCppClass::i_cppMethod,\
		LASS_CONCATENATE_3(py, i_cppClass, i_cppMethod), \
		LASS_STRINGIFY(i_cppMethod), s_doc )

/** @ingroup Python
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
 *  PY_CLASS_MEMBER_RW_EX(Foo, "spam", spam, spam, "cool get and setter, breaks on intel compiler though :)")
 *  @endcode
 */
#define PY_CLASS_MEMBER_RW_EX( t_cppClass, s_memberName, i_cppGetter, i_cppSetter, s_doc )\
	inline PyObject* LASS_CONCATENATE_3( pyGetter, t_cppClass, i_cppGetter ) ( PyObject* iObject, void* iClosure)\
	{\
		typedef ::lass::python::impl::ShadowTraits<t_cppClass> TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return ::lass::python::impl::CallMethod<TCppClass>::get(\
			cppObject, &TCppClass::i_cppGetter );\
	}\
	inline int LASS_CONCATENATE_3( pySetter, t_cppClass, i_cppSetter ) ( PyObject* iObject, PyObject* iArgs, void* iClosure )\
	{\
		typedef ::lass::python::impl::ShadowTraits<t_cppClass> TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return ::lass::python::impl::CallMethod<TCppClass>::set(\
			iArgs, cppObject, &TCppClass::i_cppSetter );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyClassMemberRW_, t_cppClass, i_cppGetter ),\
		t_cppClass::GetSetters.insert(\
			t_cppClass::GetSetters.begin(),\
			::lass::python::createPyGetSetDef(\
				s_memberName, LASS_CONCATENATE_3( pyGetter, t_cppClass, i_cppGetter ), \
				LASS_CONCATENATE_3( pySetter, t_cppClass, i_cppSetter ), s_doc, 0));\
	)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_MEMBER_RW_EX
 */
#define PY_CLASS_MEMBER_RW_DOC( t_cppClass, s_memberName, i_cppGetter, i_cppSetter, s_doc )\
	PY_CLASS_MEMBER_RW_EX( t_cppClass, s_memberName, i_cppGetter, i_cppSetter, s_doc )

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_MEMBER_RW_EX
 */
#define PY_CLASS_MEMBER_RW( t_cppClass, s_memberName, i_cppGetter, i_cppSetter )\
	PY_CLASS_MEMBER_RW_EX( t_cppClass, s_memberName, i_cppGetter, i_cppSetter, 0 )



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
#define PY_CLASS_MEMBER_R_EX( t_cppClass, s_memberName, i_cppGetter, s_doc )\
	inline PyObject* LASS_CONCATENATE_3( pyGetter, t_cppClass, i_cppGetter ) ( PyObject* iObject, void* iClosure)\
	{\
		typedef ::lass::python::impl::ShadowTraits<t_cppClass> TShadowTraits;\
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
	( LASS_CONCATENATE_3( lassExecutePyClassMemberR_, t_cppClass, i_cppGetter ),\
		t_cppClass::GetSetters.insert(\
			t_cppClass::GetSetters.begin(),\
			::lass::python::createPyGetSetDef(\
				s_memberName, LASS_CONCATENATE_3( pyGetter, t_cppClass, i_cppGetter ), 0, \
				s_doc, 0));\
	)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_MEMBER_R_EX
 */
#define PY_CLASS_MEMBER_R_DOC( t_cppClass, s_memberName, i_cppGetter , s_doc)\
	PY_CLASS_MEMBER_R_EX( t_cppClass, s_memberName, i_cppGetter , s_doc)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_MEMBER_R_EX
 */
#define PY_CLASS_MEMBER_R( t_cppClass, s_memberName, i_cppGetter )\
	PY_CLASS_MEMBER_R_EX( t_cppClass, s_memberName, i_cppGetter, 0 )



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
#define PY_CLASS_PUBLIC_MEMBER_EX( i_cppClass, i_cppMember, s_memberName, s_doc )\
	inline PyObject* LASS_CONCATENATE_3( pyPublicGetter, i_cppClass, i_cppMember ) ( PyObject* iObject, void* iClosure)\
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
	inline int LASS_CONCATENATE_3( pyPublicSetter, i_cppClass, i_cppMember ) ( PyObject* iObject,PyObject* iArgs, void* iClosure )\
	{\
		typedef ::lass::python::impl::ShadowTraits<i_cppClass> TShadowTraits;\
		typedef TShadowTraits::TCppClass TCppClass;\
		TCppClass* cppObject = TShadowTraits::cppObject(iObject);\
		if (!cppObject)\
		{\
			return 0;\
		}\
		return ::lass::python::pyGetSimpleObject( iArgs, cppObject->i_cppMember );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePyClassPublicMember_, i_cppClass, i_cppMember ),\
		i_cppClass::GetSetters.insert(\
			i_cppClass::GetSetters.begin(),\
			::lass::python::createPyGetSetDef(\
				s_memberName, LASS_CONCATENATE_3( pyPublicGetter, i_cppClass, i_cppMember ),\
				LASS_CONCATENATE_3( pyPublicSetter, i_cppClass, i_cppMember ), s_doc, 0));\
	)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_PUBLIC_MEMBER_EX
 */
#define PY_CLASS_PUBLIC_MEMBER_DOC( i_cppClass, i_cppMember , s_doc)\
	PY_CLASS_PUBLIC_MEMBER_EX(\
		i_cppClass, i_cppMember, LASS_STRINGIFY( i_cppMember ),  s_doc )

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_PUBLIC_MEMBER_EX
 */
#define PY_CLASS_PUBLIC_MEMBER( i_cppClass, i_cppMember )\
	PY_CLASS_PUBLIC_MEMBER_DOC( i_cppClass, i_cppMember, 0 )



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
 *  PY_CLASS_CONSTRUCTOR(Foo, meta::NullType) // constructor without arguments.
 *  typedef meta::type_list::Make<int, std::string>::Type TArguments;
 *  PY_CLASS_CONSTRUCTOR(Foo, TArguments) // constructor with some arguments. *
 *  @endcode
 */
#define PY_CLASS_CONSTRUCTOR_EX( t_cppClass, t_params, i_dispatcher )\
	static newfunc LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) = 0;\
	PyObject* i_dispatcher(PyTypeObject *iSubtype, PyObject *iArgs, PyObject *iKwds)\
	{\
		if (LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ))\
		{\
			PyObject* result = LASS_CONCATENATE( pyOverloadChain_, i_dispatcher )(\
				iSubtype, iArgs, iKwds);\
			if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_TypeError))\
			{\
				PyErr_Clear();\
			}\
			else\
			{\
				return result;\
			}\
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
		LASS_CONCATENATE(lassExecutePyClassConstructor_, i_dispatcher),\
		LASS_CONCATENATE(pyOverloadChain_, i_dispatcher) = t_cppClass::Type.tp_new;\
		t_cppClass::Type.tp_new = i_dispatcher; \
	)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_CONSTRUCTOR_EX with
 *  @a i_dispatcher = pyConstruct @a t_cppClass __LINE__
 */
#define PY_CLASS_CONSTRUCTOR( t_cppClass, t_params )\
	PY_CLASS_CONSTRUCTOR_EX(\
		t_cppClass, t_params, LASS_UNIQUENAME(LASS_CONCATENATE(pyConstruct_, t_cppClass)))

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
		LASS_UNIQUENAME(LASS_CONCATENATE(pyConstructorArguments, t_cppClass));\
	PY_CLASS_CONSTRUCTOR(\
		t_cppClass, LASS_UNIQUENAME(LASS_CONCATENATE(pyConstructorArguments, t_cppClass)))
]$


// --- implementation macros -----------------------------------------------------------------------

#define PY_IMPL_SUBSCRIBE_CLASS_METHOD( t_cppClass, s_methodName, s_doc, i_dispatcher )\
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE_3( lassExecutePySubscribeClassMethod_, t_cppClass, i_dispatcher ),\
		::std::vector<PyMethodDef>::iterator i = ::std::find_if(\
			t_cppClass::Methods.begin(), t_cppClass::Methods.end(),\
			::lass::python::impl::PyMethodEqual(s_methodName));\
		if (i == t_cppClass::Methods.end())\
		{\
			t_cppClass::Methods.insert(\
				t_cppClass::Methods.begin(),\
				::lass::python::createPyMethodDef(\
					s_methodName, (PyCFunction) i_dispatcher, METH_VARARGS , s_doc));\
		}\
		else\
		{\
			LASS_CONCATENATE( pyOverloadChain_, i_dispatcher ) = i->ml_meth;\
			i->ml_meth = i_dispatcher;\
		};\
	)

namespace lass
{
namespace python
{
namespace impl
{

struct StaticMember
{
	PyObject* object;
	PyTypeObject* parentType;
	std::vector<PyMethodDef>* methods;
	std::vector<PyGetSetDef>* getSetters;
	const std::vector<StaticMember>* statics;
	const char* name;
	const char* doc;
};

// currently, i'm implementing these functions inline in this header, just to see where we get with them.
// But they should probably move to some .cpp or .inl (for templates) file. [BdG]

void finalizePyType(PyTypeObject& iPyType, PyTypeObject& iPyParentType, 
					std::vector<PyMethodDef>& iMethods,
					std::vector<PyGetSetDef>& iGetSetters, 
					const std::vector<StaticMember>& iStatics, 
					const char* iModuleName, const char* iDocumentation);

inline void injectStaticMembers(PyTypeObject& iPyType, const std::vector<StaticMember>& iStatics)
{
	for (std::vector<StaticMember>::const_iterator i = iStatics.begin(); i != iStatics.end(); ++i)
	{
		if (PyType_Check(i->object))
		{
			finalizePyType(*reinterpret_cast<PyTypeObject*>(i->object), *i->parentType, 
				*i->methods, *i->getSetters, *i->statics, iPyType.tp_name, i->doc);
		}
		PyDict_SetItemString(iPyType.tp_dict, i->name, i->object);
	}
}

inline void finalizePyType(PyTypeObject& iPyType, PyTypeObject& iPyParentType, 
						   std::vector<PyMethodDef>& iMethods,
						   std::vector<PyGetSetDef>& iGetSetters, 
						   const std::vector<StaticMember>& iStatics, 
						   const char* iModuleName, const char* iDocumentation)
{
	std::string fullName = std::string(iModuleName) + std::string(".") + std::string(iPyType.tp_name);
	char* fullNameCharPtr = new char[fullName.size()+1];
	strcpy(fullNameCharPtr,fullName.c_str());

	iPyType.tp_name = fullNameCharPtr;
	iPyType.tp_methods = &iMethods[0];
	iPyType.tp_getset = &iGetSetters[0];
	iPyType.tp_doc = const_cast<char*>(iDocumentation);
	iPyType.tp_base = &iPyParentType;
	LASS_ENFORCE( PyType_Ready( &iPyType ) >= 0 );
	Py_INCREF( &iPyType );

	injectStaticMembers(iPyType, iStatics);
}

template <typename CppClass>
inline void injectClassInModule(PyObject* iModule, const char* iClassDocumentation)
{
	char* shortName = CppClass::Type.tp_name; // finalizePyType will expand tp_name with module name.
	finalizePyType(CppClass::Type, *CppClass::GetParentType(), CppClass::Methods, CppClass::GetSetters,
		CppClass::Statics, PyModule_GetName(iModule), iClassDocumentation);
	PyModule_AddObject(iModule, shortName, reinterpret_cast<PyObject*>(&CppClass::Type));
}

template<typename T>
inline void classStaticConst(std::vector<StaticMember>& iStatics, const char* iName, const T& iValue)
{
	StaticMember temp;
	temp.name = iName;
	temp.object = pyBuildSimpleObject(iValue);
	temp.parentType = 0;
	temp.methods = 0;
	temp.getSetters = 0;
	temp.statics = 0;
	temp.doc = 0;
	iStatics.push_back(temp);
}

template <typename CppClass>
inline void classInnerClass(std::vector<StaticMember>& iOuterStatics, const char* iInnerClassName, 
							const char* iDocumentation)
{
	StaticMember temp;
	temp.name = iInnerClassName;
	temp.object = reinterpret_cast<PyObject*>(&CppClass::Type);
	temp.parentType = CppClass::GetParentType();
	temp.methods = &CppClass::Methods;
	temp.getSetters = &CppClass::GetSetters;
	temp.statics = &CppClass::Statics;
	temp.doc = iDocumentation;
	iOuterStatics.push_back(temp);
}

}
}
}

#endif

// EOF
