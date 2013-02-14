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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_PLANE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_PLANE_3D_H

#include "prim_common.h"
#include "line_segment_3d.h"
#include "plane_3d.h"

namespace lass
{
namespace prim
{

/** Find the intersection of a plane and line segment by their parameter t on the ray.
 *  @relates lass::prim::Plane3D
 *  @relates lass::prim::LineSegment3D
 *
 *  @param plane [in] the plane
 *  @param lineSegment [in] the line segment
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections found
 *                          @a t is not assigned.
 *          @arg rOne       exactly one intersection found
 *                          @a t represents it.
 *          @arg rInfinite  infinite many intersections found (segment is coincident with plane),
 *                          @a t is not assigned.
 *          @arg rInvalid   @a plane or @a lineSegment is invalid, no intersection.
 *                          @a t is not assigned.
 */
template<typename T, class EPPlane, class NPPlane, class PPRay>
Result intersect(
		const Plane3D<T, EPPlane, NPPlane>& plane, const LineSegment3D<T, PPRay>& lineSegment,
		T& t)
{
	typedef typename Vector3D<T>::TValue TValue;
	typedef typename Vector3D<T>::TNumTraits TNumTraits;

	if (!plane.isValid())
	{
		return rInvalid;
	}

	const TValue eTail = plane.equation(lineSegment.tail());
	const TValue eHead = plane.equation(lineSegment.head());

	if (eTail == eHead)
	{
		return eTail == TNumTraits::zero ? rInfinite : rNone;
	}
	else
	{
		// find candidate of intersection.
		const TValue tCandidate = eTail / (eTail - eHead);
		if (tCandidate >= TNumTraits::zero && tCandidate <= TNumTraits::one)
		{
			t = tCandidate;
			return rOne;
		}
		return rNone;
	}
}

/** reflect a linesegment in a plane.
 *  @relates lass::prim::LineSegment3D
 *  @relates lass::prim::Plane3D
 *
 *  @param plane [in] the reflection plane
 *  @param lineSegment [in] the line segment to be reflected
 *  @return the reflected line segment
 */
template <typename T, class EP, class NP, class PP>
LineSegment3D<T, PP> reflect(
		const Plane3D<T, EP, NP>& plane, const LineSegment3D<T, PP>& lineSegment)
{
	return LineSegment3D<T, PP>(plane.reflect(lineSegment.tail()), plane.reflect(lineSegment.head()));
}

/** project a linesegment on a plane.
 *  @relates lass::prim::LineSegment3D
 *  @relates lass::prim::Plane3D
 *
 *  @param plane [in] the projection plane
 *  @param lineSegment [in] the line segment to be projected
 *  @return the projected line segment
 */
template <typename T, class EP, class NP, class PP>
LineSegment3D<T, PP> project(
		const Plane3D<T, EP, NP>& plane, const LineSegment3D<T, PP>& lineSegment)
{
	return LineSegment3D<T, PP>(plane.project(lineSegment.tail()), plane.project(lineSegment.head()));
}

}
}

#endif

// EOF
