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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_RANDOM_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_RANDOM_INL

#include "num_common.h"
#include "random.h"

namespace lass
{
namespace num
{

// --- RandomStandard ------------------------------------------------------------------------------

/** draw a random number
 */
inline const RandomStandard::TValue RandomStandard::operator ()() const
{
	return rand();
}



/** draw a random number remapped to range [0, iSupremum)
 */
inline const RandomStandard::TValue RandomStandard::operator ()(const TValue iSupremum) const
{
	return rand() % iSupremum;
}



template <typename OutputIterator>
OutputIterator RandomStandard::getState(OutputIterator iFirst) const
{
	return iFirst;
}



template <typename InputIterator>
void RandomStandard::setState(InputIterator iFirst, InputIterator iLast)
{
	LASS_ASSERT(iFirst == iLast);
}



// --- RandomParkMiller ----------------------------------------------------------------------------

/** draw a random number remapped to range [0, iSupremum)
 */
inline const RandomParkMiller::TValue RandomParkMiller::operator ()(const TValue iSupremum)
{
	return (*this)() % iSupremum;
}



template <typename OutputIterator>
OutputIterator RandomParkMiller::getState(OutputIterator iFirst) const
{
	*iFirst = buffer_;
	return iFirst;
}



template <typename InputIterator>
void RandomParkMiller::setState(InputIterator iFirst, InputIterator iLast)
{
	LASS_ASSERT(iFirst != iLast);
	buffer_ = *iFirst++;
	LASS_ASSERT(iFirst == iLast);
}



// --- RandomMT19937 -------------------------------------------------------------------------------

/** construct by a range of initializing keys
 *  Requirements:
 *      @arg ForwardIterator is a forward iterator to elements of type TValue.  It must really be a
 *           forward iterator and not just an input iterator, since it must be possible to iterator
 *           over the same range more than once.
 */
template <typename ForwardIterator>
RandomMT19937::RandomMT19937(ForwardIterator iBegin, ForwardIterator iEnd)
{
	seed(iBegin, iEnd);
}



/** initialize by a range of initializing keys
 *  Requirements:
 *      @arg ForwardIterator is a forward iterator to elements of type TValue.  It must really be a
 *           forward iterator and not just an input iterator, since it must be possible to iterator
 *           over the same range more than once.
 */
template <typename ForwardIterator>
void RandomMT19937::seed(ForwardIterator iBegin, ForwardIterator iEnd)
{
	LASS_META_ASSERT(sizeof(TValue) * lass::bitsPerByte == 32,
		if_TValue_is_32_bits_then_the_wordMasks_are_not_necessary);

	seed(19650218UL);

	const size_t keySize = std::distance(iBegin, iEnd);
	int i = 1;
	int j = 0;
	ForwardIterator key = iBegin;
	for (size_t k = (stateSize_ > keySize ? stateSize_ : keySize); k > 0; --k)
	{
		state_[i] = (state_[i] ^ ((state_[i - 1] ^ (state_[i - 1] >> 30)) * 1664525UL)) + *key + j;
		//state_[i] &= wordMask_;

		++i;
		if (i >= stateSize_)
		{
			state_[0] = state_[stateSize_ - 1];
			i = 1;
		}

		++key;
		++j;
		if (key == iEnd)
		{
			key = iBegin;
			j = 0;
		}
		LASS_ASSERT(static_cast<int>(keySize) >= 0);
		LASS_ASSERT(j < static_cast<int>(keySize));
	}
	for (size_t k = stateSize_ - 1; k > 0; --k)
	{
		state_[i] = (state_[i] ^ ((state_[i - 1] ^ (state_[i - 1] >> 30)) * 1566083941UL)) - i;
		//state_[i] &= wordMask_;
		++i;
		if (i >= stateSize_)
		{
			state_[0] = state_[stateSize_ - 1];
			i = 1;
		}
	}

	state_[0] = 0x80000000;
}



/** draw a random number remapped to range [0, iSupremum)
 */
inline const RandomMT19937::TValue RandomMT19937::operator ()(const TValue iSupremum)
{
	return (*this)() % iSupremum;
}



template <typename OutputIterator>
OutputIterator RandomMT19937::getState(OutputIterator iFirst) const
{
	*iFirst++ = index_;
	return std::copy(state_, state_ + stateSize_, iFirst);
}



template <typename InputIterator>
void RandomMT19937::setState(InputIterator iFirst, InputIterator iLast)
{
	LASS_ASSERT(iFirst != iLast);
	index_ = *iFirst++;
	LASS_ASSERT(iFirst != iLast);
	TValue* end = std::copy(iFirst, iLast, state_);
	LASS_ASSERT(end == state_ + stateSize_);
}



}

}

#endif

// EOF
