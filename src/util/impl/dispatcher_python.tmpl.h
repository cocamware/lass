/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H
#include "../util_common.h"
#include "../pyobject_plus.h"

namespace lass
{
namespace util
{
namespace impl
{

LASS_DLL_EXPORT std::string LASS_CALL fetchException(); // see dispatcher_python.cpp

}
}
}

#endif



namespace lass
{
namespace util
{
namespace impl
{

#ifdef LASS_UTIL_CALLBACK_PYTHON_0
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_0
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_0

/** Dispatcher for lass::util::Callback0 to an python callback.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
class Dispatcher0Python: public Dispatcher0
{
public:
	typedef python::PyObjectPtr<PyObject>::Type TPyPtr;

	Dispatcher0Python(const TPyPtr& iCallable):
		callable_(iCallable)
	{
	}

	void operator()() const
	{
		LASS_ASSERT(callable_);
		TPyPtr result(PyObject_CallObject(callable_.get(), 0));
		if (!result)
		{
			LASS_THROW(fetchException());
		}
	}

	bool isEquivalent(const Dispatcher0* iOther) const
	{
		const Dispatcher0Python* other = dynamic_cast<const Dispatcher0Python*>(iOther);
		return other && callable_ == other->callable_;
	}

private:

	TPyPtr callable_;
};

#	endif
#endif

$[
#ifdef LASS_UTIL_CALLBACK_PYTHON_$x
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_$x
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_$x

/** Dispatcher for lass::util::Callback$x to an python callback.
 *  @internal
 *  @sa Callback$x
 *  @author Bram de Greve [Bramz]
 */
template <$(typename P$x)$>
class Dispatcher$xPython: public Dispatcher$x<$(P$x)$>
{
public:
	typedef python::PyObjectPtr<PyObject>::Type TPyPtr;

	Dispatcher$xPython(const TPyPtr& iCallable):
		callable_(iCallable)
	{
	}

	void operator()($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		LASS_ASSERT(callable_);
		TPyPtr args(PyTuple_New($x));
		$(PyTuple_SetItem(args.get(), $x - 1, python::pyBuildSimpleObject(iP$x));
		)$
		TPyPtr result(PyObject_CallObject(callable_.get(), args.get()));
		if (!result)
		{
			LASS_THROW(fetchException());
		}
	}

private:

	TPyPtr callable_;
};

#	endif
#endif
]$

#ifdef LASS_UTIL_CALLBACK_PYTHON_R0
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_R0
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_R0

/** Dispatcher for lass::util::CallbackR0 to an python callback.
 *  @internal
 *  @sa CallbackR0
 *  @author Bram de Greve [Bramz]
 */
template <typename R>
class DispatcherR0Python: public DispatcherR0<R>
{
public:
	typedef python::PyObjectPtr<PyObject>::Type TPyPtr;

	DispatcherR0Python(const TPyPtr& iCallable):
		callable_(iCallable)
	{
	}

	R operator()() const
	{
		LASS_ASSERT(callable_);
		TPyPtr result(PyObject_CallObject(callable_.get(), 0));
		if (!result)
		{
			LASS_THROW(fetchException());
		}

		typedef python::impl::ArgumentTraits<R> TraitsR;
		typename TraitsR::TStorage temp;
		if (python::pyGetSimpleObject(result.get(), temp) != 0)
		{
			LASS_THROW("bad result");
		}
		return TraitsR::arg(temp);
	}

private:

	TPyPtr callable_;
};

#	endif
#endif

$[
#ifdef LASS_UTIL_CALLBACK_PYTHON_R$x
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_R$x
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_R$x

/** Dispatcher for lass::util::CallbackR$x to an python callback.
 *  @internal
 *  @sa CallbackR$x
 *  @author Bram de Greve [Bramz]
 */
template <typename R, $(typename P$x)$>
class DispatcherR$xPython: public DispatcherR$x<R, $(P$x)$>
{
public:
	typedef python::PyObjectPtr<PyObject>::Type TPyPtr;

	DispatcherR$xPython(const TPyPtr& iCallable):
		callable_(iCallable)
	{
	}

	R operator()($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		LASS_ASSERT(callable_);
		TPyPtr args(PyTuple_New($x));
		$(PyTuple_SetItem(args.get(), $x - 1, python::pyBuildSimpleObject(iP$x));
		)$
		TPyPtr result(PyObject_CallObject(callable_.get(), args.get()));
		if (!result)
		{
			LASS_THROW(fetchException());
		}

		typedef python::impl::ArgumentTraits<R> TraitsR;
		typename TraitsR::TStorage temp;
		if (python::pyGetSimpleObject(result.get(), temp) != 0)
		{
			LASS_THROW("bad result");
		}
		return TraitsR::arg(temp);
	}

private:

	TPyPtr callable_;
};

#	endif
#endif
]$

}

}

}

// EOF
