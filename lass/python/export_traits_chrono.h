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

#pragma once

#include "python_common.h"
#include "export_traits.h"
#include <chrono>

namespace lass
{
namespace python
{

namespace impl
{
	LASS_PYTHON_DLL PyObject* buildTimedelta(int days, int secs, int usecs);
	LASS_PYTHON_DLL int getTimedelta(PyObject* obj, int &days, int &secs, int &usecs);
}

/** @ingroup Python
 *
 *  std::chrono::duration is mapped on datetime.timedelta by copy.
 */
template <typename Rep, typename Period>
struct PyExportTraits<std::chrono::duration<Rep, Period>>
{
	using TDuration = std::chrono::duration<Rep, Period>;

	static PyObject* build(TDuration v)
	{
		using namespace std::chrono;
		using TDays = duration<int, std::ratio<86400>>;
		using TSeconds = duration<int>;
		using TMicroSeconds = duration<num::Tint64, std::micro>;

		TMicroSeconds usecs = duration_cast<TMicroSeconds>(v);
		const TDays days = duration_cast<TDays>(usecs);
		usecs -= days;
		const TSeconds secs = duration_cast<TSeconds>(usecs);
		usecs -= secs;

		return impl::buildTimedelta(days.count(), secs.count(), static_cast<int>(usecs.count()));
	}
	static int get(PyObject* obj, TDuration& v)
	{
		using namespace std::chrono;
		using TDays = duration<int, std::ratio<86400>>;

		int days, secs, usecs;
		if (impl::getTimedelta(obj, days, secs, usecs) != 0)
		{
			return 1;
		}
		v = duration_cast<TDuration>(TDays(days)) +
			duration_cast<TDuration>(seconds(secs)) +
			duration_cast<TDuration>(microseconds(usecs));
		return 0;
	}
};


/** @ingroup Python
 *
 *  std::chrono::time_point<std::chrono::system_clock> is mapped on a
 *  timezone-unaware datetime.datetime instance by copy, in local time.
 *
 *  datetime.datetime instances with a timezone will correctly be
 *  converted when interpreted as a time_point.
 */
template <>
struct PyExportTraits<std::chrono::time_point<std::chrono::system_clock>>
{
	using TClock = std::chrono::system_clock;
	using TDuration = TClock::duration;
	using TTimePoint = std::chrono::time_point<TClock>;
	LASS_PYTHON_DLL static PyObject* build(const TTimePoint& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, TTimePoint& v);
};


#ifdef LASS_HAVE_STD_CHRONO_CPP20


/** @ingroup Python
 *
 *  std::chrono::utc_clock::time_point is mapped on a timezone-aware
 *  datetime.datetime instance by copy, in UTC.
 *
 *  datetime.datetime instances with other timezones will correctly be converted
 *  when interpreted as a time_point.
 *
 *  Naive datetime.datetime instances without timezone will be interpreted as
 *  local time.
 */
template <>
struct PyExportTraits<std::chrono::utc_clock::time_point>
{
	LASS_PYTHON_DLL static PyObject* build(const std::chrono::utc_clock::time_point& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::chrono::utc_clock::time_point& v);
};



/** @ingroup Python
 *
 *  std::chrono::gps_clock::time_point is mapped on a timezone-aware
 *  datetime.datetime instance by copy, in UTC.
 *
 *  datetime.datetime instances with other timezones will correctly be converted
 *  when interpreted as a time_point.
 *
 *  Naive datetime.datetime instances without timezone will be interpreted as
 *  local time.
 */
template <>
struct PyExportTraits<std::chrono::gps_clock::time_point>
{
	LASS_PYTHON_DLL static PyObject* build(const std::chrono::gps_clock::time_point& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::chrono::gps_clock::time_point& v);
};



/** @ingroup Python
 *
 *  std::chrono::tai_clock::time_point is mapped on a timezone-aware
 *  datetime.datetime instance by copy, in UTC.
 *
 *  datetime.datetime instances with other timezones will correctly be converted
 *  when interpreted as a time_point.
 *
 *  Naive datetime.datetime instances without timezone will be interpreted as
 *  local time.
 */
template <>
struct PyExportTraits<std::chrono::tai_clock::time_point>
{
	LASS_PYTHON_DLL static PyObject* build(const std::chrono::tai_clock::time_point& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::chrono::tai_clock::time_point& v);
};



/** @ingroup Python
 *
 *  std::chrono::file_clock::time_point is mapped on a timezone-aware
 *  datetime.datetime instance by copy, in UTC.
 *
 *  datetime.datetime instances with other timezones will correctly be converted
 *  when interpreted as a time_point.
 *
 *  Naive datetime.datetime instances without timezone will be interpreted as
 *  local time.
 */
template <>
struct PyExportTraits<std::chrono::file_clock::time_point>
{
	LASS_PYTHON_DLL static PyObject* build(const std::chrono::file_clock::time_point& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::chrono::file_clock::time_point& v);
};



/** @ingroup Python
 *
 *  std::chrono::year_month_day is mapped on a datetime.date instance
 *  by copy.
 */
template <>
struct PyExportTraits<std::chrono::year_month_day>
{
	LASS_PYTHON_DLL static PyObject* build(const std::chrono::year_month_day& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::chrono::year_month_day& v);
};


#endif


}
}

// EOF
