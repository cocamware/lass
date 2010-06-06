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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_DISTRIBUTION_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_DISTRIBUTION_INL

#include "num_common.h"
#include "distribution.h"

namespace lass
{
namespace num
{
namespace impl
{

template <RangeType rangeType> struct RangePolicy;

template <>
struct RangePolicy<rtClosed>
{
	template <typename T>
	static bool isInRange(const T& x, const T& inf, const T& sup) { return x >= inf && x <= sup; }
};

template <>
struct RangePolicy<rtLeftOpen>
{
	template <typename T>
	static bool isInRange(const T& x, const T& inf, const T& sup) { return x > inf && x <= sup; }
};

template <>
struct RangePolicy<rtRightOpen>
{
	template <typename T>
	static bool isInRange(const T& x, const T& inf, const T& sup) { return x >= inf && x < sup; }
};

template <>
struct RangePolicy<rtOpen>
{
	template <typename T>
	static bool isInRange(const T& x, const T& inf, const T& sup) { return x > inf && x < sup; }
};

}



// --- DistributionUniform -------------------------------------------------------------------------

/** construct an empty distribution.
 *  @warning drawing numbers from an empty distribution results in undefined behaviour, probably
 *		causing an access violation.
 */
template <typename T, class RG, RangeType RT>
DistributionUniform<T, RG, RT>::DistributionUniform():
	generator_(0)
{
}

/** construct a uniform distribution mapper
 *  @param generator random number generator to be used as input.  Lifespan of @a generator must
 *                     be at least that of the distribution.
 *  @param infimum infimum of the output range: [inf, sup], or (inf, sup] if range is left open.
 *  @param supremum supremum of the output range: [inf, sup], or [inf, sup) if range is right open.
 */
template <typename T, class RG, RangeType RT>
DistributionUniform<T, RG, RT>::DistributionUniform(
		TGenerator& generator, TParam infimum, TParam supremum):
	generator_(&generator),
	infimum_(infimum),
	supremum_(supremum),
	scale_(static_cast<long double>(supremum - infimum) / TGenerator::max)
{
}



template <typename T, class RG, RangeType RT> inline
typename DistributionUniform<T, RG, RT>::TValue
DistributionUniform<T, RG, RT>::operator()() const
{
	LASS_ASSERT(generator_);
	TValue result;
	do
	{
		result = infimum_ + static_cast<TValue>(scale_ * (*generator_)());
	}
	while (!impl::RangePolicy<RT>::isInRange(result, infimum_, supremum_));
	return result;
}



/** draw a random number from generator and transform it by a uniform distribution
 *  @relates DistributionUniform
 */
template <typename T, typename RandomGenerator> inline
T distributeUniform(RandomGenerator& generator, T infimum, T supremum)
{
	DistributionUniform<T, RandomGenerator> distribution(generator, infimum, supremum);
	return distribution();
}



// --- DistributionExponential ---------------------------------------------------------------------

/** construct an empty distribution.
 *  @warning drawing numbers from an empty distribution results in undefined behaviour, probably
 *		causing an access violation.
 */
template <typename T, class RG>
DistributionExponential<T, RG>::DistributionExponential():
	generator_(0)
{
}



/** construct a normal distribution mapper
 *  @param generator random number generator to be used as input.  Lifespan of @a generator must
 *                     be at least that of the distribution.
 *  @param mean mean of distribution
 *  @param stddev standard deviation of
 */
template <typename T, class RG>
DistributionExponential<T, RG>::DistributionExponential(TGenerator& generator, TParam rateOfChange):
	generator_(&generator),
	rateOfChange_(rateOfChange)
{
}



template <typename T, class RG> inline
typename DistributionExponential<T, RG>::TValue
DistributionExponential<T, RG>::operator()() const
{
	LASS_ASSERT(generator_);
	TValue temp;
	do
	{
		temp = static_cast<TValue>((*generator_)()) / TGenerator::max;
	}
	while (temp == TNumTraits::zero);
	return -num::log(temp) / rateOfChange_;
}



/** draw a random number from generator and transform it by a exponential distribution
 *  @relates DistributionExponential
 */
template <typename T, typename RandomGenerator> inline
T distributeExponential(RandomGenerator& generator, T rateOfChange)
{
	DistributionExponential<T, RandomGenerator> distribution(generator, rateOfChange);
	return distribution();
}



// --- DistributionNormal --------------------------------------------------------------------------

/** construct an empty distribution.
 *  @warning drawing numbers from an empty distribution results in undefined behaviour, probably
 *		causing an access violation.
 */
template <typename T, class RG>
DistributionNormal<T, RG>::DistributionNormal():
	generator_(0)
{
}

	
	
/** construct a normal distribution mapper
 *  @param generator random number generator to be used as input.  Lifespan of @a generator must
 *                     be at least that of the distribution.
 *  @param mean mean of distribution
 *  @param standardDeviation standard deviation of distribution
 */
template <typename T, class RG>
DistributionNormal<T, RG>::DistributionNormal(
		TGenerator& generator, TParam mean, TParam standardDeviation):
	generator_(&generator),
	mean_(mean),
	standardDeviation_(standardDeviation),
	iset_(false)
{
}



template <typename T, class RG> inline
typename DistributionNormal<T, RG>::TValue
DistributionNormal<T, RG>::operator()() const
{
	LASS_ASSERT(generator_);

	if (iset_)
	{
		iset_ = false;
		return mean_ + standardDeviation_ * gset_;
	}

	const TValue scale = static_cast<TValue>(2) / TGenerator::max;
	TValue rsq,v1,v2;
	do
	{
		v1 = scale * (*generator_)() - TNumTraits::one;
		v2 = scale * (*generator_)() - TNumTraits::one;
		rsq = v1 * v1 + v2 * v2;
	}
	while (rsq >= 1.0 || rsq == 0.0);
	const TValue fac = num::sqrt(-2 * num::log(rsq) / rsq);

	gset_ = v1 * fac;
	iset_ = true;
	return mean_ + standardDeviation_ * v2 * fac;
}



/** draw a random number from generator and transform it by a normal distribution
 *  @relates DistributionNormal
 */
template <typename T, typename RandomGenerator> inline
T distributeNormal(RandomGenerator& generator, T mean, T standardDeviation)
{
	DistributionNormal<T, RandomGenerator> distribution(generator, mean, standardDeviation);
	return distribution();
}



// --- backwards compatibility ---------------------------------------------------------------------

/** @ingroup Distribution
 *  @return a uniform random sample from [0,1]
 */
template<class T,class RG> T uniform(RG& generator)
{
	return distributeUniform(generator);
}



/** @ingroup Distribution
 *  @return a gaussian distributed (aka normal distributed) random with mean 0 and stddev 1
 */
template<class T,class RG> T unitGauss(RG& generator)
{
	return distributeNormal(generator);
}



/** @ingroup Distribution
 *  @return a gaussian distributed (aka normal distributed) random sample with @a mean and
 *      stddev @a stddev
 */
template<class T,class RG> T gauss(
		RG& generator, typename util::CallTraits<T>::TParam mean, 
		typename util::CallTraits<T>::TParam stddev)
{
	return distributeNormal(generator, mean, stddev);
}



}

}

#endif

// EOF
