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
#include "xyz.h"

namespace lass
{
namespace prim
{

/** intializes iterator to @a x axis.
 */
XYZ::XYZ() throw():
    value_(0)
{
}



/** initializes iterator to an axis by character: 'x', 'y' or 'z'.
 */
XYZ::XYZ(char iAxis)
{
    reset(iAxis);
}



/** initializes iterator to an axis by number.
 *  ..., -1 == @a z, 0 == @a x, 1 == @a y, 2 == @a z, 3 == @a x, ...
 */
XYZ::XYZ(int iValue) throw():
    value_(iValue)
{
}



/** initializes iterator to an axis by character: "x", "y" or "z".
 */
XYZ::XYZ(const std::string& iAxis)
{
	if (iAxis.length() != 1)
	{
		LASS_THROW("Invalid parameter iAxis '" << iAxis << "'.  It must be a single character.");
	}
	reset(iAxis[0]);
}



/** return axis by character: "x", "y" or "z".
 */
const char XYZ::axis() const throw()
{
    LASS_ASSERT(value_ >= 0 && value_ < 3);
	const char axes[] = { 'x', 'y', 'z' };
	return axes[value_];
}



XYZ& XYZ::operator++() throw()
{
    LASS_ASSERT(value_ >= 0 && value_ < dimension);
    ++value_;
    return *this;
}



XYZ& XYZ::operator--() throw()
{
    LASS_ASSERT(value_ >= 0 && value_ < dimension);
    --value_;
    return *this;
}



XYZ XYZ::operator++(int) throw()
{
    XYZ result(*this);
    ++*this;
    return result;
}



XYZ XYZ::operator--(int) throw()
{
    XYZ result(*this);
    --*this;
    return result;
}



XYZ& XYZ::operator+=(int iOffset) throw()
{
    value_ += iOffset;
    return *this;
}



XYZ& XYZ::operator-=(int iOffset) throw()
{
    value_ -= iOffset;
    return *this;
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

inline void XYZ::reset(char iAxis)
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

	case 2:
    case 'z':
    case 'Z':
        value_ = 2;
		break;

	default:
		LASS_THROW("Invalid parameter iAxis '" << iAxis 
            << "'.  Try 'x', 'X', 'y', 'Y', 'z' or 'Z'.");
	}
}



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::XYZ
 */
bool operator==(const XYZ& iA, const XYZ& iB) throw()
{
    return iA.value_ == iB.value_;
}



/** @relates lass::prim::XYZ
 */
bool operator==(const XYZ& iA, char iB) throw()
{
	return iA.axis() == iB;
}



/** @relates lass::prim::XYZ
 */
bool operator==(char iA, const XYZ& iB) throw()
{
	return iA == iB.axis();
}



/** @relates lass::prim::XYZ
 */
bool operator==(const XYZ& iA, const std::string& iB)
{
	return std::string(1, iA.axis()) == iB;
}



/** @relates lass::prim::XYZ
 */
bool operator==(const std::string& iA, const XYZ& iB)
{
	return iA == std::string(1, iB.axis());
}




/** @relates lass::prim::XYZ
 */
bool operator!=(const XYZ& iA, const XYZ& iB) throw()
{
    return !(iA == iB);
}




/** @relates lass::prim::XYZ
 */
bool operator!=(const XYZ& iA, char iB) throw()
{
    return !(iA == iB);
}




/** @relates lass::prim::XYZ
 */
bool operator!=(char iA, const XYZ& iB) throw()
{
    return !(iA == iB);
}




/** @relates lass::prim::XYZ
 */
bool operator!=(const XYZ& iA, const std::string& iB)
{
    return !(iA == iB);
}




/** @relates lass::prim::XYZ
 */
bool operator!=(const std::string& iA, const XYZ& iB)
{
    return !(iA == iB);
}



/** @relates lass::prim::XYZ
 */
XYZ operator+(const XYZ& iA, int iOffset) throw()
{
    XYZ result(iA);
    result += iOffset;
    return result;
}



/** @relates lass::prim::XYZ
 */
XYZ operator-(const XYZ& iA, int iOffset) throw()
{
    XYZ result(iA);
    result -= iOffset;
    return result;
}



/** @relates lass::prim::XYZ
 */
std::ostream& operator<<(std::ostream& ioS, const XYZ& iXYZ)
{
	ioS << iXYZ.axis();
	return ioS;
}



}

}

// EOF
