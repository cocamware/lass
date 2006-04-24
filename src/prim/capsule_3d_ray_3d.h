#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_CAPSULE_3D_RAY_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_CAPSULE_3D_RAY_3D_H

#include "prim_common.h"
#include "ray_3d.h"
#include "capsule_3d.h"

namespace lass
{

namespace prim
{

template<typename T, class NP, class PP>
Result intersect(const Capsule3D<T>& iCapsule, const Ray3D<T, NP, PP>& iRay, 
				 T& oT, const T& iMinT = T());

}

}

#include "capsule_3d_ray_3d.inl"

#endif

