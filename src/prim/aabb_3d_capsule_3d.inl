#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_CAPSULE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_CAPSULE_3D_INL
#pragma once

#include "prim_common.h"
#include "aabb_3d_capsule_3d.h"

namespace lass
{
namespace prim
{

/** determine axis aligned bounding box of a 3D capsule
 *  @relates Aabb3D
 */
template <typename T> 
Aabb3D<T> aabb(const Capsule3D<T>& iCapsule)
{
	const typename Capsule3D<T>::TLineSegment& l = iCapsule.axis_();
	const typename Capsule3D<T>::TValue r = iCapsule.radius();
	Vector3D<T> v = l.vector();
	const Vector3D<T> extent(sign(v)*r, sign(v)*r, sign(v)*r);
	return Aabb3D<T>(l.head() - extent, l.tail() + extent);
}
}

}

#endif
