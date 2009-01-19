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

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_BSD
#	ifdef toupper
#		undef toupper
#	endif
#	ifdef tolower
#		undef tolower
#	endif
#endif

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
std::basic_string<Char, Traits, Alloc>
replace_all(const std::basic_string<Char, Traits, Alloc>& input,
	const Char* to_be_replaced,
	const Char* replacement);

template <typename Char, typename Traits, typename Alloc>
bool begins_with(const std::basic_string<Char, Traits, Alloc>& input,
	const std::basic_string<Char, Traits, Alloc>& prefix);
template <typename Char, typename Traits, typename Alloc>
bool begins_with(const std::basic_string<Char, Traits, Alloc>& input,
	const Char* prefix);

template <typename Char, typename Traits, typename Alloc>
bool ends_with(const std::basic_string<Char, Traits, Alloc>& input,
	const std::basic_string<Char, Traits, Alloc>& suffix);
template <typename Char, typename Traits, typename Alloc>
bool ends_with(const std::basic_string<Char, Traits, Alloc>& input,
	const Char* suffix);

template <typename Char, typename Traits, typename Alloc>
std::vector< std::basic_string<Char, Traits, Alloc> >
split(const std::basic_string<Char, Traits, Alloc>& to_be_split);

template <typename Char, typename Traits, typename Alloc>
std::vector< std::basic_string<Char, Traits, Alloc> >
split(const std::basic_string<Char, Traits, Alloc>& to_be_split,
	  const std::basic_string<Char, Traits, Alloc>& seperator,
	  size_t max_split = 0);
template <typename Char, typename Traits, typename Alloc>
std::vector< std::basic_string<Char, Traits, Alloc> >
split(const std::basic_string<Char, Traits, Alloc>& to_be_split,
	  const Char* seperator,
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
