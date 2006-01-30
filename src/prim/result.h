/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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
