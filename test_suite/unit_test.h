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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_UNIT_TEST_H
#define LASS_GUARDIAN_OF_INCLUSION_TEST_UNIT_TEST_H

#include "test_common.h"
#include "../lass/num/floating_point_comparison.h"
#include "../lass/util/callback_0.h"

#define LASS_TEST_CASE(o_functor) \
	::lass::test::TestCase(::lass::util::makeCallback(o_functor), LASS_STRINGIFY(o_functor), LASS_FILE, LASS_LINE)

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

class TestCase
{
public:
	TestCase(const util::Callback0& iTest, const std::string& iName, const std::string& iFile, unsigned iLine);
	void operator()() const;
private:
	util::Callback0 test_;
	std::string name_;
	std::string file_;
	unsigned line_;
};

typedef std::vector<TestCase> TTestCases;
typedef TTestCases TUnitTest;
typedef std::map<std::string, TUnitTest> TTestSuite;

bool runTests(const TTestCases& iTests, int argc, char* argv[], unsigned* oNumErrors, unsigned* oNumFatalErrors);

const std::string workPath();
std::string& inputDir();
std::string& outputDir();

class TestStream
{
public:

	enum AllowSaving
	{
		asAllowSaving,
		asForbidSaving
	};

	TestStream();
	TestStream(const std::string& iPatternFile, AllowSaving iAllowSaving = asForbidSaving);
	~TestStream();

	template <typename T>
	TestStream& operator<<(const T& iValue)
	{
		buffer_ << iValue;
		return *this;
	}
	TestStream& operator<<(std::ostream& (*iManip) (std::ostream&))
	{
		buffer_ << iManip;
		return *this;
	}
	TestStream& operator<<(std::ios_base& (*iManip) (std::ios_base&))
	{
		buffer_ << iManip;
		return *this;
	}
	
	bool isEqual(const std::string& iPattern);
	bool matchPattern();
	std::ostream& stream();
private:
	std::stringstream buffer_;
	std::string pattern_;
	std::string patternFile_;
	AllowSaving allowSaving_;
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
std::set<std::string>& savePatterns();
bool isSavingPattern(const std::string& iFilename);

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
