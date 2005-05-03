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



#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_STRING_INL
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_STRING_INL

#include "stde_common.h"
#include "extended_string.h"

#include <locale>

// --- implemenation details -----------------------------------------------------------------------

namespace lass
{
namespace stde
{

/** @ingroup extended_string
 *  convert std::basic_string to lower case by using user locale
 */
template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
tolower(const std::basic_string<Char, Traits, Alloc>& input,
		const std::locale& locale)
{
	std::basic_string<Char, Traits, Alloc> result(input);
	typedef typename std::basic_string<Char, Traits, Alloc>::iterator iterator;
	for (iterator i = result.begin(); i != result.end(); ++i)
	{
		*i = std::tolower<Char>(*i, locale);
	}
	return result;
}



/** @ingroup extended_string
 *  convert std::basic_string to upper case by using user locale
 */
template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
toupper(const std::basic_string<Char, Traits, Alloc>& input,
		const std::locale& locale)
{
	std::basic_string<Char, Traits, Alloc> result(input);
	typedef typename std::basic_string<Char, Traits, Alloc>::iterator iterator;
	for (iterator i = result.begin(); i != result.end(); ++i)
	{
		*i = std::toupper<Char>(*i, locale);
	}
	return result;
}

/** @ingroup extended_string
 *  replace all instances of @a to_be_replaced in @a input by @a replacement.
 */
template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
replace_all(const std::basic_string<Char, Traits, Alloc>& input,
			const std::basic_string<Char, Traits, Alloc>& to_be_replaced,
			const std::basic_string<Char, Traits, Alloc>& replacement)
{
	typedef std::basic_string<Char, Traits, Alloc> string_type;
	typename string_type::size_type size_to_be_replaced = to_be_replaced.size();
	typename string_type::size_type size_replacement = replacement.size();
	string_type result(input);

	typename string_type::size_type i = result.find(to_be_replaced);
	while (i != string_type::npos)
	{
		result.replace(i, size_to_be_replaced, replacement);
		i = result.find(to_be_replaced, i + size_replacement);
	}
	return result;
}



/** @ingroup extended_string
 *  returns true if @a input begins with the input @a prefix
 */
template <typename Char, typename Traits, typename Alloc>
bool begins_with(const std::basic_string<Char, Traits, Alloc>& input,
				 const std::basic_string<Char, Traits, Alloc>& prefix)
{
	return input.find(prefix) == 0;
}



/** @ingroup extended_string
 *  returns true if @a input ends with the input @a suffix
 */
template <typename Char, typename Traits, typename Alloc>
bool ends_with(const std::basic_string<Char, Traits, Alloc>& input,
			   const std::basic_string<Char, Traits, Alloc>& suffix)
{
	return input.rfind(suffix) == input.length() - suffix.length();
}



/** @ingroup extended_string
 *  Reflects the Python function @c split without seperator argument
 *
 *  Return a vector of the words of the string @a to_be_split.  The words are separated by arbitrary
 *  strings of whitespace characters (space, tab, newline, return, formfeed). 
 *
 *  If @a to_be_split is empty, then the result will have an empty vector.
 */
template <typename Char, typename Traits, typename Alloc>
std::vector< std::basic_string<Char, Traits, Alloc> >
split(const std::basic_string<Char, Traits, Alloc>& to_be_split)
{
	typedef std::basic_string<Char, Traits, Alloc> string_type;
	typedef typename string_type::size_type size_type;

	const string_type seperators = " \t\n";
	std::vector< std::basic_string<Char, Traits, Alloc> > result;

	if (to_be_split.empty())
	{
		return result;
	}

	size_type begin = 0;
	size_type i = to_be_split.find_first_of(seperators);
	while (i != string_type::npos)
	{
		result.push_back(to_be_split.substr(begin, i - begin));
		begin = to_be_split.find_first_not_of(seperators, i);
		if (begin == string_type::npos)
		{
			result.push_back(string_type());
			return result;
		}
		i = to_be_split.find_first_of(seperators, begin);
	}

	result.push_back(to_be_split.substr(begin));
	return result;
}



/** @ingroup extended_string
 *  Reflects the Python function @c split without seperator argument
 *
 *  Return a vector of the words of the string @a to_be_split.  The second argument @a seperator
 *  specifies a string to be used as the word separator. The returned vector will then have one
 *  more item than the number of non-overlapping occurrences of the separator in the string.
 *
 *  The optional third argument @a max_split defaults to 0.  If it is nonzero, at most
 *  @a max_split number of splits occur, and the remainder of the string is returned as the final
 *  element of the list (thus, the list will have at most @a max_split + 1 elements).
 *
 *  If @a to_be_split is empty, then the result will have an empty string as only element.
 */
template <typename Char, typename Traits, typename Alloc>
std::vector< std::basic_string<Char, Traits, Alloc> >
split(const std::basic_string<Char, Traits, Alloc>& to_be_split,
	  const std::basic_string<Char, Traits, Alloc>& seperator,
	  size_t max_split)
{
	typedef std::basic_string<Char, Traits, Alloc> string_type;
	typedef typename string_type::size_type size_type;

	std::vector< std::basic_string<Char, Traits, Alloc> > result;
	if (max_split == 0)
	{
		max_split = result.max_size() - 1;
	}

	const size_type seperator_size = seperator.size();
	size_type begin = 0;
	while (result.size() < max_split())
	{
		size_type i = to_be_split.find(seperator);
		if (i == string_type::npos)
		{
			break;
		}

		result.push_back(to_be_split.substr(begin, i - begin));
		begin = i + seperator_size;
	}

	result.push_back(to_be_split.substr(begin));
	return result;
}



}

}

#endif

// EOF
