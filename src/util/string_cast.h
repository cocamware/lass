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

/** @fun stringCast
 *  @brief casts a value of one type to another by using stringstreams.
 *  @author Bram de Greve [Bramz]
 *  @date 2004
 *
 *  This function is especially usefull when non-string values like floats, ints, ... should 
 *  be converted to their string representation or vice-versa.  The way to do this is by using
 *  a @c std::stringstream that does this conversion for you.  This little function wraps this
 *  all for your convenience:
 *
 *  @code
 *  int a = util::stringCast<int>(5); // a == 5
 *	double b = util::stringCast<double>(5); // b == 5.0
 *	std::string c = util::stringCast<std::string>(5); // c == "5"
 *
 *	int d = util::stringCast<int>(5.0); // d == 5
 *	double e = util::stringCast<double>(5.0); // e == 5.0
 *	std::string f = util::stringCast<std::string>(5.0); // f == "5"
 *
 *	std::string five = "5";
 *	int g = util::stringCast<int>(five); // g == 5
 *	double h = util::stringCast<double>(five); // h == 5.0
 *	std::string i = util::stringCast<std::string>(five); // i == "5"
 *
 *	void* p = reinterpret_cast<void*>(0x1a2b3c4d);
 *	std::string j = util::stringCast<std::string>(p); // j == "1A2B3C4D"
 *  @endcode
 *
 *  @section acknowledgment
 *
 *  this stringCast started as a very simple implementation, but was later somehow adapted to the 
 *  boost::lexical_cast implementation by Kevlin Henney.
 *
 *  <i>Copyright Kevlin Henney, 2000-2003. All rights reserved.
 *
 *	Permission to use, copy, modify, and distribute this software for any
 *	purpose is hereby granted without fee, provided that this copyright and
 *	permissions notice appear in all copies and derivatives.
 *
 *	This software is provided "as is" without express or implied warranty.</i>
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_STRING_CAST_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_STRING_CAST_H

#include "util_common.h"
#include <limits>

namespace lass
{
namespace util
{

// --- implementation ------------------------------------------------------------------------------

namespace impl
{

template <typename Out, typename In>
class StringCaster
{
public:

	StringCaster()
	{
        buffer_.unsetf(std::ios::skipws);
		if (std::numeric_limits<Out>::is_specialized)
		{
			buffer_.precision(std::numeric_limits<Out>::digits10 + 1);
		}
        else if(std::numeric_limits<In>::is_specialized)
		{
			buffer_.precision(std::numeric_limits<In>::digits10 + 1);
		}
	}

	bool operator<<(const In& iIn)
	{
		return !(buffer_ << iIn).fail();
	}

	template <typename T>
	bool operator>>(T& oOut)
	{
		return buffer_ >> oOut && (buffer_ >> std::ws).eof();
	}

	bool operator>>(std::string& oOut)
	{
		oOut = buffer_.str();
		return true;
	}

private:

	std::stringstream buffer_;
};

}



// --- interface -----------------------------------------------------------------------------------

template <typename Out, typename In>
Out stringCast(const In& iIn)
{
	Out result;
	impl::StringCaster<Out, In> caster;
	if (!(caster << iIn && caster >> result))
	{
		LASS_THROW("cast has failed");
	}
	return result;
}



}

}

#endif

// EOF
