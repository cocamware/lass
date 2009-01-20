/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_CALL_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_CALL_INL

#include "python_common.h"
#include "pyobject_plus.h"
#include "py_tuple.h"
#include "pyshadow_object.h"
#include "../util/call_traits.h"
#include "../meta/if.h"
#include "../meta/select.h"
#include "../meta/wrap.h"
#include "../meta/type_list.h"

#define LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN_EX(v_errorReturnValue)\
	catch (const ::lass::python::PythonException& error)\
	{\
		handlePythonException(error);\
		return v_errorReturnValue;\
	}\
	catch (const ::lass::util::Exception& error)\
	{\
		handleLassException(error);\
		return v_errorReturnValue;\
	}\
	catch (::std::exception& error)\
	{\
		handleStdException(error);\
		return v_errorReturnValue;\
	}

#define LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN\
	LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN_EX(0)

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4267) // conversion from 'size_t' to 'unsigned int', possible loss of data
#endif

namespace lass
{
namespace python
{
namespace impl
{

// --- exception handlers -------------------------------------------------------------------------

inline void handlePythonException(const PythonException& error)
{
	PyErr_Restore(
		::lass::python::fromSharedPtrToNakedCast(error.type()),
		::lass::python::fromSharedPtrToNakedCast(error.value()),
		::lass::python::fromSharedPtrToNakedCast(error.traceback()));
}

inline void handleLassException(const util::Exception& error)
{
	::std::ostringstream buffer;
	buffer << error.message() << "\n\n(" << error.location() << ")";
	PyErr_SetString(PyExc_Exception, buffer.str().c_str());
}

inline void handleStdException(const std::exception& error)
{
	PyErr_SetString(PyExc_Exception, error.what());
}

// --- ArgumentTraits -----------------------------------------------------------------------------

template <typename T>
struct ArgValue
{
	typedef T TStorage;
protected:
	static const T* get(const TStorage& storage) { return &storage; }
};

template <typename T>
struct ArgPyPtr
{
	typedef typename PyObjectPtr<T>::Type TStorage;
protected:
	static T* get(const TStorage& storage) { return storage.get(); }
};


/** determines the right type for temporary storage of function arguments.
 */
template <typename T>
struct ArgumentTraits: public meta::If< IsPyObject<T>, meta::NullType, ArgValue<T> >
{
	template <typename S> static const T& arg(const S& storage) { return *get(storage); }
};

template <typename T>
struct ArgumentTraits<const T>: public meta::If< IsPyObject<T>, meta::NullType, ArgValue<T> >
{
	template <typename S> static const T& arg(const S& storage) { return *get(storage); }
};

template <typename T> 
struct ArgumentTraits<T&>: public meta::If< IsPyObject<T>, ArgPyPtr<T>, meta::NullType >
{
	template <typename S> static T& arg(const S& storage) { return *get(storage); }
};

template <typename T>
struct ArgumentTraits<const T&>: public meta::If< IsPyObject<T>, ArgPyPtr<T>, ArgValue<T> >
{
	template <typename S> static const T& arg(const S& storage) { return *get(storage); }
};

template <typename T> 
struct ArgumentTraits<T*>: public meta::If< IsPyObject<T>, ArgPyPtr<T>, meta::NullType >
{
	template <typename S> static T* arg(const S& storage) { return get(storage); }
};

template <typename T> 
struct ArgumentTraits<const T*>: public meta::If< IsPyObject<T>, ArgPyPtr<T>, meta::NullType >
{
	template <typename S> static const T* arg(const S& storage) { return get(storage); }
};

template <typename T> 
struct ArgumentTraits<T* const>: public meta::If< IsPyObject<T>, ArgPyPtr<T>, meta::NullType >
{
	template <typename S> static T* arg(const S& storage) { return get(storage); }
};

template <typename T> 
struct ArgumentTraits<const T* const>: public meta::If< IsPyObject<T>, ArgPyPtr<T>, meta::NullType >
{
	template <typename S> static const T* arg(const S& storage) { return get(storage); }
};



// --- actual callers ------------------------------------------------------------------------------

/** calls the actual function with provided parameters, and returns result as a PyObject pointer.
 */
template <typename R>
struct Caller
{
	// free function

	template <typename Function>
	static PyObject* function( Function iFunction )
	{
		try
		{
			return pyBuildSimpleObject( iFunction() );
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	};
	$[
	template <typename Function, $(typename P$x)$>
	static PyObject* function( Function iFunction, $(P$x p$x)$ )
	{
		try
		{
			return pyBuildSimpleObject( iFunction($(p$x)$) );
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	}
	]$

	// method

	template <typename CppClassPtr, typename Method>
	static PyObject* method( CppClassPtr iObject, Method iMethod )
	{
		try
		{
			return pyBuildSimpleObject( (iObject->*iMethod)() );
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	};
	$[
	template <typename CppClassPtr, typename Method, $(typename P$x)$>
	static PyObject* method( CppClassPtr iObject, Method iMethod, $(P$x p$x)$ )
	{
		try
		{
			return pyBuildSimpleObject( (iObject->*iMethod)($(p$x)$) );
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	}
	]$
};

/** specialisation for functions without return value, calls function and returns Py_None.
 */
template <>
struct Caller<void>
{
	// free functions

	template <typename Function>
	static PyObject* function( Function iFunction )
	{
		try
		{
			iFunction();
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
		Py_INCREF( Py_None );
		return Py_None;
	};
	$[
	template <typename Function, $(typename P$x)$>
	static PyObject* function( Function iFunction, $(P$x p$x)$ )
	{
		try
		{
			iFunction($(p$x)$);
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
		Py_INCREF( Py_None );
		return Py_None;
	}
	]$

	// methods

	template <typename CppClassPtr, typename Method>
	static PyObject* method( CppClassPtr iObject, Method iMethod )
	{
		try
		{
			(iObject->*iMethod)();
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
		Py_INCREF( Py_None );
		return Py_None;
	};
	$[
	template <typename CppClassPtr, typename Method, $(typename P$x)$>
	static PyObject* method( CppClassPtr iObject, Method iMethod, $(P$x p$x)$ )
	{
		try
		{
			(iObject->*iMethod)($(p$x)$);
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
		Py_INCREF( Py_None );
		return Py_None;
	}
	]$
};



// --- free functions ------------------------------------------------------------------------------

/** calls C++ function without arguments
 */
template <typename R>
PyObject* callFunction( PyObject* args, R (*iFunction)() )
{
	typedef R(*TFunction)();
	if( decodeTuple(args) != 0 )
	{
		return 0;
	}
	return Caller<R>::template function<TFunction>( iFunction );
}
$[
/** calls C++ function with $x arguments, translated from python arguments
 */
template <typename R, $(typename P$x)$>
PyObject* callFunction( PyObject* args, R (*iFunction)($(P$x)$) )
{
	typedef R (*TFunction)($(P$x)$);
	$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
	)$
	if( decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
	{
		return 0;
	}
	return Caller<R>::template function<TFunction, $(P$x)$>( 
		iFunction, $(TArg$x::arg(p$x))$ );
}
]$



// --- methods -------------------------------------------------------------------------------------

template <class CppClass>
struct CallMethod
{
	// non const methods

	/** call non const method without arguments
	 */
	template <typename C, typename R>
	static PyObject* call( PyObject* args, CppClass* iObject, R (C::*iMethod)() )
	{
		typedef R (C::*TMethod)();
		if( decodeTuple(args) != 0 )
		{
			return 0;
		}
		return Caller<R>::template method<CppClass*, TMethod>(
			iObject, iMethod );
	}
$[
	/** call non const method with $x arguments, translated from python arguments
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* call( PyObject* args, CppClass* iObject, R (C::*iMethod)($(P$x)$) )
	{
		typedef R (C::*TMethod)($(P$x)$);
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		return Caller<R>::template method<CppClass*, TMethod, $(P$x)$>( 
			iObject, iMethod, $(TArg$x::arg(p$x))$ );
	}
]$

	// const methods

	/** call const method without arguments
	 */
	template <typename C, typename R>
	static PyObject* call( PyObject* args, const CppClass* iObject, R (C::*iMethod)() const )
	{
		typedef R (C::*TMethod)() const;
		if( decodeTuple(args) != 0 )
		{
			return 0;
		}
		return Caller<R>::template method<const CppClass*, TMethod>( 
			iObject, iMethod );
	}
$[
	/** call const method with $x argument, translated from python arguments
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* call( PyObject* args, const CppClass* iObject, R (C::*iMethod)($(P$x)$) const )
	{
		typedef R (C::*TMethod)($(P$x)$) const;
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		return Caller<R>::template method<const CppClass*, TMethod, $(P$x)$>(
			iObject, iMethod, $(TArg$x::arg(p$x))$ );
	}
]$
	// "free" non-const methods passing iObject as pointer

	/** call non const "free method" without arguments, passing iObject as pointer
	 */
	template <typename C, typename R>
	static PyObject* callFree( PyObject* args, CppClass* iObject, R (*iFreeMethod)(C*) )
	{
		typedef R (*TFunction)(C*);
		if( decodeTuple(args) != 0 )
		{
			return 0;
		}
		return Caller<R>::template function<TFunction, C* >(
			iFreeMethod, iObject );
	}
$[
	/** call non-const "free method" with $x arguments translated from python arguments, 
	 *  passing iObject as pointer
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* callFree( PyObject* args, CppClass* iObject, R (*iFreeMethod)(C*, $(P$x)$) )
	{
		typedef R (*TFunction)(C*, $(P$x)$);
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		return Caller<R>::template function<TFunction, C*, $(P$x)$>(
			iFreeMethod, iObject, $(TArg$x::arg(p$x))$ );
	}
]$

	// "free" non-const methods passing iObject as reference

	/** call non const "free method" without arguments, passing iObject as reference
	 */
	template <typename C, typename R>
	static PyObject* callFree( PyObject* args, CppClass* iObject, R (*iFreeMethod)(C&) )
	{
		typedef R (*TFunction)(C&);
		if( decodeTuple(args) != 0 )
		{
			return 0;
		}
		LASS_ASSERT(iObject);
		return Caller<R>::template function<TFunction, C& >(
			iFreeMethod, *iObject );
	}
$[
	/** call non-const "free method" with $x arguments translated from python arguments, 
	 *  passing iObject as reference
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* callFree( PyObject* args, CppClass* iObject, R (*iFreeMethod)(C&, $(P$x)$) )
	{
		typedef R (*TFunction)(C&, $(P$x)$);
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		LASS_ASSERT(iObject);
		return Caller<R>::template function<TFunction, C&, $(P$x)$>(
			iFreeMethod, *iObject, $(TArg$x::arg(p$x))$ );
	}
]$

	// "free" const methods passing iObject as pointer

	/** call const "free method" without arguments, passing iObject as pointer
	 */
	template <typename C, typename R>
	static PyObject* callFree( PyObject* args, const CppClass* iObject, R (*iFreeMethod)(const C*) )
	{
		typedef R (*TFunction)(const C*);
		if( decodeTuple(args) != 0 )
		{
			return 0;
		}
		return Caller<R>::template function<TFunction, const C* >(
			iFreeMethod, iObject );
	}
$[
	/** call const "free method" with $x arguments translated from python arguments, 
	 *  passing iObject as pointer
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* callFree( PyObject* args, const CppClass* iObject, R (*iFreeMethod)(const C*, $(P$x)$) )
	{
		typedef R (*TFunction)(const C*, $(P$x)$);
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		return Caller<R>::template function<TFunction, const C*, $(P$x)$>(
			iFreeMethod, iObject, $(TArg$x::arg(p$x))$ );
	}
]$

	// "free" const methods passing iObject as reference

	/** call non const "free method" without arguments, passing iObject as pointer
	 */
	template <typename C, typename R>
	static PyObject* callFree( PyObject* args, const CppClass* iObject, R (*iFreeMethod)(const C&) )
	{
		typedef R (*TFunction)(const C&);
		if( decodeTuple(args) != 0 )
		{
			return 0;
		}
		LASS_ASSERT(iObject);
		return Caller<R>::template function<TFunction, const C& >(
			iFreeMethod, *iObject );
	}
$[
	/** call const "free method" with $x arguments translated from python arguments, 
	 *  passing iObject as pointer
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* callFree( PyObject* args, const CppClass* iObject, R (*iFreeMethod)(const C&, $(P$x)$) )
	{
		typedef R (*TFunction)(const C&, $(P$x)$);
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		LASS_ASSERT(iObject);
		return Caller<R>::template function<TFunction, const C&, $(P$x)$>(
			iFreeMethod, *iObject, $(TArg$x::arg(p$x))$ );
	}
]$

	// member getters and setters

	/** call const getter function
	 */
	template <typename C, typename R>
	static PyObject* get( const CppClass* iObject, R (C::*iMethod)() const)
	{
		try
		{
			return pyBuildSimpleObject((iObject->*iMethod)());
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	}

	/** call getter function, for non-assignable, although not read-only members
	 */
	template <typename C, typename R>
	static PyObject* get( CppClass* iObject, R (C::*iMethod)() )
	{
		try
		{
			return pyBuildSimpleObject((iObject->*iMethod)());
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	}


	/** call explicit setter function like <tt>void Foo::setBar(const Bar& iBar)</tt>
	 */
	template <typename P>
	static int set( PyObject* args, CppClass* iObject, void (CppClass::*iMethod)(P) )
	{
		typedef ArgumentTraits<P> TArg;
		typename TArg::TStorage p;

		if( pyGetSimpleObject( args, p ) != 0 )
		{
			return -1;
		}

		try
		{
			(iObject->*iMethod)(TArg::arg(p));
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN_EX(-1)
		return 0;
	}

	/** call implicit setter function like <tt>Bar& Foo::bar()</tt>
	 */
	template <typename P>
	static int set( PyObject* args, CppClass* iObject, P& (CppClass::*iMethod)() )
	{
		typedef ArgumentTraits<P> TArg;
		typename TArg::TStorage p;

		if( pyGetSimpleObject( args, p ) != 0 )
		{
			return -1;
		}

		try
		{
			(iObject->*iMethod)() = TArg::arg(p);
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN_EX(-1)
		return 0;
	}
};



// --- constructors --------------------------------------------------------------------------------

/** allocate a new object with default constructor.
 */
template <class PyObjectClass>
PyObject* construct( PyTypeObject* iSubtype, PyObject* args )
{
	typedef ShadowTraits< PyObjectClass > TPyShadowTraits;
	typedef typename TPyShadowTraits::TCppClass TCppClass;
	
	if( decodeTuple(args) != 0 )
	{
		return 0;
	}
	
	try
	{
		PyObjectClass* result = TPyShadowTraits::pyObject( new TCppClass );
		// actually the iSubType should be used but appearently the tp_dict does not get properly initialized
		// so for now, old-style, consequence: no inheritance from extension classes
		impl::fixObjectType(result);
		result->ob_type = iSubtype;
		// do not track the object using GC
//#pragma LASS_TODO("Check if we have created memory leak... yes we have :-/, read the comments here.")
		/* The reason why we can't use the GC mechanism of Python is that objects needs extra members for this and we don't 
		   want to add them to PyObjectPlus.  So we do our own GC... tricky but one needs to live on the edge >:-D 
		   In the future this should happen:
		   - PyObjectPlus objects should be final... so no derivation and no GC problems.
		   - PyObjectPlusSuperDeluxe will support everything.
		   The shadowclasses can then be the 'deluxe' version 
		*/
		result->ob_type->tp_flags &= (~Py_TPFLAGS_HAVE_GC);
		Py_INCREF(result->ob_type);
		return result;
	}
	LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
}
$[
/** allocate a new object with $x arguments.
 */
template <class PyObjectClass, $(typename P$x)$>
PyObject* construct( PyTypeObject* iSubtype, PyObject* args )
{
	typedef ShadowTraits< PyObjectClass > TPyShadowTraits;
	typedef typename TPyShadowTraits::TCppClass TCppClass;
	$(typedef ArgumentTraits< P$x > TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
	)$

	if ( decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
	{
		return 0;
	}

	try
	{
		PyObjectClass* result = TPyShadowTraits::pyObject( new TCppClass( $(TArg$x::arg(p$x))$ ) );
		impl::fixObjectType(result);
		result->ob_type = iSubtype;
		result->ob_type->tp_flags &= (~Py_TPFLAGS_HAVE_GC);
		Py_INCREF(result->ob_type);
		return result;
	}
	LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
}
]$



// --- explicit resolver ---------------------------------------------------------------------------

// This explicit resolver construct is a wicked meta function that will call the write
// template function based on template parameters in TypeListType.

template <class CppClass, typename R, typename TypeListType> struct ExplicitResolver;
$[template <class CppClass, typename R, $(typename P$x)$, typename TypeListType> struct ExplicitResolver$x;
]$
template <class CppClass, typename R, $(typename P$x)$, typename P$y, typename TypeListType> struct ExplicitResolver$y;


template <class CppClass, typename R>
struct ExplicitResolver<CppClass, R, lass::meta::NullType>
{
	struct Impl
	{
		static PyObject* callFunction( PyObject* args, R (*iFunction)() )
		{
			return ::lass::python::impl::callFunction<R>( args, iFunction );
		}
		static PyObject* callMethod( PyObject* args, CppClass* iObject, R (CppClass::*iMethod)() )
		{
			return CallMethod<CppClass>::call( args, iObject, iMethod );
		}
		static PyObject* callMethod( PyObject* args, const CppClass* iObject, R (CppClass::*iMethod)() const )
		{
			return CallMethod<CppClass>::call( args, iObject, iMethod );
		}
		static PyObject* callConstructor( PyTypeObject* iSubtype, PyObject* args )
		{
			return construct<CppClass>( iSubtype, args );
		}
		static bool isConstMember( R (CppClass::*)() ) { return false; }
		static bool isConstMember( R (CppClass::*)() const ) { return true; }
	};
	typedef Impl TImpl;
};
$[
template <class CppClass, typename R, $(typename P$x)$>
struct ExplicitResolver$x<CppClass, R, $(P$x)$, lass::meta::NullType>
{
	struct Impl
	{
		static PyObject* callFunction( PyObject* args, R (*iFunction)($(P$x)$) )
		{
			return ::lass::python::impl::callFunction<R, $(P$x)$>( args, iFunction );
		}
		static PyObject* callMethod( PyObject* args, CppClass* iObject, R (CppClass::*iMethod)($(P$x)$) )
		{
			return CallMethod<CppClass>::call( args, iObject, iMethod );
		}
		static PyObject* callMethod( PyObject* args, const CppClass* iObject, R (CppClass::*iMethod)($(P$x)$) const )
		{
			return CallMethod<CppClass>::call( args, iObject, iMethod );
		}
		static bool isConstMember( R (CppClass::*)($(P$x)$) ) { return false; }
		static bool isConstMember( R (CppClass::*)($(P$x)$) const ) { return true; }
		static PyObject* callFreeMethod( PyObject* args, CppClass* iObject, R (*iFreeMethod)($(P$x)$) )
		{
			return CallMethod<CppClass>::callFree( args, iObject, iFreeMethod );
		}
		static PyObject* callConstructor( PyTypeObject* iSubtype, PyObject* args )
		{
			return construct<CppClass, $(P$x)$>( iSubtype, args );
		}
	};
	typedef Impl TImpl;
};
]$

template <class CppClass, typename R, typename Head, typename Tail>
struct ExplicitResolver<CppClass, R, meta::TypeList<Head, Tail> >
{
	typedef typename ExplicitResolver1<CppClass, R, Head, Tail>::TImpl TImpl;
};
$[
template <class CppClass, typename R, $(typename P$x)$, typename Head, typename Tail>
struct ExplicitResolver$x<CppClass, R, $(P$x)$, meta::TypeList<Head, Tail> >
{
	typedef typename ExplicitResolver$y<CppClass, R, $(P$x)$, Head, Tail>::TImpl TImpl;
};
]$


}

}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif

// EOF
