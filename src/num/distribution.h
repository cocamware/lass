/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @defgroup Distribution
 *
 *  A set of distribution for random numbers
 *
 *	@ref DistributionUniform
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
 *  a uniform distribution mapper.
 *
 *  template arguments:
 *  @arg @a T result type
 *  @arg @a RandomGenerator type of random number generator, see @ref Random.
 *  @arg @a leftOpen true if output range must be open to the left: (min, max].
 *  @arg @a rightOpen true if output range must be open to the right: [min, max).
 */
template <typename T, class RandomGenerator, bool leftOpen = false, bool rightOpen = false>
class DistributionUniform
{
public:

	typedef RandomGenerator TGenerator;						///< generator type

	typedef num::NumTraits<T> TNumTraits;					///< numeric traits of value type
	typedef typename util::CallTraits<T>::TValue TValue;	///< value type
	typedef typename util::CallTraits<T>::TParam TParam;	///< parameter value type

	DistributionUniform(TGenerator& ioGenerator, 
						TParam iMin = TNumTraits::zero, TParam iMax = TNumTraits::one);

	TValue operator()();

private:

	TGenerator* generator_;
	TValue min_;
	TValue max_;
	double scale_;
};



}

}

#include "distribution.inl"

#endif

// EOF
