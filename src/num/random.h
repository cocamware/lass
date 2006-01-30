/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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



/** @defgroup Random
 *
 *  A set of random number generators.
 *
 *  - @ref RandomStandard : uses the C standard function rand().
 *  - @ref RandomParkMiller : Minimal Standard generator by Park and Miller.
 *  - @ref RandomMT19937 : uses a mersenne twister MT19937.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_RANDOM_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_RANDOM_H

#include "num_common.h"
#include <cstdlib>
#include "../util/call_traits.h"

namespace lass
{
namespace num
{

/** Uses the C standard library function rand().
 *  @ingroup Random
 *
 *  @note this one isn't thread safe since state is stored externally.
 */
class LASS_DLL RandomStandard
{
public:
	typedef int TValue;             /**< type of return value. */
	static const TValue max;        /**< maximum return value. */


	const TValue operator()() const;
	const TValue operator()(TValue iSupremum) const;
};



/** Minimal Standard generator by Park and Miller
 *  @ingroup Random
 *
 *  RandomParkMiller is the LASS implementation of the Minimal Standard generator by Park and Miller
 *
 *  <i>Park and Miller, "Random Number Generators: Good ones are hard to find", Communications of the
 *  ACM, October 1988, Volume 31, No 10, pages 1192-1201.</i>
 */
class LASS_DLL RandomParkMiller
{
public:

	typedef num::Tuint32 TValue;    /**< type of return value. */
	static const TValue max;        /**< maximum return value. */

	RandomParkMiller();
	explicit RandomParkMiller(TValue iSeed);

	void seed(TValue iSeed);

	const TValue operator()();
	const TValue operator()(TValue iSupremum);

private:

	enum
	{
		multiplier_ = 16807,	
		modulus_ = 2147483647,
		seedMask_ = 123456789,
		schrageQuotient_ = modulus_ / multiplier_,
		schrageRest_ = modulus_ % multiplier_,
	};

	TValue buffer_;
};


/** implemenents a mersenne twister MT19937.
 *  @ingroup Random
 *
 *  RandomMT19937 is the LASS implementation of the Mersenne twister.  Mersenne Twister(MT) is a
 *  pseudorandom number generator developped by Makoto Matsumoto and Takuji Nishimura (alphabetical
 *  order) during 1996-1997. MT has the following merits:
 *
 *  - It is designed with consideration on the flaws of various existing generators.
 *  - Far longer period and far higher order of equidistribution than any other implemented
 *    generators. (It is proved that the period is 2^19937-1, and 623-dimensional equidistribution
 *    property is assured.)
 *  - Fast generation. (Although it depends on the system, it is reported that MT is sometimes
 *    faster than the standard ANSI-C library in a system with pipeline and cache memory.)
 *  - Efficient use of the memory. (The implementation consumes only 624 words of working area.)
 *
 *  <i>M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-dimensionally equidistributed uniform
 *  pseudorandom number generator", ACM Trans. on Modeling and Computer Simulation Vol. 8, No. 1,
 *  January pp.3-30 1998</i>, http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
 *
 *  Our version is implemented after the MT19937 standard code of 2002/1/26 (mt19937ar.c)
 */
class LASS_DLL RandomMT19937
{
public:

	typedef num::Tuint32 TValue;   /**< type of return value. */
	static const TValue max;        /**< maximum return value. */

	RandomMT19937();
	explicit RandomMT19937(TValue iSeed);
	template <typename ForwardIterator> RandomMT19937(ForwardIterator iBegin, ForwardIterator iEnd);

	void seed(TValue iSeed);
	template <typename ForwardIterator> void seed(ForwardIterator iBegin, ForwardIterator iEnd);

	const TValue operator()();
	const TValue operator()(TValue iSupremum);

private:

	LASS_META_ASSERT(sizeof(TValue) * lass::bitsPerByte == 32,
		MersenneTwister_is_designed_to_be_a_32_bits_random_number_generator);

	void reload();
	const TValue twist(TValue iA, TValue iB, TValue iC) const;

	enum
	{
		stateSize_  = 624,          /**< size of state vector */
		shiftSize_  = 397,
	};

	TValue state_[stateSize_];      /**< the array for the state vector. */
	int index_;                     /**< index in state vector. */

	static TValue wordMask_;
	static TValue lowerMask_;
	static TValue upperMask_;
};



}

}

#include "random.inl"

#endif

// EOF
