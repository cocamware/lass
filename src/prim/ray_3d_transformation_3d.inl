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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_TRANSFORMATION_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_TRANSFORMATION_3D_INL
#pragma once

#include "prim_common.h"
#include "ray_3d_transformation_3d.h"

namespace lass
{
namespace prim
{
namespace impl
{

template <typename NP> struct RayParameterRescaler;

template <>
struct RayParameterRescaler<prim::Unnormalized>
{
	// unnormalized directions don't alter parameter: same t is same point
	template <typename T, typename VectorType> 
	static void rescale(T& ioRayParameter, const VectorType& iNewRayDirection) 
	{ 
	}
};

template <>
struct RayParameterRescaler<prim::Normalized>
{
	// normalized ray's compress parameters because of renormalisation of direction.
	// to get same point, scale parameter by norm before renormalisation.
	template <typename T, typename VectorType> 
	static void rescale(T& ioRayParameter, const VectorType& iNewRayDirection) 
	{
		ioRayParameter *= iNewRayDirection.norm();
	}
};

}



/** apply transformation to ray
 *  @relates Transformation3D
 */
template<typename T, class NP, class PP>
Ray3D<T, NP, PP> transform(const Ray3D<T, NP, PP>& iSubject, 
						   const Transformation3D<T>& iTransformation)
{
	return Ray3D<T, NP, PP>(
		transform(iSubject.support(), iTransformation),
		transform(iSubject.direction(), iTransformation));
}



/** apply transformation to ray, and rescale a parameter to represent same point
 *
 *  @param ioParameterOfPointOnRay
 *		- INPUT & OUTPUT
 *		- if direction gets renormalized, then a point on ray is no longer represented
 *		  by same parameter.  Adjust parameter @a ioParameterOfPointOnRay to fix this.
 *		- If you fill in 1 as @a ioParameterOfPointOnRay, you can use the result to
 *		  rescale multiple parameters by multiplying it.
 *		- <tt>transform(ray, transf).point(t') == transform(ray.point(t), transf)</tt>
 *
 *  @relates Transformation3D
 */
template<typename T, class NP, class PP>
Ray3D<T, NP, PP> transform(const Ray3D<T, NP, PP>& iSubject, 
						   const Transformation3D<T>& iTransformation,
						   T& ioParameterOfPointOnRay)
{
	typedef Ray3D<T, NP, PP> TRay;
	const typename TRay::TPoint support = transform(iSubject.support(), iTransformation);
	const typename TRay::TVector direction = transform(iSubject.direction(), iTransformation);
	
	impl::RayParameterRescaler<NP>::rescale(ioParameterOfPointOnRay, direction);

	return Ray3D<T, NP, PP>(
		transform(iSubject.support(), iTransformation),
		transform(iSubject.direction(), iTransformation));
}

}

}

#endif
