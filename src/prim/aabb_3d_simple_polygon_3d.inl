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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_SIMPLE_POLYGON_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_SIMPLE_POLYGON_3D_INL

#include "prim_common.h"
#include "aabb_3d_simple_polygon_3d.h"

namespace lass
{
namespace prim
{

/** determine axis aligned bounding box of a 3D simple polygon
 *  @relates Aabb3D
 */
template <typename T, class EP, class NP> 
Aabb3D<T> aabb(const SimplePolygon3D<T, EP, NP>& iPolygon)
{
	Aabb3D<T> result;
	const size_t n = iPolygon.size();
	for (size_t i = 0; i < n; ++i)
	{
		result += iPolygon[i];
	}
	return result;
}

/** Clip a plane to an AABB and get a polygon.
 *  @relates lass::prim::Aabb3D
 *  @relates lass::prim::Plane3D
 *  @relates lass::prim::SimplePolygon3D
 *
 *  @param iAabb [in] the Aabb to clip to
 *  @param iPlane [in] the plane to be clipped
 *  @return the clipped polygon.
 */
template <typename T, class EP, class NP, class MMP>
SimplePolygon3D<T, EP, NP> clip(const Aabb3D<T, MMP>& iAabb, 
		const Plane3D<T, EP, NP>& iPlane)
{
	typedef Plane3D<T, EP, NP> TPlane;
	typedef SimplePolygon3D<T, EP, NP> TPolygon;
	typedef Point3D<T> TPoint;
	typedef typename TPoint::TVector TVector;
	typedef typename TPoint::TValue TValue;
	TPolygon poly(iPlane);

	const TPoint& min = iAabb.min();
	const TPoint& max = iAabb.max();
	
	TVector normal;
	TValue d;
	iPlane.getCartesian(normal, d);

	TVector m;
	switch (iPlane.majorAxis())
	{
		case 0: // x
		{
        		TPoint p[4] = 
			{
				TPoint(0.0, min.y, min.z), TPoint(0.0, max.y, min.z),
				TPoint(0.0, max.y, max.z), TPoint(0.0, min.y, max.z)
			};
			if (iPlane.normal().x < 0)
			{
				std::reverse(p, p + 4);
			}
			for (size_t i = 0; i < 4; ++i)
			{
				p[i].x = -(dot(normal, p[i].position()) + d) / normal.x;
				poly.add(p[i]);
			}
			m = TVector(1.0, 0.0, 0.0);
			break;
		}
		case 1: // y
		{
			TPoint p[4] = 
			{
				TPoint(min.x, 0.0, min.z), TPoint(min.x, 0.0, max.z),
				TPoint(max.x, 0.0, max.z), TPoint(max.x, 0.0, min.z)
			};
			if (iPlane.normal().y < 0)
			{
				std::reverse(p, p + 4);
			}
			for (size_t i = 0; i < 4; ++i)
			{
				p[i].y = -(dot(normal, p[i].position()) + d) / normal.y;
				poly.add(p[i]);
			}
			m = TVector(0.0, 1.0, 0.0);
			break;
		}
		case 2: // z
		{
			TPoint p[4] = 
			{
				TPoint(min.x, min.y, 0.0), TPoint(max.x, min.y, 0.0),
				TPoint(max.x, max.y, 0.0), TPoint(min.x, max.y, 0.0)
			};
			if (iPlane.normal().z < 0)
			{
				std::reverse(p, p + 4);
			}
			for (size_t i = 0; i < 4; ++i)
			{
				p[i].z = -(dot(normal, p[i].position()) + d) / normal.z;
				poly.add(p[i]);
			}
			m = TVector(0.0, 0.0, 1.0);
		}
		default:
			LASS_THROW("unreachable code");
	};

	// clip against those 'm' borders
	return clip(TPlane(m, min), clip(TPlane(-m, max), poly));
}       


}

}

#endif
