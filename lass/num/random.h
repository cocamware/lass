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
#include <stdlib.h>
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

	TValue operator()() const;
	TValue operator()(TValue supremum) const;

	template <typename OutputIterator> OutputIterator getState(OutputIterator first) const;
	template <typename InputIterator> void setState(InputIterator first, InputIterator last);


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
	explicit RandomParkMiller(TValue seed);

	void seed(TValue seed);

	TValue operator()();
	TValue operator()(TValue supremum);

	template <typename OutputIterator> OutputIterator getState(OutputIterator first) const;
	template <typename InputIterator> void setState(InputIterator first, InputIterator last);

private:

	enum
	{
		multiplier_ = 16807,	
		modulus_ = 2147483647,
		seedMask_ = 123456789,
		schrageQuotient_ = modulus_ / multiplier_,
		schrageRest_ = modulus_ % multiplier_
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
	explicit RandomMT19937(TValue seed);
	template <typename ForwardIterator> RandomMT19937(ForwardIterator first, ForwardIterator last);

	void seed(TValue seed);
	template <typename ForwardIterator> void seed(ForwardIterator first, ForwardIterator last);

	TValue operator()();
	TValue operator()(TValue supremum);

	template <typename OutputIterator> OutputIterator getState(OutputIterator first) const;
	template <typename InputIterator> void setState(InputIterator first, InputIterator last);

private:

	LASS_META_ASSERT(sizeof(TValue) * lass::bitsPerByte == 32,
		MersenneTwister_is_designed_to_be_a_32_bits_random_number_generator);

	enum
	{
		stateSize_  = 624,          /**< size of state vector */
		shiftSize_  = 397
	};

	void reload();
	TValue twist(TValue a, TValue b, TValue c) const;

	TValue state_[stateSize_];      /**< the array for the state vector. */
	TValue index_;                     /**< index in state vector. */

	static TValue wordMask_;
	static TValue lowerMask_;
	static TValue upperMask_;
};



}

}

#include "random.inl"

#endif

// EOF
