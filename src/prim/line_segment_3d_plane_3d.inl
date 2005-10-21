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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_PLANE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_PLANE_3D_INL

#include "line_segment_3d_plane_3d.h"

namespace lass
{
namespace prim
{

/** Find the intersection of a plane and line segment by their parameter t on the ray.
 *  @relates lass::prim::Plane3D
 *  @relates lass::prim::LineSegment3D
 *
 *  @param iPlane [in] the plane
 *  @param iSegment [in] the line segment
 *  @param oT [out] the parameter of the intersection point > @a iMinT.
 *  @param iMinT [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections > @a iMinT found
 *                          @a oT is not assigned.
 *          @arg rOne       exactly one intersection > @a iMinT found
 *                          @a oT represents it.
 *          @arg rInfinite  infinite many intersections found (segment is coincident with plane),
 *                          @a oT is not assigned.
 *          @arg rInvalid   @a iPlane or @a iRay is invalid, no intersection.
 *                          @a oT is not assigned.
 */
template<typename T, class EPPlane, class NPPlane, class PPRay>
Result intersect(const Plane3D<T, EPPlane, NPPlane>& iPlane,
				 const LineSegment3D<T, PPRay>& iSegment,
				 T& oT, const T& iMinT)
{
	typedef typename Vector3D<T>::TValue TValue;
	typedef typename Vector3D<T>::TNumTraits TNumTraits;

	if (!iPlane.isValid())
	{
		return rInvalid;
	}

    const TValue eTail = iPlane.equation(iSegment.tail());
    const TValue eHead = iPlane.equation(iSegment.head());

	if (eTail == eHead)
    {
		return eTail == TNumTraits::zero ? rInfinite : rNone;
    }
    else
    {
        // find candidate of intersection.
		const TValue t = eTail / (eTail - eHead);
		if (t >= TNumTraits::zero && t <= TNumTraits::one)
		{
			oT = t;
			return rOne;
		}
		return rNone;
    }
}

/** reflect a linesegment in a plane.
 *  @relates lass::prim::LineSegment3D
 *  @relates lass::prim::Plane3D
 *
 *  @param iPlane [in] the reflection plane
 *  @param iSegment [in] the line segment to be reflected
 *  @return the reflected line segment
 */
template <typename T, class EP, class NP, class PP>
LineSegment3D<T, PP> reflect(const Plane3D<T, EP, NP>& iPlane, const LineSegment3D<T, PP>& iSegment)
{
	return LineSegment3D<T, PP>(iPlane.reflect(iSegment.tail()), iPlane.reflect(iSegment.head()));
}

/** project a linesegment on a plane.
 *  @relates lass::prim::LineSegment3D
 *  @relates lass::prim::Plane3D
 *
 *  @param iPlane [in] the projection plane
 *  @param iSegment [in] the line segment to be projected
 *  @return the projected line segment
 */
template <typename T, class EP, class NP, class PP>
LineSegment3D<T, PP> project(const Plane3D<T, EP, NP>& iPlane, const LineSegment3D<T, PP>& iSegment)
{
	return LineSegment3D<T, PP>(iPlane.project(iSegment.tail()), iPlane.project(iSegment.head()));
}

}

}

#endif

// EOF
