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

template <bool leftOpen, bool rightOpen> 
struct RangePolicy
{
	template <typename T>
	static bool isInRange(const T& iX, const T& iMin, const T& iMax) { return iX >= iMin && iX <= iMax; }
};

template <>
struct RangePolicy<true, false>
{
	template <typename T>
	static bool isInRange(const T& iX, const T& iMin, const T& iMax) { return iX > iMin && iX <= iMax; }
};

template <>
struct RangePolicy<false, true>
{
	template <typename T>
	static bool isInRange(const T& iX, const T& iMin, const T& iMax) { return iX >= iMin && iX < iMax; }
};

template <>
struct RangePolicy<true, true>
{
	template <typename T>
	static bool isInRange(const T& iX, const T& iMin, const T& iMax) { return iX > iMin && iX < iMax; }
};

}



// --- DistributionUniform -------------------------------------------------------------------------

/** construct a uniform distribution mapper
 *  @param ioGenerator random number generator to be used as input.  Lifespan of @a ioGenerator must
 *                     be at least that of the distribution.
 *  @param iMin minimum of the output range: [min, max] or (min, max] if range is left open.
 *  @param iMax maximum of the output range: [min, max] or [min, max) if range is right open.
 */
template <typename T, class RG, bool LO, bool RO>
DistributionUniform<T, RG, LO, RO>::DistributionUniform(TGenerator& ioGenerator, 
														TParam iMin, TParam iMax):
	generator_(&ioGenerator),
	min_(iMin),
	max_(iMax),
	scale_(static_cast<double>(iMax - iMin) / TGenerator::max)
{
}



template <typename T, class RG, bool LO, bool RO> inline
typename DistributionUniform<T, RG, LO, RO>::TValue
DistributionUniform<T, RG, LO, RO>::operator()()
{
	TValue result;
	do
	{
		result = min_ + scale_ * (*generator());
	}
	while (!impl::RangePolicy<LO, RO>::isInRange(result, min_, max_));
	return result;
}



}

}

#endif

// EOF
