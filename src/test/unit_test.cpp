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

#include "test_common.h"
#include "unit_test.h"

#if LASS_PLATFORM_TYPE != LASS_PLATFORM_TYPE_WIN32
#	include <stdlib.h>
#endif

namespace lass
{
namespace test
{

bool runTests(const TUnitTests& iTests)
{
	impl::errors() = 0;
	impl::fatalErrors() = 0;

	for (TUnitTests::const_iterator test = iTests.begin(); test != iTests.end(); ++test)
	{
		(*test)();
	}
	impl::errorLog() << std::flush;

	if (impl::fatalErrors())
	{
		LASS_COUT << "\n*** " << impl::fatalErrors() << " fatal errors, " << impl::errors() 
			<< " errors detected" << std::endl << std::flush;
	}
	else if (impl::errors())
	{
		LASS_COUT << "\n*** " << impl::errors() << " errors detected" << std::endl << std::flush;
	}
	else
	{
		LASS_COUT << "\n*** Congratulations, no errors detected" << std::endl << std::flush;
		return true;
	}
	return false;
}

std::string workPath()
{
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
	const char* result = 0;
#else
	const char* result = getenv("srcdir");
#endif
	return result ? result : ".";
}

TestStream::TestStream() 
{
}

TestStream::TestStream(const std::string& iPatternFile)
{
	std::ifstream file(iPatternFile.c_str());
	if (!file)
	{
		LASS_THROW("Pattern file '" << iPatternFile << "' not found.");
	}
	try
	{
		std::string line;
		while (std::getline(LASS_ENFORCE_STREAM(file), line, '\n'))
		{
			pattern_ += line + '\n';
		}
	}
	catch (std::exception& error)
	{
		LASS_THROW("Reading pattern file '" << iPatternFile << "' failed: " << error.what());
	}
}



bool TestStream::isEqual(const std::string& iPattern)
{
	const bool result = stream_.str() == iPattern;
	stream_.str("");
	return result;
}



bool TestStream::matchPattern() 
{
	std::string test = stream_.str();
	stream_.str("");
	std::string::size_type n = test.length();

	if (n > pattern_.length())
	{
		pattern_ = "";
		return false;
	}

	const bool success = test == pattern_.substr(0, n);
	pattern_ = pattern_.substr(n);

	return success;
}



std::ostream& TestStream::stream() 
{ 
	return stream_; 
}



namespace impl
{

UnitTest::UnitTest(const util::Callback0& iTest, const std::string& iName, const std::string& iFile, unsigned iLine): 
	test_(iTest), 
	name_(iName),
	file_(iFile),
	line_(iLine)
{
}



void UnitTest::operator()() const
{
	LASS_CLOG << "* " << name_ << std::endl;
	try
	{
		test_(); 
		return;
	}
	LASS_TEST_IMPL_CATCH_EXCEPTIONS("fatal error in unit test '" + name_ + "'", file_, line_)
	--errors();
	++fatalErrors();
}



struct TestStatus
{
	ErrorStream errorStream;
	unsigned errorCount;
	unsigned fatalErrorCount;

	TestStatus(): 
		errorStream("test_" LASS_TEST_VERSION "_errors.log"), 
		errorCount(0), 
		fatalErrorCount(0) 
	{
	}
};

ErrorStream& errorLog()
{
	return util::Singleton<TestStatus>::instance()->errorStream;
}

unsigned& errors()
{
	return util::Singleton<TestStatus>::instance()->errorCount;
}

unsigned& fatalErrors()
{
	return util::Singleton<TestStatus>::instance()->fatalErrorCount;
}

}
}
}

// EOF
