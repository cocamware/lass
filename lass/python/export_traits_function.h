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
 *	Copyright (C) 2023-2025 the Initial Developer.
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

#pragma once

#include "python_common.h"
#include "pyobject_plus.h"
#include "export_traits.h"
#include "pyobject_call.inl"

namespace lass::python
{

namespace impl
{

class FunctionImplBase
{
public:
	virtual ~FunctionImplBase() = default;
	virtual PyObject* call(PyObject* args) const = 0;
};

template <typename R, typename... Args>
class FunctionImpl: public FunctionImplBase
{
public:
	using TFunction = std::function<R(Args...)>;

	FunctionImpl(TFunction func) : func_(std::move(func)) {}

	PyObject* call(PyObject* args) const override
	{
		return impl::callFunction(args, func_);
	}

	TFunction function() const { return func_; }
private:
	TFunction func_;
};

class LASS_PYTHON_DLL Function: public PyObjectPlus
{
	PY_HEADER(PyObjectPlus)
public:
	using TPimpl = std::unique_ptr<FunctionImplBase>;

	Function(TPimpl pimpl);

	template <typename R, typename... Args>
	Function(std::function<R(Args...)> v):
		Function(TPimpl(new FunctionImpl<R, Args...>(std::move(v))))
	{
	}

	template <typename R, typename... Args>
	bool get(std::function<R(Args...)>& v)
	{
		if (auto p = dynamic_cast< FunctionImpl<R, Args...>* >(pimpl_.get()))
		{
			v = p->function();
			return true;
		}
		return false;
	}

	static PyObject* _tp_call(PyObject* self, PyObject* args, PyObject* kwargs);
private:
	std::unique_ptr<FunctionImplBase> pimpl_;
};

template <typename R, typename... Args>
class Callable
{
public:
	Callable(TPyObjPtr callable):
		callable_(std::move(callable))
	{
	}

	R operator()(typename util::CallTraits<Args>::TParam... args) const
	{
		LockGIL LASS_UNUSED(lock);
		LASS_ASSERT(callable_);
		const TPyObjPtr oArgs = makeTuple(args...);
		const TPyObjPtr result(PyObject_CallObject(callable_.get(), oArgs.get()));
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

	TPyObjPtr callable() const { return callable_; }
private:
	TPyObjPtr callable_;
};

}

/** @ingroup Python
 *
 *  Bidirectional wrapper between Callable objects and std::function.
 *
 *  Accepts Callable objects and wraps them in a std::function without checking the
 *  parameter types or return type, allowing to call them from C++. The parameters
 *  and return type are only checked when the function is called, and an exception will
 *  be raised if the types do not match.
 *
 *  From C++ to Python, the std::function is converted to a Callable object, so that it
 *  can be called from Python. Again, the parameter types and return type will be
 *  checked when the function is called.
 *
 *  In both directions, unwrapping a previously wrapped function will be attempted.
 *  I.e. the std::function passed to Python already wraps a Callable Python object, this
 *  will be unwrapped and the Callable object will be passed back to Python. In the
 *  other direction, if a Callable object that was passed to C++ already wraps a
 *  std::function, *and* this std::function matches the correct signature, then the
 *  std::function will be unwrapped and passed to C++. In other words, both directions
 *  will guarantee a perfect round trip if possible. In all other cases, the Callable
 *  or std::function will be wrapped in a new std::function or Callable object.
 */
template <typename R, typename... Args>
struct PyExportTraits< std::function<R(Args...)> >
{
	static constexpr const char* py_typing = "Callable[[Args...], R]";

	static PyObject* build(std::function<R(Args...)> v)
	{
		using TCallable = impl::Callable<R, Args...>;
		if (TCallable* p = v.template target<TCallable>())
		{
			PyObject* r = p->callable().get();
			Py_XINCREF(r);
			return r;
		}
		return new impl::Function(std::move(v));
	}
	static int get(PyObject* obj, std::function<R(Args...)>& v)
	{
		if (PyType_IsSubtype(obj->ob_type, impl::Function::_lassPyClassDef.type()))
		{
			if (static_cast<impl::Function*>(obj)->get(v))
			{
				return 0;
			}
		}
		if (!PyCallable_Check(obj))
		{
			PyErr_SetString(PyExc_TypeError, "Not callable");
			return 1;
		}
		using TCallable = impl::Callable<R, Args...>;
		v = TCallable(fromNakedToSharedPtrCast<PyObject>(obj));
		return 0;
	}
};

}
