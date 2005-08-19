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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H
#include "util_common.h"
#include "pyobject_plus.h"
#endif

#include "impl/dispatcher_python.h"

namespace lass
{
namespace python
{

#ifdef LASS_UTIL_CALLBACK_PYTHON_0
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_0
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_0

inline int pyGetSimpleObject( PyObject* iValue, util::Callback0& oV )
{
	python::PyObjectPtr<PyObject>::Type callable;
	if (pyGetSimpleObject(iValue, callable) != 0)
	{
		impl::addMessageHeader("Callback0");
		return 1;
	}
	if (!callable)
	{
		oV.reset();
		return 0;
	}
	if (!PyCallable_Check(callable.get()))
	{
		PyErr_SetString(PyExc_TypeError, "Callback0: not callable");
		return 1;
	}
	oV = util::impl::Dispatcher0Python(callable);
	return 0;
}

#	endif
#endif

$[
#ifdef LASS_UTIL_CALLBACK_PYTHON_$x
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_$x
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_$x

template <$(typename P$x)$>
int pyGetSimpleObject( PyObject* iValue, util::Callback$x<$(P$x)$>& oV )
{
	python::PyObjectPtr<PyObject>::Type callable;
	if (pyGetSimpleObject(iValue, callable) != 0)
	{
		impl::addMessageHeader("Callback$x");
		return 1;
	}
	if (!callable)
	{
		oV.reset();
		return 0;
	}
	if (!PyCallable_Check(callable.get()))
	{
		PyErr_SetString(PyExc_TypeError, "Callback$x: not callable");
		return 1;
	}
	oV = util::impl::Dispatcher$xPython<$(P$x)$>(callable);
	return 0;
}

#	endif
#endif
]$

#ifdef LASS_UTIL_CALLBACK_PYTHON_R0
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_R0
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_R0

template <typename R>
int pyGetSimpleObject( PyObject* iValue, util::CallbackR0<R>& oV )
{
	python::PyObjectPtr<PyObject>::Type callable;
	if (pyGetSimpleObject(iValue, callable) != 0)
	{
		impl::addMessageHeader("CallbackR0");
		return 1;
	}
	if (!callable)
	{
		oV.reset();
		return 0;
	}
	if (!PyCallable_Check(callable.get()))
	{
		PyErr_SetString(PyExc_TypeError, "CallbackR0: not callable");
		return 1;
	}
	oV = util::impl::DispatcherR0Python<R>(callable);
	return 0;
}

#	endif
#endif

$[
#ifdef LASS_UTIL_CALLBACK_PYTHON_R$x
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_R$x
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_R$x

template <typename R, $(typename P$x)$>
inline int pyGetSimpleObject( PyObject* iValue, util::CallbackR$x<R, $(P$x)$>& oV )
{
	python::PyObjectPtr<PyObject>::Type callable;
	if (pyGetSimpleObject(iValue, callable) != 0)
	{
		impl::addMessageHeader("CallbackR$x");
		return 1;
	}
	if (!callable)
	{
		oV.reset();
		return 0;
	}
	if (!PyCallable_Check(callable.get()))
	{
		PyErr_SetString(PyExc_TypeError, "CallbackR$x: not callable");
		return 1;
	}
	oV = util::impl::DispatcherR$xPython<R, $(P$x)$>(callable);
	return 0;
}

#	endif
#endif
]$

}

}

// EOF
