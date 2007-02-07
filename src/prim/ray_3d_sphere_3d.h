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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_SPHERE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_SPHERE_3D_H

#include "prim_common.h"
#include "ray_3d.h"
#include "sphere_3d.h"
#include "../num/basic_ops.h"
#include "../num/floating_point_consistency.h"

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
		static Result intersect(
				const Sphere3D<T>& sphere, const Ray3D<T, NormalizingPolicy, PP>& ray, 
				T& t, const T& tMin)
		{
			typedef Vector3D<T> TVector;
			typedef typename TVector::TValue TValue;
			typedef typename TVector::TNumTraits TNumTraits;
			typedef num::Consistent<T> TConsistent;

			const TVector cs = ray.support() - sphere.center();

			// at² + bt + c == 0
			const TValue a = ray.direction().squaredNorm();
			const TValue b = dot(cs, ray.direction());
			const TValue c = cs.squaredNorm() - num::sqr(sphere.radius());

			const TValue discriminant = num::sqr(b) - a * c;

			if (discriminant > TNumTraits::zero)
			{
				const TValue sqrtD = num::sqrt(discriminant);
				const TValue invA = num::inv(a);
				const TConsistent t1 = (-b - sqrtD) * invA;
				if (t1 > tMin)
				{
					t = t1.value();
					return rOne;
				}
				const TConsistent t2 = (-b + sqrtD) * invA;
				if (t2 > tMin)
				{
					t = t2.value();
					return rOne;
				}
			}
			else if (discriminant == TNumTraits::zero)
			{
				const TConsistent t1 = -b / a;
				if (t1 > tMin)
				{
					t = t1.value();
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
		static Result intersect(const Sphere3D<T>& sphere, 
								const Ray3D<T, Normalized, PP>& ray, 
								T& t, const T& tMin)
		{
			typedef Vector3D<T> TVector;
			typedef typename TVector::TValue TValue;
			typedef typename TVector::TNumTraits TNumTraits;
			typedef num::Consistent<T> TConsistent;

			typedef typename num::DoublePrecision<T>::Type TDouble;
			typedef Vector3D<TDouble> TVectorDouble;

			const TVectorDouble cs(ray.support() - sphere.center());

			// at² + bt + c == 0
			//const TValue a = 1;
			const TDouble b = dot(cs, TVectorDouble(ray.direction()));
			const TDouble c = cs.squaredNorm() - num::sqr(TDouble(sphere.radius()));

			const TDouble discriminant = num::sqr(b) - c;

			if (discriminant > TNumTraits::zero)
			{
				const TDouble sqrtD = num::sqrt(discriminant);
				const TConsistent t1 = static_cast<TValue>(-b - sqrtD);
				if (t1 > tMin)
				{
					t = t1.value();
					return rOne;
				}
				const TConsistent t2 = static_cast<TValue>(-b + sqrtD);
				if (t2 > tMin)
				{
					t = t2.value();
					return rOne;
				}
			}
			else if (discriminant == TNumTraits::zero)
			{
				const TConsistent t1 = static_cast<TValue>(-b);
				if (t1 > tMin)
				{
					t = t1.value();
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
 *  @param sphere [in] the sphere
 *  @param ray [in] the ray
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a t > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       a intersection with @a t > @a tMin is found
 *							@a t is assigned.
 *
 *  http://flipcode.dxbug.com/wiki/index.php?title=Line-Sphere_%28Collision%29
 */
template<typename T, class NP, class PP> inline
Result intersect(
		const Sphere3D<T>& sphere, const Ray3D<T, NP, PP>& ray, 
		T& t, const T& tMin = T())
{
#ifdef NDEBUG
	return impl::RaySphere<NP>::intersect(sphere, ray, t, tMin);
#else
	const Result result = impl::RaySphere<NP>::intersect(sphere, ray, t, tMin);
	LASS_ASSERT(t > tMin || result == rNone);
	return result;
#endif
}

}
}

#endif

// EOF
