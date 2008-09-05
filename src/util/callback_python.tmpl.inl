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

#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H

#include "impl/dispatcher_python.h"

namespace lass
{
namespace python
{

#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_0_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_0
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_0

inline int pyGetSimpleObject_deprecated( PyObject* iValue, util::Callback0& oV )
{
	python::PyObjectPtr<PyObject>::Type callable;
	if (PyExportTraits< python::PyObjectPtr<PyObject>::Type >::get(iValue, callable) != 0)
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

template <>
struct PyExportTraits< util::Callback0 >
{
	static int get(PyObject* iV, util::Callback0& oV) { return pyGetSimpleObject_deprecated(iV,oV); }
	static PyObject* build( const util::Callback0& oV ) 
	{
		if (oV.isEmpty())
		{
			Py_XINCREF(Py_None);
			return Py_None;
		}
		else
			return PyExportTraits<int>::build(1);
	}
};

#	endif
#endif

$[
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_$x_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_$x
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_$x

template <$(typename P$x)$>
int pyGetSimpleObject_deprecated( PyObject* iValue, util::Callback$x<$(P$x)$>& oV )
{
	python::PyObjectPtr<PyObject>::Type callable;
	if (PyExportTraits< python::PyObjectPtr<PyObject>::Type >::get(iValue, callable) != 0)
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

template <$(typename P$x)$>
struct PyExportTraits< util::Callback$x<$(P$x)$> >
{
	static int get(PyObject* iV, util::Callback$x<$(P$x)$> & oV) { return pyGetSimpleObject_deprecated(iV,oV); }
	static PyObject* build( const util::Callback$x<$(P$x)$> & oV ) 
	{
		if (oV.isEmpty())
		{
			Py_XINCREF(Py_None);
			return Py_None;
		}
		else
			return PyExportTraits<int>::build(1);
	}
};


#	endif
#endif
]$

#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_R_0_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_R0
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_R0

template <typename R>
int pyGetSimpleObject_deprecated( PyObject* iValue, util::CallbackR0<R>& oV )
{
	python::PyObjectPtr<PyObject>::Type callable;
	if (PyExportTraits< python::PyObjectPtr<PyObject>::Type >::get(iValue, callable) != 0)
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

template <typename R>
struct PyExportTraits< util::CallbackR0<R> >
{
	static int get(PyObject* iV, util::CallbackR0<R>& oV) { return pyGetSimpleObject_deprecated(iV,oV); }
	static PyObject* build( const util::CallbackR0<R>& oV ) 
	{
		if (oV.isEmpty())
		{
			Py_XINCREF(Py_None);
			return Py_None;
		}
		else
			return PyExportTraits<int>::build(1);
	}
};

#	endif
#endif

$[
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_R_$x_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_R$x
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H_R$x

template <typename R, $(typename P$x)$>
inline int pyGetSimpleObject_deprecated( PyObject* iValue, util::CallbackR$x<R, $(P$x)$>& oV )
{
	python::PyObjectPtr<PyObject>::Type callable;
	if (PyExportTraits< python::PyObjectPtr<PyObject>::Type >::get(iValue, callable) != 0)
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

template <typename R, $(typename P$x)$>
struct PyExportTraits< util::CallbackR$x<R, $(P$x)$> >
{
	static int get(PyObject* iV, util::CallbackR$x<R, $(P$x)$>& oV) { return pyGetSimpleObject_deprecated(iV,oV); }
	static PyObject* build( const util::CallbackR$x<R, $(P$x)$>& oV ) 
	{
		if (oV.isEmpty())
		{
			Py_XINCREF(Py_None);
			return Py_None;
		}
		else
			return PyExportTraits<int>::build(1);
	}
};

#	endif
#endif
]$

}

}

#endif

// EOF
