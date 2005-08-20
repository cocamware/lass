/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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
	static bool isInRange(const T& iX, const T& iInf, const T& iSupremum) { return iX >= iInf && iX <= iSupremum; }
};

template <>
struct RangePolicy<rtLeftOpen>
{
	template <typename T>
	static bool isInRange(const T& iX, const T& iInf, const T& iSupremum) { return iX > iInf && iX <= iSupremum; }
};

template <>
struct RangePolicy<rtRightOpen>
{
	template <typename T>
	static bool isInRange(const T& iX, const T& iInf, const T& iSupremum) { return iX >= iInf && iX < iSupremum; }
};

template <>
struct RangePolicy<rtOpen>
{
	template <typename T>
	static bool isInRange(const T& iX, const T& iInf, const T& iSupremum) { return iX > iInf && iX < iSupremum; }
};

}



// --- DistributionUniform -------------------------------------------------------------------------

/** construct a uniform distribution mapper
 *  @param ioGenerator random number generator to be used as input.  Lifespan of @a ioGenerator must
 *                     be at least that of the distribution.
 *  @param iInfimum infimum of the output range: [inf, sup], or (inf, sup] if range is left open.
 *  @param iSupremum supremum of the output range: [inf, sup], or [inf, sup) if range is right open.
 */
template <typename T, class RG, RangeType RT>
DistributionUniform<T, RG, RT>::DistributionUniform(TGenerator& ioGenerator,
													TParam iInfimum, TParam iSupremum):
	generator_(&ioGenerator),
	infimum_(iInfimum),
	supremum_(iSupremum),
	scale_(static_cast<long double>(iSupremum - iInfimum) / TGenerator::max)
{
}



template <typename T, class RG, RangeType RT> inline
typename DistributionUniform<T, RG, RT>::TValue
DistributionUniform<T, RG, RT>::operator()() const
{
	TValue result;
	do
	{
		result = infimum_ + static_cast<TValue>(scale_ * (*generator_)());
	}
	while (!impl::RangePolicy<RT>::isInRange(result, infimum_, supremum_));
	return result;
}



/** draw a random number from iGenerator and transform it by a uniform distribution
 *  @relates DistributionUniform
 */
template <typename T, typename RandomGenerator> inline
T distributeUniform(RandomGenerator& iGenerator, T iInfimum, T iSupremum)
{
	DistributionUniform<T, RandomGenerator> distribution(iGenerator, iInfimum, iSupremum);
	return distribution();
}



// --- DistributionExponential ---------------------------------------------------------------------

/** construct a normal distribution mapper
 *  @param ioGenerator random number generator to be used as input.  Lifespan of @a ioGenerator must
 *                     be at least that of the distribution.
 *  @param iMean mean of distribution
 *  @param iStdDev standard deviation of
 */
template <typename T, class RG>
DistributionExponential<T, RG>::DistributionExponential(TGenerator& ioGenerator,
														TParam iRateOfChange):
	generator_(&ioGenerator),
	rateOfChange_(iRateOfChange)
{
}



template <typename T, class RG> inline
typename DistributionExponential<T, RG>::TValue
DistributionExponential<T, RG>::operator()() const
{
	TValue temp;
	do
	{
		temp = static_cast<TValue>((*generator_)()) / TGenerator::max;
	}
	while (temp == TNumTraits::zero);
	return -num::log(temp) / rateOfChange_;
}



/** draw a random number from iGenerator and transform it by a exponential distribution
 *  @relates DistributionExponential
 */
template <typename T, typename RandomGenerator> inline
T distributeExponential(RandomGenerator& iGenerator, T iRateOfChange)
{
	DistributionExponential<T, RandomGenerator> distribution(iGenerator, iRateOfChange);
	return distribution();
}



// --- DistributionNormal --------------------------------------------------------------------------

/** construct a normal distribution mapper
 *  @param ioGenerator random number generator to be used as input.  Lifespan of @a ioGenerator must
 *                     be at least that of the distribution.
 *  @param iMean mean of distribution
 *  @param iStandardDeviation standard deviation of distribution
 */
template <typename T, class RG>
DistributionNormal<T, RG>::DistributionNormal(TGenerator& ioGenerator,
											  TParam iMean, TParam iStandardDeviation):
	generator_(&ioGenerator),
	mean_(iMean),
	standardDeviation_(iStandardDeviation),
	iset_(false)
{
}



template <typename T, class RG> inline
typename DistributionNormal<T, RG>::TValue
DistributionNormal<T, RG>::operator()() const
{
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



/** draw a random number from iGenerator and transform it by a normal distribution
 *  @relates DistributionNormal
 */
template <typename T, typename RandomGenerator> inline
T distributeNormal(RandomGenerator& iGenerator, T iMean, T iStandardDeviation)
{
	DistributionNormal<T, RandomGenerator> distribution(iGenerator, iMean, iStandardDeviation);
	return distribution();
}



// --- backwards compatibility ---------------------------------------------------------------------

/** @ingroup Distribution
 *  @return a uniform random sample from [0,1]
 */
template<class T,class RG> T uniform(RG& iGenerator)
{
	return distributeUniform(iGenerator);
}



/** @ingroup Distribution
 *  @return a gaussian distributed (aka normal distributed) random with mean 0 and stddev 1
 */
template<class T,class RG> T unitGauss(RG& iGenerator)
{
	return distributeNormal(iGenerator);
}



/** @ingroup Distribution
 *  @return a gaussian distributed (aka normal distributed) random sample with @a iMean and
 *      stddev @a iStdDev
 */
template<class T,class RG> T gauss(RG& iGenerator,
									typename util::CallTraits<T>::TParam iMean,
									typename util::CallTraits<T>::TParam iStdDev)
{
	return distributeNormal(iGenerator, iMean, iStdDev);
}



}

}

#endif

// EOF
