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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PARALLELOGRAM_3D_RAY_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PARALLELOGRAM_3D_RAY_3D_INL

#include "parallelogram_3d_ray_3d.h"
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
 *  @param iParallelogram [in] the parallelogram
 *  @param iRay [in] the ray
 *  @param oT [out] the parameter of the intersection point > @a iMinT.
 *  @param iMinT [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a oT > @a iMinT found
 *                          @a oT is not assigned.
 *          @arg rOne       a intersection with @a oT > @a iMinT is found
 *							@a oT is assigned.
 */
template<typename T, class NP, class PP> inline
Result intersect(const Parallelogram3D<T>& iParallelogram, 
				 const Ray3D<T, NP, PP>& iRay, 
				 T& oT, const T& iMinT)
{
	T u;
	T v;
	return intersect(iParallelogram, iRay, u, v, oT, iMinT);
}




/** Find the intersection of a ray and a parallelogram by their parameter t on the ray
 *  and it's coordinates (u,v) on the parallelogram.
 *  @relates lass::prim::Ray3D
 *  @relates lass::prim::Parallelogram3D
 *
 *  A maximum of two possible intersections with t > 0.
 *
 *  @param iParallelogram [in] the parallelogram
 *  @param iRay [in] the ray
 *  @param oU [out] the parameter of the intersection point on the parallelogram.
 *  @param oV [out] the parameter of the intersection point on the parallelogram.
 *  @param oT [out] the parameter of the intersection point > @a iMinT.
 *  @param iMinT [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a oT > @a iMinT found
 *                          @a oT is not assigned.
 *          @arg rOne       a intersection with @a oT > @a iMinT is found
 *							@a oT is assigned.
 *  
 *  @par Algorithm, based on ray/triangle intersection from:
 *  @arg Tomas Möller and Ben Trumbore.
 *	"Fast, minimum storage ray-triangle intersection."
 *	Journal of Graphics Tools, 2(1):21--28, 1997.
 *	http://www.graphics.cornell.edu/pubs/1997/MT97.html
 */
template<typename T, class NP, class PP>
Result intersect(const Parallelogram3D<T>& iParallelogram, 
				 const Ray3D<T, NP, PP>& iRay, 
				 T& oU, T& oV, T& oT, const T& iMinT)
{
	typedef typename Parallelogram3D<T>::TVector TVector;
	typedef typename Parallelogram3D<T>::TValue TValue;
	typedef typename Parallelogram3D<T>::TNumTraits TNumTraits;
	typedef num::Consistent<T> TConsistent;

	const TVector pvec = cross(iRay.direction(), iParallelogram.sizeV());
	
	const TValue det = dot(pvec, iParallelogram.sizeU());
	if (det == TNumTraits::zero)
	{
		return rNone;
	}
	const TValue invDet = num::inv(det);

	const TVector tvec = iRay.support() - iParallelogram.support();
	const TValue u = dot(tvec, pvec) * invDet;
	if (u < TNumTraits::zero || u > TNumTraits::one)
	{
		return rNone;
	}

	const TVector qvec = cross(tvec, iParallelogram.sizeU());
	const T v = dot(iRay.direction(), qvec) * invDet;
	if (v < TNumTraits::zero || v > TNumTraits::one)
	{
		return rNone;
	}

	const TConsistent t = dot(iParallelogram.sizeV(), qvec) * invDet;
	if (t <= iMinT)
	{
		return rNone;
	}

	oU = u;
	oV = v;
	oT = t.value();
	return rOne;
}

}

}

#endif

// EOF
