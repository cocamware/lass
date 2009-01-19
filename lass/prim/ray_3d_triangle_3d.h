/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_TRIANGLE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_TRIANGLE_3D_H

#include "prim_common.h"
#include "ray_3d.h"
#include "triangle_3d.h"
#include "impl/intersect_triangle_3d.h"

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
