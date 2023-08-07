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
*	Copyright (C) 2022-2023 the Initial Developer.
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
#include "export_traits_chrono.h"

#include <datetime.h>


namespace lass
{
namespace python
{

namespace impl
{

PyObject* buildTimedelta(int days, int secs, int usecs)
{
	if (!PyDateTimeAPI)
	{
		PyDateTime_IMPORT;
	}
	return PyDelta_FromDSU(days, secs, usecs);
}

int getTimedelta(PyObject* obj, int &days, int &secs, int &usecs)
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
	days = PyDateTime_DELTA_GET_DAYS(obj);
	secs = PyDateTime_DELTA_GET_SECONDS(obj);
	usecs = PyDateTime_DELTA_GET_MICROSECONDS(obj);
	return 0;
}

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

	std::tm time {
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
	PyObject* tz = Py_None;

	if (PyDateTime_Check(obj))
	{
		time.tm_hour = PyDateTime_DATE_GET_HOUR(obj);
		time.tm_min = PyDateTime_DATE_GET_MINUTE(obj);
		time.tm_sec = PyDateTime_DATE_GET_SECOND(obj);
		uSec = std::chrono::microseconds(PyDateTime_DATE_GET_MICROSECOND(obj));

#if PY_VERSION_HEX >= 0x030a0000 // >= 3.10
		tz = PyDateTime_DATE_GET_TZINFO(obj);
#else
		TPyObjPtr tz_(PyObject_GetAttrString(obj, "tzinfo"));
		if (!tz_)
		{
			return 1;
		}
		tz = tz_.get();
#endif
	}

	if (tz == Py_None)
	{
		// naive datetime is assumed to be local time. dates are always naive too.
		const std::time_t t = mktime(&time);
		if (t < 0)
		{
			PyErr_SetString(PyExc_ValueError, "out of range");
			return 1;
		}
		v = TClock::from_time_t(t) + uSec;
	}
	else
	{
		TPyObjPtr timedelta{ PyObject_CallMethod(tz, "utcoffset", "O", obj) };
		if (!timedelta)
		{
			return 1;
		}
		std::chrono::system_clock::duration utcoffset{};
		if (PyExportTraits<std::chrono::system_clock::duration>::get(timedelta.get(), utcoffset) != 0)
		{
			return 1;
		}
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
		const std::time_t t = _mkgmtime(&time);
#else
		const std::time_t t = timegm(&time);
#endif
		if (t < 0)
		{
			PyErr_SetString(PyExc_ValueError, "out of range");
			return 1;
		}
		v = TClock::from_time_t(t) + uSec - utcoffset;
	}

	return 0;
}



#ifdef LASS_HAVE_STD_CHRONO_CPP20

PyObject* PyExportTraits<std::chrono::utc_clock::time_point>::build(const std::chrono::utc_clock::time_point& v)
{
	using namespace std::chrono_literals;

	if (!PyDateTimeAPI)
	{
		PyDateTime_IMPORT;
	}

	using TMicroSeconds = std::chrono::duration<int, std::micro>;

	const auto sysTime = std::chrono::utc_clock::to_sys(v);
	const TMicroSeconds uSec = std::chrono::duration_cast<TMicroSeconds>(sysTime.time_since_epoch() % 1s);
	const std::time_t time = std::chrono::system_clock::to_time_t(sysTime - uSec);

	std::tm local;
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
	const errno_t err = gmtime_s(&local, &time);
#else
	util::impl::lass_reset_errno();
	gmtime_r(&time, &local);
	const int err = util::impl::lass_errno();
#endif
	if (err)
	{
		PyErr_SetString(PyExc_ValueError, util::impl::lass_strerror(err).c_str());
		return nullptr;
	}

	PyObject* tz = PyDateTime_TimeZone_UTC;

	return PyDateTimeAPI->DateTime_FromDateAndTime(
		1900 + local.tm_year,
		1 + local.tm_mon,
		local.tm_mday,
		local.tm_hour,
		local.tm_min,
		local.tm_sec,
		uSec.count(),
		tz,
		PyDateTimeAPI->DateTimeType
	);
}



int PyExportTraits<std::chrono::utc_clock::time_point>::get(PyObject* obj, std::chrono::utc_clock::time_point& v)
{
	std::chrono::system_clock::time_point sysTime;
	if (PyExportTraits<std::chrono::system_clock::time_point>::get(obj, sysTime) != 0)
	{
		return 1;
	}
	v = std::chrono::utc_clock::from_sys(sysTime);
	return 0;
}



PyObject* PyExportTraits<std::chrono::gps_clock::time_point>::build(const std::chrono::gps_clock::time_point& v)
{
	auto utcTime = std::chrono::gps_clock::to_utc(v);
	return PyExportTraits<std::chrono::utc_clock::time_point>::build(utcTime);
}



int PyExportTraits<std::chrono::gps_clock::time_point>::get(PyObject* obj, std::chrono::gps_clock::time_point& v)
{
	std::chrono::utc_clock::time_point utcTime;
	if (PyExportTraits<std::chrono::utc_clock::time_point>::get(obj, utcTime) != 0)
	{
		return 1;
	}
	v = std::chrono::gps_clock::from_utc(utcTime);
	return 0;
}



PyObject* PyExportTraits<std::chrono::tai_clock::time_point>::build(const std::chrono::tai_clock::time_point& v)
{
	auto utcTime = std::chrono::tai_clock::to_utc(v);
	return PyExportTraits<std::chrono::utc_clock::time_point>::build(utcTime);
}



int PyExportTraits<std::chrono::tai_clock::time_point>::get(PyObject* obj, std::chrono::tai_clock::time_point& v)
{
	std::chrono::utc_clock::time_point utcTime;
	if (PyExportTraits<std::chrono::utc_clock::time_point>::get(obj, utcTime) != 0)
	{
		return 1;
	}
	v = std::chrono::tai_clock::from_utc(utcTime);
	return 0;
}



PyObject* PyExportTraits<std::chrono::file_clock::time_point>::build(const std::chrono::file_clock::time_point& v)
{
	auto utcTime = std::chrono::file_clock::to_utc(v);
	return PyExportTraits<std::chrono::utc_clock::time_point>::build(utcTime);
}



int PyExportTraits<std::chrono::file_clock::time_point>::get(PyObject* obj, std::chrono::file_clock::time_point& v)
{
	std::chrono::utc_clock::time_point utcTime;
	if (PyExportTraits<std::chrono::utc_clock::time_point>::get(obj, utcTime) != 0)
	{
		return 1;
	}
	v = std::chrono::file_clock::from_utc(utcTime);
	return 0;
}



PyObject* PyExportTraits<std::chrono::year_month_day>::build(const std::chrono::year_month_day& v)
{
	if (!PyDateTimeAPI)
	{
		PyDateTime_IMPORT;
	}
	return PyDate_FromDate(
		static_cast<int>(v.year()),
		static_cast<unsigned>(v.month()),
		static_cast<unsigned>(v.day())
	);
}



int PyExportTraits<std::chrono::year_month_day>::get(PyObject* obj, std::chrono::year_month_day& v)
{
	if (!PyDateTimeAPI)
	{
		PyDateTime_IMPORT;
	}
	if (!PyDate_Check(obj))
	{
		PyErr_SetString(PyExc_TypeError, "not a datetime.date");
		return 1;
	}
	v = std::chrono::year_month_day
	{
		std::chrono::year{ PyDateTime_GET_YEAR(obj) },
		std::chrono::month{ PyDateTime_GET_MONTH(obj) },
		std::chrono::day{ PyDateTime_GET_DAY(obj) }
	};
	return 0;
}


#endif

}
}
