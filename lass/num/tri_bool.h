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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



/** @class lass::num::TriBool
 *  @brief a three-state boolean value with { true, false and lass::num::unknown }
 *  @author Bram de Greve [BdG]
 *
 *  Inspired by Boost three-state boolean logic library 'boost::tribool' by Doug Gregor
 *  (gregod@cs.rpi.edu).
 *
 *  Booleans are great to express binary states, to express if something is true or false.  But they
 *  lack the concept of @e Not-A-Number (@c NaN).  Indeed, with traditional booleans, there's no way
 *  to express that you don't know, that the boolean value is invalid.  That's when this TriBool
 *  comes into action.  TriBool adds one extra state to the traditional @c true and @c false:
 *  @c unknown.  As said, this @c unknown acts like the @c NaN for floating point values and behaves
 *  much the same way (though not @e entirely the same!).
 *
 *  @section behaviour_unknown behaviour of unknown
 *
 *  The first thing to know is that the value @c unknown can not come into existance as long as all
 *  your operands are in a known state (@c true or @c false).  You have to inject an @c unknown
 *  value yourself, and this section merely describes how it will flow through your code.
 *
 *  @b rule: <i>In TriBool unary expressions, if the argument is @c unknown, then the result will
 *           yield @c unknown too.</i>.
 *
 *  Check out @c operator!  Indeed, if @c a is @c unknown, then @c !a is @c unknown too.
 *
 *  @b rule: <i>In TriBool binary expressions, if at least one of the operands is @c unknown, then
 *           the result yields @c unknown, @b except if one of the operands is known (@c true or
 *           @c false) and provides enough information to result in a known state.</i>
 *
 *  The first part is obvious.  In an expression @c a @c == @c b , if one of the operands is
 *  @c unknown, we can not possible know if @c a and @c b are equal or not.  It will yield
 *  @c unknown.  However, there are two situations in which a binary expression with an unknown
 *  operands still results in a known state.  These are:
 *
 *  - @c false @c && @c unknown yields @c false.  Indeed, for an AND operation to yield @c true,
 *    @e both operands must be @c true.  Since we know at least one of them is @c false, we can know
 *    for sure the result is @c false too.
 *  - @c true @c || @c unknown yields @c true.  Indeed, for an OR operation to yield @c true,
 *    at @e least one of the operands must be @c true.  Since we know the left operand is @c true
 *    we know for sure we have at least one operand that is true, hence the result is @c true.
 *
 *  Since AND and OR are commutative, this is also valid for @c unknown @c && @c false and
 *  @c unknown @c || @c true .  In all other cases, a binary expression with at least one @c unknown
 *  operand will yield @c unknown.
 *
 *  @note we said @c unknown behaves much like @c NaN, but that's not really true.  If you
 *        compare two operands that are @c NaN, you'll get @c false as result (a known state),
 *        where as if both operands are @c unknown, it will not result @c false but @c unknown!
 *
 *  @note in contrary to traditional boolean logic, @c a!=b @c || @c a==b does not always yield
 *        @c true!  It will be @c unknown if one of the operands is unknown.
 *
 *  @section if_consequences consequences on if/else
 *
 *  Care must be taken when TriBool variables are used in boolean context like @c if/else statements
 *  (and @c for and @c while loops ...).  For instance, the following examples are not equivalent
 *  (as they would be for plain old boolean logic).  Notice what happens if @c a is unknown!
 *
 *  @code
 *  TriBool a;
 *
 *  if (a)
 *  {
 *      foo(); // called if a is true.
 *  }
 *  else
 *  {
 *      bar(); // called if a is false OR unknown.
 *  }
 *
 *  if (!a)
 *  {
 *      bar(); // called if a is false
 *  }
 *  else
 *  {
 *      foo(); // called if a is true OR unknown.
 *  }
 *  @endcode
 *
 *  The discrepancy is due to the fact there isn't an unambigious way to reduce the three states
 *  of a TriBool to the two states of the @c if/else statement, or the traditional boolean logic.
 *  In the former, the @c if block tests if @c a is @c true and the @c else block gets the other
 *  two states.  In the latter, the @c if block tests if @c a is @c false and again the @c else
 *  block gets the other two states.  So, in any case, if @c a is unknown, you'll end up in the
 *  @c else block.
 *
 *  If you want to test for the three different cases in an @c if/else construction, your code will
 *  typically look like the following:
 *
 *  @code
 *  if (a)
 *  {
 *      foo(); // called if a is true
 *  }
 *  else if (!a)
 *  {
 *      bar(); // called if a is false
 *  }
 *  else
 *  {
 *      fun(); // called if a is unknown
 *  }
 *  @endcode
 *
 *  This is equivalent to:
 *
 *  @code
 *  switch (a.state())
 *  {
 *  case TriBool::sTrue:
 *      foo();
 *      break;
 *  case TriBool::sFalse:
 *      bar();
 *      break;
 *  default:
 *      LASS_ASSERT(a.state() == TriBool::sUnknown);
 *      fun();
 *      break;
 *  }
 *  @endcode
 *
 *  @warning a common mistake is to test if a TriBool @c a is in a unknown state by comparing it to
 *           the constant @c unknown.  You might expect it to yield @c true if @c a is indeed,
 *           unknown, but it will always return @c unknown instead.  After all, it cannot know if
 *           both sides are equal, since at least one is unknown (@c unknown itself).  To test if
 *           @c a TriBool is indeed unknown, you have to use the method @c TriBool::isUnknown().
 *
 *  @code
 *  TriBool a;
 *
 *  if (a == unknown)
 *  {
 *      // BAD! unreachable code, the test will never yield true.
 *  }
 *
 *  if (a.isUnknown())
 *  {
 *      // GOOD! this code will be reached if the state of a is unknown.
 *  }
 *  @endcode
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_TRI_BOOL_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_TRI_BOOL_H

#include "num_common.h"

namespace lass
{
namespace num
{

class LASS_DLL TriBool
{
public:

	enum State
	{
		sFalse,
		sTrue,
		sUnknown
	};

	TriBool(State iState = sUnknown);
	TriBool(bool iBool);

	State state() const;
	State& state();

	TriBool operator!() const;
	explicit operator bool() const;

	bool isTrue() const;
	bool isFalse() const;
	bool isUnknown() const;

private:

	State state_;
};

LASS_DLL TriBool LASS_CALL operator==(TriBool iA, TriBool iB);
LASS_DLL TriBool LASS_CALL operator!=(TriBool iA, TriBool iB);
LASS_DLL TriBool LASS_CALL operator&&(TriBool iA, TriBool iB);
LASS_DLL TriBool LASS_CALL operator||(TriBool iA, TriBool iB);

LASS_DLL std::ostream& LASS_CALL operator<<(std::ostream& ioS, TriBool iB);



/** constant to be used as new keyword like @c true and @c false
 *  @relates TriBool
 */
const TriBool unknown(TriBool::sUnknown);

}

}

#endif

// EOF
