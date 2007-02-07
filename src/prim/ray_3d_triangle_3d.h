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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_TRIANGLE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_TRIANGLE_3D_H

#include "prim_common.h"
#include "ray_3d.h"
#include "triangle_3d.h"

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
 *  @param triangle [in] the triangle
 *  @param ray [in] the ray
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a t > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       a intersection with @a t > @a tMin is found
 *							@a t is assigned.
 *
 *  @note MOLLER T. and TRUMBORE B. <q>Fast, Minimum Storage Ray/Triangle Intersection</q>,
 *		Journal of Graphics Tools, 2(1), 21-28 (1997).
 *		http://www.graphics.cornell.edu/pubs/1997/MT97.html
 */
template<typename T, class NP, class PP> inline
Result intersect(
		const Triangle3D<T>& triangle, const Ray3D<T, NP, PP>& ray, 
		T& t, const T& tMin = T())
{
	T u;
	T v;
	return impl::intersectTriangle3D(
		triangle[0], triangle[1] - triangle[0], triangle[2] - triangle[0],
		ray.support(), ray.direction(), u, v, t, tMin);
}




/** Find the intersection of a ray and a triangle by their parameter t on the ray
 *  and it's coordinates (u,v) on the triangle.
 *  @relates lass::prim::Ray3D
 *  @relates lass::prim::Triangle3D
 *
 *  A maximum of two possible intersections with t > 0.
 *
 *  @param triangle [in] the triangle
 *  @param ray [in] the ray
 *  @param u [out] the parameter of the intersection point on the triangle.
 *  @param v [out] the parameter of the intersection point on the triangle.
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a t > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       a intersection with @a t > @a tMin is found
 *							@a t is assigned.
 *
 *  @note MOLLER T. and TRUMBORE B. <q>Fast, Minimum Storage Ray/Triangle Intersection</q>,
 *		Journal of Graphics Tools, 2(1), 21-28 (1997).
 *		http://www.graphics.cornell.edu/pubs/1997/MT97.html
 */
template<typename T, class NP, class PP> inline
Result intersect(
		const Triangle3D<T>& triangle, const Ray3D<T, NP, PP>& ray, 
		T& u, T& v, T& t, const T& tMin = T())
{
	return impl::intersectTriangle3D(
		triangle[0], triangle[1] - triangle[0], triangle[2] - triangle[0],
		ray.support(), ray.direction(), u, v, t, tMin);
}


}
}

#endif

// EOF
