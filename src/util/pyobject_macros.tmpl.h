/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/
/** @file util/pyobject_macros.h 
 *
 *  @par last commit:
 *		$Revision$\n
 *		$Date$\n
 *		$Author$
 *
 *  $g
 */

#define LASS_PYTHON_ERR_MSG_NO_NEW_OPERATOR "Can not create object"


#define LASS_PYTHON_ERR_MSG_ARG_NOT_BOOL  "Bad argument - not bool"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_INT  "Bad argument - not int"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_LONG  "Bad argument - not long"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_FLOAT  "Bad argument - not float"
#define LASS_PYTHON_ERR_MSG_ARG_NOT_DOUBLE  "Bad argument - not double"


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
		static PyObject* __forbidden_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) \
		{ PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_NO_NEW_OPERATOR); return NULL;}

/* Place as first line of your Pythonized class.  For ParentClass use the 
*  C++ class from which you wish the python object inherits.  ParentClass
*  must also be a Pythonized class or use lass::python::PyObjectPlus as default.
*  @remark Any declarations coming after this macro are public!
*/
#define PY_HEADER( ParentClass ) \
	public: \
		static char*		  PythonClassName;\
		static char*		  PythonModuleName;\
		static PyTypeObject   Type; \
		static std::vector<PyMethodDef>    Methods; \
		static std::vector<PyGetSetDef>    GetSetters; \
		static PyTypeObject* GetParentType(void) { return &ParentClass::Type; }\
		virtual PyTypeObject *GetType(void) {return &Type;}; \
		static PyObject* __forbidden_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) \
		{ PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_NO_NEW_OPERATOR); return NULL;}

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
	CppClass__ ::__forbidden_new,/*tp_new*/\
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
	lass::python::PyObjectPlusPlus< CppClass__ , parentclass__ >::__forbidden_new,/*tp_new*/\
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
	PY_STATIC_FUNCTION_FORWARD( CppClass__, pyclass__ ) };\
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
	
/** Ideas for polymorphic functions

Have two macros :
	PY_CLASS_DECLARE_POLYMORPH_METHOD( Bar, test)
and:
	PY_CLASS_POLYMORPH_METHOD( Bar, test, complexArguments )
	PY_CLASS_POLYMORPH_METHOD( Bar, test, primArguments )

The first macro declares the datastructure needed to support the 
dynamic dispatching.  This can be a vector of pairs: each pair
contains the "argument check" static method and the "effective call"
static method.

The first macro also insert its function name in the python function
table.  It will provide the dispatching by running through the vector
of pairs and when there is an argument math effectively call the function.

The second macro just updates the vector of pairs by pushing back the
necessary pairs of functions.  These functions are also declared by the
macro itself.  The "effective call" method can be a copy of the other
METHOD macros.  The only thing not currently efficiently available is
a method for checking for valid arguments.  Altough the pyGetArguments
checks for correct typing it also try to convert the arguments which is
not necessary at this phase of the dispatching.

Remark: there is now a pyCheckArguments :)
*/

typedef std::vector< PyCFunction > TVectorCFunction;

#define PY_CLASS_DECLARE_OVERLOADED_METHOD_EX( CppClass__, pyName__ , pyFunction__, pyDoc__ )\
	TVectorCFunction LASS_CONCATENATE_3( pyOverloadSupport, CppClass__, pyName__ );\
	inline PyObject* pyFunction__( PyObject* iObject, PyObject* iArgs )\
	{\
		TVectorCFunction::const_iterator fIt = LASS_CONCATENATE_3( pyOverloadSupport, CppClass__, pyName__ ).begin();\
		TVectorCFunction::const_iterator fEIt = LASS_CONCATENATE_3( pyOverloadSupport, CppClass__, pyName__ ).end();\
		for (;fIt!=fEIt;++fIt)\
		{\
			PyObject* r =(*fIt)( iObject, iArgs );\
			if (r) return r;\
		}\
		PyErr_SetString(PyExc_TypeError, "Arguments for overloaded function could not be matched" );\
		return NULL;\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyClassOverloadMethod_, CppClass__, pyFunction__ ),\
		CppClass__::Methods.insert(CppClass__::Methods.begin(),\
			lass::python::createPyMethodDef( LASS_STRINGIFY(pyName__), (PyCFunction) pyFunction__, METH_VARARGS , pyDoc__)); )

#define PY_CLASS_DECLARE_OVERLOADED_METHOD( CppClass__, pyName__ )\
	PY_CLASS_DECLARE_OVERLOADED_METHOD_EX( CppClass__, pyName__ , LASS_CONCATENATE_3(pyOverloadFunction,CppClass__,pyName__), NULL )

#define PY_CLASS_OVERLOADED_METHOD( CppClass__, pyName__, method__ )\
	inline PyObject* LASS_CONCATENATE_3( LASS_CONCATENATE( pyOverloadCall, CppClass__ ), pyName__, method__ ) (PyObject* iObject, PyObject* iArgs)\
	{\
		CppClass__* object = static_cast<CppClass__*>( iObject );\
		return lass::python::impl::PyCallMethod< CppClass__ >::call( iArgs, object, CppClass__ ::method__ );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyClassOverloadMethodDefs_, CppClass__, pyFunction__ ),\
		LASS_CONCATENATE_3( pyOverloadSupport, CppClass__, pyName__ ).push_back( \
			LASS_CONCATENATE_3( LASS_CONCATENATE( pyOverloadCall, CppClass__ ), pyName__, method__  ) ); )



#define PY_CLASS_METHOD_EX( CppClass__, method__, pyName__, pyDoc__, pyFunction__)\
	static PyCFunction LASS_CONCATENATE( pyOverloadChain_, pyFunction__ ) = 0;\
	template <int isShadow>\
	struct LASS_CONCATENATE( PyShadowResolver_, pyFunction__ )\
	{\
		template <typename Obj>\
		static PyObject* call( Obj* iObject, PyObject* iArgs )\
		{\
			typedef typename Obj::RealObject TRealObject;\
			return lass::python::impl::PyCallMethod<TRealObject>::call(\
				iArgs, iObject->value_, TRealObject::method__ );\
		}\
	};\
	template <>\
	struct LASS_CONCATENATE( PyShadowResolver_, pyFunction__ )<false>\
	{\
		template <typename Obj>\
		static PyObject* call( Obj* iObject, PyObject* iArgs )\
		{\
			return lass::python::impl::PyCallMethod<Obj>::call(	iArgs, iObject, Obj::method__ );\
		}\
	};\
	inline PyObject* pyFunction__( PyObject* iObject, PyObject* iArgs )\
	{\
		if (LASS_CONCATENATE( pyOverloadChain_, pyFunction__ ))\
		{\
			PyObject* result = LASS_CONCATENATE( pyOverloadChain_, pyFunction__ )(iObject, iArgs);\
			if (result)\
			{\
				return result;\
			}\
		}\
		CppClass__* object = static_cast<CppClass__*>( iObject );\
		return LASS_CONCATENATE( PyShadowResolver_, pyFunction__ )\
		<\
			lass::python::IsShadowType<CppClass__>::value\
		>\
		::call( object, iArgs );\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE_3( lassExecutePyClassMethod_, CppClass__, pyFunction__ ),\
		std::vector<PyMethodDef>::iterator i =\
			std::find_if(CppClass__::Methods.begin(), CppClass__::Methods.end(), \
						 lass::python::PyMethodEqual(pyName__));\
		if (i == CppClass__::Methods.end())\
		{\
			CppClass__::Methods.insert(CppClass__::Methods.begin(),\
				lass::python::createPyMethodDef(\
					pyName__, (PyCFunction) pyFunction__, METH_VARARGS , pyDoc__));\
		}\
		else\
		{\
			LASS_CONCATENATE( pyOverloadChain_, pyFunction__ ) = i->ml_meth;\
			i->ml_meth = pyFunction__;\
		};\
	)

#define PY_CLASS_METHOD_NAME_DOC( CppClass__, method__, pyName__, pyDoc__ )\
		PY_CLASS_METHOD_EX( CppClass__, method__, pyName__, pyDoc__, LASS_CONCATENATE_3(py, CppClass__, method__))

#define PY_CLASS_METHOD_NAME( CppClass__, method__, pyName__ )\
		PY_CLASS_METHOD_NAME_DOC( CppClass__, method__, pyName__, 0 )

#define PY_CLASS_METHOD_DOC( CppClass__, method__, pyDoc__ )\
		PY_CLASS_METHOD_NAME_DOC( CppClass__, method__, LASS_STRINGIFY(method__), pyDoc__)

#define PY_CLASS_METHOD( CppClass__, method__ )\
		PY_CLASS_METHOD_DOC( CppClass__, method__, NULL )



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

/** makes a abstract python class concreet by adding a constructor.
 *  All python classes are abstract by default, you don't have any constructors to create
 *  instances.  To make a class concrete, you must provide a constructor by using this macro.
 *
 *  @param CppClass__ the C++ class
 *  @param Parameters__ a meta::TypeList of the parameters needed by the constructor.
 *                      Use meta::NullType if the constructor doesn't take any parameters.
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      Foo(int iA, const std::string& iB);
 *  };
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  typedef meta::type_list::Make<int, std::string>::Type TArguments;
 *  PY_CLASS_CONSTRUCTOR(Foo, TArguments)
 *
 *  // constructor without arguments:
 *  PY_CLASS_CONSTRUCTOR(Bar, meta::NullType)
 *  @endcode
 */
#define PY_CLASS_CONSTRUCTOR( CppClass__, Parameters__ )\
    PyObject* LASS_CONCATENATE(pyConstruct, CppClass__)(PyTypeObject *iSubtype, PyObject *iArgs, PyObject *iKwds)\
    {\
        typedef lass::python::impl::PyConstructor<CppClass__, Parameters__>::TImpl TImpl;\
        return TImpl::construct(iArgs);\
    }\
    LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE(lassExecutePyClassConstructor_, CppClass__),\
        CppClass__::Type.tp_new = LASS_CONCATENATE(pyConstruct, CppClass__); )

/** convenience macro, wraps PY_CLASS_CONSTRUCTOR for 0 arguments
 */
#define PY_CLASS_CONSTRUCTOR_0( CppClass__ )\
    PY_CLASS_CONSTRUCTOR( CppClass__, lass::meta::NullType )
$[
/** convenience macro, wraps PY_CLASS_CONSTRUCTOR for $x arguments
 */
#define PY_CLASS_CONSTRUCTOR_$x( CppClass__, $(P$x__)$ )\
    typedef LASS_TYPE_LIST_$x( $(P$x__)$ ) LASS_CONCATENATE(pyConstructorArguments, CppClass__);\
    PY_CLASS_CONSTRUCTOR( CppClass__, LASS_CONCATENATE(pyConstructorArguments, CppClass__) )
]$



/* History:
 *
 * $Log$
 * Revision 1.8  2004/09/06 13:24:47  tgdemuer
 * *** empty log message ***
 *
 * Revision 1.7  2004/09/06 13:08:44  tgdemuer
 * *** empty log message ***
 *
 * Revision 1.6  2004/08/26 13:59:46  bdegreve
 * overloaded functions in piggy back style
 *
 * Revision 1.5  2004/08/12 11:47:42  tgdemuer
 * overloaded member methods working
 *
 * Revision 1.4  2004/08/12 09:17:04  tgdemuer
 * pyCheckArguments bypassed, this means that pyGetSimpleObject should check first and that pyCallArguments should have an early out
 *
 * Revision 1.3  2004/08/11 16:02:35  tgdemuer
 * various
 * overloaded functions for python
 *
 * Revision 1.2  2004/08/02 15:08:37  tgdemuer
 * experiment with python submodules
 *
 * Revision 1.1  2004/07/27 15:35:09  bdegreve
 * adding constructor support up to N arguments.
 * PY_ABSTRACT_HEADER and PY_DECLARE_ABSTRACT_CLASS are gone.
 * all pythonized classes are not constructable by default.
 * Simply use PY_HEADER and PY_DECLARE_CLASS.
 *
 *
 * RENAMED: pyobject_macros.h -> pyobject_macros.tmpl.h
 *
 * Revision 1.22  2004/07/16 16:51:48  bdegreve
 * various
 *
 * Revision 1.21  2004/07/05 13:37:39  bdegreve
 * bypassed PYTHON_SHARED_PTR_SUPPORT with a template
 *
 * Revision 1.20  2004/06/30 07:47:43  tgdemuer
 * cleanup of experimental code and added some comments
 *
 * Revision 1.19  2004/06/29 15:36:24  bdegreve
 * various fixes on msvc2003 migration
 *
 * Revision 1.18  2004/06/29 06:31:09  tgdemuer
 * VS2003 migration
 *
 * Revision 1.17  2004/06/26 19:59:47  bdegreve
 * various
 *
 * Revision 1.16  2004/06/25 17:36:35  bdegreve
 * expanded PY_INJECT_OBJECT_IN_MODULE_AT_RUNTIME for general kind of objects
 *
 * Revision 1.15  2004/06/24 13:49:00  bdegreve
 * fixed EXECUTE problem concerning same line number
 *
 * Revision 1.14  2004/06/22 21:04:00  bdegreve
 * fixing project configuration
 * (PS: PY_CLASS_MEMBER_RW is broken on intel for fancy getter/setters)
 *
 * Revision 1.13  2004/06/22 16:39:49  bdegreve
 * removing cast in PY_CLASS_METHOD_EX to avoid copy constructor of object
 *
 * Revision 1.12  2004/06/22 14:28:43  tgdemuer
 * injection of objects
 *
 * Revision 1.11  2004/06/22 14:28:08  tgdemuer
 * injection of objects
 *
 * Revision 1.10  2004/06/22 14:11:11  bdegreve
 * PY_CLASS_MEMBER_RW_EX accepts both kind of setters now: void Bar::setFoo(const Foo&) and Foo& Bar::foo()
 *
 */

// EOF