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
#include "../lass/num/inverse_transform_sampling.h"

namespace lass
{
namespace test
{

void testNumInverseTransformSampling2D()
{
	typedef double TValue;

	typedef num::InverseTransformSampling2D<TValue> TInverseTransformSampling;
	typedef TInverseTransformSampling::TSample TSample;
	typedef TInverseTransformSampling::TIndex TIndex;

	num::RandomMT19937 random;
	num::DistributionUniform<TValue, num::RandomMT19937> uniform(random);

	const size_t numSamples = 3000000;

	const size_t uSize = 20;
	const size_t vSize = 10;
	const size_t size = uSize * vSize;
	
	std::vector<TValue> f;
	std::generate_n(std::back_inserter(f), size, uniform);
	const TValue fSum = std::accumulate(f.begin(), f.end(), TValue(0));
	const TValue fIntegral = fSum / size;

	std::vector<TValue> pdf(size);
	for (size_t k = 0; k < size; ++k)
	{
		pdf[k] = f[k] / fIntegral;
	}

	const num::InverseTransformSampling2D<TValue> inverse(f.begin(), f.end(), uSize, vSize);

	std::vector<size_t> count(size);
	for (size_t k = 0; k < numSamples; ++k)
	{
		TIndex index;
		double p;
		TSample sample = inverse(TSample(uniform(), uniform()), p, index);
		LASS_TEST_CHECK_EQUAL(static_cast<size_t>(num::floor(uSize * sample.x)), index.x);
		LASS_TEST_CHECK_EQUAL(static_cast<size_t>(num::floor(vSize * sample.y)), index.y);
		const size_t i = index.x * vSize + index.y;
		LASS_TEST_CHECK_CLOSE(p, pdf[i], 1e-9);
		LASS_TEST_CHECK_CLOSE(f[i] / p, fIntegral, 1e-9);
		count[i] += 1;
	}

	for (size_t k = 0; k < size; ++k)
	{
		LASS_TEST_CHECK_CLOSE(static_cast<TValue>(count[k]) / numSamples, f[k] / fSum, 1e-1);
	}
}

TUnitTest test_num_inverse_transform_sampling()
{
	return TUnitTest(1, LASS_TEST_CASE(testNumInverseTransformSampling2D));
}


}

}

// EOF
