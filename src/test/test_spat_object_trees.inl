/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_SPAT_OBJECT_TREES_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_SPAT_OBJECT_TREES_INL

#include "test_common.h"
#include "../spat/aabb_tree.h"
#include "../spat/aabp_tree.h"
#include "../meta/select.h"
#include "../num/distribution.h"
#include "../num/random.h"
#include "../prim/aabb_2d.h"
#include "../prim/aabb_3d.h"
#include "../prim/ray_2d.h"
#include "../prim/ray_3d.h"
#include "../prim/sphere_3d.h"
#include "../prim/triangle_2d.h"
#include "../util/clock.h"
#include "../util/stop_watch.h"

namespace lass
{
namespace test
{
namespace object_trees
{

template <typename T, typename RandomGenerator, typename OuputIterator>
OuputIterator generateObjects(const prim::Aabb2D<T>& iBound,
							  T iMaxSize,
							  RandomGenerator& iRandom,
							  size_t iNumber,
							  OuputIterator iObjects)
{
	typedef prim::Aabb2D<T> TAabb;
	typedef prim::Vector2D<T> TVector;
	typedef prim::Point2D<T> TPoint;
	const TVector halfSize = TVector(1, 1) * iMaxSize / 2;
	for (size_t i = 0; i < iNumber; ++i)
	{
		const TPoint center = iBound.random(iRandom);
		const TPoint min = pointwiseMax(iBound.min(), center - halfSize);
		const TPoint max = pointwiseMin(iBound.max(), center + halfSize);
		const TAabb bound(min, max);
		const TPoint a = bound.random(iRandom);
		const TPoint b = bound.random(iRandom);
		const TPoint c = bound.random(iRandom);
		prim::Triangle2D<T> triangle(a, b, c);
		if (triangle.orientation() == prim::oClockWise)
		{
			triangle.flip();
		}
		*iObjects++ = triangle;
	}
	return iObjects;
}



template <typename T, typename RandomGenerator, typename OutputIterator>
OutputIterator generateObjects(const prim::Aabb3D<T>& iBound,
							   T iMaxSize,
							   RandomGenerator& iRandom,
							   size_t iNumber,
							   OutputIterator iObjects)
{
	typedef prim::Point3D<T> TPoint;
	typedef prim::Vector3D<T> TVector;
	for (size_t i = 0; i < iNumber; ++i)
	{
		const TPoint center = iBound.random(iRandom);
		const TVector centerToBound = prim::pointwiseMin(
			center - iBound.min(), iBound.max() - center);
		const T maxRadius = std::min(iMaxSize / 2, std::min(centerToBound.x, centerToBound.y));
		const T radius = num::distributeUniform(iRandom, T(0), maxRadius);
		*iObjects++ = prim::Sphere3D<T>(center, radius);
	}
	return iObjects;
}

}

template <typename T, size_t dim>
void testSpatObjectTrees()
{
	const T tolerance = 1e-5f;

	const T extent = T(1000);
	const T maxSize = T(10);
	const size_t numberOfObjects = 10000;
	// validation
	const size_t numberOfContainValidations = 1000;
	const size_t numberOfIntersectionValidations = 1000;
	// speed tests
	const size_t numberOfContainSpeedTestTargets = 1000;
	const size_t numberOfContainSpeedTestRuns = 10;
	const size_t numberOfIntersectionSpeedTestTargets = 1000;
	const size_t numberOfIntersectionSpeedTestRuns = 10;

	typedef typename meta::Select< meta::Bool<dim == 2>, prim::Triangle2D<T>, prim::Sphere3D<T> >::Type TObject;
	typedef typename meta::Select< meta::Bool<dim == 2>, prim::Aabb2D<T>, prim::Aabb3D<T> >::Type TAabb;
	typedef typename meta::Select< meta::Bool<dim == 2>, prim::Ray2D<T>, prim::Ray3D<T> >::Type TRay;

	typedef typename TObject::TPoint TPoint;
	typedef typename TObject::TVector TVector;
	typedef typename TObject::TValue TValue;
	typedef typename TObject::TNumTraits TNumTraits;

	typedef std::vector<TObject> TObjects;
	typedef spat::DefaultObjectTraits<TObject, TAabb, TRay> TObjectTraits;
	typedef typename TObjectTraits::TObjectIterator TObjectIterator;

	typedef spat::AabbTree<TObject, TObjectTraits, spat::DefaultSplitHeuristics<8> > TAabbTree;
	typedef spat::AabpTree<TObject, TObjectTraits, spat::DefaultSplitHeuristics<2> > TAabpTree;
	
	// set bounds
	//
	TPoint min;
	TPoint max;
	for (size_t i = 0; i < dim; ++i)
	{
		min[i] = -extent;
		max[i] = extent;
	}
	const TAabb bounds(min, max);

	// random generator
	//
	num::RandomMT19937 generator;

	// create test subjects
	//
	TObjects objects;
	object_trees::generateObjects(
		bounds, maxSize, generator, numberOfObjects, std::back_inserter(objects));
	const TObjectIterator objectBegin = &objects[0];
	const TObjectIterator objectEnd = objectBegin + numberOfObjects;

	// create trees
	//
	TAabbTree aabbTree(objectBegin, objectEnd);
	TAabpTree aabpTree(objectBegin, objectEnd);

	typedef std::vector<TObjectIterator> TObjectHits;

	// contain test
	//
	for (unsigned i = 0; i < numberOfContainValidations; ++i)
	{
		TPoint target = bounds.random(generator);
		TObjectHits naiveHits;
		bool naiveContains = false;
		for (TObjectIterator obj = objectBegin; obj != objectEnd; ++obj)
		{
			if (obj->contains(target))
			{
				naiveContains = true;
				naiveHits.push_back(obj);
			}
		}
		std::sort(naiveHits.begin(), naiveHits.end());

		bool aabbContains = aabbTree.contains(target);
		TObjectHits aabbHits;
		aabbTree.find(target, std::back_inserter(aabbHits));
		std::sort(aabbHits.begin(), aabbHits.end());
		LASS_TEST_CHECK_EQUAL(naiveContains, aabbContains);
		LASS_TEST_CHECK_EQUAL(naiveContains, !aabbHits.empty());
		LASS_TEST_CHECK(naiveHits.size() == aabbHits.size() && 
			std::equal(naiveHits.begin(), naiveHits.end(), aabbHits.begin()));

		bool aabpContains = aabpTree.contains(target);
		TObjectHits aabpHits;
		aabpTree.find(target, std::back_inserter(aabpHits));
		std::sort(aabpHits.begin(), aabpHits.end());
		LASS_TEST_CHECK_EQUAL(naiveContains, aabpContains);
		LASS_TEST_CHECK_EQUAL(naiveContains, !aabpHits.empty());
		LASS_TEST_CHECK(naiveHits.size() == aabpHits.size() && 
			std::equal(naiveHits.begin(), naiveHits.end(), aabpHits.begin()));
	}

	// intersection test
	//
	for (unsigned i = 0; i < numberOfIntersectionValidations; ++i)
	{
		TPoint support = bounds.random(generator);
		TVector direction = TVector::random(generator);
		TRay ray(support, direction);

		T naiveT = TNumTraits::infinity;
		TObjectIterator naiveIntersection = objectEnd;
		for (TObjectIterator obj = objectBegin; obj != objectEnd; ++obj)
		{
			T t;
			const prim::Result result = prim::intersect(*obj, ray, t);
			if (result != prim::rNone && t < naiveT)
			{
				naiveIntersection = obj;
				naiveT = t;
			}
		}

		T aabbT = TNumTraits::infinity;
		TObjectIterator aabbIntersection = aabbTree.intersect(ray, aabbT);
		LASS_TEST_CHECK_EQUAL(naiveIntersection, aabbIntersection);
		if (naiveT == TNumTraits::infinity)
		{
			LASS_TEST_CHECK_EQUAL(naiveT, aabbT);
		}
		else
		{
			LASS_TEST_CHECK_CLOSE(naiveT, aabbT, tolerance);
		}

		T aabpT = TNumTraits::infinity;
		TObjectIterator aabpIntersection = aabpTree.intersect(ray, aabpT);
		LASS_TEST_CHECK_EQUAL(naiveIntersection, aabpIntersection);
		if (naiveT == TNumTraits::infinity)
		{
			LASS_TEST_CHECK_EQUAL(naiveT, aabpT);
		}
		else
		{
			LASS_TEST_CHECK_CLOSE(naiveT, aabpT, tolerance);
		}
	}

	// SPEED TESTS
	//
	LASS_COUT << "object tree speed tests: " << typeid(T).name() << " " << dim << "D\n";

	// contain speed test
	//
	util::Clock clock;
	util::StopWatch stopWatch(clock);
	std::vector<TPoint> containTargets;
	for (size_t i = 0; i < numberOfContainSpeedTestTargets; ++i)
	{
		containTargets.push_back(bounds.random(generator));
	}	
	size_t aabbHits = 0;
	stopWatch.restart();
	for (size_t k = 0; k < numberOfContainSpeedTestRuns; ++k)
	{
		for (size_t i = 0; i < numberOfContainSpeedTestTargets; ++i)
		{
			aabbHits += aabbTree.contains(containTargets[i]) ? 1 : 0;
		}
	}
	const util::Clock::TTime aabbContainTime = stopWatch.stop();	
	size_t aabpHits = 0;
	stopWatch.restart();
	for (size_t k = 0; k < numberOfContainSpeedTestRuns; ++k)
	{
		for (size_t i = 0; i < numberOfContainSpeedTestTargets; ++i)
		{
			aabpHits += aabpTree.contains(containTargets[i]) ? 1 : 0;
		}
	}
	const util::Clock::TTime aabpContainTime = stopWatch.stop();
	LASS_TEST_CHECK_EQUAL(aabbHits, aabpHits);
	LASS_COUT << "contains: aabb " << aabbContainTime << "\taabp " << aabpContainTime << std::endl;

	// intersection speed test
	//
	std::vector<TRay> intersectionTargets;
	for (size_t i = 0; i < numberOfIntersectionSpeedTestTargets; ++i)
	{
		TPoint support = bounds.random(generator);
		TVector direction = TVector::random(generator);
		intersectionTargets.push_back(TRay(support, direction));
	}
	T aabbTotal = 0;
	stopWatch.restart();
	for (size_t k = 0; k < numberOfIntersectionSpeedTestRuns; ++k)
	{
		for (size_t i = 0; i < numberOfIntersectionSpeedTestTargets; ++i)
		{
			T aabbT = TNumTraits::infinity;
			if (aabbTree.intersect(intersectionTargets[i], aabbT) != objectEnd)
			{
				aabbTotal += aabbT;
			}
		}
	}
	const util::Clock::TTime aabbIntersectionTime = stopWatch.stop();
	T aabpTotal = 0;
	stopWatch.restart();
	for (size_t k = 0; k < numberOfIntersectionSpeedTestRuns; ++k)
	{
		for (size_t i = 0; i < numberOfIntersectionSpeedTestTargets; ++i)
		{
			T aabpT = TNumTraits::infinity;
			if (aabpTree.intersect(intersectionTargets[i], aabpT) != objectEnd)
			{
				aabpTotal += aabpT;
			}
		}
	}
	const util::Clock::TTime aabpIntersectionTime = stopWatch.stop();
	LASS_TEST_CHECK_EQUAL(aabbTotal, aabpTotal);
	LASS_COUT << "intersection: aabb " << aabbIntersectionTime 
		<< "\taabp " << aabpIntersectionTime << std::endl;
}



}

}

#endif

// EOF
