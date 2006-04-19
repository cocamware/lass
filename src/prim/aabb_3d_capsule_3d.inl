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
	const typename Capsule3D<T>::TLineSegment& l = iCapsule.axis();
	const typename Capsule3D<T>::TValue r = iCapsule.radius();
	const Point3D<T> tail = l.tail();
	const Point3D<T> head = l.head();
	const Point3D<T> min(tail.x < head.x ? tail.x - r : head.x - r,
						 tail.y < head.y ? tail.y - r : head.y - r,
						 tail.z < head.z ? tail.z - r : head.z - r);
	const Point3D<T> max(tail.x > head.x ? tail.x + r : head.x + r,
						 tail.y > head.y ? tail.y + r : head.y + r,
						 tail.z > head.z ? tail.z + r : head.z + r);
	return Aabb3D<T>(min, max);
}
}

}

#endif
