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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_RAY_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_RAY_2D_H

#include "prim_common.h"
#include "line_2d.h"
#include "ray_2d.h"
#include "../num/floating_point_consistency.h"

namespace lass
{
namespace prim
{

/** Find the intersection of a line and ray by their parameter t on the ray.
 *  @relates lass::prim::Line2D
 *  @relates lass::prim::Ray2D
 *
 *  @param line [in] the line
 *  @param ray [in] the ray
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       exactly one intersection > @a tMin found
 *                          @a t represents it.
 *          @arg rInfinite  infinite many intersections found (ray is coincident with plane),
 *                          @a t is not assigned.
 *          @arg rInvalid   @a iPlane or @a ray is invalid, no intersection.
 *                          @a t is not assigned.
 */
template<typename T, class EP1, class NP1, class NP2, class PP2>
Result intersect(const Line2D<T, EP1, NP1>& line,
				 const Ray2D<T, NP2, PP2>& ray,
				 T& t, const T& tMin = T())
{
	typedef typename Vector2D<T>::TValue TValue;
	typedef typename Vector2D<T>::TNumTraits TNumTraits;
	typedef num::Consistent<T> TConsistent;

	if (!line.isValid() || !ray.isValid())
	{
		return rInvalid;
	}

	const TValue nd = perpDot(line.normal(), ray.direction());
	if (nd == TNumTraits::zero)
	{
		// ray is parallel to plane, but is it also coincident?
		const Side side = line.classify(ray.support());
		LASS_ASSERT(side == sFront || side == sSurface || side == sBack);
		return side == sSurface ? rInfinite : rNone;
	}
	else
	{
		const TConsistent tCandidate = -line.equation(ray.support()) / nd;
		LASS_ASSERT(!num::isNaN(tCandidate));
		if (tCandidate > tMin)
		{
			t = tCandidate.value();
			return rOne;
		}
		return rNone;
	}
}

}
}

#endif
