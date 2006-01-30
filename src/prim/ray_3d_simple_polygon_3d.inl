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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_SIMPLE_POLYGON_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_SIMPLE_POLYGON_3D_INL

#include "ray_3d_simple_polygon_3d.h"

namespace lass
{
namespace prim
{

/** Find the intersection of a ray and a simple polygon by their parameter t on the ray.
 *  @relates lass::prim::Ray3D
 *  @relates lass::prim::SimplePolygon3D
 *
 *  @param iPolygon [in] the simple polygon
 *  @param iRay [in] the ray
 *  @param oT [out] the parameter of the intersection point > @a iMinT.
 *  @param iMinT [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a oT > @a iMinT found
 *                          @a oT is not assigned.
 *          @arg rOne       a intersection with @a oT > @a iMinT is found
 *							@a oT is assigned.
 */
template<typename T, class EP1, class NP1, class NP2, class PP2>
Result intersect(const SimplePolygon3D<T, EP1, NP1>& iPolygon, 
				 const Ray3D<T, NP2, PP2>& iRay, 
				 T& oT, const T& iMinT)
{
	typedef Point2D<T> TPoint;
	typedef typename TPoint::TValue TValue;

	TValue t;
	if (intersect(iPolygon.plane(), iRay, t, iMinT) == rOne)
	{
		if (iPolygon.contains(iRay.point(t)))
		{
			oT = t;
			return rOne;
		}
	}
	return rNone;
}

}

}

#endif

// EOF
