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
*	Copyright (C) 2004-2022 the Initial Developer.
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

#include <datetime.h>

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


PyObject* buildWideStringImpl(const wchar_t* v, size_t size)
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


PyObject* buildU16StringImpl(const char16_t* v, size_t size)
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


PyObject* buildU32StringImpl(const char32_t* v, size_t size)
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
	return impl::buildStringImpl(v, strlen(v));
}


PyObject* PyExportTraits<std::string>::build(const std::string& v)
{
	return impl::buildStringImpl(v.data(), v.length());
}


int PyExportTraits<std::string>::get(PyObject* obj, std::string& v)
{
	return getStringImpl(obj, v);
}


PyObject* PyExportTraits<const wchar_t*>::build(const wchar_t* v)
{
	return impl::buildWideStringImpl(v, wcslen(v));
}


PyObject* PyExportTraits<std::wstring>::build(const std::wstring& v)
{
	return impl::buildWideStringImpl(v.data(), v.length());
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
	return impl::buildU16StringImpl(v.data(), v.length());
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
	return impl::buildU32StringImpl(v.data(), v.length());
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


PyObject* PyExportTraits<std::chrono::system_clock::duration>::build(TDuration v)
{
	using namespace std::chrono;
	using namespace std::chrono_literals;

	if (!PyDateTimeAPI)
	{
		PyDateTime_IMPORT;
	}

	// normally, we would just duraction_cast this to std::chrono::microseconds and be done with it
	// but PyDelta_FromDSU accepts ints, not long longs...

	using TDays = duration<int, std::ratio<86400>>;
	using TSeconds = duration<int>;
	using TMicroSeconds = duration<int, std::micro>;

	const TDays days = duration_cast<TDays>(v);
	v -= days;
	const TSeconds secs = duration_cast<TSeconds>(v);
	v -= secs;
	const TMicroSeconds uSecs = duration_cast<TMicroSeconds>(v);

	return PyDelta_FromDSU(days.count(), secs.count(), uSecs.count());
}


int PyExportTraits<std::chrono::system_clock::duration>::get(PyObject* obj, TDuration& v)
{
	if (!PyDateTimeAPI)
	{
		PyDateTime_IMPORT;
	}
	if (!PyDelta_Check(obj))
	{
		PyErr_SetString(PyExc_TypeError, "not a datetime.timedelta");
		return 1;
	}

	using TDays = std::chrono::duration<int, std::ratio<86400>>;
	using TSeconds = std::chrono::seconds;
	using TMicroSeconds = std::chrono::microseconds;

	const int days = PyDateTime_DELTA_GET_DAYS(obj);
	const int secs = PyDateTime_DELTA_GET_SECONDS(obj);
	const int uSecs = PyDateTime_DELTA_GET_MICROSECONDS(obj);
	v = TDays(days) + TSeconds(secs) + TMicroSeconds(uSecs);
	return 0;
}


PyObject* PyExportTraits<std::chrono::time_point<std::chrono::system_clock>>::build(const TTimePoint& v)
{
	using namespace std::chrono_literals;

	if (!PyDateTimeAPI)
	{
		PyDateTime_IMPORT;
	}

	using TMicroSeconds = std::chrono::duration<int, std::micro>;

	const TMicroSeconds uSec = std::chrono::duration_cast<TMicroSeconds>(v.time_since_epoch() % 1s);
	const std::time_t time = TClock::to_time_t(v - uSec);

	std::tm local;
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
	const errno_t err = localtime_s(&local, &time);
#else
	util::impl::lass_reset_errno();
	localtime_r(&time, &local);
	const int err = util::impl::lass_errno();
#endif
	if (err)
	{
		PyErr_SetString(PyExc_ValueError, util::impl::lass_strerror(err).c_str());
		return nullptr;
	}

	return PyDateTime_FromDateAndTime(
		1900 + local.tm_year,
		1 + local.tm_mon,
		local.tm_mday,
		local.tm_hour,
		local.tm_min,
		local.tm_sec,
		uSec.count()
	);
}


int PyExportTraits<std::chrono::time_point<std::chrono::system_clock>>::get(PyObject* obj, TTimePoint& v)
{
	using namespace std::chrono_literals;

	if (!PyDateTimeAPI)
	{
		PyDateTime_IMPORT;
	}
	if (!PyDate_Check(obj))
	{
		PyErr_SetString(PyExc_TypeError, "not a datetime.date or datetime.datetime");
		return 1;
	}

	std::tm local {
		0, // tm_sec
		0, // tm_min
		0, // tm_hour
		PyDateTime_GET_DAY(obj), // tm_mday
		PyDateTime_GET_MONTH(obj) - 1, // tm_mon
		PyDateTime_GET_YEAR(obj) - 1900, // tm_year
		0, // tm_wday
		0, // tm_yday
		-1, // tm_isdst
#if LASS_COMPILER_TYPE != LASS_COMPILER_TYPE_MSVC
		0, // tm_gmtoff
		nullptr, // tm_zone
#endif
	};
	std::chrono::microseconds uSec{};

	if (PyDateTime_Check(obj))
	{
		local.tm_hour = PyDateTime_DATE_GET_HOUR(obj);
		local.tm_min = PyDateTime_DATE_GET_MINUTE(obj);
		local.tm_sec = PyDateTime_DATE_GET_SECOND(obj);
		uSec = std::chrono::microseconds(PyDateTime_DATE_GET_MICROSECOND(obj));

#if PY_VERSION_HEX >= 0x030a0000 // >= 3.10
		PyObject* tz = PyDateTime_DATE_GET_TZINFO(obj);
#else
		TPyObjPtr tz_(PyObject_GetAttrString(obj, "tzinfo"));
		if (!tz_)
		{
			return 1;
		}
		PyObject* tz = tz_.get();
#endif
		if (tz != Py_None)
		{
			PyErr_SetString(PyExc_ValueError, 
				"Timezone-aware datetime.datetime objects are not allowed, "
				"all datetime objects are assumed to be in local time.");
			return 1;
		}
	}

	const std::time_t time = mktime(&local);
	v = TClock::from_time_t(time) + uSec;
	return 0;
}


#if __cpp_lib_filesystem

PyObject* PyExportTraits<std::filesystem::path>::build(const std::filesystem::path& v)
{
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
	// On Windows, paths use wchar_t: we can directly return it as unicode
	static_assert(std::is_same_v<std::filesystem::path::value_type, wchar_t>);
	return PyExportTraits<std::wstring>::build(v.native());
#else
	// On Linux, paths use char: we decode them to unicode
	static_assert(std::is_same_v<std::filesystem::path::value_type, char>);
	const std::string& s = v.native();
	if (s.size() > PY_SSIZE_T_MAX)
	{
		PyErr_SetString(PyExc_OverflowError, "input too long");
		return 0;
	}
	return PyUnicode_DecodeFSDefaultAndSize(s.data(), static_cast<Py_ssize_t>(s.size()));
#endif

}


int PyExportTraits<std::filesystem::path>::get(PyObject* obj, std::filesystem::path& v)
{
	TPyObjPtr path(PyOS_FSPath(obj));
	if (!path)
	{
		return 1;
	}
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
	// On Windows, paths use wchar_t: decode bytes to unicode and get as wstring.
	static_assert(std::is_same_v<std::filesystem::path::value_type, wchar_t>);
	PyObject* strObj = nullptr;
	if (!PyUnicode_FSDecoder(obj, &strObj))
	{
		return 1;
	}
	std::wstring str;
	const int ret = PyExportTraits<std::wstring>::get(strObj, str);
	Py_DECREF(strObj);
	if (ret == 0)
	{
		v = std::move(str);
	}
	return ret;
#else
	// On POSIX, paths use char: encode unicode to bytes and get as string
	static_assert(std::is_same_v<std::filesystem::path::value_type, char>);
	PyObject* bytesObj = nullptr;
	if (!PyUnicode_FSConverter(obj, &bytesObj))
	{
		return 1;
	}
	std::string bytes;
	const int ret = PyExportTraits<std::string>::get(bytesObj, bytes);
	Py_DECREF(bytesObj);
	if (ret == 0)
	{
		v = std::move(bytes);
	}
	return ret;
#endif
}


#endif

}
}