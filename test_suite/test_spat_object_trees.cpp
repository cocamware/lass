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
 * *	Alternatively, the contents of this file may be used under the terms of the 
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

//#define LASS_SPAT_OBJECT_TREES_DIAGNOSTICS
//#define LASS_SPAT_KD_TREE_DIAGNOSTICS

#include "test_common.h"
#include "tree_test_helpers.h"

namespace lass
{
namespace test
{

template 
<
	typename ObjectTreesType, 
	typename AabbType, 
	typename GeneratorType
>
void testSpatObjectTreesSpeed(
		const ObjectTreesType& trees, const AabbType& bounds, GeneratorType& generator)
{
	typedef typename meta::tuple::Field<ObjectTreesType, 0>::Type::TRay TRay;
	typedef typename AabbType::TValue TValue;
	typedef typename AabbType::TPoint TPoint;
	typedef typename AabbType::TVector TVector;
	enum { dimension = AabbType::dimension };

	const size_t numberOfPointTestTargets = 1000;
	const size_t numberOfBoxTestTargets = 1000;
	const size_t numberOfRayTestTargets = 200;
	const TValue maxRangeRadius = 50;
	const size_t maxRangeCount = 10;
	const size_t numberOfContainSpeedTestRuns = 2;
	const size_t numberOfBoxFindSpeedTestRuns = 2;
	const size_t numberOfIntersectionSpeedTestRuns = 2;
	const size_t numberOfNearestNeighbourSpeedTestRuns = 2;
	const size_t numberOfRangeSearchSpeedTestRuns = 2;

	LASS_COUT << "object tree speed tests: " << typeid(TValue).name() << " " << int(dimension) << "D\n";
	util::Clock clock;
	util::StopWatch stopWatch(clock);

	typedef std::vector<TPoint> TPointTargets;
	TPointTargets pointTargets;
	for (size_t i = 0; i < numberOfPointTestTargets; ++i)
	{
		pointTargets.push_back(bounds.random(generator));
	}

	typedef std::vector<AabbType> TBoxTargets;
	TBoxTargets boxTargets;
	for (size_t i = 0; i < numberOfBoxTestTargets; ++i)
	{
		const TPoint center = bounds.random(generator);
		const TVector extents = tree_test_helpers::randomExtents<TVector>(maxRangeRadius, generator);
		boxTargets.push_back(AabbType(center - extents, center + extents));
	}

	// bounds to generate starting and ending point of rays, 10% larger on each side.
	AabbType rayBounds = bounds;
	rayBounds.scale(TValue(1.2));
	typedef std::vector< std::pair<TRay, TValue> > TRayTargets;
	TRayTargets rayTargets;
	for (size_t i = 0; i < numberOfRayTestTargets; ++i)
	{
		rayTargets.push_back(tree_test_helpers::generateTestRay(rayBounds, generator));
	}

	LASS_COUT << "contain tests:\n";
	tree_test_helpers::ContainSpeedTest<TPointTargets> containSpeedTest(
		pointTargets, stopWatch, numberOfContainSpeedTestRuns);
	meta::tuple::forEach(trees, containSpeedTest);

	LASS_COUT << "point find tests:\n";
	tree_test_helpers::FindSpeedTest<TPointTargets> pointFindSpeedTest(
		pointTargets, stopWatch, numberOfBoxFindSpeedTestRuns);
	meta::tuple::forEach(trees, pointFindSpeedTest);

	LASS_COUT << "aabb find tests:\n";
	tree_test_helpers::FindSpeedTest<TBoxTargets> aabbFindSpeedTest(
		boxTargets, stopWatch, numberOfBoxFindSpeedTestRuns);
	meta::tuple::forEach(trees, aabbFindSpeedTest);

	LASS_COUT << "ray find tests:\n";
	tree_test_helpers::RayFindSpeedTest<TRayTargets> rayFindSpeedTest(
		rayTargets, stopWatch, numberOfBoxFindSpeedTestRuns);
	meta::tuple::forEach(trees, rayFindSpeedTest);

	LASS_COUT << "intersection tests:\n";
	tree_test_helpers::IntersectionSpeedTest<TRayTargets> intersectionSpeedTest(
		rayTargets, stopWatch, numberOfIntersectionSpeedTestRuns);
	meta::tuple::forEach(trees, intersectionSpeedTest);

	LASS_COUT << "intersections tests:\n";
	tree_test_helpers::IntersectionsSpeedTest<TRayTargets> intersectionsSpeedTest(
		rayTargets, stopWatch, numberOfIntersectionSpeedTestRuns);
	meta::tuple::forEach(trees, intersectionsSpeedTest);

	LASS_COUT << "nearest neighbour tests:\n";
	tree_test_helpers::NearestNeighbourSpeedTest<TPointTargets> nearestNeighbourSpeedTest(
		pointTargets, stopWatch, numberOfNearestNeighbourSpeedTestRuns);
	meta::tuple::forEach(trees, nearestNeighbourSpeedTest);

	LASS_COUT << "range search tests:\n";
	tree_test_helpers::RangeSearchSpeedTest<TPointTargets, TValue> rangeSearchSpeedTest(
		pointTargets, maxRangeRadius, maxRangeCount, stopWatch, numberOfRangeSearchSpeedTestRuns);
	meta::tuple::forEach(trees, rangeSearchSpeedTest);
}

/*
template 
<
	typename ObjectTreesType, 
	typename AabbType, 
	typename GeneratorType
>
void testSpatObjectTreesContain(
		const ObjectTreesType& trees, const AabbType& bounds, GeneratorType& generator)
{
	typedef typename meta::tuple::Field<ObjectTreesType, 0>::Type::TRay TRay;
	typedef typename AabbType::TValue TValue;
	typedef typename AabbType::TPoint TPoint;
	typedef typename AabbType::TVector TVector;
	enum { dimension = AabbType::dimension };
}
*/

template <typename T, size_t dim>
void testSpatObjectTrees()
{
	// const T tolerance = 1e-5f;

	const T extent = T(1000);
	const T maxSize = T(10);
	const size_t numberOfObjects = 10000;
	const T maxRangeRadius = 50;
	const size_t maxRangeCount = 10;
	// validation
	const size_t numberOfContainValidations = 1000;
	const size_t numberOfAabbFindValidations = 1000;
	const size_t numberOfIntersectionValidations = 1000;
	const size_t numberOfNearestNeighbourValidations = 1000;
	const size_t numberOfRangeSearchValidations = 1000;
	// speed tests

	typedef typename meta::Select< meta::Bool<dim == 2>, prim::Triangle2D<T>, prim::Sphere3D<T> >::Type TObject;
	typedef typename meta::Select< meta::Bool<dim == 2>, prim::Aabb2D<T>, prim::Aabb3D<T> >::Type TAabb;
	typedef typename meta::Select< meta::Bool<dim == 2>, prim::Ray2D<T>, prim::Ray3D<T> >::Type TRay;

	typedef typename TObject::TPoint TPoint;
	typedef typename TObject::TVector TVector;
	typedef typename TObject::TNumTraits TNumTraits;

	typedef std::vector<TObject> TObjects;
	typedef spat::DefaultObjectTraits<TObject, TAabb, TRay> TObjectTraits;
	typedef typename TObjectTraits::TObjectIterator TObjectIterator;

	typedef spat::AabbTree<TObject, TObjectTraits> TAabbTree;
	typedef spat::AabpTree<TObject, TObjectTraits> TAabpTree;
	typedef spat::QuadTree<TObject, TObjectTraits> TQuadTree;
	typedef spat::QbvhTree<TObject, TObjectTraits> TQbvhTree;

	typedef typename meta::type_list::Make<TAabbTree, TAabpTree, TQuadTree, TQbvhTree>::Type TObjectTreeTypes;
	typedef meta::Tuple<TObjectTreeTypes> TObjectTrees;

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

	// bounds to generate starting and ending point of rays, 10% larger on each side.
	TAabb rayBounds = bounds;
	rayBounds.scale(T(1.2));

	// random generator
	//
	std::mt19937_64 generator;

	// create test subjects
	//
	TObjects objects;
	tree_test_helpers::generateObjects(
		bounds, maxSize, generator, numberOfObjects, std::back_inserter(objects));
	const TObjectIterator objectBegin = &objects[0];
	const TObjectIterator objectEnd = objectBegin + numberOfObjects;

	// create trees
	//
	TObjectTrees trees;
	tree_test_helpers::TreeConstructor<TObjectIterator> construct(objectBegin, objectEnd);
	meta::tuple::forEach(trees, construct);

	typedef std::set<TObjectIterator> TObjectHits;

	// contain test and point find test
	//
	for (size_t i = 0; i < numberOfContainValidations; ++i)
	{
		const TPoint target = bounds.random(generator);
		TObjectHits bruteHits;
		bool bruteContains = false;
		for (TObjectIterator obj = objectBegin; obj != objectEnd; ++obj)
		{
			if (obj->contains(target))
			{
				bruteContains = true;
				bruteHits.insert(obj);
			}
		}
		tree_test_helpers::ContainValidityTest<TPoint, TObjectHits> test(target, bruteHits, bruteContains);
		meta::tuple::forEach(trees, test);
	}

	// aabb find test
	//
	for (size_t i = 0; i < numberOfAabbFindValidations; ++i)
	{
		const TPoint center(bounds.random(generator));
		const TVector extents = tree_test_helpers::randomExtents<TVector>(maxRangeRadius, generator);
		const TAabb box(center - extents, center + extents);
		TObjectHits bruteHits;
		for (TObjectIterator obj = objectBegin; obj != objectEnd; ++obj)
		{
			if (intersects(*obj, box))
			{
				bruteHits.insert(obj);
			}
		}

		tree_test_helpers::AabbFindValidityTest<TAabb, TObjectHits> test(box, bruteHits);
		meta::tuple::forEach(trees, test);
	}

	// intersection and intersects test
	//
	{
		using Hit = typename tree_test_helpers::IntersectionValidityTest<TRay, TObjectIterator>::Hit;
		for (size_t i = 0; i < numberOfIntersectionValidations; ++i)
		{
			const auto ray = tree_test_helpers::generateTestRay(rayBounds, generator);

			Hit brute1 { TNumTraits::infinity, objectEnd, false };
			Hit brute2 { TNumTraits::infinity, objectEnd, false };
			for (TObjectIterator obj = objectBegin; obj != objectEnd; ++obj)
			{
				T t;
				const prim::Result result = prim::intersect(*obj, ray.first, t);
				if (result != prim::rNone)
				{
					if (t < brute1.t)
					{
						brute2 = brute1;
						brute1 = Hit { t, obj, true };

						// do we have a second intersection on the same primitive?
						T t2;
						const prim::Result result2 = prim::intersect(*obj, ray.first, t2, t);
						if (result2 != prim::rNone)
						{
							LASS_TEST_CHECK(t2 > t && t2 > brute1.t);
							if (t2 < brute2.t)
							{
								brute2 = Hit { t2, obj, true };
							}
						}
					}
					else if (t < brute2.t)
					{
						brute2 = Hit { t, obj, true };
					}
				}
			}
			tree_test_helpers::IntersectionValidityTest<TRay, TObjectIterator> test(ray.first, brute1, brute2);
			meta::tuple::forEach(trees, test);
		}
	}

	// ray find test
	//
	{
		for (size_t i = 0; i < numberOfIntersectionValidations; ++i)
		{
			const auto ray = tree_test_helpers::generateTestRay(rayBounds, generator);
			const T tMin = 0;
			TObjectHits bruteHits;
			for (TObjectIterator obj = objectBegin; obj != objectEnd; ++obj)
			{
				T t;
				const prim::Result result = prim::intersect(*obj, ray.first, t, tMin);
				if (result != prim::rNone && t < ray.second)
				{
					bruteHits.insert(obj);
				}
			}

			tree_test_helpers::RayFindValidityTest<TRay, TObjectHits> test(ray.first, bruteHits, tMin, ray.second);
			meta::tuple::forEach(trees, test);
		}
	}

	// nearest neighbour test
	//
	for (size_t i = 0; i < numberOfNearestNeighbourValidations; ++i)
	{
		TPoint point = bounds.random(generator);

		T bruteSqrDist = TNumTraits::infinity;
		TObjectIterator bruteNearest = objectEnd;
		for (TObjectIterator obj = objectBegin; obj != objectEnd; ++obj)
		{
			const T sqrDist = prim::squaredDistance(*obj, point);
			if (sqrDist < bruteSqrDist)
			{
				bruteNearest = obj;
				bruteSqrDist = sqrDist;
			}
		}

		tree_test_helpers::NearestNeighbourValidityTest<TPoint, TObjectIterator> test(point, bruteNearest, bruteSqrDist);
		meta::tuple::forEach(trees, test);
	}

	// range search test
	//
	typedef tree_test_helpers::Neighbour<TObjectIterator, T> TNeighbour;
	typedef std::vector<TNeighbour> TNeighbours;
	TNeighbours bruteHits;
	for (size_t i = 0; i < numberOfRangeSearchValidations; ++i)
	{
		const TPoint target = bounds.random(generator);
		bruteHits.clear();
		T bruteSquareRadius = num::sqr(maxRangeRadius);

		for (TObjectIterator obj = objectBegin; obj != objectEnd; ++obj)
		{
			const T sqrDist = prim::squaredDistance(*obj, target);
			if (sqrDist < bruteSquareRadius)
			{
				bruteHits.push_back(TNeighbour(obj, sqrDist));
				std::push_heap(bruteHits.begin(), bruteHits.end());
				if (bruteHits.size() > maxRangeCount)
				{
					std::pop_heap(bruteHits.begin(), bruteHits.end());
					bruteHits.pop_back();
				}
				LASS_ASSERT(bruteHits.size() <= maxRangeCount);
			}
		}
		std::sort_heap(bruteHits.begin(), bruteHits.end());

		tree_test_helpers::RangeSearchValidityTest<TPoint, TNeighbours> test(target, bruteHits, maxRangeRadius, maxRangeCount);
		meta::tuple::forEach(trees, test);
	}

	if (true)
	{
		testSpatObjectTreesSpeed(trees, bounds, generator);
	}

	// test deletion from quadtree.
	TQuadTree& tree = meta::tuple::field<2>(trees);
	for (TObjectIterator i = objectBegin; i != objectEnd; ++i)
	{
		tree.remove(i);
	}
	LASS_TEST_CHECK_EQUAL(tree.objectCount(), size_t(0));
}


TUnitTest test_spat_object_trees()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE((testSpatObjectTrees<float,2>)));
	result.push_back(LASS_TEST_CASE((testSpatObjectTrees<float,3>)));
#if LASS_HAVE_LONG_DOUBLE
	/* testSpatObjectTrees uses the intersect routine which requires double the precision of the input
	*  type.  So this "double" test will eventually want "long double" in the code, so we need to be sure
	*  the type is supported.  
	*/
	result.push_back(LASS_TEST_CASE((testSpatObjectTrees<double,2>)));
	result.push_back(LASS_TEST_CASE((testSpatObjectTrees<double,3>)));
#endif
	return result;
}


}

}

// EOF
