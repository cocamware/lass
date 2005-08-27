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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_TRIANGLE_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_TRIANGLE_2D_INL

#include "ray_2d_triangle_2d.h"

namespace lass
{
namespace prim
{

namespace impl
{

template <typename Point, typename Vector, typename T> inline
bool intersectTriangle2DEdge(const Point& iSupport, const Vector& iDirection, const Point& iTail, const Point& iHead, T& oTNear, T& oTFar)
{
	typedef typename Point::TNumTraits TNumTraits;

	const Vector edge = iHead - iTail;
	const T denominator = perpDot(iDirection, -edge);
	if (denominator != TNumTraits::zero)
	{
		const Vector difference = iTail - iSupport;
		const T tRay = perpDot(difference, -edge) / denominator;
		const T tEdge = perpDot(iDirection, difference) / denominator;
		if (tRay > TNumTraits::zero && tEdge >= TNumTraits::zero && tEdge <= TNumTraits::one)
		{
			oTNear = std::min(oTNear, tRay);
			oTFar = std::max(oTFar, tRay);
			return true;
		}
	}
	return false;
}

}



/** Find the intersection of a ray and a triangle by their parameter t on the ray.
 *  @relates lass::prim::Ray2D
 *  @relates lass::prim::Sphere2D
 *
 *  A maximum of two possible intersections with t > 0.
 *
 *  @param iRay the ray
 *  @param iTriangle the triangle
 *  @param oTNear the parameter of the intersection point with the smallest t > 0, if any.
 *  @param oTFar the parameter of the intersection point with the largest t > 0, if any.
 *  @return - rNone: no intersections with t > 0 found, oTNear and oTFar is invalid.
 *          - rOne: exactly one intersection with t > 0, oTNear represents it, oTFar is invalid.
 *          - rTwo: two intersections with t > 0 found, oTNear and oTFar represent them
 */
template<typename T, class NP, class PP>
Result intersect(const Ray2D<T, NP, PP>& iRay, const Triangle2D<T>& iTriangle, T& oTNear, T& oTFar)
{
	typedef Point2D<T> TPoint;
	typedef Vector2D<T> TVector;
	typedef typename TVector::TValue TValue;
	typedef typename TVector::TNumTraits TNumTraits;

	const TPoint& support = iRay.support();
	const TVector& direction = iRay.direction();

	TValue tNear = TNumTraits::infinity;
	TValue tFar = TNumTraits::zero;
	bool good = false;
	good |= impl::intersectTriangle2DEdge(support, direction, iTriangle[0], iTriangle[1], tNear, tFar);
	good |= impl::intersectTriangle2DEdge(support, direction, iTriangle[1], iTriangle[2], tNear, tFar);
	good |= impl::intersectTriangle2DEdge(support, direction, iTriangle[2], iTriangle[0], tNear, tFar);

	if (good)
	{
		if (tNear == tFar)
		{
			oTNear = tNear;
			return rOne;
		}
		else
		{
			oTNear = tNear;
			oTFar = tFar;
			return rTwo;
		}
	}
	return rNone;
}

}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------
