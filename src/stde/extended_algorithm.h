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



/** @defgroup extended_algorithm
 *  @brief extra algorithms
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_ALGORITHM_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_ALGORITHM_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

// --- pure iterator algorithms --------------------------------------------------------------------

template <class InputIterator, class Size, class OutputIterator>
OutputIterator copy_n(InputIterator first, Size count, 
                      OutputIterator result)
{
	for (Size i=0;i<count;++i)
		*result++ = *first++;
	return result;
}      

template <class InputIterator, class OutputIterator, class Size>
OutputIterator repeat(InputIterator first, InputIterator last, OutputIterator output, Size n)
{
	for (Size i = 0; i < n; ++i)
	{
		output = std::copy(first, last, output);
	}
	return output;
}



// --- range algorithms shortcuts ------------------------------------------------------------------

template <typename InputRange, typename OutputIterator> inline
void copy_r(const InputRange& range, OutputIterator output)
{
	std::copy(range.begin(), range.end(), output);
}

template <typename ForwardRange, typename Op> inline
void for_each_r(ForwardRange& range, Op op)
{
	std::for_each(range.begin(), range.end(), op);
}

template <typename InputRange, typename OutputIterator, typename Op> inline
void transform_r(const InputRange& range, OutputIterator output, Op op)
{
	std::transform(range.begin(), range.end(), output, op);
}



// --- container algorithms shortcuts --------------------------------------------------------------

template <class Container, class Size> inline
Container repeat_c(const Container& iC, Size n)
{
	Container result;
	repeat(iC.begin(), iC.end(), std::back_inserter(result), n);
	return result;
}

template <class Container, class Size> inline
Container& inplace_repeat_c(Container& iC, Size n)
{
	Container temp(iC);
	repeat(temp.begin(), temp.end(), std::back_inserter(iC), n - 1);
	return iC;
}

}
}

#endif

// EOF
