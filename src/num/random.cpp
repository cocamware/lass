/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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



#include "num_common.h"
#include "random.h"

namespace lass
{
namespace num
{

// --- RandomStandard ------------------------------------------------------------------------------

const RandomStandard::TValue RandomStandard::max = RAND_MAX;




// --- RandomParkMiller ----------------------------------------------------------------------------

const RandomParkMiller::TValue RandomParkMiller::max = RandomParkMiller::modulus_;



/** default constructor.
 *  will seed with default value
 */
RandomParkMiller::RandomParkMiller()
{
	seed(0);
}



/** default constructor.
 *  will seed with default value
 */
RandomParkMiller::RandomParkMiller(TValue iSeed)
{
	seed(iSeed);
}



void RandomParkMiller::seed(TValue iSeed)
{
	buffer_ = iSeed ^ seedMask_;
}



/** draw a random number
 */
const RandomParkMiller::TValue RandomParkMiller::operator ()()
{
	TValue k = buffer_ / schrageQuotient_;
	buffer_ = multiplier_ * (buffer_ - k * schrageQuotient_) - k * schrageRest_;
	return buffer_;
}



// --- RandomMT19937 -------------------------------------------------------------------------------

const RandomMT19937::TValue RandomMT19937::max = 0xffffffff;

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
	state_[0] = iSeed; // & 0xffffffff;
	for (unsigned i = 1; i < stateSize_; ++i)
	{
		state_[i] = (1812433253 * (state_[i - 1] ^ (state_[i - 1] >> 30)) + i);
		//state_[i] &= 0xffffffff;
	}
	index_ = stateSize_;
}



/** draw a random number 
 */
const RandomMT19937::TValue RandomMT19937::operator()()
{
	if (index_ >= stateSize_)
	{
		reload();
	}

	TValue y = state_[index_++];

	// Tempering
	//
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680;
	y ^= (y << 15) & 0xefc60000;
	y ^= (y >> 18);

	return y;
}


// private

/** generate N words at a time
 */
void RandomMT19937::reload()
{

	// if seed() has not been called, a default initial seed is used.
	//
	if (index_ == stateSize_ + 1)
	{
		seed(5489);
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


inline const RandomMT19937::TValue RandomMT19937::twist(TValue iA, TValue iB, TValue iC) const
{
	static const TValue magic01[2] = { 0x0, 0x9908b0df }; // magic01[x] = x * magic_ for x = 0, 1

	const TValue y = (iA & 0x80000000) | (iB & 0x7fffffff);
	return iC ^ (y >> 1) ^ magic01[y & 0x1];
}



}

}

// EOF
