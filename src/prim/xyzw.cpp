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
