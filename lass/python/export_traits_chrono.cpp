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
*	Copyright (C) 2022 the Initial Developer.
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

}
}