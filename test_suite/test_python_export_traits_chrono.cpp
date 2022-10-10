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

#include "test_common.h"
#include "python_embedding.h"
#include "../lass/python/python_api.h"
#include "../lass/python/export_traits_chrono.h"
#include <datetime.h>

#define TEST_PYTHON_STRING_ROUNDTRIP(in, out, kind) \
	{\
		TPyObjPtr obj(pyBuildSimpleObject(in));\
		LASS_TEST_CHECK(PyUnicode_Check(obj.get()));\
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyUnicode_KIND(obj.get())), kind);\
		std::remove_const<decltype(out)>::type t;\
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), t), 0);\
		LASS_TEST_CHECK(t == out);\
	}

namespace std
{

#if !LASS_HAVE_DURATION_OPERATOR_OSTREAM
	template <class CharT, class Traits, class Rep, class Period>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const std::chrono::duration<Rep, Period>& d)
	{
		std::chrono::duration<double> secs = d;
		std::basic_ostringstream<CharT, Traits> s;
		s.flags(os.flags());
		s.imbue(os.getloc());
		s.precision(os.precision());
		s << secs.count() << "s"; // see below
		return os << s.str();
	}
#endif

	template <class CharT, class Traits, class Clock, class Duration>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const std::chrono::time_point<Clock, Duration>& t)
	{
		return os << "time_point(" << t.time_since_epoch() << ")";
	}
}

namespace lass
{
namespace test
{

void testPythonExportTraitsChronoDuration()
{
	using namespace std::chrono_literals;
	using namespace lass::python;

	initPythonEmbedding();
	LockGIL LASS_UNUSED(lock);

	if (!PyDateTimeAPI)
	{
		PyDateTime_IMPORT;
	}

	{
		using duration = std::chrono::system_clock::duration;
		duration d{};
		TPyObjPtr obj{ pyBuildSimpleObject(d) };
		LASS_TEST_CHECK(PyDelta_Check(obj.get()));
		LASS_TEST_CHECK_EQUAL(PyDateTime_DELTA_GET_DAYS(obj.get()), 0);
		LASS_TEST_CHECK_EQUAL(PyDateTime_DELTA_GET_SECONDS(obj.get()), 0);
		LASS_TEST_CHECK_EQUAL(PyDateTime_DELTA_GET_MICROSECONDS(obj.get()), 0);

		duration d2;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), d2), 0);
		LASS_TEST_CHECK_EQUAL(d, d2);
	}

	{
		using duration = std::chrono::system_clock::duration;
		duration d = 100h + 20min + 30s + 4567ms + 89us;
		TPyObjPtr obj{ pyBuildSimpleObject(d) };
		LASS_TEST_CHECK(PyDelta_Check(obj.get()));
		LASS_TEST_CHECK_EQUAL(PyDateTime_DELTA_GET_DAYS(obj.get()), 4); // 100/24
		LASS_TEST_CHECK_EQUAL(PyDateTime_DELTA_GET_SECONDS(obj.get()), 15634); // (100%24)*3600 + 20*60 + 30 + 4567/1000
		LASS_TEST_CHECK_EQUAL(PyDateTime_DELTA_GET_MICROSECONDS(obj.get()), 567089); // (4567%1000)*1000 + 89

		duration d2;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), d2), 0);
		LASS_TEST_CHECK_EQUAL(d, d2);
	}
}



void testPythonExportTraitsChronoSystemClock()
{
	using namespace std::chrono_literals;
	using namespace lass::python;

	initPythonEmbedding();
	LockGIL LASS_UNUSED(lock);

	if (!PyDateTimeAPI)
	{
		PyDateTime_IMPORT;
	}

	{
		using time_point = std::chrono::time_point<std::chrono::system_clock>;
		time_point t{};
		TPyObjPtr obj{ pyBuildSimpleObject(t) };
		LASS_TEST_CHECK(PyDateTime_Check(obj.get()));

		// this one is tricky, as we can't simply compare it with 00:00 1 January 1970.
		// it has been shifted to local time ...
		std::time_t time = 0;
		std::tm local;
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
		LASS_TEST_CHECK_EQUAL(localtime_s(&local, &time), 0);
#else
		localtime_r(&time, &local);
#endif
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyDateTime_GET_DAY(obj.get())), local.tm_mday);
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyDateTime_GET_MONTH(obj.get())), local.tm_mon + 1);
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyDateTime_GET_YEAR(obj.get())), local.tm_year + 1900);
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyDateTime_DATE_GET_HOUR(obj.get())), local.tm_hour);
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyDateTime_DATE_GET_MINUTE(obj.get())), local.tm_min);
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyDateTime_DATE_GET_SECOND(obj.get())), local.tm_sec);
		LASS_TEST_CHECK_EQUAL(static_cast<int>(PyDateTime_DATE_GET_MICROSECOND(obj.get())), 0);
#if PY_VERSION_HEX >= 0x030a0000 // >= 3.10
		LASS_TEST_CHECK_EQUAL(PyDateTime_DATE_GET_TZINFO(obj.get()), Py_None);
#else
		TPyObjPtr tz(PyObject_GetAttrString(obj.get(), "tzinfo"));
		LASS_TEST_CHECK_EQUAL(tz.get(), Py_None);
#endif

		time_point t2;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), t2), 0);
		LASS_TEST_CHECK_EQUAL(t, t2);
	}

	{
		using time_point = std::chrono::time_point<std::chrono::system_clock>;
		time_point t = time_point::clock::now();
		TPyObjPtr obj{ pyBuildSimpleObject(t) };
		LASS_TEST_CHECK(PyDateTime_Check(obj.get()));
		time_point t2;
		LASS_TEST_CHECK_EQUAL(pyGetSimpleObject(obj.get(), t2), 0);
		time_point::duration d = t - t2;
		if (d < 0s)
			d = -d; // there's no abs until C++17
		LASS_TEST_CHECK(d < 1us);
	}
}

TUnitTest test_python_export_traits_chrono()
{
	return TUnitTest
	{
		LASS_TEST_CASE(testPythonExportTraitsChronoDuration),
		LASS_TEST_CASE(testPythonExportTraitsChronoSystemClock),
	};
}

}

}

// EOF
