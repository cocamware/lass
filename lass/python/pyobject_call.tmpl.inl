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
 *	Copyright (C) 2004-2009 the Initial Developer.
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
#include "argument_traits.h"
#include "exception.h"
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



// --- actual callers ------------------------------------------------------------------------------

/** calls the actual function with provided parameters, and returns result as a PyObject pointer.
 */
template <typename R>
struct Caller
{
	// free function

	template <typename Function>
	static PyObject* callFunction( Function function )
	{
		try
		{
			return pyBuildSimpleObject( function() );
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	};
	$[
	template <typename Function, $(typename P$x)$>
	static PyObject* callFunction( Function function, $(P$x p$x)$ )
	{
		try
		{
			return pyBuildSimpleObject( function($(p$x)$) );
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	}
	]$

	// method

	template <typename CppClassRef, typename Method>
	static PyObject* callMethod( CppClassRef object, Method method )
	{
		try
		{
			return pyBuildSimpleObject( (object.*method)() );
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	};
	$[
	template <typename CppClassRef, typename Method, $(typename P$x)$>
	static PyObject* callMethod( CppClassRef object, Method method, $(P$x p$x)$ )
	{
		try
		{
			return pyBuildSimpleObject( (object.*method)($(p$x)$) );
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
	static PyObject* callFunction( Function function )
	{
		try
		{
			function();
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
		Py_RETURN_NONE;
	};
	$[
	template <typename Function, $(typename P$x)$>
	static PyObject* callFunction( Function function, $(P$x p$x)$ )
	{
		try
		{
			function($(p$x)$);
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
		Py_RETURN_NONE;
	}
	]$

	// methods

	template <typename CppClassRef, typename Method>
	static PyObject* callMethod( CppClassRef object, Method method )
	{
		try
		{
			(object.*method)();
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
		Py_RETURN_NONE;
	};
	$[
	template <typename CppClassRef, typename Method, $(typename P$x)$>
	static PyObject* callMethod( CppClassRef object, Method method, $(P$x p$x)$ )
	{
		try
		{
			(object.*method)($(p$x)$);
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
		Py_RETURN_NONE;
	}
	]$
};



// --- free functions ------------------------------------------------------------------------------

/** calls C++ function without arguments
 */
template <typename R>
PyObject* callFunction( PyObject* args, R (*function)() )
{
	typedef R(*TFunction)();
	if( decodeTuple(args) != 0 )
	{
		return 0;
	}
	return Caller<R>::template callFunction<TFunction>( function );
}
$[
/** calls C++ function with $x arguments, translated from python arguments
 */
template <typename R, $(typename P$x)$>
PyObject* callFunction( PyObject* args, R (*function)($(P$x)$) )
{
	typedef R (*TFunction)($(P$x)$);
	$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
	)$
	if( decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
	{
		return 0;
	}
	return Caller<R>::template callFunction<TFunction, $(P$x)$>( 
		function, $(TArg$x::arg(p$x))$ );
}
]$



// --- methods -------------------------------------------------------------------------------------

template <typename ShadowTraits>
struct CallMethod
{
	// non const methods

	/** call non const method without arguments
	 */
	template <typename C, typename R>
	static PyObject* call( PyObject* args, PyObject* object, R (C::*method)() )
	{
		typedef typename ShadowTraits::TCppClass TCppClass;
		typedef typename ShadowTraits::TCppClassPtr TCppClassPtr;
		typedef R (C::*TMethod)();
		TCppClassPtr self;
		if ( ShadowTraits::getObject(object, self) != 0 || decodeTuple(args) != 0 )
		{
			return 0;
		}
		return Caller<R>::template callMethod<TCppClass&, TMethod>(
			*self, method);
	}
$[
	/** call non const method with $x arguments, translated from python arguments
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* call( PyObject* args, PyObject* object, R (C::*method)($(P$x)$) )
	{
		typedef typename ShadowTraits::TCppClass TCppClass;
		typedef typename ShadowTraits::TCppClassPtr TCppClassPtr;
		typedef R (C::*TMethod)($(P$x)$);
		TCppClassPtr self;
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if ( ShadowTraits::getObject(object, self) != 0 || decodeTuple<$(S$x)$>(args, $(p$x)$) != 0 )
		{
			return 0;
		}
		return Caller<R>::template callMethod<TCppClass&, TMethod, $(P$x)$>( 
			*self, method, $(TArg$x::arg(p$x))$ );
	}
]$

	// const methods

	/** call const method without arguments
	 */
	template <typename C, typename R>
	static PyObject* call( PyObject* args, PyObject* object, R (C::*method)() const )
	{
		typedef typename ShadowTraits::TCppClass TCppClass;
		typedef typename ShadowTraits::TConstCppClassPtr TConstCppClassPtr;
		typedef R (C::*TMethod)() const;
		TConstCppClassPtr self;
		if ( ShadowTraits::getObject(object, self) != 0 || decodeTuple(args) != 0 )
		{
			return 0;
		}
		return Caller<R>::template callMethod<const TCppClass&, TMethod>( 
			*self, method);
	}
$[
	/** call const method with $x argument, translated from python arguments
	 */
	template <typename C, typename R, $(typename P$x)$>
	static PyObject* call( PyObject* args, PyObject* object, R (C::*method)($(P$x)$) const )
	{
		typedef typename ShadowTraits::TCppClass TCppClass;
		typedef typename ShadowTraits::TConstCppClassPtr TConstCppClassPtr;
		typedef R (C::*TMethod)($(P$x)$) const;
		TConstCppClassPtr self;
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if ( ShadowTraits::getObject(object, self) != 0 || decodeTuple<$(S$x)$>(args, $(p$x)$) != 0 )
		{
			return 0;
		}
		return Caller<R>::template callMethod<const TCppClass&, TMethod, $(P$x)$>(
			*self, method, $(TArg$x::arg(p$x))$ );
	}
]$
	// "free" methods passing object as first argument

	/** call "free method" without arguments, passing object as first argument
	 */
	template <typename R>
	static PyObject* callFree( PyObject* args, PyObject* object, R (*freeMethod)(typename ShadowTraits::TCppClass&) )
	{
		typedef typename ShadowTraits::TCppClass TCppClass;
		typedef typename ShadowTraits::TCppClassPtr TCppClassPtr;
		typedef R (*TFunction)(TCppClass&);
		TCppClassPtr self;
		if ( ShadowTraits::getObject(object, self) != 0 || decodeTuple(args) != 0 )
		{
			return 0;
		}
		return Caller<R>::template callFunction<TFunction, TCppClass&>(
			freeMethod, *self );
	}
$[
	/** call "free method" with $x arguments translated from python arguments, passing object as first argument
	 */
	template <typename R, $(typename P$x)$>
	static PyObject* callFree( PyObject* args, PyObject* object, R (*freeMethod)(typename ShadowTraits::TCppClass&, $(P$x)$) )
	{
		typedef typename ShadowTraits::TCppClass TCppClass;
		typedef typename ShadowTraits::TCppClassPtr TCppClassPtr;
		typedef R (*TFunction)(TCppClass&, $(P$x)$);
		TCppClassPtr self;
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if ( ShadowTraits::getObject(object, self) != 0 || decodeTuple<$(S$x)$>(args, $(p$x)$) != 0 )
		{
			return 0;
		}
		return Caller<R>::template callFunction<TFunction, TCppClass&, $(P$x)$>(
			freeMethod, *self, $(TArg$x::arg(p$x))$ );
	}
]$

	/** call "free method" without arguments, passing object as first argument
	 */
	template <typename R>
	static PyObject* callFree( PyObject* args, PyObject* object, R (*freeMethod)(const typename ShadowTraits::TCppClass&) )
	{
		typedef typename ShadowTraits::TCppClass TCppClass;
		typedef typename ShadowTraits::TConstCppClassPtr TConstCppClassPtr;
		typedef R (*TFunction)(const TCppClass&);
		TConstCppClassPtr self;
		if ( ShadowTraits::getObject(object, self) != 0 || decodeTuple(args) != 0 )
		{
			return 0;
		}
		return Caller<R>::template callFunction<TFunction, const TCppClass&>(
			freeMethod, *self );
	}
$[
	/** call "free method" with $x arguments translated from python arguments, passing object as first argument
	 */
	template <typename R, $(typename P$x)$>
	static PyObject* callFree( PyObject* args, PyObject* object, R (*freeMethod)(const typename ShadowTraits::TCppClass&, $(P$x)$) )
	{
		typedef typename ShadowTraits::TCppClass TCppClass;
		typedef typename ShadowTraits::TConstCppClassPtr TConstCppClassPtr;
		typedef R (*TFunction)(const TCppClass&, $(P$x)$);
		TConstCppClassPtr self;
		$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
		)$
		if ( ShadowTraits::getObject(object, self) != 0 || decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
		{
			return 0;
		}
		return Caller<R>::template callFunction<TFunction, const TCppClass&, $(P$x)$>(
			freeMethod, *self, $(TArg$x::arg(p$x))$ );
	}
]$

	// member getters and setters

	/** call const getter function
	 */
	template <typename C, typename R>
	static PyObject* get(PyObject* object, R (C::*method)() const)
	{
		typename ShadowTraits::TConstCppClassPtr self;
		if (ShadowTraits::getObject(object, self) != 0)
		{
			return 0;
		}
		try
		{
			return pyBuildSimpleObject(((*self).*method)());
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	}

	/** call explicit setter function like <tt>void Foo::setBar(const Bar& iBar)</tt>
	 */
	template <typename C, typename P>
	static int set( PyObject* args, PyObject* object, void (C::*method)(P) )
	{
		typename ShadowTraits::TCppClassPtr self;
		typedef ArgumentTraits<P> TArg; typename TArg::TStorage p;
		if (ShadowTraits::getObject(object, self) != 0 || pyGetSimpleObject(args, p) != 0)
		{
			return -1;
		}
		try
		{
			((*self).*method)(TArg::arg(p));
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN_EX(-1)
		return 0;
	}

	/** call implicit setter function like <tt>Bar& Foo::bar()</tt>
	 */
	template <typename C, typename P>
	static int set( PyObject* args, PyObject* object, P& (C::*method)() )
	{
		typename ShadowTraits::TCppClassPtr self;
		typedef ArgumentTraits<P> TArg;	typename TArg::TStorage p;
		if(ShadowTraits::getObject(object, self) != 0 || pyGetSimpleObject(args, p) != 0)
		{
			return -1;
		}
		try
		{
			((*self).*method)() = TArg::arg(p);
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN_EX(-1)
		return 0;
	}

	template <typename C, typename R>
	static PyObject* freeGet(PyObject* object, R (*function)(const C&) )
	{
		typename ShadowTraits::TConstCppClassPtr self;
		if (ShadowTraits::getObject(object, self) != 0)
		{
			return 0;
		}
		try
		{
			return pyBuildSimpleObject(function(*self));
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
	}

	template <typename C, typename P>
	static int freeSet( PyObject* args, PyObject* object, void (*function)(C&, P) )
	{
		typename ShadowTraits::TCppClassPtr self;
		typedef ArgumentTraits<P> TArg;	typename TArg::TStorage p;
		if(ShadowTraits::getObject(object, self) != 0 || pyGetSimpleObject(args, p) != 0)
		{
			return -1;
		}
		try
		{
			function(*self, p);
		}
		LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN_EX(-1)
		return 0;
	}
};



// --- constructors --------------------------------------------------------------------------------

/** allocate a new object with default constructor.
 */
template <typename ShadowTraits>
PyObject* construct( PyTypeObject* subType, PyObject* args )
{
	typedef typename ShadowTraits::TCppClass TCppClass;
	typedef typename ShadowTraits::TCppClassPtr TCppClassPtr;
	typedef typename ShadowTraits::TPyClassPtr TPyClassPtr;
	
	if( decodeTuple(args) != 0 )
	{
		return 0;
	}
	
	try
	{
		const TCppClassPtr cppObject(new TCppClass);
		const TPyClassPtr result = ShadowTraits::buildObject(cppObject);
		// actually the iSubType should be used but appearently the tp_dict does not get properly initialized
		// so for now, old-style, consequence: no inheritance from extension classes
		result->ob_type = subType;
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
		return fromSharedPtrToNakedCast(result);
	}
	LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
}
$[
/** allocate a new object with $x arguments.
 */
template <typename ShadowTraits, $(typename P$x)$>
PyObject* construct( PyTypeObject* subType, PyObject* args )
{
	typedef typename ShadowTraits::TCppClass TCppClass;
	typedef typename ShadowTraits::TCppClassPtr TCppClassPtr;
	typedef typename ShadowTraits::TPyClassPtr TPyClassPtr;
	$(typedef ArgumentTraits< P$x > TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x;
	)$

	if ( decodeTuple<$(S$x)$>( args, $(p$x)$ ) != 0 )
	{
		return 0;
	}

	try
	{
		const TCppClassPtr cppObject(new TCppClass( $(TArg$x::arg(p$x))$ ));
		const TPyClassPtr result = ShadowTraits::buildObject(cppObject);
		result->ob_type = subType;
		result->ob_type->tp_flags &= (~Py_TPFLAGS_HAVE_GC);
		Py_INCREF(result->ob_type);
		return fromSharedPtrToNakedCast(result);
	}
	LASS_UTIL_PYOBJECT_CALL_CATCH_AND_RETURN
}
]$



// --- explicit resolver ---------------------------------------------------------------------------

// This explicit resolver construct is a wicked meta function that will call the write
// template function based on template parameters in TypeListType.

template <typename ShadowTraits, typename R, typename TypeListType> struct ExplicitResolver;
$[template <typename ShadowTraits, typename R, $(typename P$x)$, typename TypeListType> struct ExplicitResolver$x;
]$
template <typename ShadowTraits, typename R, $(typename P$x)$, typename P$y, typename TypeListType> struct ExplicitResolver$y;


template <typename ShadowTraits, typename R>
struct ExplicitResolver<ShadowTraits, R, lass::meta::NullType>
{
	struct Impl
	{
		static PyObject* callFunction( PyObject* args, R (*iFunction)() )
		{
			return ::lass::python::impl::callFunction<R>( args, iFunction );
		}
		template <typename C> static PyObject* callMethod( PyObject* args, PyObject* object, R (C::*method)() )
		{
			return CallMethod<ShadowTraits>::call( args, object, method );
		}
		template <typename C> static PyObject* callMethod( PyObject* args, const PyObject* object, R (C::*method)() const )
		{
			return CallMethod<ShadowTraits>::call( args, object, method );
		}
		static PyObject* callConstructor( PyTypeObject* subType, PyObject* args )
		{
			return construct<ShadowTraits>( subType, args );
		}
	};
	typedef Impl TImpl;
};
$[
template <typename ShadowTraits, typename R, $(typename P$x)$>
struct ExplicitResolver$x<ShadowTraits, R, $(P$x)$, lass::meta::NullType>
{
	struct Impl
	{
		static PyObject* callFunction( PyObject* args, R (*iFunction)($(P$x)$) )
		{
			return ::lass::python::impl::callFunction<R, $(P$x)$>( args, iFunction );
		}
		template <typename C> static PyObject* callMethod( PyObject* args, PyObject* object, R (C::*method)($(P$x)$) )
		{
			return CallMethod<ShadowTraits>::call( args, object, method );
		}
		template <typename C> static PyObject* callMethod( PyObject* args, PyObject* object, R (C::*method)($(P$x)$) const )
		{
			return CallMethod<ShadowTraits>::call( args, object, method );
		}
		template <typename C> static PyObject* callFreeMethod( PyObject* args, PyObject* object, R (*freeMethod)(C&, $(P$x)$) )
		{
			return CallMethod<ShadowTraits>::callFree( args, object, freeMethod );
		}
		template <typename C> static PyObject* callFreeMethod( PyObject* args, PyObject* object, R (*freeMethod)(const C&, $(P$x)$) )
		{
			return CallMethod<ShadowTraits>::callFree( args, object, freeMethod );
		}
		static PyObject* callConstructor( PyTypeObject* subType, PyObject* args )
		{
			return construct<ShadowTraits, $(P$x)$>( subType, args );
		}
	};
	typedef Impl TImpl;
};
]$

template <typename ShadowTraits, typename R, typename Head, typename Tail>
struct ExplicitResolver<ShadowTraits, R, meta::TypeList<Head, Tail> >
{
	typedef typename ExplicitResolver1<ShadowTraits, R, Head, Tail>::TImpl TImpl;
};
$[
template <typename ShadowTraits, typename R, $(typename P$x)$, typename Head, typename Tail>
struct ExplicitResolver$x<ShadowTraits, R, $(P$x)$, meta::TypeList<Head, Tail> >
{
	typedef typename ExplicitResolver$y<ShadowTraits, R, $(P$x)$, Head, Tail>::TImpl TImpl;
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
