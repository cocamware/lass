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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_RAY_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_RAY_2D_H

#include "prim_common.h"
#include "aabb_2d.h"
#include "ray_2d.h"
#include "impl/aabb_slab.h"

namespace lass
{
namespace prim
{

/** Find the intersection of an AABB and ray by their parameter t on the ray.
 *  @relates lass::prim::Aabb2D
 *  @relates lass::prim::Ray2D
 *
 *  @param aabb [in] the AABB
 *  @param ray [in] the ray
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a t > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       a intersection with @a t > @a tMin is found
 *							@a t is assigned.
 */
template<typename T, typename MMPAabb, typename NPRay, typename PPRay>
Result intersect(
		const Aabb2D<T, MMPAabb>& aabb, const Ray2D<T, NPRay, PPRay>& ray, 
		T& t, const T& tMin = T())
{
	if (aabb.isEmpty())
	{
		return rNone;
	}

	typedef num::NumTraits<T> TNumTraits;
	typedef Point2D<T> TPoint;
	typedef Vector2D<T> TVector;

	const TPoint& min = aabb.min();
	const TPoint& max = aabb.max();
	const TPoint& support = ray.support();
	const TVector& direction = ray.direction();

	T tNear = tMin;
	T tFar = TNumTraits::infinity;
	bool good = true;
	good &= impl::intersectSlab(min[0], max[0], support[0], direction[0], tNear, tFar);
	good &= impl::intersectSlab(min[1], max[1], support[1], direction[1], tNear, tFar);

	if (good)
	{
		if (tNear > tMin)
		{
			t = tNear;
			return rOne;
		}
		if (tFar > tMin)
		{
			t = tFar;
			return rOne;
		}
	}
	return rNone;
}



/** Find the intersection of an AABB and ray by their parameter t on the ray.
 *  @relates lass::prim::Aabb2D
 *  @relates lass::prim::Ray2D
 *
 *  This version accepts a precomputed reciprocal of the ray direction
 *  (1/x, 1/y, 1/z). This is useful when you do many intersections
 *  with the same ray, so the reciprocal needs only to be computed
 *  once.
 *
 *  @param aabb [in] the AABB
 *  @param ray [in] the ray
 *  @param invDirection [in] the reciprocal of the ray direction
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a t > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       a intersection with @a t > @a tMin is found
 *							@a t is assigned.
 */
template<typename T, typename MMPAabb, typename NPRay, typename PPRay>
Result intersect(
		const Aabb2D<T, MMPAabb>& aabb, const Ray2D<T, NPRay, PPRay>& ray, 
		const Vector2D<T>& invDirection,
		T& t, const T& tMin = T())
{
	if (aabb.isEmpty())
	{
		return rNone;
	}

	typedef num::NumTraits<T> TNumTraits;
	typedef Point2D<T> TPoint;
	typedef Vector2D<T> TVector;

	const TPoint& min = aabb.min();
	const TPoint& max = aabb.max();
	const TPoint& support = ray.support();
	const TVector& direction = ray.direction();

	T tNear = tMin;
	T tFar = TNumTraits::infinity;
	bool good = true;
	good &= impl::intersectSlab(min[0], max[0], support[0], direction[0], invDirection[0], tNear, tFar);
	good &= impl::intersectSlab(min[1], max[1], support[1], direction[1], invDirection[1], tNear, tFar);

	if (good)
	{
		if (tNear > tMin)
		{
			t = tNear;
			return rOne;
		}
		if (tFar > tMin)
		{
			t = tFar;
			return rOne;
		}
	}
	return rNone;
}

}

}

#endif

// EOF
