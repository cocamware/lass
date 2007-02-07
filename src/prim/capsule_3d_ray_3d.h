#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_CAPSULE_3D_RAY_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_CAPSULE_3D_RAY_3D_H

#include "prim_common.h"
#include "ray_3d.h"
#include "capsule_3d.h"
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
	struct CapsuleRay
	{
		template <typename T, class PP>
		static Result intersect(
				const Capsule3D<T>& capsule, const Ray3D<T, NormalizingPolicy, PP>& ray, 
				T& t, const T& tMin)
		{
			LineSegment3D<T> l(capsule.axis());

			typedef Vector3D<T> TVector;
			typedef typename TVector::TValue TValue;
			typedef typename TVector::TParam TParam;
			typedef typename TVector::TNumTraits TNumTraits;
			typedef Point3D<T> TPoint;
			typedef Sphere3D<T> TSphere;
			typedef num::Consistent<T> TConsistent;

			const TVector direction(l.vector().normal());

			const TVector v1 = ray.direction() - direction * dot(ray.direction(), direction);
			const TValue a = v1.squaredNorm();
			if(a == 0) //parallel axes
			{
				const TValue squaredDistance((l.tail() - ray.project(l.tail())).squaredNorm());
				const TValue squaredRadius(num::sqr(capsule.radius()));
				if(squaredDistance  <= squaredRadius)
				{
					const TConsistent t1 = ray.t(l.tail() - 
						num::sqrt(squaredRadius - squaredDistance) * l.vector().normal());
					const TConsistent t2 = ray.t(l.head() + 
						num::sqrt(squaredRadius - squaredDistance) * l.vector().normal());
					if(t1 > tMin)
					{
						if(t2 > tMin)
						{
							t = t1 < t2 ? t1.value() : t2.value();
							return rOne;
						}
						t = t1.value();
						return rOne;
					}
					if(t2 > tMin)
					{
						t = t2.value();
						return rOne;
					}
				}
				return rNone;
			}

			const TVector temp = ray.support() - l.tail();
			const TVector v2 = temp - direction * dot(temp,direction);

			// at² + bt + c == 0 infinite cylinder
			
			const TValue b = dot(v1,v2);
			const TValue c = v2.squaredNorm() - num::sqr(capsule.radius());

			const TValue discriminant = num::sqr(b) - a*c;

			if(discriminant > TNumTraits::zero)
			{
				const TValue sqrtD = num::sqrt(discriminant);
				const TValue invA = num::inv(a); 
				
				const TConsistent t1 = (-b - sqrtD)*invA;
				if( t1 > tMin)
				{
					const TValue test1 = l.t(ray.point(t1.value()));
					if(test1 >= 0 && test1 <= 1)
					{
						t = t1.value();
						return rOne;
					}
					const TSphere sphere((test1<0) ? l.tail():l.head(), capsule.radius());
					TParam tSphere;
					if(prim::intersect(sphere, ray, tSphere, tMin) == rOne)
					{
						t = tSphere;
						return rOne;
					}
					return rNone;
				}

				const TConsistent t2 = (-b + sqrtD)*invA;
				if(t2 > tMin)
				{
					if(capsule.contains(ray.point(tMin)))
					{
						const TValue test2 = l.t(ray.point(t2.value()));
						if(test2 >= 0 && test2 <= 1)
						{
							t = t2.value();
							return rOne;
						}

						const TSphere sphere(test2<0 ? l.tail():l.head(), capsule.radius());
						TParam tSphere;
						prim::intersect(sphere, ray, tSphere, tMin);
						TValue tempMinT = tSphere;
						prim::intersect(sphere, ray, tSphere, tempMinT);
						if(tSphere > tMin)
						{
							t = tSphere;
							return rOne;
						}
						return rNone;
					}

					const TSphere headSphere(l.head(), capsule.radius());
					const TSphere tailSphere(l.tail(), capsule.radius());
					TValue tHeadSphere;
					TValue tTailSphere;
					if(prim::intersect(headSphere, ray, tHeadSphere, tMin) == rOne)
					{
						if(prim::intersect(tailSphere, ray, tTailSphere, tMin) == rOne)
						{
							t = std::min(tTailSphere, tHeadSphere);
							LASS_ASSERT(t > tMin);
							return rOne;
						}
						LASS_ASSERT(tHeadSphere > tMin);
						t = tHeadSphere;
						return rOne;
					}
					if(prim::intersect(tailSphere, ray, tTailSphere, tMin) == rOne)
					{
						LASS_ASSERT(tTailSphere > tMin);
						t = tTailSphere;
						return rOne;
					}
					const TValue test2 = l.t(ray.point(t2.value()));
					if(test2 >= 0 && test2 <= 1)
					{
						t = t2.value();
						return rOne;
					}
					return rNone;
				}
				return rNone;
			}
			
			if (discriminant == TNumTraits::zero)
			{
				const TConsistent tCandidate = -b/a;
				if (tCandidate > tMin)
				{
					TValue test = l.t(ray.point(tCandidate.value()));
					if(test >= 0 && test <= 1)
					{
						t = tCandidate.value();
						return rOne;
					}
				}
			}
			return rNone;
		}
	};
}

template<typename T, class NP, class PP> inline
Result intersect(
		const Capsule3D<T>& capsule, const Ray3D<T, NP, PP>& ray, 
		T& t, const T& tMin = T())
{
	return impl::CapsuleRay<NP>::intersect(capsule, ray, t, tMin);
}

}
}

#endif

// EOF
