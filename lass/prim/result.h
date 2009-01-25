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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RESULT_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RESULT_H

namespace lass
{
namespace prim
{

/** @brief meta information on the result you have from an operation like an intersection ...
 *  @author [BdG]
 *  @date 2003
 *
 *  If you want to find the intersection of two lines 'a' and 'b', you expect a point as result.
 *  However, that's not always true.  'a' and 'b' can be coincident so that all points of
 *  'a' and 'b' are intersection points, or they could be not intersecting at all so that you
 *  don't have an intersection point.  'Result' will tell you this.  In this same example, it will
 *  return rNone if the lines are not intersectiong, rOne if there is exactly one intersection
 *  point, and rInfinite if the lines are coincident.
 *
 *  Unlike lass::util::Side, results can't be OR'ed.  Afterall, what would 'rNone | rOne' mean?
 *
 *  rInvalid is the invalid state of Result, if you encounter this, something is wrong.  Notice the
 *  difference with rNone that is a valid state (although it says there's no answer).
 *
 *  @note rOne does @e not mean there's only one point in the solution.  No, it means there's
 *        exactly one solution possible, and the output parameter contains this solution.
 *        In the case of an AABB this one solution can contain infinite many points.
 */
enum Result
{
	rInvalid    = 0,    /**< 0 is an invalid value, nothing is known. */
	rNone       = 1,    /**< operation has no answer, output arguments are meaningless */
	rOne        = 2,    /**< there's exactly one answer, 1 output argument contains the answer */
	rTwo        = 3,    /**< there are two solutions, 2 output argunemts contain the answers */
	rInfinite   = 4     /**< there are infinite many solutions, output arguments are meaningless */
};

}

}

#endif
