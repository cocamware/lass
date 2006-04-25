#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_CAPSULE_3D_RAY_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_CAPSULE_3D_RAY_3D_INL

#include "capsule_3d_ray_3d.h"
#include "sphere_3d.h"


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
	static Result intersect(const Capsule3D<T>& iCapsule, 
							const Ray3D<T, NormalizingPolicy, PP>& iRay, 
							T& oT, const T& iMinT)
	{
		LineSegment3D<T> l(iCapsule.axis());

		typedef Vector3D<T> TVector;
		typedef typename TVector::TValue TValue;
		typedef typename TVector::TParam TParam;
		typedef typename TVector::TNumTraits TNumTraits;
		typedef Point3D<T> TPoint;
		typedef Sphere3D<T> TSphere;

		const TVector direction(l.vector().normal());

		const TVector v1 = iRay.direction() - direction * dot(iRay.direction(), direction);
		const TValue a = v1.squaredNorm();
		if(a == 0)
		{
			TValue t = l.t(iRay.support());
			const TValue squaredDistance((l.tail() - iRay.project(l.tail())).squaredNorm());
			const TValue squaredRadius(num::sqr(iCapsule.radius()));
			if(squaredDistance  <= squaredRadius)
			{
				const TValue t1 = iRay.t(l.tail() - num::sqrt(squaredRadius - squaredDistance) * l.vector().normal());
				const TValue t2 = iRay.t(l.head() + num::sqrt(squaredRadius - squaredDistance) * l.vector().normal());
				if(t1 > iMinT)
				{
					if(t2 > iMinT)
					{
						oT = t1 < t2 ? t1 : t2;
						return rOne;
					}
					oT = t1;
					return rOne;
				}
				if(t2 > iMinT)
				{
					oT = t2;
					return rOne;
				}
			}
			return rNone;
		}

		const TVector temp = iRay.support() - l.tail();
		const TVector v2 = temp - direction * dot(temp,direction);

		// at² + bt + c == 0 infinite cylinder
		
		const TValue b = dot(v1,v2);
		const TValue c = v2.squaredNorm() - num::sqr(iCapsule.radius());

		const TValue discriminant = num::sqr(b) - a*c;

		if(discriminant > TNumTraits::zero)
		{
			const TValue sqrtD = num::sqrt(discriminant);
			const TValue invA = num::inv(a); 
			
			const TValue t1 = (-b - sqrtD)*invA;
			if( t1 > iMinT)
			{
				const TValue test1 = l.t(iRay.point(t1));
				if(test1 >= 0 && test1 <= 1)
				{
					oT = t1;
					return rOne;
				}
				const TSphere sphere((test1<0) ? l.tail():l.head(), iCapsule.radius());
				TParam tSphere;
				if(prim::intersect(sphere, iRay, tSphere, iMinT) == rOne)
				{
					oT = tSphere;
					return rOne;
				}
				return rNone;
			}

			const TValue t2 = (-b + sqrtD)*invA;
			if(t2 > iMinT)
			{
			if(iCapsule.contains(iRay.point(iMinT)))
				{
					const TValue test2 = l.t(iRay.point(t2));
					if(test2 >= 0 && test2 <= 1)
					{
						oT = t2;
						return rOne;
					}

					const TSphere sphere(test2<0 ? l.tail():l.head(), iCapsule.radius());
					TParam tSphere;
					prim::intersect(sphere, iRay, tSphere, iMinT);
					TValue tempMinT = tSphere;
					prim::intersect(sphere, iRay, tSphere, tempMinT);
					if(tSphere > iMinT)
					{
						oT = tSphere;
						return rOne;
					}
					return rNone;
				}

				const TSphere headSphere(l.head(), iCapsule.radius());
				const TSphere tailSphere(l.tail(), iCapsule.radius());
				TParam tHeadSphere;
				TParam tTailSphere;
				if(prim::intersect(headSphere, iRay, tHeadSphere, iMinT) == rOne)
				{
					if(prim::intersect(tailSphere, iRay, tTailSphere, iMinT) == rOne)
					{
						oT = std::min(tTailSphere, tHeadSphere);
						return rOne;
					}
					oT = tHeadSphere;
					return rOne;
				}
				if(prim::intersect(tailSphere, iRay, tTailSphere, iMinT) == rOne)
				{
					oT = tTailSphere;
					return rOne;
				}
				const TValue test2 = l.t(iRay.point(t2));
				if(test2 >= 0 && test2 <= 1)
				{
					oT = t2;
					return rOne;
				}
				return rNone;
			}
			return rNone;
		}
		
		if (discriminant == TNumTraits::zero)
		{
			const TValue t = -b/a;
			if (t > iMinT)
			{
				TValue test = l.t(iRay.point(t));
				if(test >= 0 && test <= 1)
				{
					oT = t;
					return rOne;
				}
			}
		}
		return rNone;
	}
};

}

template<typename T, class NP, class PP> inline
Result intersect(const Capsule3D<T>& iCapsule, 
				 const Ray3D<T, NP, PP>& iRay, 
				 T& oT, const T& iMinT)
{
	return impl::CapsuleRay<NP>::intersect(iCapsule, iRay, oT, iMinT);
}
}
}
#endif

