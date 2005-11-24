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



/** @defgroup extended_string
 *  @brief extra std::string functionality
 *  @author Bram de Greve [BdG]
 *
 *  ExtendedString is not an extended std::string implementation as the name might suggest, but
 *  a set of functions that provide extra functionality on very standard std::string strings.
 *  Most of them are inspired by the string module of Python, as you will notice.
 *  Others are std::string implementations of C functions operating on char arrays
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_STRING_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_STRING_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
tolower(const std::basic_string<Char, Traits, Alloc>& input,
		const std::locale& locale = std::locale());

template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
toupper(const std::basic_string<Char, Traits, Alloc>& input,
		const std::locale& locale = std::locale());

template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
replace_all(const std::basic_string<Char, Traits, Alloc>& input,
			const std::basic_string<Char, Traits, Alloc>& to_be_replaced,
			const std::basic_string<Char, Traits, Alloc>& replacement);

template <typename Char, typename Traits, typename Alloc>
bool begins_with(const std::basic_string<Char, Traits, Alloc>& input,
				 const std::basic_string<Char, Traits, Alloc>& prefix);

template <typename Char, typename Traits, typename Alloc>
bool ends_with(const std::basic_string<Char, Traits, Alloc>& input,
			   const std::basic_string<Char, Traits, Alloc>& suffix);

template <typename Char, typename Traits, typename Alloc>
std::vector< std::basic_string<Char, Traits, Alloc> >
split(const std::basic_string<Char, Traits, Alloc>& to_be_split);

template <typename Char, typename Traits, typename Alloc>
std::vector< std::basic_string<Char, Traits, Alloc> >
split(const std::basic_string<Char, Traits, Alloc>& to_be_split,
	  const std::basic_string<Char, Traits, Alloc>& seperator,
	  size_t max_split = 0);

template <typename Char, typename Traits, typename Alloc, typename InputIterator>
std::basic_string<Char, Traits, Alloc>
join(const std::basic_string<Char, Traits, Alloc>& joiner, InputIterator first, InputIterator last);

template <typename Char, typename Traits, typename Alloc, typename InputRange>
std::basic_string<Char, Traits, Alloc>
join_r(const std::basic_string<Char, Traits, Alloc>& joiner, const InputRange& range);

template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
lstrip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped,
	   const std::basic_string<Char, Traits, Alloc>& to_be_removed);

template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
lstrip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped);

template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
rstrip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped,
	   const std::basic_string<Char, Traits, Alloc>& to_be_removed);

template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
rstrip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped);

template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
strip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped,
	  const std::basic_string<Char, Traits, Alloc>& to_be_removed);

template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
strip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped);

}

}

#include "extended_string.inl"

#endif

// EOF
