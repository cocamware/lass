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
 *	Copyright (C) 2004-2009 the Initial Developer.
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
