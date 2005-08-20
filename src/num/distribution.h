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



/** @defgroup Distribution
 *
 *  A set of distribution for random numbers
 *
 *  distribution classes:
 *
 *  @arg @ref DistributionUniform
 *  @arg @ref DistributionExponential
 *  @arg @ref DistributionNormal
 *
 *  backwards compatible functions:
 *  
 *  @arg @ref uniform
 *  @arg @ref unitGauss
 *  @arg @ref gauss
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_DISTRIBUTION_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_DISTRIBUTION_H

#include "num_common.h"
#include "num_traits.h"

namespace lass
{
namespace num
{

/** @ingroup Distribution
 *  enumeration indicating how a real-number range must be closed.
 */
enum RangeType
{
	rtClosed    = 0x0,  ///< range is closed on both sides: [inf, sup]
	rtRightOpen = 0x1,  ///< range is open to the right: [inf, sup)
	rtLeftOpen  = 0x2,  ///< range is open to the left: (inf, sup]
	rtOpen      = 0x3,  ///< range is open to both sides: (inf, sup)
};



/** @ingroup Distribution
 *  a uniform distribution mapper.
 *
 *  template arguments:
 *  @arg @a T result type
 *  @arg @a RandomGenerator type of random number generator, see @ref Random.
 *  @arg @a rangeType value of RangeType declaring if the range must be open or closed.
 */
template <typename T, class RandomGenerator, RangeType rangeType = rtClosed>
class DistributionUniform
{
public:

	typedef RandomGenerator TGenerator;                     ///< generator type

	typedef num::NumTraits<T> TNumTraits;                   ///< numeric traits of value type
	typedef typename util::CallTraits<T>::TValue TValue;    ///< value type
	typedef typename util::CallTraits<T>::TParam TParam;    ///< parameter value type

	DistributionUniform(TGenerator& ioGenerator,
		TParam iInfimum = TNumTraits::zero, TParam iSupremum = TNumTraits::one);

	TValue operator()() const;

private:

	TGenerator* generator_;
	TValue infimum_;
	TValue supremum_;
	long double scale_;
};

template <typename T, typename RandomGenerator> 
T distributeUniform(RandomGenerator& iGenerator, T iMin = T(0), T iMax = T(1));



/** @ingroup Distribution
 *  an exponential distribution mapper.
 *
 *  template arguments:
 *  @arg @a T result type
 *  @arg @a RandomGenerator type of random number generator, see @ref Random.
 *
 *  reference: Numerical Recipes in C: The Art of Scientific Computing, section 7.2
 */
template <typename T, class RandomGenerator>
class DistributionExponential
{
public:

	typedef RandomGenerator TGenerator;                     ///< generator type

	typedef num::NumTraits<T> TNumTraits;                   ///< numeric traits of value type
	typedef typename util::CallTraits<T>::TValue TValue;    ///< value type
	typedef typename util::CallTraits<T>::TParam TParam;    ///< parameter value type

	DistributionExponential(TGenerator& ioGenerator,
		TParam iRateOfChange = TNumTraits::one);

	TValue operator()() const;

private:

	TGenerator* generator_;
	TValue rateOfChange_;
};

template <typename T, typename RandomGenerator> 
T distributeExponential(RandomGenerator& iGenerator, T iRateOfChange = T(1));



/** @ingroup Distribution
 *  a normal distribution mapper (aka guassian distribution)
 *
 *  template arguments:
 *  @arg @a T result type
 *  @arg @a RandomGenerator type of random number generator, see @ref Random.
 *
 *  reference: Numerical Recipes in C: The Art of Scientific Computing, section 7.2
 */
template <typename T, class RandomGenerator>
class DistributionNormal
{
public:

	typedef RandomGenerator TGenerator;                     ///< generator type

	typedef num::NumTraits<T> TNumTraits;                   ///< numeric traits of value type
	typedef typename util::CallTraits<T>::TValue TValue;    ///< value type
	typedef typename util::CallTraits<T>::TParam TParam;    ///< parameter value type

	DistributionNormal(TGenerator& ioGenerator,
		TParam iMean = TNumTraits::zero, TParam iStandardDeviation = TNumTraits::one);

	TValue operator()() const;

private:

	TGenerator* generator_;
	TValue mean_;
	TValue standardDeviation_;
	mutable TValue gset_;
	mutable bool iset_;
};

template <typename T, typename RandomGenerator> 
T distributeExponential(RandomGenerator& iGenerator, T iMean = T(0), T iStandardDeviation = T(1));



// backwards compatibility functions

template<class T,class RG> T uniform(RG& iGenerator);
template<class T,class RG> T unitGauss(RG& iGenerator);
template<class T,class RG> T gauss(RG& iGenerator, typename util::CallTraits<T>::TParam iMean,
								   typename util::CallTraits<T>::TParam iStdDev);

}

}

#include "distribution.inl"

#endif

// EOF
