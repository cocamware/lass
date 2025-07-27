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
*	Copyright (C) 2022-2024 the Initial Developer.
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
#include "py_tuple.h"

#include <datetime.h>


namespace lass
{
namespace python
{

namespace
{

struct DateTimeModule
{
	TPyObjPtr module;
	TPyObjPtr datetime;
	TPyObjPtr date;
	TPyObjPtr timedelta;
	TPyObjPtr utc;
};

DateTimeModule* datetimeModule()
{
	static DateTimeModule singleton;
	if (singleton.utc)
	{
		return &singleton;
	}

	TPyObjPtr mod(PyImport_ImportModule("datetime"));
	if (!mod)
	{
		return nullptr;
	}
	singleton.datetime = TPyObjPtr(PyObject_GetAttrString(mod.get(), "datetime"));
	if (!singleton.datetime)
	{
		return nullptr;
	}
	singleton.date = TPyObjPtr(PyObject_GetAttrString(mod.get(), "date"));
	if (!singleton.date)
	{
		return nullptr;
	}
	singleton.timedelta = TPyObjPtr(PyObject_GetAttrString(mod.get(), "timedelta"));
	if (!singleton.timedelta)
	{
		return nullptr;
	}
	TPyObjPtr timezone(PyObject_GetAttrString(mod.get(), "timezone"));
	if (!timezone)
	{
		return nullptr;
	}
	singleton.utc = TPyObjPtr(PyObject_GetAttrString(timezone.get(), "utc"));
	if (!singleton.utc)
	{
		return nullptr;
	}
	return &singleton;
}

}

namespace impl
{

PyObject* buildTimedelta(int days, int secs, int usecs)
{
	auto mod = datetimeModule();
	if (!mod)
	{
		return nullptr;
	}
	auto args = makeTuple(days, secs, usecs);
	return PyObject_CallObject(mod->timedelta.get(), args.get());
}

int getTimedelta(PyObject* obj, int &days, int &secs, int &usecs)
{
	auto mod = datetimeModule();
	if (!mod)
	{
		return 1;
	}
	if (!PyObject_IsInstance(obj, mod->timedelta.get()))
	{
		PyErr_SetString(PyExc_TypeError, "must be a datetime.timedelta");
		return 1;
	}
	TPyObjPtr daysObj{ PyObject_GetAttrString(obj, "days") };
	if (!daysObj || PyExportTraits<int>::get(daysObj.get(), days) != 0)
	{
		return 1;
	}
	TPyObjPtr secsObj{ PyObject_GetAttrString(obj, "seconds") };
	if (!secsObj || PyExportTraits<int>::get(secsObj.get(), secs) != 0)
	{
		return 1;
	}
	TPyObjPtr usecsObj{ PyObject_GetAttrString(obj, "microseconds") };
	if (!usecsObj || PyExportTraits<int>::get(usecsObj.get(), usecs) != 0)
	{
		return 1;
	}
	return 0;
}

}



PyObject* PyExportTraits<std::chrono::time_point<std::chrono::system_clock>>::build(const TTimePoint& v)
{
	auto mod = datetimeModule();
	if (!mod)
	{
		return nullptr;
	}

	using namespace std::chrono_literals;
	using TMicroSeconds = std::chrono::duration<int, std::micro>;

	const TMicroSeconds uSec = std::chrono::duration_cast<TMicroSeconds>(v.time_since_epoch() % 1s);
	const std::time_t time = TClock::to_time_t(v - uSec);

	std::tm local;
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
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

	auto args = makeTuple(
		1900 + local.tm_year,
		1 + local.tm_mon,
		local.tm_mday,
		local.tm_hour,
		local.tm_min,
		local.tm_sec,
		uSec.count()
	);
	if (!args)
	{
		return nullptr;
	}
	return PyObject_CallObject(mod->datetime.get(), args.get());
}



int PyExportTraits<std::chrono::time_point<std::chrono::system_clock>>::get(PyObject* obj, TTimePoint& v)
{
	auto mod = datetimeModule();
	if (!mod)
	{
		return 0;
	}

	using namespace std::chrono_literals;

	if (!PyObject_IsInstance(obj, mod->date.get()))
	{
		PyErr_SetString(PyExc_TypeError, "must be a datetime.datetime or a datetime.date");
		return 1;
	}

	int year, month, day;
	TPyObjPtr yearObj{ PyObject_GetAttrString(obj, "year") };
	if (!yearObj || PyExportTraits<int>::get(yearObj.get(), year) != 0)
	{
		return 1;
	}
	TPyObjPtr monthObj{ PyObject_GetAttrString(obj, "month") };
	if (!monthObj || PyExportTraits<int>::get(monthObj.get(), month) != 0)
	{
		return 1;
	}
	TPyObjPtr dayObj{ PyObject_GetAttrString(obj, "day") };
	if (!dayObj || PyExportTraits<int>::get(dayObj.get(), day) != 0)
	{
		return 1;
	}

	std::tm time {
		0, // tm_sec
		0, // tm_min
		0, // tm_hour
		day, // tm_mday
		month - 1, // tm_mon
		year - 1900, // tm_year
		0, // tm_wday
		0, // tm_yday
		-1, // tm_isdst
#if LASS_PLATFORM_TYPE != LASS_PLATFORM_TYPE_WIN32
		0, // tm_gmtoff
		nullptr, // tm_zone
#endif
	};
	std::chrono::microseconds uSec{};
	TPyObjPtr tz;

	if (PyObject_IsInstance(obj, mod->datetime.get()))
	{
		TPyObjPtr hourObj( PyObject_GetAttrString(obj, "hour") );
		if (!hourObj || PyExportTraits<int>::get(hourObj.get(), time.tm_hour) != 0)
		{
			return 1;
		}
		TPyObjPtr minuteObj( PyObject_GetAttrString(obj, "minute") );
		if (!minuteObj || PyExportTraits<int>::get(minuteObj.get(), time.tm_min) != 0)
		{
			return 1;
		}
		TPyObjPtr secObj( PyObject_GetAttrString(obj, "second") );
		if (!secObj || PyExportTraits<int>::get(secObj.get(), time.tm_sec) != 0)
		{
			return 1;
		}
		int usec = 0;
		TPyObjPtr usecObj( PyObject_GetAttrString(obj, "microsecond") );
		if (!usecObj || PyExportTraits<int>::get(usecObj.get(), usec) != 0)
		{
			return 1;
		}
		uSec = std::chrono::microseconds(usec);

		tz = TPyObjPtr( PyObject_GetAttrString(obj, "tzinfo") );
		if (!tz)
		{
			return 1;
		}
	}

	if (!tz || tz.get() == Py_None)
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
		TPyObjPtr timedelta( PyObject_CallMethod(tz.get(), "utcoffset", "O", obj) );
		if (!timedelta)
		{
			return 1;
		}
		std::chrono::system_clock::duration utcoffset{};
		if (PyExportTraits<std::chrono::system_clock::duration>::get(timedelta.get(), utcoffset) != 0)
		{
			return 1;
		}
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
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
	auto mod = datetimeModule();
	if (!mod)
	{
		return nullptr;
	}

	using namespace std::chrono_literals;

	using TMicroSeconds = std::chrono::duration<int, std::micro>;

	const auto sysTime = std::chrono::utc_clock::to_sys(v);
	const TMicroSeconds uSec = std::chrono::duration_cast<TMicroSeconds>(sysTime.time_since_epoch() % 1s);
	const std::time_t time = std::chrono::system_clock::to_time_t(sysTime - uSec);

	std::tm local;
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
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

	auto args = makeTuple(
		1900 + local.tm_year,
		1 + local.tm_mon,
		local.tm_mday,
		local.tm_hour,
		local.tm_min,
		local.tm_sec,
		uSec.count(),
		mod->utc
	);
	if (!args)
	{
		return nullptr;
	}
	return PyObject_CallObject(mod->datetime.get(), args.get());
}



int PyExportTraits<std::chrono::utc_clock::time_point>::get(PyObject* obj, std::chrono::utc_clock::time_point& v)
{
	std::chrono::system_clock::time_point sysTime;
	if (PyExportTraits<std::chrono::system_clock::time_point>::get(obj, sysTime) != 0)
	{
		return 1;
	}
	v = std::chrono::clock_cast<std::chrono::utc_clock>(sysTime);
	return 0;
}



PyObject* PyExportTraits<std::chrono::gps_clock::time_point>::build(const std::chrono::gps_clock::time_point& v)
{
	auto utcTime = std::chrono::clock_cast<std::chrono::utc_clock>(v);
	return PyExportTraits<std::chrono::utc_clock::time_point>::build(utcTime);
}



int PyExportTraits<std::chrono::gps_clock::time_point>::get(PyObject* obj, std::chrono::gps_clock::time_point& v)
{
	std::chrono::system_clock::time_point sysTime;
	if (PyExportTraits<std::chrono::system_clock::time_point>::get(obj, sysTime) != 0)
	{
		return 1;
	}
	v = std::chrono::clock_cast<std::chrono::gps_clock>(sysTime);
	return 0;
}



PyObject* PyExportTraits<std::chrono::tai_clock::time_point>::build(const std::chrono::tai_clock::time_point& v)
{
	auto utcTime = std::chrono::clock_cast<std::chrono::utc_clock>(v);
	return PyExportTraits<std::chrono::utc_clock::time_point>::build(utcTime);
}



int PyExportTraits<std::chrono::tai_clock::time_point>::get(PyObject* obj, std::chrono::tai_clock::time_point& v)
{
	std::chrono::system_clock::time_point sysTime;
	if (PyExportTraits<std::chrono::system_clock::time_point>::get(obj, sysTime) != 0)
	{
		return 1;
	}
	v = std::chrono::clock_cast<std::chrono::tai_clock>(sysTime);
	return 0;
}



PyObject* PyExportTraits<std::chrono::file_clock::time_point>::build(const std::chrono::file_clock::time_point& v)
{
	auto utcTime = std::chrono::clock_cast<std::chrono::utc_clock>(v);
	return PyExportTraits<std::chrono::utc_clock::time_point>::build(utcTime);
}



int PyExportTraits<std::chrono::file_clock::time_point>::get(PyObject* obj, std::chrono::file_clock::time_point& v)
{
	std::chrono::system_clock::time_point sysTime;
	if (PyExportTraits<std::chrono::system_clock::time_point>::get(obj, sysTime) != 0)
	{
		return 1;
	}
	v = std::chrono::clock_cast<std::chrono::file_clock>(sysTime);
	return 0;
}



PyObject* PyExportTraits<std::chrono::year_month_day>::build(const std::chrono::year_month_day& v)
{
	auto mod = datetimeModule();
	if (!mod)
	{
		return nullptr;
	}
	auto args = makeTuple(
		static_cast<int>(v.year()),
		static_cast<unsigned>(v.month()),
		static_cast<unsigned>(v.day())
	);
	if (!args)
	{
		return nullptr;
	}
	return PyObject_CallObject(mod->date.get(), args.get());
}



int PyExportTraits<std::chrono::year_month_day>::get(PyObject* obj, std::chrono::year_month_day& v)
{
	auto mod = datetimeModule();
	if (!mod)
	{
		return 1;
	}
	if (!PyObject_IsInstance(obj, mod->date.get()))
	{
		PyErr_SetString(PyExc_TypeError, "must be a datetime.date or datetime.datetime");
		return 1;
	}
	int year;
	TPyObjPtr yearObj{ PyObject_GetAttrString(obj, "year") };
	if (!yearObj || PyExportTraits<int>::get(yearObj.get(), year) != 0)
	{
		return 1;
	}
	unsigned int month;
	TPyObjPtr monthObj{ PyObject_GetAttrString(obj, "month") };
	if (!monthObj || PyExportTraits<unsigned int>::get(monthObj.get(), month) != 0)
	{
		return 1;
	}
	unsigned int day;
	TPyObjPtr dayObj{ PyObject_GetAttrString(obj, "day") };
	if (!dayObj || PyExportTraits<unsigned int>::get(dayObj.get(), day) != 0)
	{
		return 1;
	}
	v = std::chrono::year_month_day
	{
		std::chrono::year{ year },
		std::chrono::month{ month },
		std::chrono::day{ day }
	};
	return 0;
}


#endif

}
}
