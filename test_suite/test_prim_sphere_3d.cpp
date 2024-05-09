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
 *	Copyright (C) 2004-2024 the Initial Developer.
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

#include "../lass/prim/sphere_3d.h"
#include "../lass/prim/aabb_3d.h"
#include "../lass/num/distribution_transformations.h"

#include <random>
#include <chrono>


#if !LASS_HAVE_DURATION_OPERATOR_OSTREAM

namespace std
{

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const std::chrono::duration<double, std::milli>& msecs)
{
	std::basic_ostringstream<CharT, Traits> s;
	s.flags(os.flags());
	s.imbue(os.getloc());
	s.precision(os.precision());
	s << msecs.count() << "ms";
	return os << s.str();
}

}

#endif


namespace lass
{
namespace test
{

template
<
	typename T
>
void testPrimSphere3D()
{
	typedef prim::Sphere3D<T> TSphere;
	typedef prim::Point3D<T> TPoint;
	typedef num::NumTraits<T> TNumTraits;
	typedef prim::Aabb3D<T> TAabb;
	typedef prim::Point2D<T> TSample;

	using TMillies = std::chrono::duration<double, std::milli>;

	const TPoint origin;

	TSphere sphere;
	LASS_TEST_CHECK(sphere.isValid());
	LASS_TEST_CHECK_EQUAL(sphere.center(), origin);
	LASS_TEST_CHECK_EQUAL(sphere.radius(), static_cast<T>(0));
	LASS_TEST_CHECK_EQUAL(sphere.area(), static_cast<T>(0));
	LASS_TEST_CHECK_EQUAL(sphere.volume(), static_cast<T>(0));
	LASS_TEST_CHECK_EQUAL(sphere.classify(origin), prim::sSurface);
	LASS_TEST_CHECK_EQUAL(sphere.equation(origin), static_cast<T>(0));
	LASS_TEST_CHECK_EQUAL(sphere.signedDistance(origin), static_cast<T>(0));

	{
		const TPoint center(1, 2, 3);
		const T radius = static_cast<T>(4);

		sphere = TSphere(center, radius);
		LASS_TEST_CHECK(sphere.isValid());
		LASS_TEST_CHECK_EQUAL(sphere.center(), center);
		LASS_TEST_CHECK_EQUAL(sphere.radius(), radius);
		LASS_TEST_CHECK_EQUAL(sphere.area(), T(4) * TNumTraits::pi * num::sqr(radius));
		LASS_TEST_CHECK_EQUAL(sphere.volume(), T(4) / T(3) * TNumTraits::pi * num::sqr(radius) * radius);
		LASS_TEST_CHECK_EQUAL(sphere.classify(center), prim::sInside);
		LASS_TEST_CHECK_EQUAL(sphere.equation(center), -num::sqr(radius));
		LASS_TEST_CHECK_EQUAL(sphere.signedDistance(center), -radius);
	}

	{
		// testing boundingSphere
		std::mt19937_64 rng;
		constexpr size_t numTrials = 10000;
		constexpr size_t numPoints = 100;
		constexpr T tolerance = 1e-5f;
		const TAabb box(TPoint(100, 100, 100), TPoint(110, 110, 110));
		std::vector<TPoint> points(numPoints);

		std::chrono::high_resolution_clock clock;
		const auto start = clock.now();
		for (size_t trial = 0; trial < numTrials; ++trial)
		{
			for (size_t k = 0; k < numPoints; ++k)
			{
				points[k] = box.random(rng);
			}

			TSphere bounds = prim::boundingSphere(points);

			// these are not strictly true, but the are given the amount of points we create.
			LASS_TEST_CHECK(box.contains(bounds.center()));
			LASS_TEST_CHECK(bounds.radius() < box.size().norm() * 0.5);

			T maxDistance = 0;
			for (size_t k = 0; k < numPoints; ++k)
			{
				maxDistance = std::max(maxDistance, distance(bounds.center(), points[k]));
			}
			LASS_TEST_CHECK_CLOSE(maxDistance, bounds.radius(), tolerance);
		}
		const auto duration = std::chrono::duration_cast<TMillies>(clock.now() - start);
		LASS_COUT << "points in a box: " << duration << std::endl;
	}

	{
		// testing boundingSphere with points all on a sphere
		std::mt19937_64 rng;
		std::uniform_real_distribution<T> uniform;

		constexpr size_t numTrials = 1000;
		constexpr size_t numPoints = 100;
		constexpr T tolerance = 1e-4f;
		const TAabb box(TPoint(-1, -1, 1), TPoint(1, 1, 1));
		std::vector<TPoint> points(numPoints);

		std::chrono::high_resolution_clock clock;
		const auto start = clock.now();
		for (size_t trial = 0; trial < numTrials; ++trial)
		{
			const TPoint center = box.random(rng);
			const T radius = uniform(rng);
			for (size_t k = 0; k < numPoints; ++k)
			{
				const TSample sample(uniform(rng), uniform(rng));
				T pdf;
				points[k] = center + radius * num::uniformSphere(sample, pdf).position();
			}

			TSphere bounds = prim::boundingSphere(points);

			T maxDistance = 0;
			for (size_t k = 0; k < numPoints; ++k)
			{
				maxDistance = std::max(maxDistance, distance(bounds.center(), points[k]));
			}
			LASS_TEST_CHECK_CLOSE(maxDistance, bounds.radius(), tolerance);

			// should be close to original sphere, but error is higher due to sampling
			LASS_TEST_CHECK_CLOSE(bounds.center().x, center.x, T(1e-2));
			LASS_TEST_CHECK_CLOSE(bounds.center().y, center.y, T(1e-2));
			LASS_TEST_CHECK_CLOSE(bounds.center().z, center.z, T(1e-2));
			LASS_TEST_CHECK_CLOSE(bounds.radius(), radius, T(1e-2));
		}
		const auto duration = std::chrono::duration_cast<TMillies>(clock.now() - start);
		LASS_COUT << "points on a sphere: " << duration << std::endl;
	}

	{
		// testing boundingSphere with points all on a plane
		std::mt19937_64 rng;
		std::uniform_real_distribution<T> uniform;

		constexpr size_t numTrials = 10000;
		constexpr size_t numPoints = 100;
		constexpr T tolerance = 1e-5f;
		const TAabb box(TPoint(-1, -1, 0), TPoint(1, 1, 0));
		std::vector<TPoint> points(numPoints);

		std::chrono::high_resolution_clock clock;
		const auto start = clock.now();
		for (size_t trial = 0; trial < numTrials; ++trial)
		{
			for (size_t k = 0; k < numPoints; ++k)
			{
				points[k] = box.random(rng);
				LASS_TEST_CHECK_EQUAL(points[k].z, T(0));
			}

			TSphere bounds = prim::boundingSphere(points);
			LASS_TEST_CHECK_EQUAL(bounds.center().z, T(0));

			T maxDistance = 0;
			for (size_t k = 0; k < numPoints; ++k)
			{
				maxDistance = std::max(maxDistance, distance(bounds.center(), points[k]));
			}
			LASS_TEST_CHECK_CLOSE(maxDistance, bounds.radius(), tolerance);
		}
		const auto duration = std::chrono::duration_cast<TMillies>(clock.now() - start);
		LASS_COUT << "points on a plane: " << duration << std::endl;
	}
}


TUnitTest test_prim_sphere_3d()
{
	return TUnitTest
	{
		LASS_TEST_CASE((testPrimSphere3D<float>)),
		LASS_TEST_CASE((testPrimSphere3D<double>)),
	};
}

}

}
