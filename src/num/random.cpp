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



#include "num_common.h"
#include "random.h"

namespace lass
{
namespace num
{

// --- RandomStdLib --------------------------------------------------------------------------------

const RandomStandard::TValue RandomStandard::max = RAND_MAX;



// --- RandomMT19937 -------------------------------------------------------------------------------

const RandomMT19937::TValue RandomMT19937::max = RandomMT19937::wordMask_;

/** default constructor.
 *  will seed with default value on first use.
 */
RandomMT19937::RandomMT19937():
	index_(stateSize_ + 1)
{
}



/** construct with seed.
 */
RandomMT19937::RandomMT19937(TValue iSeed)
{
	seed(iSeed);
}



/** initializes with a seed.
 */
void RandomMT19937::seed(TValue iSeed)
{
	state_[0] = iSeed & wordMask_;
	for (unsigned i = 1; i < stateSize_; ++i)
	{
		state_[i] = (1812433253UL * (state_[i - 1] ^ (state_[i - 1] >> 30)) + i);
		state_[i] &= wordMask_;
	}
	index_ = stateSize_;
}



/** generate N words at a time
 */
void RandomMT19937::reload()
{

	// if seed() has not been called, a default initial seed is used.
	//
	if (index_ == stateSize_ + 1)
	{
		seed(5489UL);
	}

	int k;
	for (k = 0; k < stateSize_ - shiftSize_; ++k)
	{
		state_[k] = twist(state_[k], state_[k + 1], state_[k + shiftSize_]);
	}
	for (; k < stateSize_ - 1; ++k)
	{
		state_[k] = twist(state_[k], state_[k + 1], state_[k + shiftSize_ - stateSize_]);
	}
	state_[stateSize_ - 1] = twist(state_[stateSize_ - 1], state_[0], state_[shiftSize_ - 1]);
	index_ = 0;
}


inline RandomMT19937::TValue RandomMT19937::twist(TValue iA, TValue iB, TValue iC) const
{
	static const TValue magic01[2] = { 0x0UL, magic_ }; // magic01[x] = x * magic_ for x = 0, 1

	const TValue y = (iA & upperMask_) | (iB & lowerMask_);
	return iC ^ (y >> 1) ^ magic01[y & 0x1UL];
}



}

}

// EOF
