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
 *	Copyright (C) 2004-2022 the Initial Developer.
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



#include "lass_common.h"
#include "random.h"

namespace lass
{
namespace num
{

// --- RandomStandard ------------------------------------------------------------------------------

const RandomStandard::TValue RandomStandard::max = RAND_MAX;




// --- RandomParkMiller ----------------------------------------------------------------------------

const RandomParkMiller::TValue RandomParkMiller::max = RandomParkMiller::modulus_ - 1;



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
RandomParkMiller::RandomParkMiller(TValue seed)
{
	this->seed(seed);
}



void RandomParkMiller::seed(TValue seed)
{
	buffer_ = seed ^ seedMask_;
}



/** draw a random number
 */
RandomParkMiller::TValue RandomParkMiller::operator ()()
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
RandomMT19937::RandomMT19937(TValue seed)
{
	this->seed(seed);
}



/** initializes with a seed.
 */
void RandomMT19937::seed(TValue seed)
{
	LASS_META_ASSERT(sizeof(TValue) * lass::bitsPerByte == 32, if_TValue_is_32_bits_then_the_wordMasks_are_not_necessary);

	state_[0] = seed; // & wordMask_;
	for (TValue i = 1; i < stateSize_; ++i)
	{
		state_[i] = (1812433253 * (state_[i - 1] ^ (state_[i - 1] >> 30)) + i);
		//state_[i] &= wordMask_;
	}
	index_ = stateSize_;
}



/** draw a random number 
 */
RandomMT19937::TValue RandomMT19937::operator()()
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

	size_t k = 0;
	for (; k < stateSize_ - shiftSize_; ++k)
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


inline RandomMT19937::TValue RandomMT19937::twist(TValue a, TValue b, TValue c) const
{
	static const TValue magic01[2] = { 0x0, 0x9908b0df }; // magic01[x] = x * magic_ for x = 0, 1

	const TValue y = (a & 0x80000000) | (b & 0x7fffffff);
	return c ^ (y >> 1) ^ magic01[y & 0x1];
}



// --- RandomXorShift128Plus -----------------------------------------------------------------------

const RandomXorShift128Plus::TValue RandomXorShift128Plus::min = 0;
const RandomXorShift128Plus::TValue RandomXorShift128Plus::max = 0xffffffffffffffff;

RandomXorShift128Plus::RandomXorShift128Plus()
{
}

RandomXorShift128Plus::RandomXorShift128Plus(TValue seed)
{
	this->seed(seed);
}

RandomXorShift128Plus::TValue RandomXorShift128Plus::operator()()
{
	TValue x = state_[0];
	const TValue y = state_[1];
	state_[0] = y;
	x ^= x << 23; // a
	state_[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
	return state_[1] + y;
}

void RandomXorShift128Plus::seed(TValue seed)
{
	state_[0] = seed;
	state_[1] = 0xffff;
}



// --- RandomHalton --------------------------------------------------------------------------------

const RandomRadicalInverse::TValue RandomRadicalInverse::min = 0.0;
const RandomRadicalInverse::TValue RandomRadicalInverse::max = 1.0;

RandomRadicalInverse::RandomRadicalInverse(size_t base) :
	index_(0),
	base_(base),
	invBase_(1. / static_cast<TValue>(base))
{
}

RandomRadicalInverse::TValue RandomRadicalInverse::operator()()
{
	TValue result = 0;
	TValue f = 1;
	size_t n = ++index_;
	while (n)
	{
		const size_t r = n % base_;
		n /= base_;
		f *= invBase_;
		result += static_cast<TValue>(r) * f;
	}
	return result;
}

void RandomRadicalInverse::seed(size_t index)
{
	index_ = index;
}



// --- RandomXKCD ---------------------------------------------------------------------------------

const RandomXKCD::TValue RandomXKCD::min = 0; // zero-based
const RandomXKCD::TValue RandomXKCD::max = 5; // zero-based

RandomXKCD::TValue RandomXKCD::operator()()
{
	return 4;
}

}

}

// EOF
