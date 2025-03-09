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
 *	Copyright (C) 2025 the Initial Developer.
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

#include "test_common.h"

#include "../lass/prim/ray_3d.h"
#include "../lass/prim/sphere_3d.h"
#include "../lass/meta/is_same.h"
#include "../lass/num/distribution_transformations.h"
#include "../lass/util/clock.h"
#include "../lass/util/stop_watch.h"

#include <random>

namespace
{

using namespace lass;

template <typename T>
T relError(T a, T b)
{
	return num::abs(a - b) / std::max(num::abs(a), num::abs(b));
}


template <typename T, typename RNG>
prim::Point3D<T> sampleSphere(prim::Sphere3D<T>, RNG&& rng)
{
	auto uniform = std::uniform_real_distribution<T>();
	const prim::Point2D<T> sample(uniform(rng), uniform(rng));
	[[maybe_unused]] T pdf;
	return num::uniformSphere(sample, pdf);
}

}

namespace lass
{
namespace test
{

template <typename T, typename NormalizationPolicy, int stretch = 1>
void testPrimRay3DSphere3DIntersectionSmall()
{
	static_assert(stretch == 1 || std::is_same_v<NormalizationPolicy, prim::Unnormalized>);

	using TValue = T;
	using TNumTraits = num::NumTraits<T>;
	using TPoint = prim::Point3D<T>;
	using TVector = prim::Vector3D<T>;
	using TRay = prim::Ray3D<T, NormalizationPolicy>;
	using TSphere = prim::Sphere3D<T>;

	const TPoint center(-0x1.de5af80000000p+9, -0x1.ca3b200000000p+8, 0x1.5389140000000p+9);
	const TValue radius = 0x1.e8b8280000000p+1;
	const TPoint support(0x1.fbc3500000000p+9, 0x1.edfdc00000000p+9, 0x1.828c600000000p+6);
	const TVector direction(-0x1.91530c0000000p-1, -0x1.26e4840000000p-1, 0x1.db54220000000p-3);

	const TSphere sphere(center, radius);
	const TRay ray(support, direction * stretch);

	// correct for the re-normalized direction, or the stretched unnormalized direction
	const TValue scale = std::is_same_v<NormalizationPolicy, prim::Normalized>
		? direction.norm() / ray.direction().norm() // re-normalized
		: TNumTraits::one / stretch; // stretched unnormalized

	// these reference values were generated with test_prim_ray_3d_sphere_3d.py
	const TValue t1Expected = static_cast<TValue>(0x1.39e9856062562p+11 * scale);
	const TValue t2Expected = static_cast<TValue>(0x1.3a99fd4c44718p+11 * scale);

	constexpr TValue tol = TNumTraits::gamma(8);

	TValue t1;
	const TValue tMin = 0;
	auto result = intersect(sphere, ray, t1, tMin);
	LASS_TEST_CHECK_EQUAL(result, prim::rOne);
	LASS_TEST_CHECK_CLOSE(t1, t1Expected, tol);
	if (relError(t1, t1Expected) > tol)
	{
		LASS_CERR << "rel err=" << relError(t1, t1Expected) << std::endl;
	}

	// this was the value from the failing assertion error
	const TValue tNear = static_cast<TValue>(0x1.39e8580000000p+11 * scale);
	LASS_TEST_CHECK(t1 >= tNear * (1 - 1e-6f));
	
	TValue t2;
	result = intersect(sphere, ray, t2, t1);
	LASS_TEST_CHECK_EQUAL(result, prim::rOne);
	LASS_TEST_CHECK_CLOSE(t2, t2Expected, tol);
	if (relError(t2, t2Expected) > tol)
	{
		LASS_CERR << "rel err=" << relError(t2, t2Expected) << std::endl;
	}
}



template <typename T, typename NormalizationPolicy, int stretch = 1>
void testPrimRay3DSphere3DIntersectionLarge()
{
	static_assert(stretch == 1 || std::is_same_v<NormalizationPolicy, prim::Unnormalized>);

	using TValue = T;
	using TNumTraits = num::NumTraits<T>;
	using TPoint = prim::Point3D<T>;
	using TVector = prim::Vector3D<T>;
	using TRay = prim::Ray3D<T, NormalizationPolicy>;
	using TSphere = prim::Sphere3D<T>;

	const TPoint center(-0x1.de5af80000000p+9, -0x1.ca3b200000000p+8, 0x1.5389140000000p+9);
	const TValue radius = 0x1.38b8280000000p+11;
	const TPoint support(0x1.fbc3500000000p+9, 0x1.edfdc00000000p+9, 0x1.828c600000000p+6);
	const TVector direction(-0x1.91530c0000000p-1, -0x1.26e4840000000p-1, 0x1.db54220000000p-3);

	const TSphere sphere(center, radius);
	const TRay ray(support, direction * stretch);

	// correct for the re-normalized direction, or the stretched unnormalized direction
	const TValue scale = std::is_same_v<NormalizationPolicy, prim::Normalized>
		? direction.norm() / ray.direction().norm() // re-normalized
		: TNumTraits::one / stretch; // stretched unnormalized

	// these reference values were generated with test_prim_ray_3d_sphere_3d.py
	const TValue t1Expected = static_cast<TValue>(0x1.89a5cc662338bp+3 * scale);
	const TValue t2Expected = static_cast<TValue>(0x1.397cee7020523p+12 * scale);

	constexpr TValue tol = TNumTraits::gamma(8);

	TValue t1;
	const TValue tMin = 0;
	auto result = intersect(sphere, ray, t1, tMin);
	LASS_TEST_CHECK_EQUAL(result, prim::rOne);
	LASS_TEST_CHECK_CLOSE(t1, t1Expected, tol);
	if (relError(t1, t1Expected) > tol)
	{
		LASS_CERR << "rel err=" << relError(t1, t1Expected) << std::endl;
	}

	TValue t2;
	result = intersect(sphere, ray, t2, t1);
	LASS_TEST_CHECK_EQUAL(result, prim::rOne);
	LASS_TEST_CHECK_CLOSE(t2, t2Expected, tol);
	if (relError(t2, t2Expected) > tol)
	{
		LASS_CERR << "rel err=" << relError(t2, t2Expected) << std::endl;
	}
}



template <typename T, typename NormalizationPolicy, int stretch = 1>
void testPrimRay3DSphere3DIntersectionPerformance()
{
	static_assert(stretch == 1 || std::is_same_v<NormalizationPolicy, prim::Unnormalized>);

	using TValue = T;
	using TPoint = prim::Point3D<T>;
	using TRay = prim::Ray3D<T, NormalizationPolicy>;
	using TSphere = prim::Sphere3D<T>;

	const TPoint center(1, 2, 3);
	const TValue radius = 1;
	TSphere sphere(center, radius);


	constexpr size_t n = 100000;

	std::mt19937_64 rng;
	std::vector<TRay> rays;
	TSphere raySphere(center, 2 * radius);
	for (size_t i = 0; i < n; ++i)
	{
		const TPoint support = sampleSphere(raySphere, rng);
		const TPoint lookAt = sampleSphere(raySphere, rng);
		rays.emplace_back(support, lookAt);
	}

	util::Clock clock;
	util::StopWatch stopWatch(clock);

	stopWatch.restart();
	TValue total = 0;
	for (auto ray : rays)
	{
		TValue t;
		if (prim::intersect(sphere, ray, t) != prim::rNone)
		{
			total += t;
		}
	}
	const auto time = stopWatch.stop();
	std::cout << typeid(TRay).name() << ": " << time << " (" << total << ")\n";
}



TUnitTest test_prim_ray_3d_sphere_3d()
{
	return TUnitTest
	{
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionSmall<float, prim::Normalized>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionSmall<float, prim::Unnormalized, 1>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionSmall<float, prim::Unnormalized, 2>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionSmall<double, prim::Normalized>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionSmall<double, prim::Unnormalized, 1>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionSmall<double, prim::Unnormalized, 2>)),

		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionLarge<float, prim::Normalized>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionLarge<float, prim::Unnormalized, 1>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionLarge<float, prim::Unnormalized, 2>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionLarge<double, prim::Normalized>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionLarge<double, prim::Unnormalized, 1>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionLarge<double, prim::Unnormalized, 2>)),

		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionPerformance<float, prim::Normalized>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionPerformance<float, prim::Unnormalized, 1>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionPerformance<float, prim::Unnormalized, 2>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionPerformance<double, prim::Normalized>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionPerformance<double, prim::Unnormalized, 1>)),
		LASS_TEST_CASE((testPrimRay3DSphere3DIntersectionPerformance<double, prim::Unnormalized, 2>)),
	};
}

}
}
