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


 *  @date 2004
 *
 *  @par last commit:
 *		$Revision$\n
 *		$Date$\n
 *		$Author$
 *
 *  $g
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_CALL_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_CALL_INL

#include "util_common.h"
#include "call_traits.h"
#include "../meta/type_traits.h"
#include "../meta/type_list.h"

namespace lass
{
namespace python
{
namespace impl
{


// --- pyGetParameters -----------------------------------------------------------------------------
$[
/** translates a tuple of $x PyObjects to C++ objects.
 *  @bug what happens if arguments don't match
 *	     [TDM] then the function returns a 0 resulting a NULL return to Python
 *			   this in return triggers an exception with as value the last error
 *			   encountered which will be a conversion error
 */
template <$(typename P$x)$>
int pyGetArguments( PyObject* iArgs, $( typename util::CallTraits<P$x>::TReference oP$x)$  )
{
	int result = 0;
	PyObject $(*p$x)$;

	// check for tuple and correct size
	if (PyTuple_Check(iArgs)) 
	{
		result = PyArg_UnpackTuple( iArgs, "", $x, $x, $(&p$x)$ );	
		$(if (result) result = !pyGetSimpleObject( p$x, oP$x );
		)$
	}
	return result;
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
		return pyBuildSimpleObject( (*iFunction)() );
	};
	$[
	template <$(typename P$x)$>
	static PyObject* function( R (*iFunction)($(P$x)$), 
                             $(typename util::CallTraits<P$x>::TParam iP$x)$ )
	{
		return pyBuildSimpleObject( (*iFunction)($(iP$x)$) );
	}
	]$

	// method

	template <class CppClass>
	static PyObject* method( CppClass* iObject, R (CppClass::*iMethod)() )
	{
		return pyBuildSimpleObject( (iObject->*iMethod)() );
	};
	$[
	template <class CppClass, $(typename P$x)$>
	static PyObject* method( CppClass* iObject, R (CppClass::*iMethod)($(P$x)$), 
                             $(typename util::CallTraits<P$x>::TParam iP$x)$ )
	{
		return pyBuildSimpleObject( (iObject->*iMethod)($(iP$x)$) );
	}
	]$

	// const method

	template <class CppClass>
	static PyObject* method( CppClass* iObject, R (CppClass::*iMethod)() const )
	{
		return pyBuildSimpleObject( (iObject->*iMethod)() );
	};
	$[
	template <class CppClass, $(typename P$x)$>
	static PyObject* method( CppClass* iObject, R (CppClass::*iMethod)($(P$x)$) const, 
                             $(typename util::CallTraits<P$x>::TParam iP$x)$ )
	{
		return pyBuildSimpleObject( (iObject->*iMethod)($(iP$x)$) );
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
		(*iFunction)();
		Py_INCREF( Py_None );
		return Py_None;
	};
	$[
	template <$(typename P$x)$>
	static PyObject* function( void (*iFunction)($(P$x)$), 
                             $(typename util::CallTraits<P$x>::TParam iP$x)$ )
	{
		(*iFunction)($(iP$x)$);
		Py_INCREF( Py_None );
		return Py_None;
	}
	]$

    // methods

	template <class CppClass>
	static PyObject* method( CppClass* iObject, void (CppClass::*iMethod)() )
	{
		(iObject->*iMethod)();
		Py_INCREF( Py_None );
		return Py_None;
	};
	$[
	template <class CppClass, $(typename P$x)$>
	static PyObject* method( CppClass* iObject, void (CppClass::*iMethod)($(P$x)$), 
                             $(typename util::CallTraits<P$x>::TParam iP$x)$ )
	{
		(iObject->*iMethod)($(iP$x)$);
		Py_INCREF( Py_None );
		return Py_None;
	}
	]$

	// const method

	template <class CppClass>
	static PyObject* method( CppClass* iObject, void (CppClass::*iMethod)() const )
	{
		(iObject->*iMethod)();
		Py_INCREF( Py_None );
		return Py_None;
	};
	$[
	template <class CppClass, $(typename P$x)$>
	static PyObject* method( CppClass* iObject, void (CppClass::*iMethod)($(P$x)$) const, 
                             $( typename util::CallTraits<P$x>::TParam iP$x)$ )
	{
		(iObject->*iMethod)($(iP$x)$);
		Py_INCREF( Py_None );
		return Py_None;
	}
	]$
};



// --- free functions ------------------------------------------------------------------------------

/** calls C++ function without arguments
 */
template <typename R>
PyObject* pyCallFunction( PyObject* iArgs, R (*iFunction)() )
{
    return Caller<R>::function( iFunction );
}
$[
/** calls C++ function with $x arguments, translated from python arguments
 */
template <typename R, $(typename P$x)$>
PyObject* pyCallFunction( PyObject* iArgs, R (*iFunction)($(P$x)$) )
{
	$(typedef typename meta::TypeTraits<P$x>::TStorage T$x; T$x p$x;
	)$
	if( pyGetArguments<$(T$x)$>( iArgs, $(p$x)$ ) )
	{
		return Caller<R>::function( iFunction, $(p$x)$ );
	}
	return NULL; // make the exception explicit
}
]$


// --- methods -------------------------------------------------------------------------------------

template <class CppClass>
struct PyCallMethod
{
	// non const methods

	/** call non const method without arguments
	 */
	template <typename R>
	static PyObject* call( PyObject* iArgs, CppClass* iObject, R (CppClass::*iMethod)() )
	{
		// check if the caller in python really didn't provide any arguments
		if ( PyTuple_Check( iArgs ) && PyTuple_Size( iArgs ) == 0)
			return Caller<R>::method<CppClass>( iObject, iMethod );
		return NULL;
	}
$[
	/** call non const method with $x arguments, translated from python arguments
	 */
	template <typename R, $(typename P$x)$>
	static PyObject* call( PyObject* iArgs, CppClass* iObject, R (CppClass::*iMethod)($(P$x)$) )
	{
		$(typedef meta::TypeTraits<P$x>::TStorage T$x;
		)$
		$(T$x p$x;
		)$
		if( pyGetArguments<$(T$x)$>( iArgs, $(p$x)$ ) )
		{
			return Caller<R>::method<CppClass, $(P$x)$>( iObject, iMethod, $(p$x)$ );
		}
		return NULL;	// make the exception explicit
	}
]$

	// const methods

	/** call const method without arguments
	 */
	template <typename R>
	static PyObject* call( PyObject* iArgs, CppClass* iObject, R (CppClass::*iMethod)() const )
	{
		return Caller<R>::method<CppClass>( iObject, iMethod );
	}
$[
	/** call const method with $x argument, translated from python arguments
	 */
	template <typename R, $(typename P$x)$>
	static PyObject* call( PyObject* iArgs, CppClass* iObject, R (CppClass::*iMethod)($(P$x)$) const )
	{
		$(typedef meta::TypeTraits<P$x>::TStorage T$x;
		)$
		$(T$x p$x;
		)$
		if( pyGetArguments<$(T$x)$>( iArgs, $(p$x)$ ) )
		{
			return Caller<R>::method<CppClass, $(P$x)$>( iObject, iMethod, $(p$x)$ );
		}
		return NULL;	// make the exception explicit
	}
]$

	// member getters and setters

	/** call const getter function
	 */
	template <typename R>
	static PyObject* get( const CppClass* iObject, R (CppClass::*iMethod)() const)
	{
		return pyBuildSimpleObject((iObject->*iMethod)());
	}

	/** call explicit setter function like <tt>void Foo::setBar(const Bar& iBar)</tt>
	 */
	template <typename P>
	static PyObject* set( PyObject* iArgs, CppClass* iObject, void (CppClass::*iMethod)(P) )
	{
		typedef meta::TypeTraits<P>::TStorage T;
		T p;
		
		if( !pyGetSimpleObject( iArgs, p ) )
		{
			(iObject->*iMethod)(p);
			Py_INCREF( Py_None );
			return Py_None;
		}
		return NULL;	// make the exception explicit
	}

	/** call implicit setter function like <tt>Bar& Foo::bar()</tt>
     *
     *  kind of broken.
	 */
	template <typename P>
	static PyObject* set( PyObject* iArgs, CppClass* iObject, P (CppClass::*iMethod)() )
	{
		typedef meta::TypeTraits<P>::TStorage T;
		T p;
		
		if( !pyGetSimpleObject( iArgs, p ) )
		{
			(iObject->*iMethod)() = p;
			Py_INCREF( Py_None );
			return Py_None;
		}
		return NULL;	// make the exception explicit
	}
};



// --- constructors --------------------------------------------------------------------------------

/** allocate a new object with default constructor.
 */
template <class CppClass>
PyObject* pyConstruct( PyObject* iArgs )
{
    try
    {
        PyObject* result = new CppClass(); 
        result->ob_type = &CppClass::Type; 
        return result;
    }
    catch(...)
    {
        return 0;
    }
}
$[
/** allocate a new object with $x arguments.
 */
template <class CppClass, $(typename P$x)$>
PyObject* pyConstruct( PyObject* iArgs )
{
	$(typedef meta::TypeTraits<P$x>::TStorage T$x; T$x p$x;
	)$
    try
    {
	    if( !pyGetArguments<$(T$x)$>( iArgs, $(p$x)$ ) )
        {
            return 0;
        }
        PyObject* result = new CppClass( $(p$x)$ ); 
        result->ob_type = &CppClass::Type; 
        return result;
    }
    catch(...)
    {
        return 0;
    }
}
]$


template <class CppClass, typename TypeListType> struct PyConstructor;
$[template <class CppClass, $(typename P$x)$, typename TypeListType> struct PyConstructor$x;
]$
template <class CppClass, $(typename P$x)$, typename P$y, typename TypeListType> struct PyConstructor$y;


template <class CppClass>
struct PyConstructor<CppClass, meta::NullType>
{
    struct Impl
    {
        static PyObject* construct( PyObject* iArgs ) { return pyConstruct<CppClass>( iArgs ); }
    };
    typedef Impl TImpl;
};
$[
template <class CppClass, $(typename P$x)$>
struct PyConstructor$x<CppClass, $(P$x)$, meta::NullType>
{
    struct Impl
    {
        static PyObject* construct( PyObject* iArgs ) { return pyConstruct<CppClass, $(P$x)$>( iArgs ); }
    };
    typedef Impl TImpl;
};
]$

template <class CppClass, typename Head, typename Tail>
struct PyConstructor<CppClass, meta::TypeList<Head, Tail> >
{
    typedef typename PyConstructor1<CppClass, Head, Tail>::TImpl TImpl;
};
$[
template <class CppClass, $(typename P$x)$, typename Head, typename Tail>
struct PyConstructor$x<CppClass, $(P$x)$, meta::TypeList<Head, Tail> >
{
    typedef typename PyConstructor$y<CppClass, $(P$x)$, Head, Tail>::TImpl TImpl;
};
]$
}

}

}


#endif

/*
 * $Log$
 * Revision 1.25  2004/09/06 13:24:47  tgdemuer
 * *** empty log message ***
 *
 * Revision 1.24  2004/09/06 13:08:44  tgdemuer
 * *** empty log message ***
 *
 * Revision 1.23  2004/08/12 11:47:42  tgdemuer
 * overloaded member methods working
 *
 * Revision 1.22  2004/08/12 09:17:04  tgdemuer
 * pyCheckArguments bypassed, this means that pyGetSimpleObject should check first and that pyCallArguments should have an early out
 *
 * Revision 1.21  2004/08/11 16:02:35  tgdemuer
 * various
 * overloaded functions for python
 *
 * Revision 1.20  2004/07/27 15:31:21  bdegreve
 * adding constructor support up to N arguments.
 * PY_ABSTRACT_HEADER and PY_DECLARE_ABSTRACT_CLASS are gone.
 * all pythonized classes are not constructable by default.
 * Simply use PY_HEADER and PY_DECLARE_CLASS.
 *
 * Revision 1.19  2004/06/28 14:20:39  tgdemuer
 * VS2003 migration
 *
 * Revision 1.18  2004/06/22 21:04:00  bdegreve
 * fixing project configuration
 * (PS: PY_CLASS_MEMBER_RW is broken on intel for fancy getter/setters)
 *
 * Revision 1.17  2004/06/22 14:11:11  bdegreve
 * PY_CLASS_MEMBER_RW_EX accepts both kind of setters now: void Bar::setFoo(const Foo&) and Foo& Bar::foo()
 *
 */

// EOF
