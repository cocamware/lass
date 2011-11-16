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
	rtOpen      = 0x3   ///< range is open to both sides: (inf, sup)
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

	DistributionUniform();
	DistributionUniform(TGenerator& generator,
		TParam infimum = TNumTraits::zero, TParam supremum = TNumTraits::one);

	TValue operator()() const;

private:

	TGenerator* generator_;
	TValue infimum_;
	TValue supremum_;
	long double scale_;
};

template <typename T, typename RandomGenerator> 
T distributeUniform(RandomGenerator& generator, T min = T(0), T max = T(1));



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

	DistributionExponential();
	DistributionExponential(TGenerator& generator,
		TParam rateOfChange = TNumTraits::one);

	TValue operator()() const;

private:

	TGenerator* generator_;
	TValue rateOfChange_;
};

template <typename T, typename RandomGenerator> 
T distributeExponential(RandomGenerator& generator, T rateOfChange = T(1));



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

	DistributionNormal();
	DistributionNormal(TGenerator& generator,
		TParam mean = TNumTraits::zero, TParam standardDeviation = TNumTraits::one);

	TValue operator()() const;

private:

	TGenerator* generator_;
	TValue mean_;
	TValue standardDeviation_;
	mutable TValue gset_;
	mutable bool iset_;
};

template <typename T, typename RandomGenerator> 
T distributeExponential(RandomGenerator& generator, T mean = T(0), T standardDeviation = T(1));



// backwards compatibility functions

template<class T,class RG> T uniform(RG& generator);
template<class T,class RG> T unitGauss(RG& generator);
template<class T,class RG> T gauss(RG& generator, typename util::CallTraits<T>::TParam mean,
								   typename util::CallTraits<T>::TParam stddev);

}

}

#include "distribution.inl"

#endif

// EOF
