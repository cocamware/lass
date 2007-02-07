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
 *  @return @arg rNone      no intersections > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       exactly one intersection > @a tMin found
 *                          @a t represents it.
 *          @arg rInfinite  infinite many intersections found (segment is coincident with plane),
 *                          @a t is not assigned.
 *          @arg rInvalid   @a plane or @a iRay is invalid, no intersection.
 *                          @a t is not assigned.
 */
template<typename T, class EPPlane, class NPPlane, class PPRay>
Result intersect(
		const Plane3D<T, EPPlane, NPPlane>& plane, const LineSegment3D<T, PPRay>& lineSegment,
		T& t, const T& tMin = T())
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
