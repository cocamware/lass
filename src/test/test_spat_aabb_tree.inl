/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_SPAT_AABB_TREE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_SPAT_AABB_TREE_INL

#include "test_common.h"
#include "../spat/aabb_tree.h"
//#include "../spat/aabp_tree.h"
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
namespace aabb_tree
{

template <typename T, typename RandomGenerator>
void generateObjects(const prim::Aabb2D<T>& iBound,
					 T iMaxSize,
					 RandomGenerator& iRandom,
					 size_t iNumber,
					 prim::Triangle2D<T>* oObjects)
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
		oObjects[i] = prim::Triangle2D<T>(a, b, c);
		if (oObjects[i].orientation() == prim::oClockWise)
		{
			oObjects[i].flip();
		}
	}
}


template <typename T, typename RandomGenerator>
void generateObjects(const prim::Aabb3D<T>& iBound,
					 T iMaxSize,
					 RandomGenerator& iRandom,
					 size_t iNumber,
					 prim::Sphere3D<T>* oObjects)
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
		oObjects[i] = prim::Sphere3D<T>(center, radius);
	}
}

}

template <typename T, size_t dim>
void testSpatAabbTree()
{
	typedef typename meta::Select
	<
		dim == 2,
		prim::Aabb2D<T>,
		prim::Aabb3D<T>
	>::Type TAabb;

	typedef typename meta::Select
	<
		dim == 2,
		prim::Ray2D<T>,
		prim::Ray3D<T>
	>::Type TRay;

	typedef typename meta::Select
	<
		dim == 2,
		prim::Triangle2D<T>,
		prim::Sphere3D<T>
	>::Type TObject;

	typedef typename TObject::TPoint TPoint;
	typedef typename TObject::TVector TVector;
	typedef typename TObject::TNumTraits TNumTraits;

	const T extent = T(1000);
	const T maxSize = T(10);
	const size_t numberOfObjects = 10000;
	const size_t numberOfContainTests = 1000;
	const size_t numberOfIntersectionTests = 1000;

	// set bounds
	//
	TPoint min;
	TPoint max;
	for (size_t i = 0; i < TPoint::dimension; ++i)
	{
		min[i] = -extent;
		max[i] = extent;
	}
	const TAabb bounds(min, max);

	// random generator
	//
	num::RandomParkMiller generator;

	// create test subjects
	//
	TObject objects[numberOfObjects];
	aabb_tree::generateObjects(bounds, maxSize, generator, numberOfObjects, objects);
	TObject* objectsEnd = objects + numberOfObjects;

	// create tree
	//
	typedef spat::AabbTree<TObject, spat::DefaultObjectTraits<TObject, TAabb, TRay> > TTree;
	TTree tree(objects, objectsEnd);

	typedef std::vector<const TObject*> TObjectHits;

	// contain test
	//
	for (unsigned i = 0; i < numberOfContainTests; ++i)
	{
		TPoint target = bounds.random(generator);

		TObjectHits naiveHits;
		bool naiveContain = false;
		for (unsigned k = 0; k < numberOfObjects; ++k)
		{
			if (objects[k].contains(target))
			{
				naiveContain = true;
				naiveHits.push_back(&objects[k]);
			}
		}

		bool treeContain = tree.contains(target);
		TObjectHits treeHits;
		tree.find(target, std::back_inserter(treeHits));

		LASS_TEST_CHECK_EQUAL(naiveContain, treeContain);
		LASS_TEST_CHECK_EQUAL(naiveContain, !treeHits.empty());

		std::sort(naiveHits.begin(), naiveHits.end());
		std::sort(treeHits.begin(), treeHits.end());
		LASS_TEST_CHECK(naiveHits.size() == treeHits.size() &&
			std::equal(naiveHits.begin(), naiveHits.end(), treeHits.begin()));
	}

	// intersection test
	//
	for (unsigned i = 0; i < numberOfIntersectionTests; ++i)
	{
		TPoint start = bounds.random(generator);
		TVector direction = TVector::random(generator);
		TRay ray(start, direction);

		T naiveT = TNumTraits::infinity;
		TObject* naiveIntersection = objectsEnd;
		for (unsigned k = 0; k < numberOfObjects; ++k)
		{
			T tNear, tFar;
			const prim::Result result = prim::intersect(ray, objects[k], tNear, tFar);
			if (result != prim::rNone && tNear < naiveT)
			{
				naiveIntersection = &objects[k];
				naiveT = tNear;
			}
		}

		//LASS_COUT << i << "\t" << naiveIntersection << "\t" << naiveT << "\t" << ray.point(naiveT) << std::endl;

		T treeT = TNumTraits::infinity;
		const TObject* treeIntersection = tree.intersect(ray, treeT);

		LASS_TEST_CHECK_EQUAL(naiveIntersection, treeIntersection);
		LASS_TEST_CHECK_EQUAL(naiveT, treeT);
	}

	// contain speed test
	//
	util::Clock clock;
	util::StopWatch stopWatch(clock);
	TPoint containTargets[numberOfContainTests];
	for (size_t i = 0; i < numberOfContainTests; ++i)
	{
		containTargets[i] = bounds.random(generator);
	}
	stopWatch.start();
	for (size_t i = 0; i < numberOfContainTests; ++i)
	{
		for (size_t k = 0; k < numberOfObjects; ++k)
		{
			if (objects[k].contains(containTargets[i]))
			{
				break;
			}
		}
	}
	const double naiveContainTime = stopWatch.stop();
	stopWatch.reset();
	stopWatch.start();
	for (size_t i = 0; i < numberOfContainTests; ++i)
	{
		for (size_t k = 0; k < numberOfObjects; ++k)
		{
			tree.contains(containTargets[i]);
		}
	}
	const double treeContainTime = stopWatch.stop();

	LASS_COUT << "contains: naive " << naiveContainTime << "\ttree " << treeContainTime << std::endl;

	// intersection speed test
	//
	TRay intersectionTargets[numberOfIntersectionTests];
	for (size_t i = 0; i < numberOfIntersectionTests; ++i)
	{
		TPoint start = bounds.random(generator);
		TVector direction = TVector::random(generator);
		intersectionTargets[i] = TRay(start, direction);
	}
	stopWatch.reset();
	stopWatch.start();
	for (size_t i = 0; i < numberOfIntersectionTests; ++i)
	{
		T naiveT = TNumTraits::infinity;
		TObject* naiveIntersection = objectsEnd;
		for (unsigned k = 0; k < numberOfObjects; ++k)
		{
			T tNear, tFar;
			const prim::Result result = prim::intersect(intersectionTargets[i], objects[k], tNear, tFar);
			if (result != prim::rNone && tNear < naiveT)
			{
				naiveIntersection = &objects[k];
				naiveT = tNear;
			}
		}
	}
	const double naiveIntersectionTime = stopWatch.stop();
	stopWatch.reset();
	stopWatch.start();
	for (size_t i = 0; i < numberOfIntersectionTests; ++i)
	{
		T treeT = TNumTraits::infinity;
		tree.intersect(intersectionTargets[i], treeT);
	}
	const double treeIntersectionTime = stopWatch.stop();

	LASS_COUT << "intersection: naive " << naiveIntersectionTime << "\ttree " << treeIntersectionTime << std::endl;
}



}

}

#endif

// EOF
