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


#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H

#include "util_common.h"
#include "pyobject_plus.h"
#include "py_tuple.h"
#include "pyobject_call.inl"

namespace lass
{
namespace python
{
namespace impl
{
	LASS_DLL void LASS_CALL fetchAndThrowPythonException(const std::string& loc); // see impl/dispatcher_python.cpp

	/** Common implementation of a dispatcher to an python callback with void return type
	 *  @internal
	 *  @author Bramz
	 */
	class FunctorPythonBase
	{
	public:
		FunctorPythonBase(const python::TPyObjPtr& callable): callable_(callable) {}
		bool operator==(const FunctorPythonBase& other) const
		{
			return callable_.get() == other.callable_.get();
		}
	protected:
		void call(const python::TPyObjPtr& args) const
		{
			LASS_ASSERT(callable_);
			const python::TPyObjPtr result(PyObject_CallObject(callable_.get(), args.get()));
			if (!result)
			{
				fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
			}
		}
	private:
		python::TPyObjPtr callable_;
	};

	/** Common implementation of a dispatcher to an python callback with non-void return type.
	 *  @internal
	 *  @author Bramz
	 */
	template <typename R>
	class FunctorPythonRBase
	{
	public:
		FunctorPythonRBase(const python::TPyObjPtr& callable): callable_(callable) {}
		bool operator==(const FunctorPythonRBase<R>& other) const
		{
			return callable_.get() == other.callable_.get();
		}
	protected:
		R call(const python::TPyObjPtr& args) const
		{
			LASS_ASSERT(callable_);
			const python::TPyObjPtr result(PyObject_CallObject(callable_.get(), args.get()));
			if (!result)
			{
				fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
			}
			typedef ArgumentTraits<R> TraitsR;
			typename TraitsR::TStorage temp;
			if (python::PyExportTraits< typename TraitsR::TStorage >::get(result.get(), temp) != 0)
			{
				LASS_THROW("bad result");
			}
			return TraitsR::arg(temp);
		}
	private:
		python::TPyObjPtr callable_;
	};
}

/** @internal
 */
template <typename CallbackType, typename FunctorType, typename ExportTraits>
struct PyExportTraitsCallback
{
	static int get(PyObject* value, CallbackType& callback)
	{
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
		callback = FunctorType(callable);
		return 0;
	}
	static PyObject* build(const CallbackType& callback) 
	{
		if (!callback)
		{
			Py_RETURN_NONE;
		}
#pragma LASS_TODO("Return a real callback object ... [Bramz]")
		Py_RETURN_TRUE;
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
	/** @internal
	 */
	class Functor0Python: public FunctorPythonBase
	{
	public:
		Functor0Python(const python::TPyObjPtr& callable): FunctorPythonBase(callable) {}
		void operator()() const
		{
			this->call(python::TPyObjPtr());
		}
	};
}

/** @internal
 */
template <>
struct PyExportTraits< util::Callback0 >:
	public PyExportTraitsCallback<
		util::Callback0, 
		impl::Functor0Python, 
		PyExportTraits< util::Callback0 > 
	>
{
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
	/** @internal
	 */
	template <$(typename P$x)$>
	class Functor$xPython: public FunctorPythonBase
	{
	public:
		Functor$xPython(const python::TPyObjPtr& callable): FunctorPythonBase(callable) {}
		void operator()($(typename util::CallTraits<P$x>::TParam p$x)$) const
		{
			this->call(python::makeTuple($(p$x)$));
		}
	};
}

/** @internal
 */
template <$(typename P$x)$>
struct PyExportTraits< util::Callback$x<$(P$x)$> >:
	public PyExportTraitsCallback<
		util::Callback$x<$(P$x)$>, 
		impl::Functor$xPython<$(P$x)$>, 
		PyExportTraits< util::Callback$x<$(P$x)$> > 
	>
{
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
	/** @internal
	 */
	template <typename R>
	class FunctorPythonR0: public FunctorPythonRBase<R>
	{
	public:
		FunctorPythonR0(const python::TPyObjPtr& callable): FunctorPythonRBase<R>(callable) {}
		R operator()() const
		{
			return this->call(python::TPyObjPtr());
		}
	};
}

/** @internal
 */
template <typename R>
struct PyExportTraits< util::CallbackR0<R> >:
	public PyExportTraitsCallback<
		util::CallbackR0<R>, 
		impl::FunctorPythonR0<R>, 
		PyExportTraits< util::CallbackR0<R> > 
	>
{
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
	/** @internal
	 */
	template <typename R, $(typename P$x)$>
	class FunctorPythonR$x: public FunctorPythonRBase<R>
	{
	public:
		FunctorPythonR$x(const python::TPyObjPtr& callable): FunctorPythonRBase<R>(callable) {}
		R operator()($(typename util::CallTraits<P$x>::TParam p$x)$) const
		{
			return this->call(python::makeTuple($(p$x)$));
		}
	};
}

/** @internal
 */
template <typename R, $(typename P$x)$>
struct PyExportTraits< util::CallbackR$x<R, $(P$x)$> >:
	public PyExportTraitsCallback<
		util::CallbackR$x<R, $(P$x)$>, 
		impl::FunctorPythonR$x<R, $(P$x)$>, 
		PyExportTraits< util::CallbackR$x<R, $(P$x)$> > 
	>
{
	static const char* className() { return "CallbackR$x"; }
};

}
}

#	endif
#endif
]$

// EOF
