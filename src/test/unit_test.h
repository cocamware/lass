/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_UNIT_TEST_H
#define LASS_GUARDIAN_OF_INCLUSION_TEST_UNIT_TEST_H

#include "test_common.h"
#include "../num/floating_point_comparison.h"
#include "../util/callback_0.h"

#define LASS_UNIT_TEST(o_functor) \
	::lass::test::impl::UnitTest(::lass::util::makeCallback(o_functor), LASS_STRINGIFY(o_functor), LASS_FILE, LASS_LINE)

#define LASS_TEST_ERROR(s_message)\
	LASS_TEST_IMPL_ERROR(s_message, LASS_FILE, LASS_LINE)

#define LASS_TEST_CATCH_EXCEPTIONS(s_message) \
	LASS_TEST_IMPL_CATCH_EXCEPTIONS(s_message, LASS_FILE, LASS_LINE)

#define LASS_TEST_CHECK(e_predicate) \
	::lass::test::impl::check((e_predicate), "test '" LASS_STRINGIFY(e_predicate) "' failed.", LASS_FILE, LASS_LINE)

#define LASS_TEST_CHECK_MESSAGE(e_predicate, s_message) \
	::lass::test::impl::check((e_predicate), s_message, LASS_FILE, LASS_LINE)

#define LASS_TEST_CHECK_EQUAL(a, b) \
	::lass::test::impl::checkEqual((a), (b), LASS_STRINGIFY(a), LASS_STRINGIFY(b), LASS_FILE, LASS_LINE)

#define LASS_TEST_CHECK_LEXICAL(a, b) \
	::lass::test::impl::checkEqual( \
		::lass::util::stringCast< ::std::string >(a), ::lass::util::stringCast< ::std::string >(b), \
		LASS_STRINGIFY(a), LASS_STRINGIFY(b), LASS_FILE, LASS_LINE)

#define LASS_TEST_CHECK_CLOSE(a, b, relative_tolerance) \
	::lass::test::impl::checkClose( \
		(a), (b), (relative_tolerance), LASS_STRINGIFY(a), LASS_STRINGIFY(b), LASS_FILE, LASS_LINE)

#define LASS_TEST_CHECK_CLOSE_ARRAY(a, b, relative_tolerance, size) \
	::lass::test::impl::checkCloseArray( \
		(a), (b), (relative_tolerance), (size), LASS_STRINGIFY(a), LASS_STRINGIFY(b), LASS_FILE, LASS_LINE)

#define LASS_TEST_CHECK_NO_THROW(e_predicate) \
	do \
	{ \
		try \
		{ \
			e_predicate; \
		} \
		LASS_TEST_CATCH_EXCEPTIONS("exception was thrown by '" LASS_STRINGIFY(e_predicate) "'") \
	} \
	while (false)

#define LASS_TEST_CHECK_THROW(e_predicate, exception_type) \
	do \
	{ \
		try \
		{ \
			try \
			{ \
				e_predicate; \
			} \
			catch(const exception_type&) \
			{ \
				break; \
			} \
		} \
		LASS_TEST_CATCH_EXCEPTIONS("wrong exception was thrown by '" LASS_STRINGIFY(e_predicate) "'") \
		LASS_TEST_ERROR("no exception was thrown by '" << LASS_STRINGIFY(e_predicate) << "'."); \
	} \
	while (false)

namespace lass
{
namespace test
{

namespace impl { class UnitTest; }
typedef std::vector<impl::UnitTest> TUnitTests;

bool runTests(const TUnitTests& iTests);

std::string workPath();

class TestStream
{
public:

	TestStream();
	TestStream(const std::string& iPatternFile);

	template <typename T>
	TestStream& operator<<(const T& iValue)
	{
		stream_ << iValue;
		return *this;
	}
	TestStream& operator<<(std::ostream& (*iManip) (std::ostream&))
	{
		stream_ << iManip;
		return *this;
	}
	TestStream& operator<<(std::ios_base& (*iManip) (std::ios_base&))
	{
		stream_ << iManip;
		return *this;
	}
	
	bool isEqual(const std::string& iPattern);
	bool matchPattern();
	std::ostream& stream();
private:
	std::stringstream stream_;
	std::string pattern_;
};

}
}

// --- implementation ------------------------------------------------------------------------------

#define LASS_TEST_IMPL_ERROR(s_message, s_file, i_line)\
		++::lass::test::impl::errors();\
		::lass::test::impl::errorLog() << s_file << "(" << i_line << "): " << s_message << std::endl

#define LASS_TEST_IMPL_CATCH_EXCEPTION(t_exception, s_message, s_file, i_line)\
	catch (const t_exception& error)\
	{\
		LASS_TEST_IMPL_ERROR(s_message << ": " << LASS_STRINGIFY(t_exception) << " (" << error.what() << ").", s_file, i_line);\
	}

#define LASS_TEST_IMPL_CATCH_EXCEPTIONS(s_message, s_file, i_line)\
	LASS_TEST_IMPL_CATCH_EXCEPTION(::lass::util::Exception, s_message, s_file, i_line)\
	LASS_TEST_IMPL_CATCH_EXCEPTION(::std::exception, s_message, s_file, i_line)\
	catch (...)\
	{\
		LASS_TEST_IMPL_ERROR(s_message << ": unknown exception.", s_file, i_line);\
	}


namespace lass
{
namespace test
{
namespace impl
{

class UnitTest
{
public:
	UnitTest(const util::Callback0& iTest, const std::string& iName, const std::string& iFile, unsigned iLine);
	void operator()() const;
private:
	util::Callback0 test_;
	std::string name_;
	std::string file_;
	unsigned line_;
};

class ErrorStream
{
public:

	ErrorStream(const std::string& iLogFile):
		log_(iLogFile.c_str())
	{
	}

	template <typename T>
	ErrorStream& operator<<(const T& iValue)
	{
		log_ << iValue;
		LASS_CERR << iValue;
		return *this;
	}
	ErrorStream& operator<<(std::ostream& (*iManip) (std::ostream&))
	{
		log_ << iManip;
		LASS_CERR << iManip;
		return *this;
	}
	ErrorStream& operator<<(std::ios_base& (*iManip) (std::ios_base&))
	{
		log_ << iManip;
		LASS_CERR << iManip;
		return *this;
	}
private:
	std::ofstream log_;
};

// accessors

ErrorStream& errorLog();
unsigned& errors();
unsigned& fatalErrors();

// checkers ...

inline void check(bool iSuccess, const std::string& message, const std::string& iFile, 
		   unsigned iLine)
{
	if (!iSuccess)
	{
		LASS_TEST_IMPL_ERROR(message, iFile, iLine);
	}
}

template <typename A, typename B>
void checkEqual(const A& iA, const B& iB, const std::string& iAString, const std::string& iBString,
                const std::string& iFile, unsigned iLine)
{
	if (iA != iB)
	{
		LASS_TEST_IMPL_ERROR("test '" << iAString << " == " << iBString << "' failed (" << iA << " != " << iB << ").", iFile, iLine);
	}
}

template <typename T>
void checkClose(const T& iA, const T& iB, const T& iRelativeTolerance, const std::string& iAString,
				const std::string& iBString, const std::string& iFile, unsigned iLine)
{
	if (!num::almostEqual(iA, iB, iRelativeTolerance))
	{
		LASS_TEST_IMPL_ERROR("test '" << iAString << " == " << iBString << "' failed (" << iA << " != " << iB 
			<< " within relative tolerance " << iRelativeTolerance << ").", iFile, iLine);
	}
}

template <typename T>
void checkClose(const std::complex<T>& iA, const std::complex<T>& iB, const T& iRelativeTolerance, 
	const std::string& iAString, const std::string& iBString, const std::string& iFile, unsigned iLine)
{
	if (!num::almostEqual(iA.real(), iB.real(), iRelativeTolerance) || !num::almostEqual(iA.imag(), iB.imag(), iRelativeTolerance))
	{
		LASS_TEST_IMPL_ERROR("test '" << iAString << " == " << iBString << "' failed (" << iA << " != " << iB
			<< " withing relative tolerance " << iRelativeTolerance << ").", iFile, iLine);
	}
}

template <typename V, typename T>
void checkCloseArray(const V& iA, const V& iB, const T& iRelativeTolerance, size_t iSize, const std::string& iAString,
				const std::string& iBString, const std::string& iFile, unsigned iLine)
{
	for (size_t k = 0; k < iSize; ++k)
	{
		if (!num::almostEqual(iA[k], iB[k], iRelativeTolerance))
		{
			LASS_TEST_IMPL_ERROR("test '" << iAString << " == " << iBString << "' failed in component " << k 
				<< " (" << iA << " != " << iB << " within relative tolerance " << iRelativeTolerance << ").", 
				iFile, iLine);
		}
	}
}

}
}
}

#endif

// EOF
