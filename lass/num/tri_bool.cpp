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



#include "lass_common.h"
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
TriBool::State TriBool::state() const
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
 *      <tr><td>!</td>  <td>TRUE</td>   <td>FALSE</td>  <td>UNKNOWN</td></tr>
 *      <tr><td></td>   <td>false</td>  <td>true</td>   <td>unknown</td></tr>
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
 *      <tr><td>==></td>        <td>TRUE</td>       <td>FALSE</td>      <td>UNKNOWN</td></tr>
 *      <tr><td>TRUE</td>       <td>true</td>       <td>false</td>      <td>unknown</td></tr>
 *      <tr><td>FALSE</td>      <td>false</td>      <td>true</td>       <td>unknown</td></tr>
 *      <tr><td>UNKNOWN</td>    <td>unknown</td>    <td>unknown</td>    <td>unknown</td></tr>
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
 *      <tr><td>!=</td>         <td>TRUE</td>       <td>FALSE</td>      <td>UNKNOWN</td></tr>
 *      <tr><td>TRUE</td>       <td>false</td>      <td>true</td>       <td>unknown</td></tr>
 *      <tr><td>FALSE</td>      <td>true</td>       <td>false</td>      <td>unknown</td></tr>
 *      <tr><td>UNKNOWN</td>    <td>unknown</td>    <td>unknown</td>    <td>unknown</td></tr>
 *  </table>
 */
TriBool operator!=(TriBool iA, TriBool iB)
{
	return !(iA == iB);
}



/** Evaluates to true if both are true, unknown if at least one is unknown, false otherwise.
 *  @relates TriBool
 *  <table>
 *      <tr><td>&amp;&amp;</td> <td>TRUE</td>       <td>FALSE</td>      <td>UNKNOWN</td></tr>
 *      <tr><td>TRUE</td>       <td>true</td>       <td>false</td>      <td>unknown</td></tr>
 *      <tr><td>FALSE</td>      <td>false</td>      <td>false</td>      <td>@b false</td></tr>
 *      <tr><td>UNKNOWN</td>    <td>unknown</td>    <td>@b false</td>   <td>unknown</td></tr>
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
 *      <tr><td>||</td>         <td>TRUE</td>       <td>false</td>      <td>unknown</td></tr>
 *      <tr><td>TRUE</td>       <td>true</td>       <td>true</td>       <td>@b true</td></tr>
 *      <tr><td>FALSE</td>      <td>true</td>       <td>false</td>      <td>unknown</td></tr>
 *      <tr><td>UNKNOWN</td>    <td>@b true</td>    <td>unknown</td>    <td>unknown</td></tr>
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
		ioS << (ioS.flags() & std::ios::boolalpha ? "unknown" : "?");
		break;
	}

	return ioS;
}



}

}

// EOF
