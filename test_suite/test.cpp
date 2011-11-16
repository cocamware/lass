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
 *	Copyright (C) 2004-2011 the Initial Developer.
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
#include "default_arguments.h"

#include "../lass/stde/range_algorithm.h"
#include "../lass/io/logger.h"
#include "../lass/io/file_attribute.h"
#include "../lass/io/arg_parser.h"

::lass::test::TTestSuite autoTestSuite();

int main(int argc, char* argv[])
{
	using namespace lass;

	io::ArgParser parser(io::fileWithoutPath(argv[0]));
	io::ArgValue<std::string> log(parser, "l", "log", "", io::amRequired, "test_" LASS_TEST_VERSION ".log");
	io::ArgValue<std::string> inputDir(parser, "i", "input-dir", "", io::amRequired, test::defaultInputDir);
	io::ArgValue<std::string> outputDir(parser, "o", "output-dir", "", io::amRequired);
	io::ArgValue<std::string> savePatterns(parser, "", "save-pattern", "", io::amRequired | io::amMultiple);
	io::ArgParser::TArguments selectedTests;
	if (!parser.parse(argc, argv, &selectedTests))
	{
		return false;
	}
	::lass::test::inputDir() = inputDir.at(0);
	::lass::test::outputDir() = outputDir ? outputDir.at(0) : test::workPath();
	const std::string logFile = io::fileJoinPath(::lass::test::outputDir(), log.at(0));
	::lass::test::impl::savePatterns().insert(savePatterns.begin(), savePatterns.end());
	io::Logger logger(logFile);
	logger.subscribeTo(io::proxyMan()->cout());
	logger.subscribeTo(io::proxyMan()->clog());
	logger.subscribeTo(io::proxyMan()->cerr());

	LASS_EVAL(logFile);
	LASS_COUT << "LASS_VESION: " << LASS_VERSION << std::endl;
	LASS_COUT << "LASS_TEST_VERSION: " << LASS_TEST_VERSION << std::endl;
	LASS_COUT << "LASS_PLATFORM: " << LASS_PLATFORM << std::endl;
	LASS_COUT << "LASS_ADDRESS_SIZE: " << LASS_ADDRESS_SIZE << std::endl;
	LASS_COUT << "LASS_COMPILER: " << LASS_COMPILER << std::endl;
	LASS_COUT << "LASS_COMPILER_VERSION: " << LASS_COMPILER_VERSION << std::endl;

	test::TTestSuite testSuite = autoTestSuite();	
	test::TUnitTest testsToBeRun;

	if (selectedTests.empty())
	{
		for (test::TTestSuite::const_iterator unitTest = testSuite.begin(); unitTest != testSuite.end(); ++unitTest)
		{
			stde::copy_r(unitTest->second, std::back_inserter(testsToBeRun));
		}
	}
	else
	{
		const size_t n = selectedTests.size();
		for (test::TTestSuite::const_iterator unitTest = testSuite.begin(); unitTest != testSuite.end(); ++unitTest)
		{
			for (size_t i = 0; i < n; ++i)
			{
				if (unitTest->first.find(selectedTests[i]) != std::string::npos)
				{
					stde::copy_r(unitTest->second, std::back_inserter(testsToBeRun));
				}
			}
		}
	}
	unsigned errors = 0;
	unsigned fatalErrors = 0;
	const bool success = test::runTests(testsToBeRun, argc, argv, &errors, &fatalErrors);

	if (success)
	{
		return 0;
	}
	const unsigned totalErrors = errors + fatalErrors;
	return totalErrors < 255 ? static_cast<int>(totalErrors) : 255;
}

// EOF
