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
*	Copyright (C) 2004-2018 the Initial Developer.
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

#include "python_common.h"
#include "export_traits.h"
#include "../util/wchar_support.h"

namespace
{

PyObject* buildStringImpl(const char* v, size_t size)
{
	if (size > PY_SSIZE_T_MAX) 
	{
		PyErr_SetString(PyExc_OverflowError, "input too long");
		return 0;
	}
	return PyUnicode_DecodeUTF8(v, static_cast<Py_ssize_t>(size), 0);
}


PyObject* buildWideStringImpl(const wchar_t* v, size_t size)
{
	if (size > PY_SSIZE_T_MAX) 
	{
		PyErr_SetString(PyExc_OverflowError, "input too long");
		return 0;
	}
	return PyUnicode_FromWideChar(v, static_cast<Py_ssize_t>(size));
}

}

namespace lass
{
namespace python
{


PyObject* PyExportTraits<void*>::build(void* value)
{
	if (!value)
	{
		Py_RETURN_NONE;
	}
	return PyCapsule_New(value, 0, 0);
}


int PyExportTraits<void*>::get(PyObject* obj, void*& value)
{
	if (obj == Py_None)
	{
		value = 0;
		return 0;
	}
	if (!PyCapsule_CheckExact(obj))
	{
		PyErr_SetString(PyExc_TypeError, "does not evaluate to a void*");
		return 1;
	}
	void* v = PyCapsule_GetPointer(obj, 0);
	if (!v)
	{
		return 1;
	}
	value = v;
	return 0;
}


PyObject* PyExportTraits<bool>::build(bool v)
{
	if (v)
	{
		Py_RETURN_TRUE;
	}
	else
	{
		Py_RETURN_FALSE;
	}
}


int PyExportTraits<bool>::get(PyObject* obj, bool& v)
{
	int result = PyObject_IsTrue(obj);
	if (result == -1)
	{
		PyErr_SetString(PyExc_TypeError, "does not evaluate to a boolean");
		return 1;
	}
	v = (result != 0);
	return 0;
}


#ifdef HAVE_LONG_LONG

PyObject* PyExportTraits<signed PY_LONG_LONG>::build(signed PY_LONG_LONG v)
{
	return PyLong_FromLongLong(v);
}


int PyExportTraits<signed PY_LONG_LONG>::get(PyObject* obj, signed PY_LONG_LONG& v)
{
	if (PyLong_Check(obj))
	{
		v = PyLong_AsLongLong(obj);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "not an integer");
	return 1;
}


PyObject* PyExportTraits<unsigned PY_LONG_LONG>::build(unsigned PY_LONG_LONG v)
{
	return PyLong_FromUnsignedLongLong(v);
}


int PyExportTraits<unsigned PY_LONG_LONG>::get(PyObject* obj, unsigned PY_LONG_LONG& v)
{
	if (PyLong_Check(obj))
	{
		v = PyLong_AsUnsignedLongLong(obj);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "not an integer");
	return 1;
}

#endif


PyObject* PyExportTraits<const char*>::build(const char* v)
{
	if (!v)
	{
		Py_RETURN_NONE;
	}
	return buildStringImpl(v, strlen(v));
}


PyObject* PyExportTraits<std::string>::build(const std::string& v)
{
	return buildStringImpl(v.data(), v.length());
}


int PyExportTraits<std::string>::get(PyObject* obj, std::string& v)
{
	if (!PyUnicode_Check(obj))
	{
		PyErr_SetString(PyExc_TypeError, "not a string");
		return 1;
	}
	TPyObjPtr s(PyUnicode_AsUTF8String(obj));
	if (!s)
	{
		return 1;
	}
	v.resize(static_cast<size_t>(PyBytes_GET_SIZE(s.get())), '\0');
	memcpy(&v[0], PyBytes_AS_STRING(s.get()), v.size());
	return 0;
}


PyObject* PyExportTraits<const wchar_t*>::build(const wchar_t* v)
{
	return buildWideStringImpl(v, wcslen(v));
}


PyObject* PyExportTraits<std::wstring>::build(const std::wstring& v)
{
	return buildWideStringImpl(v.data(), v.length());
}


int PyExportTraits<std::wstring>::get(PyObject* obj, std::wstring& v)
{
	if (!PyUnicode_Check(obj))
	{
		PyErr_SetString(PyExc_TypeError, "not a string");
		return 1;
	}
	Py_ssize_t n = PyUnicode_AsWideChar(obj, 0, 0); // takes care of UTF-16 and UTF-32 conversions
	if (n == -1)
	{
		return 1;
	}
	const Py_ssize_t size = n - 1; // n includes terminating null character.
	if (size <= 0)
	{
		v = std::wstring();
		return 0;
	}
	std::wstring tmp(static_cast<size_t>(size), '\0');
	if (PyUnicode_AsWideChar(obj, &tmp[0], size) == -1)
	{
		return 1;
	}
	v.swap(tmp);
	return 0;
}

}
}