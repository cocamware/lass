/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
tolower(const std::basic_string<Char, Traits, Alloc>& iString, 
        const std::locale& iLocale)
{
    std::basic_string<Char, Traits, Alloc> result(iString);
    typedef typename std::basic_string<Char, Traits, Alloc>::iterator iterator;
    for (iterator i = result.begin(); i != result.end(); ++i)
    {
        *i = std::tolower<Char>(*i, iLocale);
    }
    return result;
}



/** @ingroup extended_string
 *  convert std::basic_string to upper case by using user locale
 */
template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc> 
toupper(const std::basic_string<Char, Traits, Alloc>& iString, 
        const std::locale& iLocale)
{
    std::basic_string<Char, Traits, Alloc> result(iString);
    typedef typename std::basic_string<Char, Traits, Alloc>::iterator iterator;
    for (iterator i = result.begin(); i != result.end(); ++i)
    {
        *i = std::toupper<Char>(*i, iLocale);
    }
    return result;
}

/** @ingroup extended_string
 *  replace all instance of @a iToBeReplaced in @a iInput by @a iReplacement.
 */
template <typename Char, typename Traits, typename Alloc>
std::basic_string<Char, Traits, Alloc> 
replace_all(const std::basic_string<Char, Traits, Alloc>& iInput,
            const std::basic_string<Char, Traits, Alloc>& iToBeReplaced,
            const std::basic_string<Char, Traits, Alloc>& iReplacement)
{
    typedef std::basic_string<Char, Traits, Alloc> TString;
    typename TString::size_type sizeToBeReplaced = iToBeReplaced.size();
    typename TString::size_type sizeReplacement = iReplacement.size();
    TString result(iInput);

	std::string::size_type i = result.find(iToBeReplaced);
	while (i != std::string::npos)
	{
		result.replace(i, sizeToBeReplaced, iReplacement);
		i = result.find(iToBeReplaced, i + sizeReplacement);
	}
    return result;
}



/** @ingroup extended_string
 *  returns true if @a iInput begins with the string @a iPrefix
 */
template <typename Char, typename Traits, typename Alloc>
bool begins_with(const std::basic_string<Char, Traits, Alloc>& iInput,
				 const std::basic_string<Char, Traits, Alloc>& iPrefix)
{
	return iInput.find(iPrefix) == 0;
}



/** @ingroup extended_string
 *  returns true if @a iInput ends with the string @a iSuffix
 */
template <typename Char, typename Traits, typename Alloc>
bool ends_with(const std::basic_string<Char, Traits, Alloc>& iInput,
			   const std::basic_string<Char, Traits, Alloc>& iSuffix)
{
	return iInput.rfind(iSuffix) == iInput.length() - iSuffix.length();
}



}

}

#endif

// EOF
