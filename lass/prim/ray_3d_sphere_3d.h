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
 *	Copyright (C) 2004-2025 the Initial Developer.
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
			using TValue = T;
			using TVector = Vector3D<TValue>;
			using TNumTraits = typename TVector::TNumTraits;

			using TDouble = std::conditional_t<std::is_same_v<T, float>, double, T>;
			using TVectorDouble = Vector3D<TDouble>;

			const TVector cs = ray.support() - sphere.center();
			const TVector& d = ray.direction();
			const TValue r2 = num::sqr(sphere.radius());

			// at² + bt + c == 0
			const TValue a = d.squaredNorm();
			const TValue b = dot(cs, d);
			const TValue c = static_cast<TValue>(
				static_cast<TVectorDouble>(cs).squaredNorm() -
				num::sqr(static_cast<TDouble>(sphere.radius())));
			// const TValue discriminant = num::sqr(b) - a * c;

			// Hearn and Baker's geometrical reformulation:
			const TVector l = cs - (b / a) * d;
			const TValue discriminant = r2 - l.squaredNorm();

			if (discriminant > TNumTraits::zero)
			{
				// Press et al. Numerical Recipes
				const TValue q = -(b + std::copysign(num::sqrt(a * discriminant), b));
				TValue t1 = c / q;
				TValue t2 = q / a;
				if (t1 > t2)
				{
					std::swap(t1, t2);
				}
				if (t1 > tMin)
				{
					t = t1;
					return rOne;
				}
				if (t2 > tMin)
				{
					t = t2;
					return rOne;
				}
			}
			else if (discriminant == TNumTraits::zero)
			{
				const TValue t1 = - b / a;
				if (t1 > tMin)
				{
					t = t1;
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
			using TValue = T;
			using TVector = Vector3D<TValue>;
			using TNumTraits = typename TVector::TNumTraits;

			using TDouble = std::conditional_t<std::is_same_v<T, float>, double, T>;
			using TVectorDouble = Vector3D<TDouble>;

			const TVector cs = ray.support() - sphere.center();
			const TVector& d = ray.direction();
			const TValue r2 = num::sqr(sphere.radius());

			// at² + bt + c == 0
			// const TValue a = d.squaredNorm() == 1;
			const TValue b = dot(cs, d);
			const TValue c = static_cast<TValue>(
				static_cast<TVectorDouble>(cs).squaredNorm() -
				num::sqr(static_cast<TDouble>(sphere.radius())));
			// const TValue discriminant = num::sqr(b) - c;

			// Hearn and Baker's geometrical reformulation:
			const TVector l = cs - b * d;
			const TValue discriminant = r2 - l.squaredNorm();

			if (discriminant > TNumTraits::zero)
			{
				// Press et al. Numerical Recipes
				const TValue q = -(b + std::copysign(num::sqrt(discriminant), b));
				TValue t1 = c / q;
				TValue t2 = q;
				if (t1 > t2)
				{
					std::swap(t1, t2);
				}
				if (t1 > tMin)
				{
					t = t1;
					return rOne;
				}
				if (t2 > tMin)
				{
					t = t2;
					return rOne;
				}
			}
			else if (discriminant == TNumTraits::zero)
			{
				const TValue t1 = -b;
				if (t1 > tMin)
				{
					t = t1;
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
 *                          @a t is assigned.
 *
 *  For implementation details on numerical precision:
 * 
 *  Haines, E., Günther, J., Akenine-Möller, T. (2019). Precision Improvements for Ray/Sphere Intersection.
 *  In: Haines, E., Akenine-Möller, T. (eds) Ray Tracing Gems. Apress, Berkeley, CA.
 *  https://doi.org/10.1007/978-1-4842-4427-2_7
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
