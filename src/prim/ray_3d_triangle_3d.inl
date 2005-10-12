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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_TRIANGLE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_TRIANGLE_3D_INL

#include "ray_3d_triangle_3d.h"

namespace lass
{
namespace prim
{


/** Find the intersection of a ray and a triangle by their parameter t on the ray.
 *  @relates lass::prim::Ray3D
 *  @relates lass::prim::Triangle3D
 *
 *  A maximum of two possible intersections with t > 0.
 *
 *  @param iTriangle [in] the triangle
 *  @param iRay [in] the ray
 *  @param oT [out] the parameter of the intersection point > @a iMinT.
 *  @param iMinT [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a oT > @a iMinT found
 *                          @a oT is not assigned.
 *          @arg rOne       a intersection with @a oT > @a iMinT is found
 *							@a oT is assigned.
 *
 *  @note MOLLER T. and TRUMBORE B. <q>Fast, Minimum Storage Ray/Triangle Intersection</q>,
 *		Journal of Graphics Tools, 2(1), 21-28 (1997).
 *		http://www.graphics.cornell.edu/pubs/1997/MT97.html
 */
template<typename T, class NP, class PP>
Result intersect(const Triangle3D<T>& iTriangle, 
				 const Ray3D<T, NP, PP>& iRay, 
				 T& oT, const T& iMinT)
{
	typedef Point3D<T> TPoint;
	typedef Vector3D<T> TVector;
	typedef typename TVector::TValue TValue;
	typedef typename TVector::TNumTraits TNumTraits;

	const TPoint& support = iRay.support();
	const TVector& direction = iRay.direction();

	const TVector edge1 = iTriangle[1] - iTriangle[0];
	const TVector edge2 = iTriangle[2] - iTriangle[0];
	const TVector pvec = cross(direction, edge2);
	const TValue det = dot(pvec, edge1);

	if (det == TNumTraits::zero)
	{
		return rNone;
	}
	const TValue invDet = num::inv(det);

	const TVector tvec = support - iTriangle[0];
	const TValue u = dot(tvec, pvec) * invDet;
	if (u < TNumTraits::zero || u > TNumTraits::one)
	{
		return rNone;
	}

	const TVector qvec = cross(tvec, edge1);
	const TValue v = dot(tvec, qvec) * invDet;
	if (v < TNumTraits::zero || (u + v) > TNumTraits::one)
	{
		return rNone;
	}

	const TValue t = dot(edge2, qvec) * invDet;
	if (t <= iMinT)
	{
		return rNone;
	}

	oT = t;
	return rOne;
}

}

}

#endif

// EOF
