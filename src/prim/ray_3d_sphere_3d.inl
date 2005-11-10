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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_SPHERE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_SPHERE_3D_INL

#include "ray_3d_sphere_3d.h"
#include "../num/basic_ops.h"

namespace lass
{
namespace prim
{
namespace impl
{

/** @internal
 */
template <class NormalizingPolicy>
struct RaySphere
{
	template <typename T, class PP>
	static Result intersect(const Sphere3D<T>& iSphere, 
							const Ray3D<T, NormalizingPolicy, PP>& iRay, 
							T& oT, const T& iMinT)
	{
		typedef Vector3D<T> TVector;
		typedef typename TVector::TValue TValue;
		typedef typename TVector::TNumTraits TNumTraits;

		const TVector cs = iRay.support() - iSphere.center();

		// at² + bt + c == 0
		const TValue a = iRay.direction().squaredNorm();
		const TValue b = dot(cs, iRay.direction());
		const TValue c = cs.squaredNorm() - num::sqr(iSphere.radius());

		const TValue discriminant = num::sqr(b) - a * c;

		if (discriminant > TNumTraits::zero)
		{
			const TValue sqrtD = num::sqrt(discriminant);
			const TValue invA = num::inv(a);
			const TValue t1 = (-b - sqrtD) * invA;
			if (t1 > iMinT)
			{
				oT = t1;
				return rOne;
			}
			const TValue t2 = (-b + sqrtD) * invA;
			if (t2 > iMinT)
			{
				oT = t2;
				return rOne;
			}
		}
		else if (discriminant == TNumTraits::zero)
		{
			const TValue t = -b / a;
			if (t > iMinT)
			{
				oT = t;
				return rOne;
			}
		}
		return rNone;
	}
};

/** @internal
 */
template <>
struct RaySphere<Normalized>
{
	template <typename T, class PP>
	static Result intersect(const Sphere3D<T>& iSphere, 
							const Ray3D<T, Normalized, PP>& iRay, 
							T& oT, const T& iMinT)
	{
		typedef Vector3D<T> TVector;
		typedef typename TVector::TValue TValue;
		typedef typename TVector::TNumTraits TNumTraits;

		typedef typename num::DoublePrecision<T>::Type TDouble;
		typedef Vector3D<TDouble> TVectorDouble;

		const TVectorDouble cs(iRay.support() - iSphere.center());

		// at² + bt + c == 0
		//const TValue a = 1;
		const TDouble b = dot(cs, TVectorDouble(iRay.direction()));
		const TDouble c = cs.squaredNorm() - num::sqr(TDouble(iSphere.radius()));

		const TDouble discriminant = num::sqr(b) - c;

		if (discriminant > TNumTraits::zero)
		{
			const TDouble sqrtD = num::sqrt(discriminant);
			const TDouble t1 = (-b - sqrtD);
			if (t1 > iMinT)
			{
				oT = static_cast<TValue>(t1);
				return rOne;
			}
			const TDouble t2 = (-b + sqrtD);
			if (t2 > iMinT)
			{
				oT = static_cast<TValue>(t2);
				return rOne;
			}
		}
		else if (discriminant == TNumTraits::zero)
		{
			const TDouble t = -b;
			if (t > iMinT)
			{
				oT = static_cast<TValue>(t);
				return rOne;
			}
		}
		return rNone;
	}
};

}




/** Find the intersection of a ray and a sphere by their parameter t on the ray.
 *  @relates lass::prim::Ray3D
 *  @relates lass::prim:Sphere3D
 *
 *  @param iSphere [in] the sphere
 *  @param iRay [in] the ray
 *  @param oT [out] the parameter of the intersection point > @a iMinT.
 *  @param iMinT [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a oT > @a iMinT found
 *                          @a oT is not assigned.
 *          @arg rOne       a intersection with @a oT > @a iMinT is found
 *							@a oT is assigned.
 */
template<typename T, class NP, class PP> inline
Result intersect(const Sphere3D<T>& iSphere, 
				 const Ray3D<T, NP, PP>& iRay, 
				 T& oT, const T& iMinT)
{
	return impl::RaySphere<NP>::intersect(iSphere, iRay, oT, iMinT);
}

}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------
