#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_CAPSULE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_CAPSULE_3D_H

#include "prim_common.h"
#include "aabb_3d.h"
#include "capsule_3d.h"

namespace lass
{

namespace prim
{

template <typename T> 
Aabb3D<T> aabb(const Capsule3D<T>& iCapsule);

}

}

#include "aabb_3d_capsule_3d.inl"

#endif