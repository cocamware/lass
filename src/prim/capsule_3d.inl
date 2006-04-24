#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_CAPSULE3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_CAPSULE3D_INL


#include "capsule_3d.h"
#include "../num/floating_point_comparison.h"

namespace lass
{
namespace prim
{

template <typename T, class PP>
Capsule3D<T, PP>::Capsule3D():
	axis_(),
	radius_(TNumTraits::zero)
{
}

template <typename T, class PP>
Capsule3D<T, PP>::Capsule3D(const TLineSegment& iAxis, TParam iRadius):
	axis_(iAxis),
	radius_(iRadius)
{
}

template <typename T, class PP>
const typename Capsule3D<T, PP>::TLineSegment&
Capsule3D<T, PP>::axis() const
{
	return axis_;
}

template <typename T, class PP>
typename Capsule3D<T, PP>::TLineSegment&
Capsule3D<T,PP>::axis()
{
	return axis_;
}

template<typename T, class PP>
typename Capsule3D<T, PP>::TConstReference
Capsule3D<T, PP>::radius() const
{
	return radius_;
}

template<typename T, class PP>
typename Capsule3D<T, PP>::TReference
Capsule3D<T, PP>::radius()
{
	return radius_;
}

template<typename T, class PP>
const typename Capsule3D<T, PP>::TValue
Capsule3D<T, PP>::area() const
{
	return T(2) * TNumTraits::pi * radius_ * axis_.length() + T(4) * TNumTraits::pi * num::sqr(radius_);
}

template<typename T, class PP>
const typename Capsule3D<T, PP>::TValue
Capsule3D<T, PP>::volume() const
{
	return TNumTraits::pi * num::sqr(radius_) * axis_.length() + T(4)/T(3)*TNumTraits::pi * num::sqr(radius_) * radius_;
}

template <typename T, class PP>
const bool Capsule3D<T, PP>::isValid() const
{
	return radius_ >= TNumTraits::zero;
}

template <typename T, class PP>
const bool Capsule3D<T, PP>::contains(const TPoint &iPoint) const
{
	return (axis_.distance(iPoint) <= radius_);
}

}
}

#endif

