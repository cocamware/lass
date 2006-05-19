#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_CAPSULE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_CAPSULE_3D_INL

#include "test_common.h"

#include "../prim/capsule_3d.h"
#include "../prim/ray_3d.h"
#include "../num/random.h"

namespace lass
{
namespace test
{

template<typename T>
void testPrimCapsule3D()
{
	typedef prim::Capsule3D<T> TCapsule;
	typedef prim::Point3D<T> TPoint;
	typedef prim::LineSegment3D<T> TLineSegment;
	typedef prim::Vector3D<T> TVector;
	typedef prim::Ray3D<T> TRay;
	typedef num::NumTraits<T> TNumTraits;
	typedef prim::Aabb3D<T> TAabb;

	const T tolerance = 1e-5f;
	const unsigned n = 100000;

	const TPoint origin;

	TCapsule capsule;
	LASS_TEST_CHECK(capsule.isValid());
	LASS_TEST_CHECK_EQUAL(capsule.axis().head(), origin);
	LASS_TEST_CHECK_EQUAL(capsule.axis().tail(), origin);
	LASS_TEST_CHECK_EQUAL(capsule.radius(), 0);
	LASS_TEST_CHECK_EQUAL(capsule.area(), 0);
	LASS_TEST_CHECK_EQUAL(capsule.volume(), 0);

	const TPoint tail(-4, 0, 6);
	const TPoint head(10, 2, 1);
	const TLineSegment axis(tail, head);
	const T radius = 2;

	capsule = TCapsule(axis, radius);
	LASS_TEST_CHECK(capsule.isValid());
	LASS_TEST_CHECK_EQUAL(capsule.axis().head(), head);
	LASS_TEST_CHECK_EQUAL(capsule.axis().tail(), tail);
	LASS_TEST_CHECK_EQUAL(capsule.radius(), radius);
	LASS_TEST_CHECK_EQUAL(capsule.area(), T(2) * TNumTraits::pi * radius * axis.length() + T(4) * TNumTraits::pi * num::sqr(radius));

	num::RandomMT19937 generator;

	TAabb a = aabb(capsule);
	a.grow(radius);
	TRay testRay;
	T t;
	for (unsigned i = 0; i < n; ++i)
	{
		testRay = TRay(a.random(generator), TVector::random(generator));
		if(intersect(capsule, testRay, t) == prim::rOne)
		{
            LASS_TEST_CHECK_CLOSE( capsule.axis().squaredDistance(testRay.point(t)), num::sqr(capsule.radius()), tolerance); 
			LASS_TEST_CHECK(squaredDistance(capsule.axis(), testRay) <= num::sqr(capsule.radius())); 
		}
		else
		{
			LASS_TEST_CHECK(squaredDistance(capsule.axis(), testRay) > num::sqr(capsule.radius())); 
		}
	}
}
}
}


#endif


