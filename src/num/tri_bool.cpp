/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 */

#include "num_common.h"
#include "tri_bool.h"
#include "../util/bit_manip.h"

namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

/** Construct a TriBool, with as default state the "unknown".
 */
TriBool::TriBool(State iState):
	state_(iState)
{
}



/** Converts a C++ bool to a TriBool.  
 */
TriBool::TriBool(bool iBool):
	state_(iBool ? sTrue : sFalse)
{
}



/** return state
 */
const TriBool::State TriBool::state() const
{
	return state_;
}



/** access state
 */
TriBool::State& TriBool::state()
{
	return state_;
}



/** Negation: true becomes false, false becomes true, unknown remains unknown.
 *  <table>
 *		<tr><td>!</td>	<td>TRUE</td>	<td>FALSE</td>	<td>UNKNOWN</td></tr>
 *		<tr><td></td>	<td>false</td>	<td>true</td>	<td>unknown</td></tr>
 *  </table>
 */
TriBool TriBool::operator!() const
{
	if (state_ == sUnknown)
	{
		return sUnknown;
	}
	return state_ == sTrue ? sFalse : sTrue;
}



/** Convert TriBool to a bool in a boolean context.
 *
 *  Will evaluate true if a boolean context only when the TriBool is definitely true. By this, you 
 *  can use TriBools in if-statements:
 * 
 *  @code
 *  TriBool a;
 *  if (a)
 *  {
 *      LASS_ASSERT(a.isTrue());
 *  }
 *  @endcode
 *
 *  Because @c !a makes @c true @c false, @c false @c true and @c unknown @c unknown, the following
 *  also works:
 * 
 *  @code
 *  TriBool a;
 *  if (!a)
 *  {
 *      LASS_ASSERT(a.isFalse());
 *  }
 */
TriBool::operator SafeBool() const
{
	return state_ == sTrue ? safeTrue : safeFalse;
}



/** return true if state is @c true, return false otherwise.
 */
bool TriBool::isTrue() const
{
    return state_ == sTrue;
}



/** return true if state is @c false, return false otherwise.
 */
bool TriBool::isFalse() const
{
    return state_ == sFalse;
}



/** return true if state is @c unknown, return false otherwise.
 */
bool TriBool::isUnknown() const
{
    return state_ == sUnknown;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** Evaluates if two TriBool's are equal, if at least one is unknown then we don't know the result.
 *  @relates TriBool
 *  <table>
 *		<tr><td>==></td>		<td>TRUE</td>		<td>FALSE</td>		<td>UNKNOWN</td></tr>
 *		<tr><td>TRUE</td>		<td>true</td>		<td>false</td>		<td>unknown</td></tr>
 *		<tr><td>FALSE</td>		<td>false</td>		<td>true</td>		<td>unknown</td></tr>
 *		<tr><td>UNKNOWN</td>	<td>unknown</td>	<td>unknown</td>	<td>unknown</td></tr>
 *  </table>
 *
 *  @warning don't expect @c unknown @c == @c unknown yields @c true!  Use @c unknown.isUnknown() 
 *  instead.
 */
TriBool operator==(TriBool iA, TriBool iB)
{
    if (iA.isUnknown() || iB.isUnknown())
	{
		return TriBool::sUnknown;
	}
	return iA.state() == iB.state();
}



/** Evaluates if two TriBool's differ, if at least one is unknown then we don't know the result.
 *  @relates TriBool
 *  <table>
 *		<tr><td>!=</td>			<td>TRUE</td>		<td>FALSE</td>		<td>UNKNOWN</td></tr>
 *		<tr><td>TRUE</td>		<td>false</td>		<td>true</td>		<td>unknown</td></tr>
 *		<tr><td>FALSE</td>		<td>true</td>		<td>false</td>		<td>unknown</td></tr>
 *		<tr><td>UNKNOWN</td>	<td>unknown</td>	<td>unknown</td>	<td>unknown</td></tr>
 *  </table>
 */
TriBool operator!=(TriBool iA, TriBool iB)
{
	return !(iA == iB);
}



/** Evaluates to true if both are true, unknown if at least one is unknown, false otherwise.
 *  @relates TriBool
 *  <table>
 *		<tr><td>&amp;&amp;</td>	<td>TRUE</td>		<td>FALSE</td>		<td>UNKNOWN</td></tr>
 *		<tr><td>TRUE</td>		<td>true</td>		<td>false</td>		<td>unknown</td></tr>
 *		<tr><td>FALSE</td>		<td>false</td>		<td>false</td>		<td>@b false</td></tr>
 *		<tr><td>UNKNOWN</td>	<td>unknown</td>	<td>@b false</td>	<td>unknown</td></tr>
 *  </table>
 */
TriBool operator&&(TriBool iA, TriBool iB)
{
    if (iA.isFalse() || iB.isFalse())
	{
		return TriBool::sFalse;
	}
    if (iA.isTrue() && iB.isTrue())
	{
		return TriBool::sTrue;
	}
	return TriBool::sUnknown;
}



/** Evaluates to true if at least one is true, false if both are false, unknown otherwise.
 *  @relates TriBool
 *  <table>
 *		<tr><td>||</td>			<td>TRUE</td>		<td>false</td>		<td>unknown</td></tr>
 *		<tr><td>TRUE</td>		<td>true</td>		<td>true</td>		<td>@b true</td></tr>
 *		<tr><td>FALSE</td>		<td>true</td>		<td>false</td>		<td>unknown</td></tr>
 *		<tr><td>UNKNOWN</td>	<td>@b true</td>	<td>unknown</td>	<td>unknown</td></tr>
 *  </table>
 */
TriBool operator||(TriBool iA, TriBool iB)
{
    if (iA.isTrue() || iB.isTrue())
	{
		return TriBool::sTrue;
	}
    if (iA.isFalse() && iB.isFalse())
	{
		return TriBool::sFalse;
	}
	return TriBool::sUnknown;
}



/** @relates TriBool
 */
std::ostream& operator<<(std::ostream& ioS, TriBool iB)
{
	switch (iB.state())
	{
	case TriBool::sTrue:
		ioS << true;
		break;
		
	case TriBool::sFalse:
		ioS << false;
		break;

	default:
		LASS_ASSERT(iB.state() == TriBool::sUnknown);
		ioS << (ioS.flags() & std::ios_base::boolalpha ? "unknown" : "?");
		break;
	}

	return ioS;
}



}

}

// EOF
