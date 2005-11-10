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



/** @defgroup stde_select
 *  @brief functors to select element of std::pair or stde::triple
 *
 *  @code
 *	typedef std::map<std::string, int> map_type;
 *	map_type a;
 *	a["one"] = 1;
 *	a["two"] = 2;
 *	a["three"] = 3;
 *	std::transform(a.begin(), a.end(), 
 *		std::ostream_iterator<map_type::key_type>(std::cout, " "),
 *		stde::select_1st<map_type::value_type>());
 *  @endcode
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_SELECT_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_SELECT_H

#include "stde_common.h"
#include "../util/call_traits.h"

namespace lass
{
namespace stde
{

/** selects first element of std::pair or stde::triple and applies supplied operator on it
 *  @ingroup stde_select
 */
template <typename Pair>
struct select_1st: public std::unary_function<Pair, typename Pair::first_type>
{
	typename util::CallTraits<typename Pair::first_type>::TParam 
	operator()(typename util::CallTraits<Pair>::TParam iArgument) 
	{ 
		return iArgument.first; 
	}
};



/** selects second element of std::pair or stde::triple and applies supplied operator on it
 *  @ingroup stde_select
 */
template <typename Pair>
struct select_2nd: public std::unary_function<Pair, typename Pair::second_type>
{
	typename util::CallTraits<typename Pair::second_type>::TParam 
	operator()(typename util::CallTraits<Pair>::TParam iArgument) 
	{ 
		return iArgument.second; 
	}
};



/** selects second element of std::pair or stde::triple and applies supplied operator on it
 *  @ingroup stde_select
 */
template <typename Triple>
struct select_3rd: public std::unary_function<Triple, typename Triple::third_type>
{
	typename util::CallTraits<typename Triple::third_type>::TParam 
	operator()(typename util::CallTraits<Triple>::TParam iArgument) 
	{ 
		return iArgument.third; 
	}
};

}

}

#endif

// EOF
