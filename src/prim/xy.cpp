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



#include "prim_common.h"
#include "xy.h"

namespace lass
{
namespace prim
{

/** intializes iterator to @a x axis.
 */
XY::XY():
    value_(0)
{
}




/** initializes iterator to an axis by character: 'x' or 'y'.
 */
XY::XY(char iAxis)
{
    reset(iAxis);
}



/** initializes iterator to an axis by number.
 *  ..., -1 == @a y, 0 == @a x, 1 == @a y, 2 == @a x, ...
 */
XY::XY(int iValue):
    value_(iValue)
{
}



/** initializes iterator to an axis by character: "x" or "y".
 */
XY::XY(const std::string& iAxis)
{
	if (iAxis.length() != 1)
	{
		LASS_THROW("Invalid parameter iAxis '" << iAxis << "'.  It must be a single character.");
	}
	reset(iAxis[0]);
}



/** return axis by character: "x" or "y".
 */
const char XY::axis() const
{
    LASS_ASSERT(value_ >= 0 && value_ < dimension);
	const char axes[] = { 'x', 'y' };
	return axes[value_];
}



XY& XY::operator++()
{
    LASS_ASSERT(value_ >= 0 && value_ < dimension);
    ++value_;
    return *this;
}



XY& XY::operator--()
{
    LASS_ASSERT(value_ >= 0 && value_ < dimension);
    --value_;
    return *this;
}



XY XY::operator++(int)
{
    XY result(*this);
    ++*this;
    return result;
}



XY XY::operator--(int)
{
    XY result(*this);
    --*this;
    return result;
}



XY& XY::operator+=(int iOffset)
{
    value_ += iOffset;
    return *this;
}



XY& XY::operator-=(int iOffset)
{
    value_ -= iOffset;
    return *this;
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

inline void XY::reset(char iAxis)
{
	switch (iAxis)
	{
	case 0:
	case 'x':
	case 'X':
		value_ = 0;
		break;

	case 1:
	case 'y':
	case 'Y':
		value_ = 1;
		break;

	default:
		LASS_THROW("Invalid parameter iAxis '" << iAxis << "'.  Try 'x', 'X', 'y' or 'Y'.");
	}
}



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::XY
 */
bool operator==(const XY& iA, const XY& iB)
{
    return iA.value_ == iB.value_;
}



/** @relates lass::prim::XY
 */
bool operator==(const XY& iA, char iB)
{
	return iA.axis() == iB;
}



/** @relates lass::prim::XY
 */
bool operator==(char iA, const XY& iB)
{
	return iA == iB.axis();
}



/** @relates lass::prim::XY
 */
bool operator==(const XY& iA, const std::string& iB)
{
	return std::string(1, iA.axis()) == iB;
}



/** @relates lass::prim::XY
 */
bool operator==(const std::string& iA, const XY& iB)
{
	return iA == std::string(1, iB.axis());
}




/** @relates lass::prim::XY
 */
bool operator!=(const XY& iA, const XY& iB)
{
    return !(iA == iB);
}




/** @relates lass::prim::XY
 */
bool operator!=(const XY& iA, char iB)
{
    return !(iA == iB);
}




/** @relates lass::prim::XY
 */
bool operator!=(char iA, const XY& iB)
{
    return !(iA == iB);
}




/** @relates lass::prim::XY
 */
bool operator!=(const XY& iA, const std::string& iB)
{
    return !(iA == iB);
}




/** @relates lass::prim::XY
 */
bool operator!=(const std::string& iA, const XY& iB)
{
    return !(iA == iB);
}



/** @relates lass::prim::XY
 */
XY operator+(const XY& iA, int iOffset)
{
    XY result(iA);
    result += iOffset;
    return result;
}



/** @relates lass::prim::XY
 */
XY operator-(const XY& iA, int iOffset)
{
    XY result(iA);
    result -= iOffset;
    return result;
}



}

}

// EOF
