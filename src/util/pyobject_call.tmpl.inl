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
 *
 *  $g
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_CALL_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_CALL_INL

#include "util_common.h"
#include "call_traits.h"
#include "../meta/if.h"
#include "../meta/type_2_type.h"
#include "../meta/type_list.h"

#define LASS_UTIL_PYOBJECT_CALL_TRY_EX(e_expression, v_errorReturnValue)\
    try\
    {\
        e_expression;\
    }\
    catch (std::exception& error)\
    {\
        PyErr_SetString(PyExc_Exception, error.what());\
        return v_errorReturnValue;\
    }

#define LASS_UTIL_PYOBJECT_CALL_TRY(e_expression)\
    LASS_UTIL_PYOBJECT_CALL_TRY_EX(e_expression, 0)
    
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




// --- getParameters -------------------------------------------------------------------------------
$[
/** translates a tuple of $x PyObjects to C++ objects.
 *  @bug what happens if arguments don't match
 *	     [TDM] then the function returns a 0 resulting a NULL return to Python
 *			   this in return triggers an exception with as value the last error
 *			   encountered which will be a conversion error
 */
template <$(typename P$x)$>
bool getArguments( PyObject* iArgs, $( P$x& oP$x)$ )
{
	PyObject $(*p$x)$;

	if (!PyTuple_Check(iArgs))
    {
        PyErr_SetString(PyExc_TypeError, "lass internal error: iArgs isn't a tuple");
        return false;
    }


    if (!PyArg_UnpackTuple( iArgs, "", $x, $x, $(&p$x)$ ))
    {
        return false;
    }

    $(if (pyGetSimpleObject( p$x, oP$x ) != 0)
    {
		impl::addMessageHeader("Bad Argument on $xth position");
        return false;
	}
    )$
	return true;
}
]$


// --- actual callers ------------------------------------------------------------------------------

/** calls the actual function with provided parameters, and returns result as a PyObject pointer.
 */
template <typename R>
struct Caller
{
    // free function

	static PyObject* function( R (*iFunction)() )
	{
        LASS_UTIL_PYOBJECT_CALL_TRY( return pyBuildSimpleObject( (*iFunction)() ) )
	};
	$[
	template <$(typename P$x)$>
	static PyObject* function( R (*iFunction)($(P$x)$), 
                             $(typename lass::util::CallTraits<P$x>::TParam iP$x)$ )
	{
        LASS_UTIL_PYOBJECT_CALL_TRY( return pyBuildSimpleObject( (*iFunction)($(iP$x)$) ) )
	}
	]$

	// method

	template <class CppClass>
	static PyObject* method( CppClass* iObject, R (CppClass::*iMethod)() )
	{
        LASS_UTIL_PYOBJECT_CALL_TRY( return pyBuildSimpleObject( (iObject->*iMethod)() ) )
	};
	$[
	template <class CppClass, $(typename P$x)$>
	static PyObject* method( CppClass* iObject, R (CppClass::*iMethod)($(P$x)$), 
                             $(typename util::CallTraits<P$x>::TParam iP$x)$ )
	{
        LASS_UTIL_PYOBJECT_CALL_TRY( return pyBuildSimpleObject( (iObject->*iMethod)($(iP$x)$) ) )
	}
	]$

	// const method

	template <class CppClass>
	static PyObject* method( const CppClass* iObject, R (CppClass::*iMethod)() const )
	{
        LASS_UTIL_PYOBJECT_CALL_TRY( return pyBuildSimpleObject( (iObject->*iMethod)() ) )
	};
	$[
	template <class CppClass, $(typename P$x)$>
	static PyObject* method( const CppClass* iObject, R (CppClass::*iMethod)($(P$x)$) const, 
                             $(typename util::CallTraits<P$x>::TParam iP$x)$ )
	{
        LASS_UTIL_PYOBJECT_CALL_TRY( return pyBuildSimpleObject( (iObject->*iMethod)($(iP$x)$) ) )
	}
	]$

};

/** specialisation for functions with return value, alls function and returns Py_None. 
 */
template <>
struct Caller<void>
{
    // free functions

	static PyObject* function( void (*iFunction)() )
	{
        LASS_UTIL_PYOBJECT_CALL_TRY( (*iFunction)() )
		Py_INCREF( Py_None );
		return Py_None;
	};
	$[
	template <$(typename P$x)$>
	static PyObject* function( void (*iFunction)($(P$x)$), 
                             $(typename util::CallTraits<P$x>::TParam iP$x)$ )
	{
		LASS_UTIL_PYOBJECT_CALL_TRY( (*iFunction)($(iP$x)$) )
		Py_INCREF( Py_None );
		return Py_None;
	}
	]$

    // methods

	template <class CppClass>
	static PyObject* method( CppClass* iObject, void (CppClass::*iMethod)() )
	{
		LASS_UTIL_PYOBJECT_CALL_TRY( (iObject->*iMethod)() )
		Py_INCREF( Py_None );
		return Py_None;
	};
	$[
	template <class CppClass, $(typename P$x)$>
	static PyObject* method( CppClass* iObject, void (CppClass::*iMethod)($(P$x)$), 
                             $(typename lass::util::CallTraits<P$x>::TParam iP$x)$ )
	{
		LASS_UTIL_PYOBJECT_CALL_TRY( (iObject->*iMethod)($(iP$x)$) )
		Py_INCREF( Py_None );
		return Py_None;
	}
	]$

	// const method

    template <class CppClass>
	static PyObject* method( const CppClass* iObject, void (CppClass::*iMethod)() const )
	{
		LASS_UTIL_PYOBJECT_CALL_TRY( (iObject->*iMethod)() )
		Py_INCREF( Py_None );
		return Py_None;
	};
	$[
	template <class CppClass, $(typename P$x)$>
	static PyObject* method( const CppClass* iObject, void (CppClass::*iMethod)($(P$x)$) const, 
                             $( typename lass::util::CallTraits<P$x>::TParam iP$x)$ )
	{
		LASS_UTIL_PYOBJECT_CALL_TRY( (iObject->*iMethod)($(iP$x)$) )
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
	if ( !checkSequenceSize(iArgs, 0) )
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
	if( !getArguments<$(S$x)$>( iArgs, $(p$x)$ ) )
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
	template <typename R>
	static PyObject* call( PyObject* iArgs, CppClass* iObject, R (CppClass::*iMethod)() )
	{
		if ( !checkSequenceSize( iArgs, 0 ) )
		{
			return 0;
		}
		return Caller<R>::method( iObject, iMethod );
	}
$[
	/** call non const method with $x arguments, translated from python arguments
	 */
	template <typename R, $(typename P$x)$>
	static PyObject* call( PyObject* iArgs, CppClass* iObject, R (CppClass::*iMethod)($(P$x)$) )
	{
        $(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( !getArguments<$(S$x)$>( iArgs, $(p$x)$ ) )
		{
			return 0;
		}
		return Caller<R>::method( iObject, iMethod, $(TArg$x::arg(p$x))$ );
	}
]$

	// const methods

	/** call const method without arguments
	 */
	template <typename R>
	static PyObject* call( PyObject* iArgs, const CppClass* iObject, R (CppClass::*iMethod)() const )
	{
		if ( !checkSequenceSize( iArgs, 0 ) )
		{
			return 0;
		}
		return Caller<R>::method( iObject, iMethod );
	}
$[
	/** call const method with $x argument, translated from python arguments
	 */
	template <typename R, $(typename P$x)$>
	static PyObject* call( PyObject* iArgs, const CppClass* iObject, R (CppClass::*iMethod)($(P$x)$) const )
	{
        $(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if( !getArguments<$(S$x)$>( iArgs, $(p$x)$ ) )
		{
			return 0;
		}
		return Caller<R>::method( iObject, iMethod, $(TArg$x::arg(p$x))$ );
	}
]$

	// member getters and setters

	/** call const getter function
	 */
	template <typename R>
	static PyObject* get( const CppClass* iObject, R (CppClass::*iMethod)() const)
	{
		LASS_UTIL_PYOBJECT_CALL_TRY( return pyBuildSimpleObject((iObject->*iMethod)()) )
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

        LASS_UTIL_PYOBJECT_CALL_TRY_EX( (iObject->*iMethod)(TArg::arg(p)), -1 )
		Py_INCREF( Py_None );
		return 0;
	}

	/** call implicit setter function like <tt>Bar& Foo::bar()</tt>
     *
     *  kind of broken.
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
		
        LASS_UTIL_PYOBJECT_CALL_TRY_EX( (iObject->*iMethod)() = TArg::arg(p), -1 )
		Py_INCREF( Py_None );
		return 0;
	}
};




// --- constructors --------------------------------------------------------------------------------

/** allocate a new object with default constructor.
 */
template <class PyObjectClass>
PyObject* construct( PyObject* iArgs )
{
    typedef ShadowTraits<PyObjectClass> TPyShadowTraits;
    typedef typename TPyShadowTraits::TCppClass TCppClass;
	LASS_UTIL_PYOBJECT_CALL_TRY
	(
        PyObjectClass* result = TPyShadowTraits::pyObject( new TCppClass ); 
        result->ob_type = &PyObjectClass::Type; 
        return result;
	)
}
$[
/** allocate a new object with $x arguments.
 */
template <class PyObjectClass, $(typename P$x)$>
PyObject* construct( PyObject* iArgs )
{
    typedef ShadowTraits<PyObjectClass> TPyShadowTraits;
    typedef typename TPyShadowTraits::TCppClass TCppClass;
    $(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
	)$

	if ( !getArguments<$(S$x)$>( iArgs, $(p$x)$ ) )
	{
		return 0;
	}

	LASS_UTIL_PYOBJECT_CALL_TRY
	(
        PyObjectClass* result = TPyShadowTraits::pyObject( new TCppClass( $(TArg$x::arg(p$x))$ ) ); 
        result->ob_type = &PyObjectClass::Type; 
        return result;
	)
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
        static PyObject* callConstructor( PyObject* iArgs ) 
		{ 
			return construct<CppClass>( iArgs ); 
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
        static PyObject* callConstructor( PyObject* iArgs ) 
		{ 
			return construct<CppClass, $(P$x)$>( iArgs ); 
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
