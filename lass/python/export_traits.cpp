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

#include "python_common.h"
#include "export_traits.h"
#include "../util/wchar_support.h"

namespace
{

template <typename string_type>
int getStringImpl(PyObject* obj, string_type& v)
{
	typedef typename string_type::value_type value_type;
	static_assert(sizeof(value_type) == 1, "string_type should be narrow string");
	if (!PyUnicode_Check(obj))
	{
		PyErr_SetString(PyExc_TypeError, "not a string");
		return 1;
	}
	Py_ssize_t size;
	const char* data = PyUnicode_AsUTF8AndSize(obj, &size);
	if (!data)
	{
		return 1;
	}
	v = string_type(reinterpret_cast<const value_type*>(data), static_cast<size_t>(size));
	return 0;
}

template <typename string_type>
int getWideStringImpl(PyObject * obj, string_type& v)
{
	typedef typename string_type::value_type value_type;
	static_assert(sizeof(value_type) == sizeof(wchar_t), "string_type should be wide string");
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
		v = string_type();
		return 0;
	}
	string_type tmp(static_cast<size_t>(size), '\0');
	if (PyUnicode_AsWideChar(obj, reinterpret_cast<wchar_t*>(&tmp[0]), size) == -1)
	{
		return 1;
	}
	v = std::move(tmp);
	return 0;
}

}


namespace lass
{
namespace python
{
namespace impl
{

PyObject* buildStringImpl(const char* v, size_t size)
{
	if (!v)
	{
		Py_RETURN_NONE;
	}
	if (size > PY_SSIZE_T_MAX) 
	{
		PyErr_SetString(PyExc_OverflowError, "input too long");
		return 0;
	}
	return PyUnicode_DecodeUTF8(v, static_cast<Py_ssize_t>(size), 0);
}


PyObject* buildStringImpl(const wchar_t* v, size_t size)
{
	if (!v)
	{
		Py_RETURN_NONE;
	}
	if (size > PY_SSIZE_T_MAX) 
	{
		PyErr_SetString(PyExc_OverflowError, "input too long");
		return 0;
	}
	return PyUnicode_FromWideChar(v, static_cast<Py_ssize_t>(size));
}


#if LASS_HAVE_STD_U8STRING

PyObject* buildStringImpl(const char8_t* v, size_t size)
{
	return buildStringImpl(reinterpret_cast<const char*>(v), size);
}

#endif


PyObject* buildStringImpl(const char16_t* v, size_t size)
{
	if (!v)
	{
		Py_RETURN_NONE;
	}
	if (size > (PY_SSIZE_T_MAX / 2))
	{
		PyErr_SetString(PyExc_OverflowError, "input too long");
		return 0;
	}
	return PyUnicode_DecodeUTF16(reinterpret_cast<const char*>(v), static_cast<Py_ssize_t>(size * 2), nullptr, nullptr);
}


PyObject* buildStringImpl(const char32_t* v, size_t size)
{
	if (!v)
	{
		Py_RETURN_NONE;
	}
	if (size > (PY_SSIZE_T_MAX / 4))
	{
		PyErr_SetString(PyExc_OverflowError, "input too long");
		return 0;
	}
	return PyUnicode_DecodeUTF32(reinterpret_cast<const char*>(v), static_cast<Py_ssize_t>(size * 4), nullptr, nullptr);
}

}



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



PyObject* PyExportTraits<std::nullptr_t>::build(std::nullptr_t /* value */)
{
	Py_RETURN_NONE;
}


int PyExportTraits<std::nullptr_t>::get(PyObject* obj, std::nullptr_t& value)
{
	if (obj != Py_None)
	{
		return 1;
	}
	value = nullptr;
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


PyObject* PyExportTraits<std::string>::build(const std::string& v)
{
	return impl::buildStringImpl(v.data(), v.length());
}


int PyExportTraits<std::string>::get(PyObject* obj, std::string& v)
{
	return getStringImpl(obj, v);
}


PyObject* PyExportTraits<std::wstring>::build(const std::wstring& v)
{
	return impl::buildStringImpl(v.data(), v.length());
}


int PyExportTraits<std::wstring>::get(PyObject* obj, std::wstring& v)
{
	return getWideStringImpl(obj, v);
}


#if LASS_HAVE_STD_U8STRING

PyObject* PyExportTraits<std::u8string>::build(const std::u8string& v)
{
	return impl::buildStringImpl(reinterpret_cast<const char*>(v.data()), v.length());
}


int PyExportTraits<std::u8string>::get(PyObject* obj, std::u8string& v)
{
	return getStringImpl(obj, v);
}

#endif


PyObject* PyExportTraits<std::u16string>::build(const std::u16string& v)
{
	return impl::buildStringImpl(v.data(), v.length());
}


int PyExportTraits<std::u16string>::get(PyObject* obj, std::u16string& v)
{
#if LASS_HAVE_WCHAR_T == 2
	return getWideStringImpl(obj, v);
#else
	if (!PyUnicode_Check(obj))
	{
		PyErr_SetString(PyExc_TypeError, "not a string");
		return 1;
	}
	if (PyUnicode_READY(obj) != 0)
	{
		return 1;
	}
	if (PyUnicode_KIND(obj) == PyUnicode_2BYTE_KIND)
	{
		// all code points < 65536 are represented by a single 2-byte code unit in UTF-16.
		v = std::u16string(reinterpret_cast<char16_t*>(PyUnicode_2BYTE_DATA(obj)), static_cast<size_t>(PyUnicode_GET_LENGTH(obj)));
		return 0;
	}
	TPyObjPtr bytes(PyUnicode_AsUTF16String(obj));
	if (!bytes)
	{
		return 1;
	}
	LASS_ASSERT(PyBytes_Check(bytes.get()));
	const Py_ssize_t n = PyBytes_Size(bytes.get());
	const char16_t* data = reinterpret_cast<char16_t* >(PyBytes_AsString(bytes.get()));
	// according to PyUnicode_AsUTF16String, data must always start with a BOM for native byteorder
	LASS_ASSERT(data[0] == 0xfeff);
	v = std::u16string(data + 1, static_cast<size_t>((n / 2) - 1));
	return 0;
#endif
}


PyObject* PyExportTraits<std::u32string>::build(const std::u32string& v)
{
	return impl::buildStringImpl(v.data(), v.length());
}


int PyExportTraits<std::u32string>::get(PyObject* obj, std::u32string& v)
{
#if LASS_HAVE_WCHAR_T == 4
	return getWideStringImpl(obj, v);
#else
	if (!PyUnicode_Check(obj))
	{
		PyErr_SetString(PyExc_TypeError, "not a string");
		return 1;
	}
	Py_ssize_t size = PyUnicode_GetLength(obj);
	if (size <= 0)
	{
		v = std::u32string();
		return 0;
	}
	std::u32string tmp(static_cast<size_t>(size), '\0');
	if (PyUnicode_AsUCS4(obj, reinterpret_cast<Py_UCS4*>(&tmp[0]), size, 0) == nullptr)
	{
		return 1;
	}
	v = std::move(tmp);
	return 0;
#endif
}


}
}
