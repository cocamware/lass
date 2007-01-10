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

#include <locale>

namespace lass
{
namespace stde
{
namespace impl
{
	template <typename StringType> 
	StringType whitespace(const StringType&) 
	{ 
		return StringType(" \t\n"); 
	}
}

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
 *  replace all instances of @a to_be_replaced in @a input by @a replacement.
 */
template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
replace_all(const std::basic_string<Char, Traits, Alloc>& input,
	const Char* to_be_replaced,
	const Char* replacement)
{
	return replace_all(input, std::string(to_be_replaced), std::string(replacement));
}



/** @ingroup extended_string
 *  returns true if @a input begins with the input @a prefix
 */
template <typename Char, typename Traits, typename Alloc>
bool begins_with(const std::basic_string<Char, Traits, Alloc>& input,
	const std::basic_string<Char, Traits, Alloc>& prefix)
{
	return prefix.length() <= input.length() && std::equal(prefix.begin(), prefix.end(), input.begin());
}

/** @ingroup extended_string
 *  returns true if @a input begins with the input @a prefix
 */
template <typename Char, typename Traits, typename Alloc>
bool begins_with(const std::basic_string<Char, Traits, Alloc>& input,
	const Char* prefix)
{
	return begins_with(input, std::string(prefix));
}



/** @ingroup extended_string
 *  returns true if @a input ends with the input @a suffix
 */
template <typename Char, typename Traits, typename Alloc>
bool ends_with(const std::basic_string<Char, Traits, Alloc>& input,
	const std::basic_string<Char, Traits, Alloc>& suffix)
{
	return suffix.length() <= input.length() && 
		std::equal(suffix.begin(), suffix.end(), input.begin() + input.length() - suffix.length());
}

/** @ingroup extended_string
 *  returns true if @a input ends with the input @a suffix
 */
template <typename Char, typename Traits, typename Alloc>
bool ends_with(const std::basic_string<Char, Traits, Alloc>& input,
	const Char* suffix)
{
	return ends_with(input, std::string(suffix));
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

	const string_type seperators = impl::whitespace(to_be_split);
	std::vector< std::basic_string<Char, Traits, Alloc> > result;

	if (to_be_split.empty())
	{
		return result;
	}

	size_type begin = 0;
	size_type end = to_be_split.find_first_of(seperators);
	while (end != string_type::npos)
	{
		result.push_back(to_be_split.substr(begin, end - begin));
		begin = to_be_split.find_first_not_of(seperators, end);
		if (begin == string_type::npos)
		{
			result.push_back(string_type());
			return result;
		}
		end = to_be_split.find_first_of(seperators, begin);
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
	while (result.size() < max_split)
	{
		const size_type end = to_be_split.find(seperator, begin);
		if (end == string_type::npos)
		{
			break;
		}

		result.push_back(to_be_split.substr(begin, end - begin));
		begin = end + seperator_size;
	}

	result.push_back(to_be_split.substr(begin));
	return result;
}

/** @ingroup extended_string
 *  Reflects the Python function @c split without seperator argument
 */
template <typename Char, typename Traits, typename Alloc>
std::vector< std::basic_string<Char, Traits, Alloc> >
split(const std::basic_string<Char, Traits, Alloc>& to_be_split,
	const Char* seperator,
	size_t max_split)
{
	return split(to_be_split, std::string(seperator), max_split);
}



/** @ingroup extended_string
 */
template <typename Char, typename Traits, typename Alloc, typename InputIterator>
std::basic_string<Char, Traits, Alloc>
join(const std::basic_string<Char, Traits, Alloc>& joiner, InputIterator first, InputIterator last)
{
	std::basic_ostringstream<Char, Traits, Alloc> buffer;
	if (first != last)
	{
		buffer << *first++;
	}
	while (first != last)
	{
		buffer << joiner << *first++;
	}
	return buffer.str();
}



template <typename Char, typename Traits, typename Alloc, typename InputRange> inline
std::basic_string<Char, Traits, Alloc>
join_r(const std::basic_string<Char, Traits, Alloc>& joiner, const InputRange& range)
{
	return join(joiner, range.begin(), range.end());
}



/** @ingroup extended_string
 *  Return a copy of the string @a to_be_stripped with leading characters removed.
 *
 *  The characters in the string @a to_be_removed will be stripped from the beginning of the string
 *  @a to_be_stripped. 
 */
template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
lstrip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped,
	   const std::basic_string<Char, Traits, Alloc>& to_be_removed)
{
	typedef std::basic_string<Char, Traits, Alloc> string_type;
	const typename string_type::size_type begin = to_be_stripped.find_first_not_of(to_be_removed);
	return begin == string_type::npos ? string_type() : to_be_stripped.substr(begin);
}



/** @ingroup extended_string
 *  Return a copy of the string @a to_be_stripped with leading whitespace removed.
 */
template <typename Char, typename Traits, typename Alloc> inline
std::basic_string<Char, Traits, Alloc>
lstrip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped)
{
	return lstrip(to_be_stripped, impl::whitespace(to_be_stripped));
}



/** @ingroup extended_string
 *  Return a copy of the string @a to_be_stripped with trailing characters removed.
 *
 *  The characters in the string @a to_be_removed will be stripped from the ending of the string
 *  @a to_be_stripped. 
 */
template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
rstrip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped,
	   const std::basic_string<Char, Traits, Alloc>& to_be_removed)
{
	typedef std::basic_string<Char, Traits, Alloc> string_type;
	const typename string_type::size_type end = to_be_stripped.find_last_not_of(to_be_removed);
	return end == string_type::npos ? string_type() : to_be_stripped.substr(0, end + 1);
}



/** @ingroup extended_string
 *  Return a copy of the string @a to_be_stripped with trailing whitespace removed.
 */
template <typename Char, typename Traits, typename Alloc> inline
std::basic_string<Char, Traits, Alloc>
rstrip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped)
{
	return rstrip(to_be_stripped, impl::whitespace(to_be_stripped));
}



/** @ingroup extended_string
 *  Return a copy of the string @a to_be_stripped with both leading and trailing characters removed.
 *
 *  The characters in the string @a to_be_removed will be stripped from both the beginning and the 
 *  ending of the string @a to_be_stripped. 
 */
template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc>
strip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped,
	  const std::basic_string<Char, Traits, Alloc>& to_be_removed)
{
	typedef std::basic_string<Char, Traits, Alloc> string_type;
	const typename string_type::size_type begin = to_be_stripped.find_first_not_of(to_be_removed);
	const typename string_type::size_type end = to_be_stripped.find_last_not_of(to_be_removed);
	return begin == string_type::npos ? string_type() : to_be_stripped.substr(begin, end - begin + 1);
}



/** @ingroup extended_string
 *  Return a copy of the string @a to_be_stripped with leading and trailing whitespace removed.
 */
template <typename Char, typename Traits, typename Alloc> inline
std::basic_string<Char, Traits, Alloc>
strip(const std::basic_string<Char, Traits, Alloc>& to_be_stripped)
{
	return strip(to_be_stripped, impl::whitespace(to_be_stripped));
}



}

}

// EOF
