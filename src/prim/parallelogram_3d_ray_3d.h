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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PARALLELOGRAM_3D_RAY_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PARALLELOGRAM_3D_RAY_3D_H

#include "prim_common.h"
#include "parallelogram_3d.h"
#include "ray_3d.h"
#include "../num/floating_point_consistency.h"

namespace lass
{
namespace prim
{

/** Find the intersection of a ray and a parallelogram by their parameter t on the ray.
 *  @relates lass::prim::Ray3D
 *  @relates lass::prim::Parallelogram3D
 *
 *  A maximum of two possible intersections with t > 0.
 *
 *  @param parallelogram [in] the parallelogram
 *  @param ray [in] the ray
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a t > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       a intersection with @a t > @a tMin is found
 *							@a t is assigned.
 */
template<typename T, class NP, class PP> inline
Result intersect(
		const Parallelogram3D<T>& parallelogram, const Ray3D<T, NP, PP>& ray, 
		T& t, const T& tMin = ())
{
	T u;
	T v;
	return intersect(parallelogram, ray, u, v, t, tMin);
}




/** Find the intersection of a ray and a parallelogram by their parameter t on the ray
 *  and it's coordinates (u,v) on the parallelogram.
 *  @relates lass::prim::Ray3D
 *  @relates lass::prim::Parallelogram3D
 *
 *  A maximum of two possible intersections with t > 0.
 *
 *  @param parallelogram [in] the parallelogram
 *  @param ray [in] the ray
 *  @param oU [out] the parameter of the intersection point on the parallelogram.
 *  @param oV [out] the parameter of the intersection point on the parallelogram.
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a t > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       a intersection with @a t > @a tMin is found
 *							@a t is assigned.
 *  
 *  @par Algorithm, based on ray/triangle intersection from:
 *  @arg Tomas Möller and Ben Trumbore.
 *	"Fast, minimum storage ray-triangle intersection."
 *	Journal of Graphics Tools, 2(1):21--28, 1997.
 *	http://www.graphics.cornell.edu/pubs/1997/MT97.html
 */
template<typename T, class NP, class PP>
Result intersect(
		const Parallelogram3D<T>& parallelogram, const Ray3D<T, NP, PP>& ray, 
		T& u, T& v, T& t, const T& tMin = T())
{
	typedef typename Parallelogram3D<T>::TVector TVector;
	typedef typename Parallelogram3D<T>::TValue TValue;
	typedef typename Parallelogram3D<T>::TNumTraits TNumTraits;
	typedef num::Consistent<T> TConsistent;

	const TVector pvec = cross(ray.direction(), parallelogram.sizeV());
	
	const TValue det = dot(pvec, parallelogram.sizeU());
	if (det == TNumTraits::zero)
	{
		return rNone;
	}
	const TValue invDet = num::inv(det);

	const TVector tvec = ray.support() - parallelogram.support();
	const TValue uCandidate = dot(tvec, pvec) * invDet;
	if (uCandidate < TNumTraits::zero || uCandidate > TNumTraits::one)
	{
		return rNone;
	}

	const TVector qvec = cross(tvec, parallelogram.sizeU());
	const TValue vCandidate = dot(ray.direction(), qvec) * invDet;
	if (vCandidate < TNumTraits::zero || vCandidate > TNumTraits::one)
	{
		return rNone;
	}

	const TConsistent tCandidate = dot(parallelogram.sizeV(), qvec) * invDet;
	if (tCandidate <= tMin)
	{
		return rNone;
	}

	u = uCandidate;
	v = vCandidate;
	t = tCandidate.value();
	return rOne;
}

}

}

#endif

// EOF
