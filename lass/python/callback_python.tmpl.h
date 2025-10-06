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
 *	Copyright (C) 2004-2025 the Initial Developer.
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


#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H

#include "python_common.h"
#include "pyobject_plus.h"
#include "py_tuple.h"
#include "pyobject_call.inl"
#include "exception.h"
#include "_lass_module.h"

namespace lass
{
namespace python
{
namespace impl
{
	/** Common implementation of a dispatcher to an python callback with void return type
	 *  @internal
	 *  @author Bramz
	 */
	class FunctorPythonBase
	{
	public:
		FunctorPythonBase(const TPyObjPtr& callable): callable_(callable) {}
		bool operator==(const FunctorPythonBase& other) const
		{
			return callable_.get() == other.callable_.get();
		}
		bool operator!() const
		{
			return !callable_;
		}
		const TPyObjPtr& callable() const
		{
			return callable_;
		}
	protected:
		void call(const TPyObjPtr& args) const
		{
			LockGIL LASS_UNUSED(lock);
			LASS_ASSERT(callable_);
			const TPyObjPtr result(PyObject_CallObject(callable_.get(), args.get()));
			if (!result)
			{
				fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
			}
		}
	private:
		TPyObjPtr callable_;
	};

	/** Common implementation of a dispatcher to an python callback with non-void return type.
	 *  @internal
	 *  @author Bramz
	 */
	template <typename R>
	class FunctorPythonRBase
	{
	public:
		FunctorPythonRBase(const TPyObjPtr& callable): callable_(callable) {}
		bool operator==(const FunctorPythonRBase<R>& other) const
		{
			return callable_.get() == other.callable_.get();
		}
		bool operator!() const
		{
			return !callable_;
		}
		const TPyObjPtr& callable() const
		{
			return callable_;
		}
	protected:
		R call(const TPyObjPtr& args) const
		{
			LockGIL LASS_UNUSED(lock);
			LASS_ASSERT(callable_);
			const TPyObjPtr result(PyObject_CallObject(callable_.get(), args.get()));
			if (!result)
			{
				fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
			}
			typedef ArgumentTraits<R> TraitsR;
			typename TraitsR::TStorage temp;
			if (pyGetSimpleObject(result.get(), temp) != 0)
			{
				fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
			}
			return TraitsR::arg(temp);
		}
	private:
		TPyObjPtr callable_;
	};

	class PyCallbackImplBase
	{
	public:
		virtual ~PyCallbackImplBase() = default;
		virtual PyObject* call(PyObject* args) const = 0;
	};

	class LASS_PYTHON_DLL PyCallback: public PyObjectPlus
	{
		PY_HEADER(PyObjectPlus)
	public:
		using TPimpl = std::unique_ptr<PyCallbackImplBase>;

		PyCallback(TPimpl impl);

		template <typename PyCallbackImplType>
		bool get(typename PyCallbackImplType::TCallback& v) const
		{
			PyCallbackImplBase* p = pimpl_.get();
			if (p && typeid(*p) == typeid(PyCallbackImplType))
			{
				v = static_cast<PyCallbackImplType*>(p)->callable();
				return true;
			}
			return false;
		}

		static PyObject* _tp_call(PyObject* self, PyObject* args, PyObject* kwargs);
	private:
		TPimpl pimpl_;
	};
}

/** Helper class to implement PyExportTraits for Callback types
 * 
 *  Only the `get()` method is implemented, which allows you to use callbacks as
 *  C++ function parameters, but not als return values. Returning a callback from C++
 *  to Python is not supported.
 * 
 *  For full bidirectional support, use std::function instead.
 *
 *  @ingroup PyExportTraits
 */
template <typename CallbackType, typename PyCallbackImplType, typename ExportTraits>
struct PyExportTraitsCallback
{
	static int get(PyObject* value, CallbackType& callback)
	{
		if (value == Py_None)
		{
			callback.reset();
			return 0;
		}
		impl::initLassModule(); // ensure the module is initialized
		if (PyType_IsSubtype(Py_TYPE(value), impl::PyCallback::_lassPyClassDef.type()))
		{
			if (static_cast<impl::PyCallback*>(value)->get<PyCallbackImplType>(callback))
			{
				return 0;
			}
		}
		TPyObjPtr callable;
		if (pyGetSimpleObject(value, callable) != 0)
		{
			impl::addMessageHeader(ExportTraits::className());
			return 1;
		}
		if (!callable) // null pointer
		{
			callback.reset();
			return 0;
		}
		if (!PyCallable_Check(callable.get()))
		{
			std::ostringstream buffer;
			buffer << ExportTraits::className() << ": not callable";
			PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
			return 1;
		}
		using TFunctor = typename PyCallbackImplType::TFunctor;
		callback = TFunctor(callable);
		return 0;
	}
	static PyObject* build(const CallbackType& callback)
	{
		if (!callback)
		{
			Py_RETURN_NONE;
		}

		using TDispatcher = typename PyCallbackImplType::TDispatcher;
		auto dispatcher = callback.dispatcher().get();
		if (dispatcher && typeid(*dispatcher) == typeid(TDispatcher))
		{
			// already a FunctorType, return the original callable
			return fromSharedPtrToNakedCast(static_cast<TDispatcher*>(dispatcher)->function().callable());
		}
		// wrap in a PyCallback object
		impl::PyCallback::TPimpl pimpl(new PyCallbackImplType(callback));
		return new impl::PyCallback(std::move(pimpl));
	}
};

}
}

#endif

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_CALLBACK_0)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_CALLBACK_0
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_CALLBACK_0

namespace lass
{
namespace python
{
namespace impl
{
	class Functor0Python: public FunctorPythonBase
	{
	public:
		Functor0Python(const python::TPyObjPtr& callable): FunctorPythonBase(callable) {}
		void operator()() const
		{
			LockGIL LASS_UNUSED(lock);
			this->call(python::TPyObjPtr());
		}
	};

	class PyCallback0Impl: public PyCallbackImplBase
	{
	public:
		using TCallback = util::Callback0;
		using TFunctor = Functor0Python;
		using TDispatcher = util::impl::Dispatcher0Function<TFunctor>;

		PyCallback0Impl(TCallback callback): callback_(std::move(callback)) {}
		const TCallback& callable() const { return callback_; }
		
		PyObject* call(PyObject* args) const override
		{
			if ( decodeTuple(args) != 0 )
			{
				return nullptr;
			}
			return Caller<void>::callFunction<const TCallback&>(callback_);
		}
	private:
		TCallback callback_;
	};
}

/** Bidirectional mapping between util::Callback0 and a Python callable object
 *
 *  Accepts Callable objects and wraps them in a util::Callback0 without checking the
 *  parameter types or return type, allowing to call them from C++. The parameters
 *  and return type are only checked when the function is called, and an exception will
 *  be raised if the types do not match.
 *
 *  From C++ to Python, the util::Callback0 is converted to a Callable object, so that
 *  it can be called from Python. Again, the parameter types and return type will be
 *  checked when the function is called.
 *
 *  In both directions, unwrapping a previously wrapped function will be attempted.
 *  I.e. if the util::Callback0 passed to Python already wraps a Callable Python
 *  object, this will be unwrapped and the Callable object will be passed back to Python.
 *  In the other direction, if a Callable object that was passed to C++ already wraps a
 *  util::Callback0, *and* this util::Callback0 matches the correct signature, then
 *  the util::Callback0 will be unwrapped and passed to C++. In other words, both
 *  directions will guarantee a perfect round trip if possible. In all other cases, the
 *  Callable or util::Callback0 will be wrapped in a new util::Callback0 or Callable
 *  object.
 *
 *  @ingroup PyExportTraits
 */
template <>
struct PyExportTraits< util::Callback0 >:
	public PyExportTraitsCallback<
		util::Callback0,
		impl::PyCallback0Impl,
		PyExportTraits< util::Callback0 >
	>
{
	static constexpr const char* py_typing = "Callable[[], None]";

	static const char* className() { return "Callback0"; }
};

}
}

#	endif
#endif
$[
#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_CALLBACK_$x)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_CALLBACK_$x
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_CALLBACK_$x

namespace lass
{
namespace python
{
namespace impl
{
	template <$(typename P$x)$>
	class Functor$xPython: public FunctorPythonBase
	{
	public:
		Functor$xPython(const TPyObjPtr& callable): FunctorPythonBase(callable) {}
		void operator()($(typename util::CallTraits<P$x>::TParam p$x)$) const
		{
			LockGIL LASS_UNUSED(lock);
			this->call(makeTuple($(p$x)$));
		}
	};

	template <$(typename P$x)$>
	class PyCallback$xImpl: public PyCallbackImplBase
	{
	public:
		using TCallback = util::Callback$x<$(P$x)$>;
		using TFunctor = Functor$xPython<$(P$x)$>;
		using TDispatcher = util::impl::Dispatcher$xFunction<$(P$x)$, TFunctor>;

		PyCallback$xImpl(TCallback callback): callback_(std::move(callback)) {}
		const TCallback& callable() const { return callback_; }
		
		PyObject* call(PyObject* args) const override
		{
			$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x = S$x();
			)$
			if ( decodeTuple<$(S$x)$>(args, $(p$x)$) != 0 )
			{
				return nullptr;
			}
			return Caller<void>::template callFunction<const TCallback&>(callback_, $(TArg$x::arg(p$x))$);
		}
	private:
		TCallback callback_;
	};
}

/** Bidirectional mapping between util::Callback$x and a Python callable object
 *
 *  Accepts Callable objects and wraps them in a util::Callback$x without checking the
 *  parameter types or return type, allowing to call them from C++. The parameters
 *  and return type are only checked when the function is called, and an exception will
 *  be raised if the types do not match.
 *
 *  From C++ to Python, the util::Callback$x is converted to a Callable object, so that
 *  it can be called from Python. Again, the parameter types and return type will be
 *  checked when the function is called.
 *
 *  In both directions, unwrapping a previously wrapped function will be attempted.
 *  I.e. if the util::Callback$x passed to Python already wraps a Callable Python
 *  object, this will be unwrapped and the Callable object will be passed back to Python.
 *  In the other direction, if a Callable object that was passed to C++ already wraps a
 *  util::Callback$x, *and* this util::Callback$x matches the correct signature, then
 *  the util::Callback$x will be unwrapped and passed to C++. In other words, both
 *  directions will guarantee a perfect round trip if possible. In all other cases, the
 *  Callable or util::Callback$x will be wrapped in a new util::Callback$x or Callable
 *  object.
 *
 *  @ingroup PyExportTraits
 */
template <$(typename P$x)$>
struct PyExportTraits< util::Callback$x<$(P$x)$> >:
	public PyExportTraitsCallback<
		util::Callback$x<$(P$x)$>,
		impl::PyCallback$xImpl<$(P$x)$>,
		PyExportTraits< util::Callback$x<$(P$x)$> >
	>
{
	static constexpr const char* py_typing = "Callable[[$(P$x!)$], None]";

	static const char* className() { return "Callback$x"; }
};

}
}

#	endif
#endif
]$




#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_CALLBACK_R0)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_CALLBACK_R0
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_CALLBACK_R0

namespace lass
{
namespace python
{
namespace impl
{
	template <typename R>
	class FunctorPythonR0: public FunctorPythonRBase<R>
	{
	public:
		FunctorPythonR0(const TPyObjPtr& callable): FunctorPythonRBase<R>(callable) {}
		R operator()() const
		{
			LockGIL LASS_UNUSED(lock);
			return this->call(TPyObjPtr());
		}
	};

	template <typename R>
	class PyCallbackR0Impl: public PyCallbackImplBase
	{
	public:
		using TCallback = util::CallbackR0<R>;
		using TFunctor = FunctorPythonR0<R>;
		using TDispatcher = util::impl::DispatcherR0Function<R, TFunctor>;

		PyCallbackR0Impl(TCallback callback): callback_(std::move(callback)) {}
		const TCallback& callable() const { return callback_; }
		
		PyObject* call(PyObject* args) const override
		{
			if ( decodeTuple(args) != 0 )
			{
				return nullptr;
			}
			return Caller<R>::template callFunction<const TCallback&>(callback_);
		}
	private:
		TCallback callback_;
	};
}

/** Bidirectional mapping between util::CallbackR0 and a Python callable object
 *
 *  Accepts Callable objects and wraps them in a util::CallbackR0 without checking the
 *  parameter types or return type, allowing to call them from C++. The parameters
 *  and return type are only checked when the function is called, and an exception will
 *  be raised if the types do not match.
 *
 *  From C++ to Python, the util::CallbackR0 is converted to a Callable object, so that
 *  it can be called from Python. Again, the parameter types and return type will be
 *  checked when the function is called.
 *
 *  In both directions, unwrapping a previously wrapped function will be attempted.
 *  I.e. if the util::CallbackR0 passed to Python already wraps a Callable Python
 *  object, this will be unwrapped and the Callable object will be passed back to Python.
 *  In the other direction, if a Callable object that was passed to C++ already wraps a
 *  util::CallbackR0, *and* this util::CallbackR0 matches the correct signature, then
 *  the util::CallbackR0 will be unwrapped and passed to C++. In other words, both
 *  directions will guarantee a perfect round trip if possible. In all other cases, the
 *  Callable or util::CallbackR0 will be wrapped in a new util::CallbackR0 or Callable
 *  object.
 *
 *  @ingroup PyExportTraits
 */
template <typename R>
struct PyExportTraits< util::CallbackR0<R> >:
	public PyExportTraitsCallback<
		util::CallbackR0<R>,
		impl::PyCallbackR0Impl<R>,
		PyExportTraits< util::CallbackR0<R> >
	>
{
	static constexpr const char* py_typing = "Callable[[], R!]";
	
	static const char* className() { return "CallbackR0"; }
};

}
}

#	endif
#endif
$[
#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_CALLBACK_R$x)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_CALLBACK_R$x
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_CALLBACK_R$x

namespace lass
{
namespace python
{
namespace impl
{
	template <typename R, $(typename P$x)$>
	class FunctorPythonR$x: public FunctorPythonRBase<R>
	{
	public:
		FunctorPythonR$x(const TPyObjPtr& callable): FunctorPythonRBase<R>(callable) {}
		R operator()($(typename util::CallTraits<P$x>::TParam p$x)$) const
		{
			LockGIL LASS_UNUSED(lock);
			return this->call(makeTuple($(p$x)$));
		}
	};

	template <typename R, $(typename P$x)$>
	class PyCallbackR$xImpl: public PyCallbackImplBase
	{
	public:
		using TCallback = util::CallbackR$x<R, $(P$x)$>;
		using TFunctor = FunctorPythonR$x<R, $(P$x)$>;
		using TDispatcher = util::impl::DispatcherR$xFunction<R, $(P$x)$, TFunctor>;

		PyCallbackR$xImpl(TCallback callback): callback_(std::move(callback)) {}
		const TCallback& callable() const { return callback_; }
		
		PyObject* call(PyObject* args) const override
		{
			$(typedef ArgumentTraits<P$x> TArg$x; typedef typename TArg$x::TStorage S$x; S$x p$x = S$x();
			)$
			if ( decodeTuple<$(S$x)$>(args, $(p$x)$) != 0 )
			{
				return nullptr;
			}
			return Caller<R>::template callFunction<const TCallback&>(callback_, $(TArg$x::arg(p$x))$);
		}
	private:
		TCallback callback_;
	};

}

/** Bidirectional mapping between util::CallbackR$x and a Python callable object
 *
 *  Accepts Callable objects and wraps them in a util::CallbackR$x without checking the
 *  parameter types or return type, allowing to call them from C++. The parameters
 *  and return type are only checked when the function is called, and an exception will
 *  be raised if the types do not match.
 *
 *  From C++ to Python, the util::CallbackR$x is converted to a Callable object, so that
 *  it can be called from Python. Again, the parameter types and return type will be
 *  checked when the function is called.
 *
 *  In both directions, unwrapping a previously wrapped function will be attempted.
 *  I.e. if the util::CallbackR$x passed to Python already wraps a Callable Python
 *  object, this will be unwrapped and the Callable object will be passed back to Python.
 *  In the other direction, if a Callable object that was passed to C++ already wraps a
 *  util::CallbackR$x, *and* this util::CallbackR$x matches the correct signature, then
 *  the util::CallbackR$x will be unwrapped and passed to C++. In other words, both
 *  directions will guarantee a perfect round trip if possible. In all other cases, the
 *  Callable or util::CallbackR$x will be wrapped in a new util::CallbackR$x or Callable
 *  object.
 *
 *  @ingroup PyExportTraits
 */
template <typename R, $(typename P$x)$>
struct PyExportTraits< util::CallbackR$x<R, $(P$x)$> >:
	public PyExportTraitsCallback<
		util::CallbackR$x<R, $(P$x)$>,
		impl::PyCallbackR$xImpl<R, $(P$x)$>,
		PyExportTraits< util::CallbackR$x<R, $(P$x)$> >
	>
{
	static constexpr const char* py_typing = "Callable[[$(P$x!)$], R!]";
	
	static const char* className() { return "CallbackR$x"; }
};

}
}

#	endif
#endif
]$

// EOF
