/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define LASS_PYTHON_ERR_MSG_NO_NEW_OPERATOR "Can not create object"


#define LASS_PYTHON_ERR_MSG_ARG_NOT_BOOL  "not bool"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_INT  "not int"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_LONG  "not long"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_FLOAT  "not float"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_DOUBLE  "not double"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_STRING  "not string"


/* This macro is only used in the PyObjectPlus class and is for internal LASS
*  use.  Do not use in custom objects... DON'T! 
*/
#define PY_HEADER_INTERNAL \
	public: \
		static char*		  PythonClassName;\
		static char*		  PythonModuleName;\
		static PyTypeObject   Type; \
		static std::vector<PyMethodDef>    Methods; \
		static std::vector<PyGetSetDef>    GetSetters; \
		virtual PyTypeObject *GetType(void) {return &Type;}; \
		/*static PyObject* __forbidden_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) \
		{ PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_NO_NEW_OPERATOR); return NULL;}*/

/* Place as first line of your Pythonized class.  For ParentClass use the 
*  C++ class from which you wish the python object inherits.  ParentClass
*  must also be a Pythonized class or use lass::python::PyObjectPlus as default.
*  @remark Any declarations coming after this macro are public!
*/
#define PY_HEADER( ParentClass ) \
	public: \
		static char* PythonClassName;\
		static char* PythonModuleName;\
		static PyTypeObject   Type; \
		static std::vector<PyMethodDef>    Methods; \
		static std::vector<PyGetSetDef>    GetSetters; \
		static PyTypeObject* GetParentType(void) { return &ParentClass::Type; }\
		virtual PyTypeObject *GetType(void) {return &Type;}; \
		/*static PyObject* __forbidden_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) \
		{ PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_NO_NEW_OPERATOR); return NULL;}*/

/** PY_PYTHONIZE.  Use this macro to make objects derived from PyObjectPlus have 
*   a type in Python too.  If you don't use this macro you can still derive from the PyObjectPlus
*	parent class with the type as argument for the constructor.  For single inheritance this last
*	method is preferred.  For classes with a multitude of constructors the PYTHONIZE macro can 
*	be considered.
*/
#define PY_PYTHONIZE \
	{this->ob_type = &this->Type;\
	_Py_NewReference( this );}


#define PY_DECLARE_MODULE( module__ ) \
	PyObject* LASS_CONCATENATE( lassPythonModule, module__ ) = NULL;\
	std::vector<PyMethodDef> LASS_CONCATENATE_3( lassPythonModule, Methods, module__ ); \
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE( lassExecutePyDeclareModule_, module__ ), \
		LASS_CONCATENATE_3( lassPythonModule, Methods, module__ ).push_back( \
			lass::python::createPyMethodDef( NULL, NULL, NULL, NULL ) ) ; )

#define PY_INJECT_MODULE_AT_RUNTIME( module__, doc__ ) \
	{ \
		Py_Initialize(); \
		LASS_CONCATENATE( lassPythonModule, module__ ) = Py_InitModule3(\
			const_cast<char*>( LASS_STRINGIFY(module__) ), \
			&LASS_CONCATENATE_3( lassPythonModule, Methods, module__ )[0], doc__ ); \
	}

#define PY_INJECT_MODULE( module__, doc__ ) \
	extern "C" { \
		void LASS_CONCATENATE( inject, module__ ) () \
		{ \
			PY_INJECT_MODULE_AT_RUNTIME( module__, doc__ )\
		}\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE( lassExecutePyInjectModule_, module__ ), \
		LASS_CONCATENATE( inject, module__ ) (); )


#define PY_INJECT_MODULE_EX_AT_RUNTIME( module__, moduleName__, documentation__ ) \
	{\
		LASS_CONCATENATE( lassPythonModule, module__ ) = Py_InitModule3(\
			moduleName__, \
			&LASS_CONCATENATE_3( lassPythonModule, Methods, module__ )[0], documentation__ ); \
	}

#define PY_INJECT_CLASS_IN_MODULE( CppClass__, module__, documentation__ ) \
	extern PyObject* LASS_CONCATENATE( lassPythonModule, module__ );\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyInjectClassInModule_, CppClass__, module__ ),\
		PY_INJECT_CLASS_IN_MODULE_AT_RUNTIME( CppClass__, module__, documentation__ ) )


/* Use this macro for backward compatibility when wrapper functions don't
*  need to be automatically generated or you want specific Python behaviour. 
*/
#define PY_MODULE_PY_FUNCTION_EX( module__, pyFunction__, pyName__, pyDoc__ )\
	extern std::vector< PyMethodDef > LASS_CONCATENATE_3( lassPythonModule, Methods, module__ );\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyModulePyFunction_, module__, pyFunction__ ),\
		LASS_CONCATENATE_3( lassPythonModule, Methods , module__ ).insert(\
			LASS_CONCATENATE_3( lassPythonModule, Methods , module__ ).begin(),\
			lass::python::createPyMethodDef( pyName__, pyFunction__ , METH_VARARGS  , pyDoc__ ) ) ; )



#define PY_MODULE_FUNCTION_EX( module__, function__, pyFunction__, pyName__, pyDoc__ )\
	inline PyObject* pyFunction__( PyObject* iIgnore, PyObject* iArgs )\
	{\
		return lass::python::impl::pyCallFunction( iArgs, function__ );\
	}\
	extern std::vector< PyMethodDef > LASS_CONCATENATE_3( lassPythonModule, Methods, module__ );\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyModuleFunction_, module__, pyFunction__ ),\
		LASS_CONCATENATE_3( lassPythonModule, Methods, module__ ).insert(\
			LASS_CONCATENATE_3( lassPythonModule, Methods, module__ ).begin(),\
			lass::python::createPyMethodDef( pyName__, pyFunction__, METH_VARARGS, pyDoc__ ) ) ; )

#define PY_MODULE_FUNCTION_DOC( module__, function__, pyDoc__ )\
	PY_MODULE_FUNCTION_EX( module__, function__, LASS_CONCATENATE(py, function__), LASS_STRINGIFY(function__), pyDoc__ )


#define PY_MODULE_FUNCTION( module__, function__ )\
	PY_MODULE_FUNCTION_DOC( module__, function__, NULL )


#define PY_STATIC_FUNCTION_FORWARD( CppClass__, pyclass__ )	\
	PyObject_HEAD_INIT(&PyType_Type)\
	0,		\
	(char*)( pyclass__ ),	\
	sizeof( CppClass__ ),			\
	0,							\
	\
	PyObjectPlus::__dealloc,	\
	0,							\
	0,						/*tp_getattr*/\
	0,						/*tp_setattr*/\
	0,							\
	0,		\
	0,				/*tp_as_number*/\
	0,				/*tp_as_sequence*/\
	0,				/*tp_as_mapping*/\
	0,				/*tp_hash*/\
	0,				/*tp_call */\
	0,				/*tp_str */\
	PyObject_GenericGetAttr ,				/*tp_getattro */\
	PyObject_GenericSetAttr,				/*tp_setattro */\
	0,				/*tp_as_buffer*/\
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_CLASS ,		/*tp_flags*/\
	0,				/*tp_doc*/\
	0,				/*tp_traverse*/\
	0,				/*tp_clear*/\
	0,				/*tp_richcompare*/\
	0,				/*tp_weaklistoffset*/\
	0,				/*tp_iter*/\
	0,				/*tp_iternext*/\
	0,				/*tp_methods*/\
	0,				/*tp_members*/\
	0,				/*tp_getset*/\
	0,				/*tp_base*/\
	0,				/*tp_dict*/\
	0,				/*tp_descr_get*/\
	0,				/*tp_descr_set*/\
	0,				/*tp_dictoffset*/\
	0,				/*tp_init*/\
	0,/*tp_alloc*/\
	0/*CppClass__ ::__forbidden_new*/,/*tp_new*/\
	0,				/*tp_free*/\
	0,				/*tp_is_gc*/\
	0,				/*tp_bases*/\
	0,				/*tp_mro*/\
	0,				/*tp_cache*/\
	0,				/*tp_subclasses*/\
	0,				/*tp_weaklist*/   

#define PY_STATIC_FUNCTION_FORWARD_PLUS( CppClass__, parentclass__, pyclass__ )	\
	PyObject_HEAD_INIT(&PyType_Type)\
	0,		\
	(char*)( pyclass__ ),	\
	sizeof( CppClass__ ),			\
	0,							\
	\
	PyObjectPlus::__dealloc,	\
	0,							\
	0,						/*tp_getattr*/\
	0,						/*tp_setattr*/\
	0,							\
	PyObjectPlus::__repr,		\
	0,				/*tp_as_number*/\
	0,				/*tp_as_sequence*/\
	0,				/*tp_as_mapping*/\
	0,				/*tp_hash*/\
	0,				/*tp_call */\
	PyObjectPlus::__str,				/*tp_str */\
	PyObject_GenericGetAttr ,				/*tp_getattro */\
	PyObject_GenericSetAttr,				/*tp_setattro */\
	0,				/*tp_as_buffer*/\
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_CLASS ,		/*tp_flags*/\
	0,				/*tp_doc*/\
	0,				/*tp_traverse*/\
	0,				/*tp_clear*/\
	0,				/*tp_richcompare*/\
	0,				/*tp_weaklistoffset*/\
	0,				/*tp_iter*/\
	0,				/*tp_iternext*/\
	0,				/*tp_methods*/\
	0,				/*tp_members*/\
	0,				/*tp_getset*/\
	0,				/*tp_base*/\
	0,				/*tp_dict*/\
	0,				/*tp_descr_get*/\
	0,				/*tp_descr_set*/\
	0,				/*tp_dictoffset*/\
	0,				/*tp_init*/\
	0,/*tp_alloc*/\
	0/*lass::python::PyObjectPlusPlus< CppClass__ , parentclass__ >::__forbidden_new*/,/*tp_new*/\
	0,				/*tp_free*/\
	0,				/*tp_is_gc*/\
	0,				/*tp_bases*/\
	0,				/*tp_mro*/\
	0,				/*tp_cache*/\
	0,				/*tp_subclasses*/\
	0,				/*tp_weaklist*/   



#define PY_DECLARE_CLASS_EX( CppClass__, pyclass__ ) \
	char* CppClass__::PythonClassName = (char*) ( pyclass__ ) ;\
	char* CppClass__::PythonModuleName = (char*) "!no assigned module!" ;\
	PyTypeObject CppClass__::Type = {\
	PY_STATIC_FUNCTION_FORWARD_PLUS( CppClass__, CppClass__, pyclass__ ) };\
	std::vector<PyMethodDef> CppClass__::Methods;\
	std::vector<PyGetSetDef> CppClass__::GetSetters;\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE( lassExecutePyDeclareClass_, CppClass__ ),\
		CppClass__::Methods.push_back( lass::python::createPyMethodDef( NULL, NULL, NULL, NULL ) ) ; \
		CppClass__::GetSetters.push_back( lass::python::createPyGetSetDef( NULL, NULL, NULL, NULL ) ) ; \
)

#define PY_DECLARE_CLASS( CppClass__ ) \
	PY_DECLARE_CLASS_EX( CppClass__, LASS_STRINGIFY( CppClass__) );



#define PY_DECLARE_CLASS_PLUS_EX( CppClass__, pyclass__ ) \
	char* CppClass__::PythonClassName = (char*) ( pyclass__ ) ;\
	char* CppClass__::PythonModuleName = (char*) "!no assigned module!" ;\
	PyTypeObject CppClass__::Type = {\
	PY_STATIC_FUNCTION_FORWARD_PLUS( CppClass__::TPyClassSelf, CppClass__::TPyClassParent, pyclass__ ) };\
	std::vector<PyMethodDef> CppClass__::Methods;\
	std::vector<PyGetSetDef> CppClass__::GetSetters;\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE( lassExecutePyDeclareClassPlus_, CppClass__ ),\
		CppClass__::Methods.push_back( lass::python::createPyMethodDef( NULL, NULL, NULL, NULL ) ) ; \
		CppClass__::GetSetters.push_back( lass::python::createPyGetSetDef( NULL, NULL, NULL, NULL ) ) ; \
)

#define PY_DECLARE_CLASS_PLUS( CppClass__ ) \
	PY_DECLARE_CLASS_PLUS_EX( CppClass__, LASS_STRINGIFY( CppClass__ ) )



#define PY_INJECT_CLASS_IN_MODULE_AT_RUNTIME( CppClass__, module__, documentation__ ) \
	{\
		CppClass__::Type.tp_methods = &CppClass__::Methods[0];\
		CppClass__::Type.tp_getset = &CppClass__::GetSetters[0];\
		CppClass__::Type.tp_doc = documentation__;\
		CppClass__::Type.tp_base = CppClass__::GetParentType();\
		CppClass__::PythonModuleName = LASS_STRINGIFY( module__ );\
		Py_INCREF(& CppClass__::Type );\
		PyModule_AddObject( LASS_CONCATENATE( lassPythonModule, module__ ), \
			const_cast<char*>( CppClass__::PythonClassName ) , (PyObject *)& CppClass__::Type); \
	}

#define PY_INJECT_CLASS_IN_MODULE( CppClass__, module__, documentation__ ) \
	extern PyObject* LASS_CONCATENATE( lassPythonModule, module__ );\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyInjectClassInModule_, CppClass__, module__ ),\
		PY_INJECT_CLASS_IN_MODULE_AT_RUNTIME( CppClass__, module__, documentation__ ) )



#define PY_INJECT_OBJECT_IN_MODULE_AT_RUNTIME( object__, module__, name__ )\
	{ PyModule_AddObject( LASS_CONCATENATE( lassPythonModule, module__ ), name__, lass::python::pyBuildSimpleObject(object__) ); }

// --- methods -------------------------------------------------------------------------------------

#define PY_CLASS_PY_METHOD_EX( CppClass__, pyFunction__, pyName__, pyDoc__  )\
	inline PyObject* LASS_CONCATENATE( staticDispatch, pyFunction__) ( PyObject* iObject, PyObject* iArgs )\
	{\
		CppClass__* object = static_cast<CppClass__*>(iObject);\
		return object->pyFunction__( iArgs );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyClassPyMethod_, CppClass__, pyFunction__ ),\
		CppClass__::Methods.insert(CppClass__::Methods.begin(),\
			lass::python::createPyMethodDef(pyName__, (PyCFunction) LASS_CONCATENATE(staticDispatch, pyFunction__), METH_VARARGS , pyDoc__)); )
	

// --- methods -------------------------------------------------------------------------------------

/** @ingroup Python
 *	Exports a C++ method to Python
 *
 *  @param CppClass__ 
 *		the C++ class you're exporting a method of
 *  @param cppMethod__ 
 *		the name of the method in C++
 *  @param pyMethod__ 
 *		the name the method will have in Python
 *  @param pyDoc__ 
 *		documentation of method as shown in Python (zero terminated C string)
 *  @param dispatcher__ 
 *		A unique name of the static C++ dispatcher function to be generated.  This name will be
 *		used for the names of automatic generated variables and functions and should be unique
 *		per exported C++ class/method pair.
 *
 *  Invoke this macro to export a method to python.  You can use this to generate overloaded
 *  Python methods by exporting multiple methods with the same @a pyMethod__ name.  
 *
 *  @note 
 *		unlike in C++ overload issues will be not be solved by best fit, but by first fit.
 *		If such an overloaded Python method is called, the different overloads are called in
 *		the same order of export.  The first one that fits the arguments will be called.
 *
 *	@note
 *		the documentation of the Python method will be the @a pyDoc__ of the first exported
 *		overload.
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      void barA(int a);
 *		void barB(const std::string& b);
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_METHOD_EX(Foo, barA, "bar", 0, foo_bar_a)
 *  PY_CLASS_METHOD_EX(Foo, barB, "bar", 0, foo_bar_b)
 *  @endcode
 */
#define PY_CLASS_METHOD_EX( CppClass__, cppMethod__, pyMethod__, pyDoc__, dispatcher__)\
	static PyCFunction LASS_CONCATENATE( pyOverloadChain_, dispatcher__ ) = 0;\
	template <int isShadow>\
	struct LASS_CONCATENATE( PyShadowResolver_, dispatcher__ )\
	{\
		template <typename Obj>\
		static PyObject* call( Obj* iObject, PyObject* iArgs )\
		{\
			typedef typename Obj::RealObject TRealObject;\
			return lass::python::impl::PyCallMethod<TRealObject>::call(\
				iArgs, iObject->value_, TRealObject::cppMethod__ );\
		}\
	};\
	template <>\
	struct LASS_CONCATENATE( PyShadowResolver_, dispatcher__ )<false>\
	{\
		template <typename Obj>\
		static PyObject* call( Obj* iObject, PyObject* iArgs )\
		{\
			return lass::python::impl::PyCallMethod<Obj>::call(	iArgs, iObject, Obj::cppMethod__ );\
		}\
	};\
	PY_IMPL_DISPATCHER_CLASS_METHOD( CppClass__, dispatcher__ )\
	PY_IMPL_SUBSCRIBE_CLASS_METHOD( CppClass__, pyMethod__, pyDoc__, dispatcher__ )

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_EX with 
 *  @a dispatcher__ = py @a CppClass__ @a cppMethod__ __LINE__.
 */
#define PY_CLASS_METHOD_NAME_DOC( CppClass__, cppMethod__, pyMethod__, pyDoc__ )\
		PY_CLASS_METHOD_EX( CppClass__, cppMethod__, pyMethod__, pyDoc__,\
							LASS_UNIQUENAME(LASS_CONCATENATE_3(py, CppClass__, cppMethod__)))

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_NAME_DOC with @a pyDoc__ = 0.
 */
#define PY_CLASS_METHOD_NAME( CppClass__, cppMethod__, pyMethod__ )\
		PY_CLASS_METHOD_NAME_DOC( CppClass__, cppMethod__, pyMethod__, 0 )

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_NAME_DOC with @a pyMethod__ = "@a cppMethod__".
 */
#define PY_CLASS_METHOD_DOC( CppClass__, cppMethod__, pyDoc__ )\
		PY_CLASS_METHOD_NAME_DOC( CppClass__, cppMethod__, LASS_STRINGIFY(cppMethod__), pyDoc__)

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_NAME_DOC  with @a pyMethod__ = "@a cppMethod__" 
 *  and @a pyDoc__ = 0.
 */
#define PY_CLASS_METHOD( CppClass__, cppMethod__ )\
		PY_CLASS_METHOD_DOC( CppClass__, cppMethod__, NULL )



// --- explicit qualified methods ------------------------------------------------------------------

/** @ingroup Python
 *	Exports a C++ method to Python will fully qualified return type and parameters
 *
 *  @param CppClass__ 
 *		the C++ class you're exporting a method of
 *  @param cppMethod__ 
 *		the name of the method in C++
 *	@param R__
 *		the return type of @a cppMethod__
 *	@param PList__
 *		a lass::meta::TypeList of the parameter types of @a cppMethod__
 *  @param pyMethod__ 
 *		the name the method will have in Python
 *  @param pyDoc__ 
 *		documentation of method as shown in Python (zero terminated C string)
 *  @param dispatcher__ 
 *		A unique name of the static C++ dispatcher function to be generated.  This name will be
 *		used for the names of automatic generated variables and functions and should be unique
 *		per exported C++ class/method pair.
 *
 *  You can use this macro instead of PY_CLASS_METHOD_EX if there's an ambiguity on @a cppMethod__.
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
 *		void bar(const std::string& b);
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_METHOD_QUALIFIED_EX(Foo, bar, void, LASS_TYPE_LIST_1(int), "bar", 0, foo_bar_a)
 *  PY_CLASS_METHOD_QUALIFIED_EX(Foo, bar, void, LASS_TYPE_LIST_1(const std::string&), "bar", 0, foo_bar_b)
 *  @endcode
 */
#define PY_CLASS_METHOD_QUALIFIED_EX(CppClass__, cppMethod__, R__, PList__, pyMethod__, pyDoc__, dispatcher__)\
	static PyCFunction LASS_CONCATENATE( pyOverloadChain_, dispatcher__ ) = 0;\
	template <int isShadow>\
	struct LASS_CONCATENATE( PyShadowResolver_, dispatcher__ )\
	{\
		template <typename Obj>\
		static PyObject* call( Obj* iObject, PyObject* iArgs )\
		{\
			typedef typename Obj::RealObject TRealObject;\
			return lass::python::impl::PyExplicitResolver\
			<\
				TRealObject,\
				R__,\
				PList__\
			>\
			::TImpl::callMethod(iArgs, iObject->value, TRealObject::cppMethod__);\
		}\
	};\
	template <>\
	struct LASS_CONCATENATE( PyShadowResolver_, dispatcher__ )<false>\
	{\
		template <typename Obj>\
		static PyObject* call( Obj* iObject, PyObject* iArgs )\
		{\
			return lass::python::impl::PyExplicitResolver\
			<\
				Obj,\
				R__,\
				PList__\
			>\
			::TImpl::callMethod(iArgs, iObject, Obj::cppMethod__);\
		}\
	};\
	PY_IMPL_DISPATCHER_CLASS_METHOD( CppClass__, dispatcher__ )\
	PY_IMPL_SUBSCRIBE_CLASS_METHOD( CppClass__, pyMethod__, pyDoc__, dispatcher__ )

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_EX_0( CppClass__, cppMethod__, R__, pyMethod__, pyDoc__, dispatcher__ )\
	PY_CLASS_METHOD_QUALIFIED_EX( CppClass__, cppMethod__, R__, lass::meta::NullType, pyMethod__, pyDoc__, dispatcher__ )
$[
/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_EX_$x( CppClass__, cppMethod__, R__, $(P$x__)$, pyMethod__, pyDoc__, dispatcher__ )\
    typedef LASS_TYPE_LIST_$x( $(P$x__)$ ) LASS_UNIQUENAME(LASS_CONCATENATE(pyMethodArguments, CppClass__));\
    PY_CLASS_METHOD_QUALIFIED_EX( CppClass__, cppMethod__, R__, LASS_UNIQUENAME(LASS_CONCATENATE(pyMethodArguments, CppClass__), pyMethod__, pyDoc__, dispatcher__ )
]$

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_EX with 
 *	@a dispatcher__ = py @a CppClass__ @a cppMethod__ __LINE__.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC( CppClass__, cppMethod__, R__, PList__, pyMethod__, pyDoc__ )\
		PY_CLASS_METHOD_QUALIFIED_EX( CppClass__, cppMethod__, R__, PList__, pyMethod__, pyDoc__,\
									  LASS_UNIQUENAME(LASS_CONCATENATE_3(py, CppClass__, cppMethod__)))

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC_0( CppClass__, cppMethod__, R__, PList__, pyMethod__, pyDoc__ )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC( CppClass__, cppMethod__, R__, lass::meta::NullType, pyMethod__, pyDoc__ )
$[
/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_DOC_$x( CppClass__, cppMethod__, R__, $(P$x__)$, pyMethod__, pyDoc__ )\
    typedef LASS_TYPE_LIST_$x( $(P$x__)$ ) LASS_UNIQUENAME(LASS_CONCATENATE(pyMethodArguments, CppClass__));\
    PY_CLASS_METHOD_QUALIFIED_NAME_DOC( CppClass__, cppMethod__, R__, LASS_UNIQUENAME(LASS_CONCATENATE(pyMethodArguments, CppClass__)), pyMethod__, pyDoc__ )
]$

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC with @a pyDoc__ = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME( CppClass__, cppMethod__, R__, PList__, pyMethod__ )\
		PY_CLASS_METHOD_QUALIFIED_NAME_DOC( CppClass__, cppMethod__, R__, PList__, pyMethod__, 0 )

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_0( CppClass__, cppMethod__, R__, pyMethod__ )\
	PY_CLASS_METHOD_QUALIFIED_NAME_DOC( CppClass__, cppMethod__, R__, lass::meta::NullType, pyMethod__ )
$[
/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_NAME_$x( CppClass__, cppMethod__, R__, $(P$x__)$, pyMethod__ )\
    typedef LASS_TYPE_LIST_$x( $(P$x__)$ ) LASS_UNIQUENAME(LASS_CONCATENATE(pyMethodArguments, CppClass__));\
    PY_CLASS_METHOD_QUALIFIED_NAME( CppClass__, cppMethod__, R__, LASS_UNIQUENAME(LASS_CONCATENATE(pyMethodArguments, CppClass__)), pyMethod__ )
]$

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC 
 *	with @a pyMethod__ = "@a cppMethod__".
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC( CppClass__, cppMethod__, R__, PList__, pyDoc__ )\
		PY_CLASS_METHOD_QUALIFIED_NAME_DOC( CppClass__, cppMethod__, R__, PList__,\
											LASS_STRINGIFY(cppMethod__), pyDoc__)

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_DOC for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC_0( CppClass__, cppMethod__, R__, pyDoc__ )\
	PY_CLASS_METHOD_QUALIFIED_DOC( CppClass__, cppMethod__, R__, lass::meta::NullType, pyDoc__ )
$[
/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_DOC for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_DOC_$x( CppClass__, cppMethod__, R__, $(P$x__)$, pyDoc__ )\
    typedef LASS_TYPE_LIST_$x( $(P$x__)$ ) LASS_UNIQUENAME(LASS_CONCATENATE(pyMethodArguments, CppClass__));\
    PY_CLASS_METHOD_QUALIFIED_DOC( CppClass__, cppMethod__, R__, LASS_UNIQUENAME(LASS_CONCATENATE(pyMethodArguments, CppClass__)), pyDoc__ )
]$

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_NAME_DOC
 *	with @a pyMethod__ = "@a cppMethod__" and @a pyDoc__ = 0.
 */
#define PY_CLASS_METHOD_QUALIFIED( CppClass__, cppMethod__, R__, PList__ )\
		PY_CLASS_METHOD_QUALIFIED_DOC( CppClass__, cppMethod__, R__, PList__, NULL )

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_DOC for 0 arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_0( CppClass__, cppMethod__, R__ )\
	PY_CLASS_METHOD_QUALIFIED( CppClass__, cppMethod__, R__, lass::meta::NullType )
$[
/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_METHOD_QUALIFIED_DOC for $x arguments
 */
#define PY_CLASS_METHOD_QUALIFIED_$x( CppClass__, cppMethod__, R__, $(P$x__)$ )\
    typedef LASS_TYPE_LIST_$x( $(P$x__)$ ) LASS_UNIQUENAME(LASS_CONCATENATE(pyMethodArguments, CppClass__));\
    PY_CLASS_METHOD_QUALIFIED( CppClass__, cppMethod__, R__, LASS_UNIQUENAME(LASS_CONCATENATE(pyMethodArguments, CppClass__)) )
]$



// --- static methods ------------------------------------------------------------------------------

#define PY_CLASS_STATIC_METHOD_EX( CppClass__, function__, pyFunction__, pyName__, pyDoc__ )\
	inline PyObject* pyFunction__( PyObject* iIgnore, PyObject* iArgs )\
	{\
		return lass::python::impl::pyCallFunction( iArgs, function__ );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyClassStaticMethod_, CppClass__, pyFunction__ ),\
		CppClass__::Methods.insert(CppClass__::Methods.begin(),\
		lass::python::createPyMethodDef(pyName__, (PyCFunction) pyFunction__, METH_VARARGS  | METH_CLASS , pyDoc__)); )

#define PY_CLASS_STATIC_METHOD_DOC( CppClass__, staticmethod__, pyDoc__ )\
		PY_CLASS_STATIC_METHOD_EX( CppClass__, CppClass__::staticmethod__, LASS_CONCATENATE_3(py, CppClass__, staticmethod__),\
		LASS_STRINGIFY(staticmethod__), pyDoc__ )

#define PY_CLASS_STATIC_METHOD( CppClass__, staticmethod__ )\
		PY_CLASS_STATIC_METHOD_DOC( CppClass__, staticmethod__, NULL )



// --- data members --------------------------------------------------------------------------------

#define PY_CLASS_MEMBER_RW_EX( CppClass__, pyMemberName__, cppGetter__, cppSetter__, pyDoc__ )\
	inline PyObject* LASS_CONCATENATE_3( pyGetter, CppClass__, cppGetter__ ) ( PyObject* iObject, void* iClosure)\
	{\
		const CppClass__* object = static_cast<const CppClass__*>(iObject);\
		return lass::python::impl::PyCallMethod<CppClass__>::get( object, CppClass__::cppGetter__ );\
	}\
	inline int LASS_CONCATENATE_3( pySetter, CppClass__, cppSetter__ ) ( PyObject* iObject, PyObject* iArgs, void* iClosure )\
	{\
		CppClass__* object = static_cast<CppClass__*>(iObject);\
		lass::python::impl::PyCallMethod<CppClass__>::set( iArgs, object, CppClass__::cppSetter__ );\
		return 0;\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyClassMemberRW_, CppClass__, cppGetter__ ),\
		CppClass__::GetSetters.insert(CppClass__::GetSetters.begin(),\
			lass::python::createPyGetSetDef(pyMemberName__, LASS_CONCATENATE_3( pyGetter, CppClass__, cppGetter__ ), LASS_CONCATENATE_3( pySetter, CppClass__, cppSetter__ ), pyDoc__, NULL)); )

#define PY_CLASS_MEMBER_RW_DOC( CppClass__, pyMemberName__, cppGetter__, cppSetter__, pyDoc__ )\
	PY_CLASS_MEMBER_RW_EX( CppClass__, pyMemberName__, cppGetter__, cppSetter__, pyDoc__ )

#define PY_CLASS_MEMBER_RW( CppClass__, pyMemberName__, cppGetter__, cppSetter__ )\
	PY_CLASS_MEMBER_RW_EX( CppClass__, pyMemberName__, cppGetter__, cppSetter__, NULL )



#define PY_CLASS_MEMBER_R_EX( CppClass__, pyMemberName__, cppGetter__, pyDoc__ )\
	inline PyObject* LASS_CONCATENATE_3( pyGetter, CppClass__, cppGetter__ ) ( PyObject* iObject, void* iClosure)\
	{\
		const CppClass__* object = static_cast<const CppClass__*>(iObject);\
		return lass::python::impl::PyCallMethod<CppClass__>::get( object, CppClass__::cppGetter__ );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyClassMemberR_, CppClass__, cppGetter__ ),\
		CppClass__::GetSetters.insert(CppClass__::GetSetters.begin(),\
			lass::python::createPyGetSetDef(pyMemberName__, LASS_CONCATENATE_3( pyGetter, CppClass__, cppGetter__ ), NULL, pyDoc__, NULL)); )

#define PY_CLASS_MEMBER_R_DOC( CppClass__, pyMemberName__, cppGetter__ , pyDoc__)\
	PY_CLASS_MEMBER_R_EX( CppClass__, pyMemberName__, cppGetter__ , pyDoc__)

#define PY_CLASS_MEMBER_R( CppClass__, pyMemberName__, cppGetter__ )\
	PY_CLASS_MEMBER_R_EX( CppClass__, pyMemberName__, cppGetter__, NULL )



#define PY_CLASS_PUBLIC_MEMBER_EX( CppClass__, cppMember__, pyMemberName__, pyDoc__ )\
	inline PyObject* LASS_CONCATENATE_3( pyPublicGetter, CppClass__, cppMember__ ) ( PyObject* iObject, void* iClosure)\
	{\
		CppClass__* object = static_cast<CppClass__*>(iObject);\
		return lass::python::pyBuildSimpleObject( object->cppMember__);\
	}\
	inline int LASS_CONCATENATE_3( pyPublicSetter, CppClass__, cppMember__ ) ( PyObject* iObject,PyObject* iArgs, void* iClosure )\
	{\
		CppClass__* object = static_cast<CppClass__*>(iObject);\
		return lass::python::pyGetSimpleObject( iArgs, object->cppMember__ );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyClassPublicMember_, CppClass__, cppMember__ ),\
		CppClass__::GetSetters.insert(CppClass__::GetSetters.begin(),\
			lass::python::createPyGetSetDef(pyMemberName__, LASS_CONCATENATE_3( pyPublicGetter, CppClass__, cppMember__ ), LASS_CONCATENATE_3( pyPublicSetter, CppClass__, cppMember__ ), pyDoc__, NULL)); )

#define PY_CLASS_PUBLIC_MEMBER_DOC( CppClass__, cppMember__ , pyDoc__)\
	PY_CLASS_PUBLIC_MEMBER_EX( CppClass__, cppMember__, LASS_STRINGIFY( cppMember__ ),  pyDoc__ )

#define PY_CLASS_PUBLIC_MEMBER( CppClass__, cppMember__ )\
	PY_CLASS_PUBLIC_MEMBER_DOC( CppClass__, cppMember__, NULL )



// --- constructors --------------------------------------------------------------------------------

/** @ingroup Python
 *	makes a abstract python class concreet by adding a constructor.
 *
 *  All python classes are abstract by default, you don't have any constructors to create
 *  instances.  To make a class concrete, you must provide a constructor by using this macro.
 *  You can overload this constructor by the same way you can overload class methods.
 *
 *  @param CppClass__ 
 *		the C++ class to generate the constructor 
 *  @param PList__
 *		a meta::TypeList of the parameters needed by the constructor.
 *      Use meta::NullType if the constructor doesn't take any parameters.
 *  @param dispatcher__ 
 *		A unique name of the static C++ dispatcher function to be generated.  This name will be
 *		used for the names of automatic generated variables and functions and should be unique
 *		per exported C++ class/constructor pair.
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *		Foo();
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
#define PY_CLASS_CONSTRUCTOR_EX( CppClass__, PList__, dispatcher__ )\
	static newfunc LASS_CONCATENATE( pyOverloadChain_, dispatcher__ ) = 0;\
    PyObject* dispatcher__(PyTypeObject *iSubtype, PyObject *iArgs, PyObject *iKwds)\
    {\
		if (LASS_CONCATENATE( pyOverloadChain_, dispatcher__ ))\
		{\
			PyObject* result = LASS_CONCATENATE( pyOverloadChain_, dispatcher__ )(iSubtype, iArgs, iKwds);\
			if (result)\
			{\
				return result;\
			}\
		}\
		return lass::python::impl::PyExplicitResolver\
		<\
			CppClass__,\
			lass::meta::NullType,\
			PList__\
		>\
		::TImpl::callConstructor(iArgs);\
    }\
    LASS_EXECUTE_BEFORE_MAIN_EX(\
		LASS_CONCATENATE(lassExecutePyClassConstructor_, dispatcher__),\
		LASS_CONCATENATE( pyOverloadChain_, dispatcher__ ) = CppClass__::Type.tp_new;\
        CppClass__::Type.tp_new = dispatcher__; \
	)

/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_CONSTRUCTOR_EX with 
 *	@a dispatcher__ = pyConstruct @a CppClass__ __LINE__
 */
#define PY_CLASS_CONSTRUCTOR( CppClass__, PList__ )\
	PY_CLASS_CONSTRUCTOR_EX( CppClass__, PList__, LASS_UNIQUENAME(LASS_CONCATENATE(pyConstruct_, CppClass__)) )

/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_CONSTRUCTOR for 0 arguments
 */
#define PY_CLASS_CONSTRUCTOR_0( CppClass__ )\
    PY_CLASS_CONSTRUCTOR( CppClass__, lass::meta::NullType )
$[
/** @ingroup Python
 *	convenience macro, wraps PY_CLASS_CONSTRUCTOR for $x arguments
 */
#define PY_CLASS_CONSTRUCTOR_$x( CppClass__, $(P$x__)$ )\
    typedef LASS_TYPE_LIST_$x( $(P$x__)$ ) LASS_UNIQUENAME(LASS_CONCATENATE(pyConstructorArguments, CppClass__));\
    PY_CLASS_CONSTRUCTOR( CppClass__, LASS_UNIQUENAME(LASS_CONCATENATE(pyConstructorArguments, CppClass__)) )
]$


// --- implementation macros -----------------------------------------------------------------------

#define PY_IMPL_DISPATCHER_CLASS_METHOD( CppClass__, dispatcher__ )\
	inline PyObject* dispatcher__( PyObject* iObject, PyObject* iArgs )\
	{\
		if (LASS_CONCATENATE( pyOverloadChain_, dispatcher__ ))\
		{\
			PyObject* result = LASS_CONCATENATE( pyOverloadChain_, dispatcher__ )(iObject, iArgs);\
			if (result)\
			{\
				return result;\
			}\
		}\
		CppClass__* object = static_cast<CppClass__*>( iObject );\
		return LASS_CONCATENATE( PyShadowResolver_, dispatcher__ )\
		<\
			lass::python::IsShadowType<CppClass__>::value\
		>\
		::call( object, iArgs );\
	}



#define PY_IMPL_SUBSCRIBE_CLASS_METHOD( CppClass__, pyMethod__, pyDoc__, dispatcher__ )\
	LASS_EXECUTE_BEFORE_MAIN_EX(\
		LASS_CONCATENATE_3( lassExecutePySubscribeClassMethod_, CppClass__, dispatcher__ ),\
		std::vector<PyMethodDef>::iterator i =\
			std::find_if(CppClass__::Methods.begin(), CppClass__::Methods.end(), \
						 lass::python::impl::PyMethodEqual(pyMethod__));\
		if (i == CppClass__::Methods.end())\
		{\
			CppClass__::Methods.insert(CppClass__::Methods.begin(),\
				lass::python::createPyMethodDef(\
					pyMethod__, (PyCFunction) dispatcher__, METH_VARARGS , pyDoc__));\
		}\
		else\
		{\
			LASS_CONCATENATE( pyOverloadChain_, dispatcher__ ) = i->ml_meth;\
			i->ml_meth = dispatcher__;\
		};\
	)

// EOF
