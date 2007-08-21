/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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
#include "unit_test.h"
#include "../io/arg_parser.h"
#include "../io/file_attribute.h"
#include "../util/environment.h"

namespace lass
{
namespace test
{

const bool runTests(const TUnitTests& iTests, int argc, char* argv[], 
		unsigned* oNumErrors, unsigned* oNumFatalErrors)
{
	impl::errors() = 0;
	impl::fatalErrors() = 0;
	if (oNumErrors) *oNumErrors = 0;
	if (oNumFatalErrors) *oNumFatalErrors = 0;

	io::ArgParser parser(io::fileWithoutPath(argv[0]));
	io::ArgValue<std::string> savePatterns(
		parser, "", "save-pattern", "", io::amRequired | io::amMultiple);
	if (!parser.parse(argc, argv))
	{
		return false;
	}
	impl::savePatterns().insert(savePatterns.begin(), savePatterns.end());

	for (TUnitTests::const_iterator test = iTests.begin(); test != iTests.end(); ++test)
	{
		(*test)();
	}
	impl::errorLog() << std::flush;

	if (oNumErrors) *oNumErrors = impl::errors();
	if (oNumFatalErrors) *oNumFatalErrors = impl::fatalErrors();
	
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

const std::string workPath()
{
	std::string result = "";
	try
	{
		result = util::getEnvironment<std::string>("srcdir");
	}
	catch (...)
	{
	}
	return !result.empty() ? result : ".";
}

TestStream::TestStream()
{
}

TestStream::TestStream(const std::string& iPatternFile, AllowSaving iAllowSaving):
	patternFile_(iPatternFile),
	allowSaving_(iAllowSaving)
{
	if (!impl::isSavingPattern(patternFile_))
	{
		const std::string filename = io::fileJoinPath(workPath(), patternFile_);
		std::ifstream file(filename.c_str());
		if (!file)
		{
			LASS_THROW("Pattern file '" << filename << "' not found.");
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
			LASS_THROW("Reading pattern file '" << patternFile_ << "' failed: " << error.what());
		}
	}
}



TestStream::~TestStream()
{
	if (impl::isSavingPattern(patternFile_))
	{
		if (allowSaving_ == asAllowSaving)
		{
			try
			{
				const std::string filename = io::fileJoinPath(workPath(), patternFile_);
				std::ofstream file(filename.c_str());
				file << buffer_.str();
				file.close();
			}
			catch (...)
			{
				LASS_CERR << "[LASS RUN MSG] WARNING: Failed to save test pattern '"
					<< patternFile_ << "'." << std::endl;
			}
		}
		else
		{
			LASS_LOG("Not allowed to save test pattern '" << patternFile_ << "'.");
		}
	}
}



bool TestStream::isEqual(const std::string& iPattern)
{
	const bool result = buffer_.str() == iPattern;
	buffer_.str("");
	return result;
}



bool TestStream::matchPattern() 
{
	if (impl::isSavingPattern(patternFile_))
	{
		return true;
	}
	else
	{
		std::string test = buffer_.str();
		buffer_.str("");
		std::string::size_type n = test.length();

		//LASS_ASSERT(n <= pattern_.length());
		if (n > pattern_.length())
		{
			pattern_ = "";
			return false;
		}

		const bool success = test == pattern_.substr(0, n);
		if (!success)
		{
			LASS_EVAL(test);
			LASS_EVAL(pattern_.substr(0, n));
		}
		//LASS_ASSERT(success);
		pattern_ = pattern_.substr(n);

		return success;
	}
}



std::ostream& TestStream::stream() 
{ 
	return buffer_; 
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
	typedef std::set<std::string> TSavePatterns;

	ErrorStream errorStream;
	unsigned errorCount;
	unsigned fatalErrorCount;
	TSavePatterns savePatterns;

	TestStatus(): 
		errorStream("test_" LASS_TEST_VERSION "_errors.log"), 
		errorCount(0), 
		fatalErrorCount(0),
		savePatterns()
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

std::set<std::string>& savePatterns()
{
	return util::Singleton<TestStatus>::instance()->savePatterns;
}

const bool isSavingPattern(const std::string& iFilename)
{
	if (iFilename.empty())
	{
		return false;
	}
	return savePatterns().find(iFilename) != savePatterns().end();
}

}
}
}

// EOF
