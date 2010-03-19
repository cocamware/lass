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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_SIMPLE_POLYGON_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_SIMPLE_POLYGON_3D_H

#include "prim_common.h"
#include "aabb_3d.h"
#include "simple_polygon_3d.h"

namespace lass
{
namespace prim
{

/** determine axis aligned bounding box of a 3D simple polygon
 *  @relates Aabb3D
 */
template <typename T, class EP, class NP> 
Aabb3D<T> aabb(const SimplePolygon3D<T, EP, NP>& polygon)
{
	Aabb3D<T> result;
	const size_t n = polygon.size();
	for (size_t i = 0; i < n; ++i)
	{
		result += polygon[i];
	}
	return result;
}

/** Clip a plane to an AABB and get a polygon.
 *  @relates lass::prim::Aabb3D
 *  @sa lass::prim::Plane3D
 *  @sa lass::prim::SimplePolygon3D
 *
 *  @param box [in] the Aabb to clip to
 *  @param plane [in] the plane to be clipped
 *  @return the clipped polygon.
 */
template <typename T, class EP, class NP, class MMP>
SimplePolygon3D<T, EP, NP> clip(const Aabb3D<T, MMP>& box, const Plane3D<T, EP, NP>& plane)
{
	typedef Plane3D<T, EP, NP> TPlane;
	typedef SimplePolygon3D<T, EP, NP> TPolygon;
	typedef Point3D<T> TPoint;
	typedef typename TPoint::TVector TVector;
	typedef typename TPoint::TValue TValue;
	TPolygon poly(plane);

	const TPoint& min = box.min();
	const TPoint& max = box.max();
	
	TVector normal;
	TValue d;
	plane.getCartesian(normal, d);

	TVector m;
	switch (plane.majorAxis())
	{
		case 0: // x
		{
			TPoint p[4] = 
			{
				TPoint(0.0, min.y, min.z), TPoint(0.0, max.y, min.z),
				TPoint(0.0, max.y, max.z), TPoint(0.0, min.y, max.z)
			};
			if (plane.normal().x < 0)
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
			if (plane.normal().y < 0)
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
			if (plane.normal().z < 0)
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
			LASS_ENFORCE_UNREACHABLE;
	};

	// clip against those 'm' borders
	return clip(TPlane(m, min), clip(TPlane(-m, max), poly));
}



/** Clip a polygon to an AABB .
 *  @relates lass::prim::Aabb3D
 *  @sa lass::prim::SimplePolygon3D
 *
 *  @param box [in] the Aabb to clip to
 *  @param polygon [in] the polygon to be clipped
 *  @return the clipped polygon.
 */
template <typename T, class EP, class NP, class MMP>
SimplePolygon3D<T, EP, NP> clip(const Aabb3D<T, MMP>& box, const SimplePolygon3D<T, EP, NP>& polygon)
{
	typedef SimplePolygon3D<T, EP, NP> TPoly;
	typedef typename TPoly::TPlane TPlane;
	typedef typename TPoly::TPoint TPoint;
	typedef typename TPoly::TVector TVector;

	const TPoint& min = box.min();
	const TPoint& max = box.max();

	const TVector i(1, 0, 0);
	TPoly poly = clip(TPlane(i, -min.x), clip(TPlane(-i, max.x), polygon));
	const TVector j(0, 1, 0);
	poly = clip(TPlane(j, -min.y), clip(TPlane(-j, max.y), poly));
	const TVector k(0, 0, 1);
	return clip(TPlane(k, -min.z), clip(TPlane(-k, max.z), poly));
}



}
}

#endif

// EOF
