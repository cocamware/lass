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
 *	Copyright (C) 2004-2024 the Initial Developer.
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

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4996) // 'deprecated-declaration': deprecation-message (or "was declared deprecated")
#else
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace lass
{
namespace num
{

// --- RandomStandard ------------------------------------------------------------------------------

/** draw a random number
 */
inline RandomStandard::TValue RandomStandard::operator ()() const
{
	return rand();
}



/** draw a random number remapped to range [0, supremum)
 *  @deprecated this is A VERY BAD WAY TO DO IT!
 */
inline RandomStandard::TValue RandomStandard::operator ()(const result_type supremum) const
{
	return rand() % supremum;
}



template <typename OutputIterator>
OutputIterator RandomStandard::getState(OutputIterator first) const
{
	return first;
}



template <typename InputIterator>
void RandomStandard::setState([[maybe_unused]] InputIterator first, [[maybe_unused]] InputIterator last)
{
	LASS_ASSERT(first == last);
}



// --- RandomParkMiller ----------------------------------------------------------------------------

/** draw a random number remapped to range [0, supremum)
 *  @deprecated this is A VERY BAD WAY TO DO IT!
 */
inline RandomParkMiller::TValue RandomParkMiller::operator ()(const result_type supremum)
{
	return (*this)() % supremum;
}



template <typename OutputIterator>
OutputIterator RandomParkMiller::getState(OutputIterator first) const
{
	*first = buffer_;
	return first;
}



template <typename InputIterator>
void RandomParkMiller::setState(InputIterator first, InputIterator LASS_UNUSED(last))
{
	LASS_ASSERT(first != last);
	buffer_ = *first++;
	LASS_ASSERT(first == last);
}



// --- RandomMT19937 -------------------------------------------------------------------------------

/** construct by a range of initializing keys
 *  Requirements:
 *      @arg ForwardIterator is a forward iterator to elements of type TValue.  It must really be a
 *           forward iterator and not just an input iterator, since it must be possible to iterator
 *           over the same range more than once.
 */
template <typename ForwardIterator>
RandomMT19937::RandomMT19937(ForwardIterator first, ForwardIterator last)
{
	seed(first, last);
}



/** initialize by a range of initializing keys
 *  Requirements:
 *      @arg ForwardIterator is a forward iterator to elements of type TValue.  It must really be a
 *           forward iterator and not just an input iterator, since it must be possible to iterator
 *           over the same range more than once.
 */
template <typename ForwardIterator>
void RandomMT19937::seed(ForwardIterator first, ForwardIterator last)
{
	LASS_META_ASSERT(sizeof(TValue) * lass::bitsPerByte == 32, if_TValue_is_32_bits_then_the_wordMasks_are_not_necessary);

	seed(19650218);

	const size_t keySize = static_cast<size_t>(std::distance(first, last));
	LASS_ASSERT(keySize < num::NumTraits<TValue>::max);

	result_type i = 1;
	result_type j = 0;
	ForwardIterator key = first;
	for (size_t k = std::max<size_t>(stateSize_, keySize); k > 0; --k)
	{
		state_[i] = (state_[i] ^ ((state_[i - 1] ^ (state_[i - 1] >> 30)) * 1664525)) + *key + j;
		//state_[i] &= wordMask_;

		++i;
		if (i >= stateSize_)
		{
			state_[0] = state_[stateSize_ - 1];
			i = 1;
		}

		++key;
		++j;
		if (key == last)
		{
			key = first;
			j = 0;
		}
	}
	for (size_t k = stateSize_ - 1; k > 0; --k)
	{
		state_[i] = (state_[i] ^ ((state_[i - 1] ^ (state_[i - 1] >> 30)) * 1566083941)) - i;
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



/** draw a random number remapped to range [0, supremum)
 *  @deprecated this is A VERY BAD WAY TO DO IT!
 */
inline RandomMT19937::TValue RandomMT19937::operator ()(const result_type supremum)
{
	return (*this)() % supremum;
}



template <typename OutputIterator>
OutputIterator RandomMT19937::getState(OutputIterator first) const
{
	*first++ = index_;
	return std::copy(state_, state_ + stateSize_, first);
}



template <typename InputIterator>
void RandomMT19937::setState(InputIterator first, InputIterator last)
{
	LASS_ASSERT(first != last);
	index_ = *first++;
	LASS_ASSERT(first != last);
	[[maybe_unused]] result_type* end = std::copy(first, last, state_);
	LASS_ASSERT(end == state_ + stateSize_);
}



}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#else
#	pragma GCC diagnostic pop
#endif

#endif

// EOF
