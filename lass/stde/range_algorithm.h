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



/** @defgroup range_algorithm
 *  @brief extra algorithms
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_RANGE_ALGORITHM_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_RANGE_ALGORITHM_H

#include "stde_common.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4996)
#endif

namespace lass
{
namespace stde
{

/** std::for_each wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename Op> inline
Op for_each_r(InputRange& range, Op op)
{
	return std::for_each(range.begin(), range.end(), op);
}

/** std::find wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename T> inline
typename InputRange::iterator 
find_r(const InputRange& range, const T& value)
{
	return std::find(range.begin(), range.end(), value);
}

/** std::find_if wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename Predicate> inline
typename InputRange::iterator 
find_if_r(const InputRange& range, Predicate pred)
{
	return std::find(range.begin(), range.end(), pred);
}

/** std::find_end wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange1, typename ForwardRange2> inline
typename ForwardRange1::iterator 
find_end_r(const ForwardRange1& range1, const ForwardRange2& range2)
{
	return std::find_end(range1.begin(), range1.end(), range2.begin(), range2.end());
}

/** std::find_end wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange1, typename ForwardRange2, typename BinaryPredicate> inline
typename ForwardRange1::iterator 
find_end_r(const ForwardRange1& range1, const ForwardRange2& range2, BinaryPredicate pred)
{
	return std::find_end(range1.begin(), range1.end(), range2.begin(), range2.end(), pred);
}

/** std::find_first_of wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange1, typename ForwardRange2> inline
typename ForwardRange1::iterator 
find_first_of_r(const ForwardRange1& range1, const ForwardRange2& range2)
{
	return std::find_first_of(range1.begin(), range1.end(), range2.begin(), range2.end());
}

/** std::find_first_of wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange1, typename ForwardRange2, typename BinaryPredicate> inline
typename ForwardRange1::iterator 
find_first_of_r(const ForwardRange1& range1,const ForwardRange2& range2, BinaryPredicate pred)
{
	return std::find_first_of(range1.begin(), range1.end(), range2.begin(), range2.end(), pred);
}

/** std::adjacent_find wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange> inline
typename ForwardRange::iterator 
adjacent_find_r(const ForwardRange& range)
{
	return std::adjacent_find(range.begin(), range.end());
}

/** std::adjacent_find wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename Predicate> inline
typename ForwardRange::iterator 
adjacent_find_r(const ForwardRange& range, Predicate pred)
{
	return std::adjacent_find(range.begin(), range.end(), pred);
}

/** std::count wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename T> inline
typename std::iterator_traits<typename InputRange::iterator>::difference_type
count_r(const InputRange& range, const T& value)
{
	return std::count(range.begin(), range.end(), value);
}

/** std::count_if wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename Predicate> inline
typename std::iterator_traits<typename InputRange::iterator>::difference_type
count_if_r(const InputRange& range, Predicate pred)
{
	return std::count_if(range.begin(), range.end(), pred);
}

/** std::mismatch wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange1, typename InputRange2, typename BinaryPredicate> inline
std::pair<typename InputRange1::iterator, typename InputRange2::iterator>
mismatch_r(const InputRange1& range1, const InputRange2& range2, BinaryPredicate pred)
{
	return std::mismatch(range1.begin(), range1.end(), range2.begin(), range2.end(), pred);
}

/** std::mismatch wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange1, typename InputRange2> inline
std::pair<typename InputRange1::iterator, typename InputRange2::iterator>
mismatch_r(const InputRange1& range1, const InputRange2& range2)
{
	return std::mismatch(range1.begin(), range1.end(), range2.begin(), range2.end());
}

/** std::mismatch wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange1, typename InputRange2> inline
bool equal_r(const InputRange1& range1, const InputRange2& range2)
{
	return std::equal(range1.begin(), range1.end(), range2.begin());
}

/** std::mismatch wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange1, typename InputRange2, typename BinaryPredicate> inline
bool equal_r(const InputRange1& range1, const InputRange2& range2, BinaryPredicate pred)
{
	return std::equal(range1.begin(), range1.end(), range2.begin(), pred);
}

/** std::search wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange1, typename ForwardRange2> inline
typename ForwardRange1::iterator 
search_r(const ForwardRange1& range1,const ForwardRange2& range2)
{
	return std::search(range1.begin(), range1.end(), range2.begin(), range2.end());
}

/** std::search wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange1, typename ForwardRange2, typename BinaryPredicate> inline
typename ForwardRange1::iterator 
search_r(const ForwardRange1& range1,const ForwardRange2& range2, BinaryPredicate pred)
{
	return std::search(range1.begin(), range1.end(), range2.begin(), range2.end(), pred);
}

/** std::search_n wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename Size, typename T> inline
typename ForwardRange::iterator 
search_n_r(const ForwardRange& range, Size count, const T& value)
{
	return std::search_n(range.begin(), range.end(), count, value);
}

/** std::search_n wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename Size, typename T, typename BinaryPredicate> inline
typename ForwardRange::iterator 
search_n_r(const ForwardRange& range, Size count, const T& value, BinaryPredicate pred)
{
	return std::search_n(range.begin(), range.end(), count, value, pred);
}

/** std::copy wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename OutputIterator> inline
OutputIterator copy_r(const InputRange& range, OutputIterator result)
{
	return std::copy(range.begin(), range.end(), result);
}

/** std::copy_backward wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename BidirectionalRange, typename BidirectionalIterator> inline
BidirectionalIterator copy_backward_r(const BidirectionalRange& range, BidirectionalIterator result)
{
	return std::copy_backward(range.begin(), range.end(), result);
}

/** std::swap_ranges wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange1, typename ForwardRange2> inline
typename ForwardRange2::iterator
swap_ranges_r(ForwardRange1& range1, ForwardRange2& range2)
{
	return std::swap_ranges(range1.begin(), range1.end(), range2.begin());
}

/** std::transform wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename OutputIterator, typename UnaryOperation> inline
OutputIterator transform_r(const InputRange& range, OutputIterator result, UnaryOperation op)
{
	return std::transform(range.begin(), range.end(), result, op);
}

/** std::transform wrapper for ranges
*	@ingroup range_algorithm
*
*	Overload for MSVC compiler to preserve output array as checked iterator.
*/
template <typename InputRange, typename OutputType, size_t N, typename UnaryOperation> inline
OutputType* transform_r(const InputRange& range, OutputType (&result)[N], UnaryOperation op)
{
	return std::transform(range.begin(), range.end(), result, op);
}

/** std::transform wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange1, typename InputRange2, typename OutputIterator, typename BinaryOperation> inline
OutputIterator transform_r(const InputRange1& range1, const InputRange2& range2, OutputIterator result, BinaryOperation op)
{
	return std::transform(range1.begin(), range1.end(), range2.begin(), range2.end(), result, op);
}

/** std::replace wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename T> inline
void replace_r(ForwardRange& range, const T& old_value, const T& new_value)
{
	std::replace(range.begin(), range.end(), old_value, new_value);
}

/** std::replace_if wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename Predicate, typename T> inline
void replace_if_r(ForwardRange& range, Predicate pred, const T& new_value)
{
	std::replace_if(range.begin(), range.end(), pred, new_value);
}

/** std::replace_copy wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename OutputIterator, typename T> inline
OutputIterator replace_copy_r(const InputRange& range, OutputIterator result, const T& old_value, const T& new_value)
{
	return std::replace(range.begin(), range.end(), result, old_value, new_value);
}

/** std::replace_copy_if wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename OutputIterator, typename Predicate, typename T> inline
OutputIterator replace_copy_if_r(const InputRange& range, OutputIterator result, Predicate pred, const T& new_value)
{
	return std::replace_if(range.begin(), range.end(), result, pred, new_value);
}

/** std::fill wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename T> inline
void fill_r(ForwardRange& range, const T& value)
{
	std::fill(range.begin(), range.end(), value);
}

/** std::generate wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename Generator> inline
void generate_r(ForwardRange& range, Generator gen)
{
	std::generate(range.begin(), range.end(), gen);
}

/** std::remove wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename T> inline
typename ForwardRange::iterator
remove_r(ForwardRange& range, const T& value)
{
	return std::remove(range.begin(), range.end(), value);
}

/** std::remove wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename Predicate> inline
typename ForwardRange::iterator
remove_if_r(ForwardRange& range, Predicate pred)
{
	return std::remove_if(range.begin(), range.end(), pred);
}

/** std::remove_copy wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename OutputIterator, typename T> inline
OutputIterator remove_copy_r(const InputRange& range, OutputIterator result, const T& value)
{
	return std::remove_copy(range.begin(), range.end(), result, value);
}

/** std::remove_copy wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename OutputIterator, typename Predicate> inline
OutputIterator remove_copy_if_r(const InputRange& range, OutputIterator result, Predicate pred)
{
	return std::remove_copy_if(range.begin(), range.end(), result, pred);
}

/** std::unique wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange> inline
typename ForwardRange::iterator
unique_r(ForwardRange& range)
{
	return std::unique(range.begin(), range.end());
}

/** std::unique wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename Predicate> inline
typename ForwardRange::iterator
unique_r(ForwardRange& range, Predicate pred)
{
	return std::unique(range.begin(), range.end(), pred);
}

/** std::unique_copy_r wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename OutputIterator> inline
OutputIterator unique_copy_r(const InputRange& range, OutputIterator result)
{
	return std::unique_copy(range.begin(), range.end(), result);
}

/** std::unique_copy_r wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename InputRange, typename OutputIterator, typename Predicate> inline
OutputIterator unique_copy_r(const InputRange& range, OutputIterator result, Predicate pred)
{
	return std::unique_copy(range.begin(), range.end(), result, pred);
}

/** std::reverse wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename BidirectionalRange> inline
void reverse_r(BidirectionalRange& range)
{
	std::reverse(range.begin(), range.end());
}

/** std::reverse_copy wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename BidirectionalRange, typename OutputIterator> inline
OutputIterator reverse_copy_r(const BidirectionalRange& range, OutputIterator result)
{
	return std::reverse_copy(range.begin(), range.end(), result);
}

/** std::rotate wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange> inline
void rotate_r(ForwardRange& range, typename ForwardRange::iterator middle)
{
	return std::rotate(range.begin(), middle, range.end());
}

/** std::rotate_copy wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename ForwardRange, typename OutputIterator> inline
void rotate_copy_r(const ForwardRange& range, typename ForwardRange::iterator middle, OutputIterator result)
{
	return std::rotate_copy(range.begin(), middle, range.end(), result);
}

#if LASS_HAVE_RANDOM_SHUFFLE

/** std::random_shuffle wrapper for ranges
 *	@ingroup range_algorithm 
 */ 
template <typename RandomAccessRange> inline
void random_shuffle_r(RandomAccessRange& range)
{
	return std::random_shuffle(range.begin(), range.end());
}

/** std::random_shuffle wrapper for ranges
 *	@ingroup range_algorithm
 *  @pre @a rand must take a value n so that it returns a value in the range [0, n)
 */ 
template <typename RandomAccessRange, typename RandomNumberGenerator> inline
void random_shuffle_r(RandomAccessRange& range, RandomNumberGenerator& rand)
{
	return std::random_shuffle(range.begin(), range.end(), rand);
}

#endif

/** std::partition wrapper for ranges
 *  @ingroup range_algorithm
 */
template <typename BidirectionalRange, typename Predicate>
typename BidirectionalRange::iterator 
partition_r(BidirectionalRange& range, Predicate pred)
{
	return std::partition(range.begin(), range.end(), pred);
}

/** std::stable_partition wrapper for ranges
 *  @ingroup range_algorithm
 */
template <typename BidirectionalRange, typename Predicate>
typename BidirectionalRange::iterator 
stable_partition_r(BidirectionalRange& range, Predicate pred)
{
	return std::stable_partition(range.begin(), range.end(), pred);
}

}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif

// EOF
