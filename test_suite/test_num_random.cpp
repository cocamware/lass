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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

#include "../lass/num/random.h"
#include "../lass/num/distribution.h"
#include "../lass/io/file_attribute.h"

namespace lass
{
namespace test
{

void testNumRandomMersenne()
{
	// we use a test file provided by the mersenne twister authors.
	//
	TestStream output(io::fileJoinPath(test::inputDir(), "mt19937ar.out"), TestStream::asForbidSaving);

	// just something in the file
	//
	output << "1000 outputs of genrand_int32()\n";
	LASS_TEST_CHECK(output.matchPattern());

	// initialize the generator with an array of 4 values
	//
	const num::RandomMT19937::TValue keys[4] = {0x123, 0x234, 0x345, 0x456};
	num::RandomMT19937 generator(keys, keys + 4);

	// test 1000 draws, 200 lines of 5 each.
	for (unsigned i = 0; i < 200; ++i)
	{
		for (unsigned j = 0; j < 5; ++j)
		{
			output << std::setw(10) << generator() << " ";
		}
		output << "\n";
	}
	LASS_TEST_CHECK(output.matchPattern());
}


void testNumRandomXKCD()
{
	num::RandomXKCD random;
	for (size_t i = 0; i < 1000; ++i)
	{
		LASS_TEST_CHECK_EQUAL(random(), 4);
	}
}


void testNumDistributions()
{
	typedef double TValue;
	typedef num::NumTraits<TValue> TNumTraits;

	enum { testSize = 10000 };
	const double tolerance = .05;

	num::RandomMT19937 random;
	unsigned i;
	TValue mean, variance;

	// unit uniform distribution
	//
	num::DistributionUniform<double, num::RandomMT19937, num::rtOpen> distUnitUniform(random);
	mean = TNumTraits::zero;
	variance = TNumTraits::zero;
	for (i = 0; i < testSize; ++i)
	{
		const TValue draw = distUnitUniform();
		LASS_TEST_CHECK(draw > 0 && draw < 1);
		mean += draw;
		variance += draw * draw;
	}
	mean /= testSize;
	variance = variance / testSize - mean * mean;
	LASS_TEST_CHECK_CLOSE(mean, 0.5, tolerance);
	LASS_TEST_CHECK_CLOSE(variance, 1. / 12., tolerance);

	// uniform distribution
	//
	num::DistributionUniform<double, num::RandomMT19937, num::rtOpen> distUniform(random, -100., 200.);
	mean = TNumTraits::zero;
	variance = TNumTraits::zero;
	for (i = 0; i < testSize; ++i)
	{
		const TValue draw = distUniform();
		mean += draw;
		variance += draw * draw;
	}
	mean /= testSize;
	variance = variance / testSize - mean * mean;
	LASS_TEST_CHECK_CLOSE(mean, 50., tolerance);
	LASS_TEST_CHECK_CLOSE(variance, 300. * 300. / 12., tolerance);

	// unit exponential distribution
	//
	num::DistributionExponential<double, num::RandomMT19937> distUnitExponential(random);
	mean = TNumTraits::zero;
	variance = TNumTraits::zero;
	for (i = 0; i < testSize; ++i)
	{
		const TValue draw = distUnitExponential();
		mean += draw;
		variance += draw * draw;
	}
	mean /= testSize;
	variance = variance / testSize - mean * mean;
	LASS_TEST_CHECK_CLOSE(mean, 1., tolerance);
	LASS_TEST_CHECK_CLOSE(variance, 1., tolerance);

	// exponential distribution
	//
	num::DistributionExponential<double, num::RandomMT19937> distExponential(random, 100.);
	mean = TNumTraits::zero;
	variance = TNumTraits::zero;
	for (i = 0; i < testSize; ++i)
	{
		const TValue draw = distExponential();
		mean += draw;
		variance += draw * draw;
	}
	mean /= testSize;
	variance = variance / testSize - mean * mean;
	LASS_TEST_CHECK_CLOSE(mean, 1. / 100., tolerance);
	LASS_TEST_CHECK_CLOSE(variance, 1. / (100. * 100.), tolerance);

	// normal distribution
	//
	num::DistributionNormal<double, num::RandomMT19937> distUnitNormal(random);
	mean = TNumTraits::zero;
	variance = TNumTraits::zero;
	for (i = 0; i < testSize; ++i)
	{
		const TValue draw = distUnitNormal();
		mean += draw;
		variance += draw * draw;
	}
	mean /= testSize;
	variance = variance / testSize - mean * mean;
	LASS_TEST_CHECK(mean < tolerance);
	LASS_TEST_CHECK_CLOSE(variance, 1., tolerance);

	// normal distribution
	//
	num::DistributionNormal<double, num::RandomMT19937> distNormal(random, 200., 100.);
	mean = TNumTraits::zero;
	variance = TNumTraits::zero;
	for (i = 0; i < testSize; ++i)
	{
		const TValue draw = distNormal();
		mean += draw;
		variance += draw * draw;
	}
	mean /= testSize;
	variance = variance / testSize - mean * mean;
	LASS_TEST_CHECK_CLOSE(mean, 200., tolerance);
	LASS_TEST_CHECK_CLOSE(variance, 100. * 100., tolerance);
}

TUnitTest test_num_random()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testNumRandomMersenne));
	result.push_back(LASS_TEST_CASE(testNumRandomXKCD));
	result.push_back(LASS_TEST_CASE(testNumDistributions));
	return result;
}


}

}

// EOF
