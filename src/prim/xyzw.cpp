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



#include "prim_common.h"
#include "xyzw.h"

namespace lass
{
namespace prim
{

/** intializes iterator to @a x axis.
 */
XYZW::XYZW():
	value_(0)
{
}



/** initializes iterator to an axis by character: 'x', 'y', 'z' or 'w'.
 */
XYZW::XYZW(char iAxis)
{
	reset(iAxis);
}



/** initializes iterator to an axis by number.
 *  ..., -1 == @a w, 0 == @a x, 1 == @a y, 2 == @a z, 3 == @a w, 4 == @a x...
 */
XYZW::XYZW(int iValue):
	value_(iValue)
{
}



/** initializes iterator to an axis by character: "x", "y", "z" or "w".
 */
XYZW::XYZW(const std::string& iAxis)
{
	if (iAxis.length() != 1)
	{
		LASS_THROW("Invalid parameter iAxis '" << iAxis << "'.  It must be a single character.");
	}
	reset(iAxis[0]);
}



/** return axis by character: 'x', 'y', 'z' or 'w'.
 */
const char XYZW::axis() const
{
	LASS_ASSERT(value_ >= 0 && value_ < 4);
	const char axes[] = { 'x', 'y', 'z', 'w' };
	return axes[value_];
}



XYZW& XYZW::operator++()
{
	LASS_ASSERT(value_ >= 0 && value_ < dimension);
	++value_;
	return *this;
}



XYZW& XYZW::operator--()
{
	LASS_ASSERT(value_ >= 0 && value_ < dimension);
	--value_;
	return *this;
}



XYZW XYZW::operator++(int)
{
	XYZW result(*this);
	++*this;
	return result;
}



XYZW XYZW::operator--(int)
{
	XYZW result(*this);
	--*this;
	return result;
}



XYZW& XYZW::operator+=(int iOffset)
{
	value_ += iOffset;
	return *this;
}



XYZW& XYZW::operator-=(int iOffset)
{
	value_ -= iOffset;
	return *this;
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

inline void XYZW::reset(char iAxis)
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

	case 3:
	case 'w':
	case 'W':
		value_ = 3;
		break;

	default:
		LASS_THROW("Invalid parameter iAxis '" << iAxis
			<< "'.  Try 'x', 'X', 'y', 'Y', 'z', 'Z', 'w' or 'W'.");
	}
}



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::XYZW
 */
bool operator==(const XYZW& iA, const XYZW& iB)
{
	return iA.value_ == iB.value_;
}



/** @relates lass::prim::XYZW
 */
bool operator==(const XYZW& iA, char iB)
{
	return iA.axis() == iB;
}



/** @relates lass::prim::XYZW
 */
bool operator==(char iA, const XYZW& iB)
{
	return iA == iB.axis();
}



/** @relates lass::prim::XYZW
 */
bool operator==(const XYZW& iA, const std::string& iB)
{
	return std::string(1, iA.axis()) == iB;
}



/** @relates lass::prim::XYZW
 */
bool operator==(const std::string& iA, const XYZW& iB)
{
	return iA == std::string(1, iB.axis());
}




/** @relates lass::prim::XYZW
 */
bool operator!=(const XYZW& iA, const XYZW& iB)
{
	return !(iA == iB);
}




/** @relates lass::prim::XYZW
 */
bool operator!=(const XYZW& iA, char iB)
{
	return !(iA == iB);
}




/** @relates lass::prim::XYZW
 */
bool operator!=(char iA, const XYZW& iB)
{
	return !(iA == iB);
}




/** @relates lass::prim::XYZW
 */
bool operator!=(const XYZW& iA, const std::string& iB)
{
	return !(iA == iB);
}




/** @relates lass::prim::XYZW
 */
bool operator!=(const std::string& iA, const XYZW& iB)
{
	return !(iA == iB);
}



/** @relates lass::prim::XYZW
 */
XYZW operator+(const XYZW& iA, int iOffset)
{
	XYZW result(iA);
	result += iOffset;
	return result;
}



/** @relates lass::prim::XYZW
 */
XYZW operator-(const XYZW& iA, int iOffset)
{
	XYZW result(iA);
	result -= iOffset;
	return result;
}



}

}

// EOF
