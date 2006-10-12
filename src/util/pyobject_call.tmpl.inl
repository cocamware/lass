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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_CALL_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_CALL_INL

#include "util_common.h"
#include "pyobject_plus.h"
#include "py_tuple.h"
#include "pyshadow_object.h"
#include "call_traits.h"
#include "../meta/if.h"
#include "../meta/type_2_type.h"
#include "../meta/type_list.h"

#define LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN_EX(v_errorReturnValue)\
	catch (lass::util::Exception& error)\
	{\
		std::ostringstream buffer;\
		buffer << error.message() << "\n\n(" << error.location() << ")";\
		PyErr_SetString(PyExc_Exception, buffer.str().c_str());\
		return v_errorReturnValue;\
	}\
	catch (std::exception& error)\
	{\
		PyErr_SetString(PyExc_Exception, error.what());\
		return v_errorReturnValue;\
	}

#define LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN\
	LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN_EX(0)

namespace lass
{
namespace python
{
namespace impl
{

// --- ArgumentTraits -----------------------------------------------------------------------------

template <typename T, bool isPyObject>
struct ArgTraitsHelper
{
	static T* get(const typename PyObjectPtr<T>::Type& iArg) { return iArg.get(); }
};

template <typename T>
struct ArgTraitsHelper<T, false>
{
	static const T* get(const T& iArg) { return &iArg; }
};

/** determines the right type for temporary storage of function arguments.
 */
template <typename T>
struct ArgumentTraits
{
	enum { isPyObject = IsPyObject<T>::value };
	typedef typename meta::If<isPyObject, meta::NullType, meta::Type2Type<T> >::Type TStorage;
	static const T& arg(const TStorage& iArg) { return *ArgTraitsHelper<T, isPyObject>::get(iArg); }
};

template <typename T>
struct ArgumentTraits<const T>
{
	enum { isPyObject = IsPyObject<T>::value };
	typedef typename meta::If<isPyObject, meta::NullType, meta::Type2Type<T> >::Type TStorage;
	static const T& arg(const TStorage& iArg) { return *ArgTraitsHelper<T, isPyObject>::get(iArg); }
};

template <typename T> struct ArgumentTraits<T&>
{
	enum { isPyObject = IsPyObject<T>::value };
	typedef typename meta::If<isPyObject, PyObjectPtr<T>, meta::NullType>::Type TStorage;
	static T& arg(const TStorage& iArg) { return *ArgTraitsHelper<T, isPyObject>::get(iArg); }
};

template <typename T>
struct ArgumentTraits<const T&>
{
	enum { isPyObject = IsPyObject<T>::value };
	typedef typename meta::If<isPyObject, PyObjectPtr<T>, meta::Type2Type<T> >::Type TStorage;
	static const T& arg(const TStorage& iArg) { return *ArgTraitsHelper<T, isPyObject>::get(iArg); }
};

template <typename T> struct ArgumentTraits<T*>
{
	enum { isPyObject = IsPyObject<T>::value };
	typedef typename meta::If<isPyObject, PyObjectPtr<T>, meta::NullType>::Type TStorage;
	static T* arg(const TStorage& iArg) { return ArgTraitsHelper<T, isPyObject>::get(iArg); }
};

template <typename T> struct ArgumentTraits<const T*>
{
	enum { isPyObject = IsPyObject<T>::value };
	typedef typename meta::If<isPyObject, PyObjectPtr<T>, meta::NullType>::Type TStorage;
	static const T* arg(const TStorage& iArg) { return ArgTraitsHelper<T, isPyObject>::get(iArg); }
};

template <typename T> struct ArgumentTraits<T* const>
{
	enum { isPyObject = IsPyObject<T>::value };
	typedef typename meta::If<isPyObject, PyObjectPtr<T>, meta::NullType>::Type TStorage;
	static T* arg(const TStorage& iArg) { return ArgTraitsHelper<T, isPyObject>::get(iArg); }
};

template <typename T> struct ArgumentTraits<const T* const>
{
	enum { isPyObject = IsPyObject<T>::value };
	typedef typename meta::If<isPyObject, PyObjectPtr<T>, meta::NullType>::Type TStorage;
	static const T* arg(const TStorage& iArg) { return ArgTraitsHelper<T, isPyObject>::get(iArg); }
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
	static PyObject* function( Function iFunction, $(P$x iP$x)$ )
	{
		try
		{
			return pyBuildSimpleObject( iFunction($(iP$x)$) );
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	}
	]$

	// method

	template <typename CppClass, typename Method>
	static PyObject* method( CppClass* iObject, Method iMethod )
	{
		try
		{
			return pyBuildSimpleObject( (iObject->*iMethod)() );
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	};
	$[
	template <typename CppClass, typename Method, $(typename P$x)$>
	static PyObject* method( CppClass* iObject, Method iMethod, $(P$x iP$x)$ )
	{
		try
		{
			return pyBuildSimpleObject( (iObject->*iMethod)($(iP$x)$) );
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
	static PyObject* function( Function iFunction, $(P$x iP$x)$ )
	{
		try
		{
			iFunction($(iP$x)$);
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
		Py_INCREF( Py_None );
		return Py_None;
	}
	]$

	// methods

	template <typename CppClass, typename Method>
	static PyObject* method( CppClass* iObject, Method iMethod )
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
	template <typename CppClass, typename Method, $(typename P$x)$>
	static PyObject* method( CppClass* iObject, Method iMethod, $(P$x iP$x)$ )
	{
		try
		{
			(iObject->*iMethod)($(iP$x)$);
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
PyObject* callFunction( PyObject* iArgs, R (*iFunction)() )
{
	if( decodeTuple(iArgs) != 0 )
	{
		return 0;
	}
	return Caller<R>::function( iFunction );
}
$[
/** calls C++ function with $x arguments, translated from python arguments
 */
template <typename R, $(typename P$x)$>
PyObject* callFunction( PyObject* iArgs, R (*iFunction)($(P$x)$) )
{
	$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
	)$
	if( decodeTuple<$(S$x)$>( iArgs, $(p$x)$ ) != 0 )
	{
		return 0;
	}
	return Caller<R>::function( iFunction, $(TArg$x::arg(p$x))$ );
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
	static PyObject* call( PyObject* iArgs, CppClass* iObject, R (C::*iMethod)() )
	{
		if( decodeTuple(iArgs) != 0 )
		{
			return 0;
		}
		return Caller<R>::method( iObject, iMethod );
	}
$[
	/** call non const method with $x arguments, translated from python arguments
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* call( PyObject* iArgs, CppClass* iObject, R (C::*iMethod)($(P$x)$) )
	{
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( iArgs, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		return Caller<R>::method( iObject, iMethod, $(TArg$x::arg(p$x))$ );
	}
]$

	// const methods

	/** call const method without arguments
	 */
	template <typename C, typename R>
	static PyObject* call( PyObject* iArgs, const CppClass* iObject, R (C::*iMethod)() const )
	{
		if( decodeTuple(iArgs) != 0 )
		{
			return 0;
		}
		return Caller<R>::method( iObject, iMethod );
	}
$[
	/** call const method with $x argument, translated from python arguments
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* call( PyObject* iArgs, const CppClass* iObject, R (C::*iMethod)($(P$x)$) const )
	{
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( iArgs, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		return Caller<R>::method( iObject, iMethod, $(TArg$x::arg(p$x))$ );
	}
]$
	// "free" non-const methods passing iObject as pointer

	/** call non const "free method" without arguments, passing iObject as pointer
	 */
	template <typename C, typename R>
	static PyObject* callFree( PyObject* iArgs, CppClass* iObject, R (*iFreeMethod)(C* iObject) )
	{
		if( decodeTuple(iArgs) != 0 )
		{
			return 0;
		}
		return Caller<R>::function( iFreeMethod, iObject );
	}
$[
	/** call non-const "free method" with $x arguments translated from python arguments, 
	 *  passing iObject as pointer
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* callFree( PyObject* iArgs, CppClass* iObject, R (*iFreeMethod)(C* iObject, $(P$x)$) )
	{
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( iArgs, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		return Caller<R>::function( iFreeMethod, iObject, $(TArg$x::arg(p$x))$ );
	}
]$

	// "free" non-const methods passing iObject as reference

	/** call non const "free method" without arguments, passing iObject as reference
	 */
	template <typename C, typename R>
	static PyObject* callFree( PyObject* iArgs, CppClass* iObject, R (*iFreeMethod)(C& iObject) )
	{
		if( decodeTuple(iArgs) != 0 )
		{
			return 0;
		}
		LASS_ASSERT(iObject);
		return Caller<R>::function( iFreeMethod, *iObject );
	}
$[
	/** call non-const "free method" with $x arguments translated from python arguments, 
	 *  passing iObject as reference
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* callFree( PyObject* iArgs, CppClass* iObject, R (*iFreeMethod)(C& iObject, $(P$x)$) )
	{
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( iArgs, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		LASS_ASSERT(iObject);
		return Caller<R>::function( iFreeMethod, *iObject, $(TArg$x::arg(p$x))$ );
	}
]$

	// "free" const methods passing iObject as pointer

	/** call const "free method" without arguments, passing iObject as pointer
	 */
	template <typename C, typename R>
	static PyObject* callFree( PyObject* iArgs, const CppClass* iObject, R (*iFreeMethod)(const C* iObject) )
	{
		if( decodeTuple(iArgs) != 0 )
		{
			return 0;
		}
		return Caller<R>::function( iFreeMethod, iObject );
	}
$[
	/** call const "free method" with $x arguments translated from python arguments, 
	 *  passing iObject as pointer
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* callFree( PyObject* iArgs, const CppClass* iObject, R (*iFreeMethod)(const C* iObject, $(P$x)$) )
	{
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( iArgs, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		return Caller<R>::function( iFreeMethod, iObject, $(TArg$x::arg(p$x))$ );
	}
]$

	// "free" const methods passing iObject as reference

	/** call non const "free method" without arguments, passing iObject as pointer
	 */
	template <typename C, typename R>
	static PyObject* callFree( PyObject* iArgs, const CppClass* iObject, R (*iFreeMethod)(const C& iObject) )
	{
		if( decodeTuple(iArgs) != 0 )
		{
			return 0;
		}
		LASS_ASSERT(iObject);
		return Caller<R>::function( iFreeMethod, *iObject );
	}
$[
	/** call const "free method" with $x arguments translated from python arguments, 
	 *  passing iObject as pointer
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* callFree( PyObject* iArgs, const CppClass* iObject, R (*iFreeMethod)(const C& iObject, $(P$x)$) )
	{
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( decodeTuple<$(S$x)$>( iArgs, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		LASS_ASSERT(iObject);
		return Caller<R>::function( iFreeMethod, *iObject, $(TArg$x::arg(p$x))$ );
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
	static int set( PyObject* iArgs, CppClass* iObject, void (CppClass::*iMethod)(P) )
	{
		typedef ArgumentTraits<P> TArg;
		typename TArg::TStorage p;

		if( pyGetSimpleObject( iArgs, p ) != 0 )
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
	static int set( PyObject* iArgs, CppClass* iObject, P& (CppClass::*iMethod)() )
	{
		typedef ArgumentTraits<P> TArg;
		typename TArg::TStorage p;

		if( pyGetSimpleObject( iArgs, p ) != 0 )
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
PyObject* construct( PyTypeObject* iSubtype, PyObject* iArgs )
{
	typedef ShadowTraits< PyObjectClass > TPyShadowTraits;
	typedef typename TPyShadowTraits::TCppClass TCppClass;
	
	if( decodeTuple(iArgs) != 0 )
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
#pragma LASS_TODO("Check if we have created memory leak... yes we have :-/, read the comments here.")
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
PyObject* construct( PyTypeObject* iSubtype, PyObject* iArgs )
{
	typedef ShadowTraits< PyObjectClass > TPyShadowTraits;
	typedef typename TPyShadowTraits::TCppClass TCppClass;
	$(typedef ArgumentTraits< P$x > TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
	)$

	if ( decodeTuple<$(S$x)$>( iArgs, $(p$x)$ ) != 0 )
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
		static PyObject* callFunction( PyObject* iArgs, R (*iFunction)() )
		{
			return ::lass::python::impl::callFunction<R>( iArgs, iFunction );
		}
		static PyObject* callMethod( PyObject* iArgs, CppClass* iObject, R (CppClass::*iMethod)() )
		{
			return CallMethod<CppClass>::call( iArgs, iObject, iMethod );
		}
		static PyObject* callMethod( PyObject* iArgs, const CppClass* iObject, R (CppClass::*iMethod)() const )
		{
			return CallMethod<CppClass>::call( iArgs, iObject, iMethod );
		}
		static PyObject* callConstructor( PyTypeObject* iSubtype, PyObject* iArgs )
		{
			return construct<CppClass>( iSubtype, iArgs );
		}
	};
	typedef Impl TImpl;
};
$[
template <class CppClass, typename R, $(typename P$x)$>
struct ExplicitResolver$x<CppClass, R, $(P$x)$, lass::meta::NullType>
{
	struct Impl
	{
		static PyObject* callFunction( PyObject* iArgs, R (*iFunction)($(P$x)$) )
		{
			return ::lass::python::impl::callFunction<R, $(P$x)$>( iArgs, iFunction );
		}
		static PyObject* callMethod( PyObject* iArgs, CppClass* iObject, R (CppClass::*iMethod)($(P$x)$) )
		{
			return CallMethod<CppClass>::call( iArgs, iObject, iMethod );
		}
		static PyObject* callMethod( PyObject* iArgs, const CppClass* iObject, R (CppClass::*iMethod)($(P$x)$) const )
		{
			return CallMethod<CppClass>::call( iArgs, iObject, iMethod );
		}
		static PyObject* callFreeMethod( PyObject* iArgs, CppClass* iObject, R (*iFreeMethod)($(P$x)$) )
		{
			return CallMethod<CppClass>::callFree( iArgs, iObject, iFreeMethod );
		}
		static PyObject* callConstructor( PyTypeObject* iSubtype, PyObject* iArgs )
		{
			return construct<CppClass, $(P$x)$>( iSubtype, iArgs );
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


#endif

// EOF
