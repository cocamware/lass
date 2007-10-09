/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_SPAT_OBJECT_TREES_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_SPAT_OBJECT_TREES_INL

//#define LASS_TEST_SPAT_OBJECT_TREES_DIAGNOSTICS
#define LASS_SPAT_KD_TREE_DIAGNOSTICS

#include "test_common.h"
#include "../spat/aabb_tree.h"
#include "../spat/aabp_tree.h"
#include "../spat/quad_tree.h"
#include "../spat/kd_tree.h"
#include "../meta/select.h"
#include "../meta/tuple.h"
#include "../meta/type_list.h"
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
#include "../io/xml_o_file.h"
#include "../io/matlab_o_stream.h"

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


template <typename ObjectIterator, typename T>
class Neighbour
{
public:
	Neighbour(ObjectIterator object, T squaredDistance): object_(object), squaredDistance_(squaredDistance) {}
	const ObjectIterator& object() const { return object_; }
	const T& squaredDistance() const { return squaredDistance_; }
	const bool operator<(const Neighbour& other) const { return squaredDistance_ < other.squaredDistance_; }
	template <typename Other> const bool operator==(const Other& other) const
	{
		return object_ == other.object() && squaredDistance_ == other.squaredDistance();
	}
private:
	ObjectIterator object_;
	T squaredDistance_;
};



template <typename ObjectIterator>
class TreeConstructor
{
public: 
	TreeConstructor(ObjectIterator first, ObjectIterator last): first_(first), last_(last) {}
	template <typename Tree> void operator()(Tree& tree) const
	{
		tree.reset(first_, last_);
	}
private:
	ObjectIterator first_;
	ObjectIterator last_;
};



template <typename Point, typename ObjectHits>
class ContainValidityTest
{
public:
	ContainValidityTest(const Point& target, const ObjectHits& bruteHits, bool bruteContains): 
		target_(target), bruteHits_(bruteHits), bruteContains_(bruteContains) 
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		const bool treeContains = tree.contains(target_);
		ObjectHits treeHits;
		tree.find(target_, std::back_inserter(treeHits));
		std::sort(treeHits.begin(), treeHits.end());
		LASS_TEST_CHECK_EQUAL(treeContains, bruteContains_);
		LASS_TEST_CHECK(treeHits.size() == bruteHits_.size() && std::equal(treeHits.begin(), treeHits.end(), bruteHits_.begin()));
	}
private:
	Point target_;
	const ObjectHits& bruteHits_;
	bool bruteContains_;
};


template <typename Ray, typename ObjectIterator>
class IntersectionValidityTest
{
public:
	typedef typename Ray::TValue TValue;
	IntersectionValidityTest(const Ray& ray, ObjectIterator bruteHit, TValue bruteT):
		ray_(ray), bruteHit_(bruteHit), bruteT_(bruteT) 
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		typedef typename Ray::TNumTraits TNumTraits;
		TValue treeT = TNumTraits::infinity;
		ObjectIterator treeHit = tree.intersect(ray_, treeT);
		LASS_TEST_CHECK_EQUAL(treeHit, bruteHit_);
		if (treeT == TNumTraits::infinity)
		{
			LASS_TEST_CHECK_EQUAL(treeT, bruteT_);
		}
		else
		{
			LASS_TEST_CHECK_CLOSE(treeT, bruteT_, TValue(1e-5f));
		}
	}
private:
	const Ray& ray_;
	ObjectIterator bruteHit_;
	TValue bruteT_;
};



template <typename Point, typename ObjectIterator>
class NearestNeighbourValidityTest
{
public:
	typedef typename Point::TValue TValue;
	NearestNeighbourValidityTest(const Point& point, ObjectIterator bruteNearest, TValue bruteSqrDist):
		point_(point), bruteNearest_(bruteNearest), bruteSqrDist_(bruteSqrDist) 
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		const typename Tree::Neighbour treeNearest = tree.nearestNeighbour(point_);
		if (treeNearest.object() != bruteNearest_)
		{
			typename Tree::Neighbour temp = tree.nearestNeighbour(point_);
		}
		LASS_TEST_CHECK_EQUAL(treeNearest.object(), bruteNearest_);
		if (bruteSqrDist_ == Point::TNumTraits::infinity)
		{
			LASS_TEST_CHECK_EQUAL(treeNearest.squaredDistance(), bruteSqrDist_);
		}
		else
		{
			LASS_TEST_CHECK_CLOSE(treeNearest.squaredDistance(), bruteSqrDist_, TValue(1e-5f));
		}
	}
private:
	const Point& point_;
	ObjectIterator bruteNearest_;
	TValue bruteSqrDist_;
};



template <typename Point, typename BruteNeighbours>
class RangeSearchValidityTest
{
public:
	typedef typename Point::TValue TValue;
	typedef typename Point::TParam TParam;
	RangeSearchValidityTest(
			const Point& target, const BruteNeighbours& bruteHits, TParam maxRadius, size_t maxCount): 
		target_(target), bruteHits_(bruteHits), maxRadius_(maxRadius), maxCount_(maxCount)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		typedef std::vector<typename Tree::Neighbour> TTreeNeighbours;
		TTreeNeighbours treeHits(maxCount_ + 1);
		typename TTreeNeighbours::iterator last = tree.rangeSearch(target_, maxRadius_, maxCount_, treeHits.begin());
		const size_t treeN = static_cast<size_t>(last - treeHits.begin());
		std::sort_heap(treeHits.begin(), last);
		LASS_TEST_CHECK(bruteHits_.size() == treeN && std::equal(bruteHits_.begin(), bruteHits_.end(), treeHits.begin()));
	}
private:
	Point target_;
	const BruteNeighbours& bruteHits_;
	TValue maxRadius_;
	size_t maxCount_;
};



template <typename Targets>
class ContainSpeedTest
{
public:
	ContainSpeedTest(const Targets& targets, util::StopWatch& stopWatch, size_t numberOfRuns):
		targets_(targets), stopWatch_(stopWatch), numberOfRuns_(numberOfRuns)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		size_t hits = 0;
		stopWatch_.restart();
		for (size_t k = 0; k < numberOfRuns_; ++k)
		{
			const typename Targets::const_iterator end = targets_.end();
			for (typename Targets::const_iterator i = targets_.begin(); i != end; ++i)
			{
				hits += tree.contains(*i) ? 1 : 0;
			}
		}
		const util::Clock::TTime time = stopWatch_.stop();
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time << std::endl;
	}
private:
	const Targets& targets_;
	util::StopWatch& stopWatch_;
	size_t numberOfRuns_;
};

template <typename Targets>
class IntersectionSpeedTest
{
public:
	IntersectionSpeedTest(const Targets& targets, util::StopWatch& stopWatch, size_t numberOfRuns):
		targets_(targets), stopWatch_(stopWatch), numberOfRuns_(numberOfRuns)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		typedef typename Tree::TValue TValue;
		typedef num::NumTraits<TValue> TNumTraits;
		TValue total = 0;
		stopWatch_.restart();
		for (size_t k = 0; k < numberOfRuns_; ++k)
		{
			const typename Targets::const_iterator targetEnd = targets_.end();
			for (typename Targets::const_iterator i = targets_.begin(); i != targetEnd; ++i)
			{
				TValue t = TNumTraits::infinity;
				if (tree.intersect(*i, t) != tree.end())
				{
					total += t;
				}
			}
		}
		const util::Clock::TTime time = stopWatch_.stop();
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time << std::endl;
	}
private:
	const Targets& targets_;
	util::StopWatch& stopWatch_;
	size_t numberOfRuns_;
};



template <typename Targets>
class NearestNeighbourSpeedTest
{
public:
	NearestNeighbourSpeedTest(const Targets& targets, util::StopWatch& stopWatch, size_t numberOfRuns):
		targets_(targets), stopWatch_(stopWatch), numberOfRuns_(numberOfRuns)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		typedef typename Tree::TValue TValue;
		typedef num::NumTraits<TValue> TNumTraits;
		TValue total = 0;
		stopWatch_.restart();
		for (size_t k = 0; k < numberOfRuns_; ++k)
		{
			const typename Targets::const_iterator targetEnd = targets_.end();
			for (typename Targets::const_iterator i = targets_.begin(); i != targetEnd; ++i)
			{
				const typename Tree::Neighbour nearest = tree.nearestNeighbour(*i);
				if (nearest.object() != tree.end())
				{
					total += nearest.squaredDistance();
				}
			}
		}
		const util::Clock::TTime time = stopWatch_.stop();
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time << std::endl;
	}
private:
	const Targets& targets_;
	util::StopWatch& stopWatch_;
	size_t numberOfRuns_;
};



template <typename Targets, typename T>
class RangeSearchSpeedTest
{
public:
	RangeSearchSpeedTest(const Targets& targets, T maxRadius, size_t maxCount, util::StopWatch& stopWatch, size_t numberOfRuns):
		targets_(targets), maxRadius_(maxRadius), maxCount_(maxCount), stopWatch_(stopWatch), numberOfRuns_(numberOfRuns)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		typedef typename Tree::Neighbour TNeighbour;
		typedef std::vector<TNeighbour> TNeighbours;
		TNeighbours hits(maxCount_ + 1);
		size_t count = 0;
		stopWatch_.restart();
		for (size_t k = 0; k < numberOfRuns_; ++k)
		{
			const typename Targets::const_iterator end = targets_.end();
			for (typename Targets::const_iterator i = targets_.begin(); i != end; ++i)
			{
				typename TNeighbours::iterator last = tree.rangeSearch(*i, maxRadius_, maxCount_, hits.begin());
				count += (last - hits.begin());
			}
		}
		const util::Clock::TTime time = stopWatch_.stop();
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time << std::endl;
	}
private:
	const Targets& targets_;
	T maxRadius_;
	size_t maxCount_;
	util::StopWatch& stopWatch_;
	size_t numberOfRuns_;
};



}

template <typename T, size_t dim>
void testSpatObjectTrees()
{
	// const T tolerance = 1e-5f;

	const T extent = T(1000);
	const T maxSize = T(10);
	const size_t numberOfObjects = 10000;
	const size_t numberOfPointTestTargets = 1000;
	const size_t numberOfRayTestTargets = 1000;
	const T maxRangeRadius = 50;
	const size_t maxRangeCount = 10;
	// validation
	const size_t numberOfContainValidations = 1000;
	const size_t numberOfIntersectionValidations = 1000;
	const size_t numberOfNearestNeighbourValidations = 1000;
	const size_t numberOfRangeSearchValidations = 1000;
	// speed tests
	const size_t numberOfContainSpeedTestRuns = 10;
	const size_t numberOfIntersectionSpeedTestRuns = 10;
	const size_t numberOfNearestNeighbourSpeedTestRuns = 10;
	const size_t numberOfRangeSearchSpeedTestRuns = 10;

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

	typedef typename meta::type_list::Make<
		spat::AabbTree<TObject, TObjectTraits, spat::DefaultSplitHeuristics<8> >,
		spat::AabpTree<TObject, TObjectTraits, spat::DefaultSplitHeuristics<2> >,
		spat::QuadTree<TObject, TObjectTraits>
	>::Type TObjectTreeTypes;

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
	TObjectTrees trees;
	object_trees::TreeConstructor<TObjectIterator> construct(objectBegin, objectEnd);
	meta::tuple::forEach(trees, construct);

	LASS_COUT << meta::tuple::field<2>(trees).depth() << "\t" << meta::tuple::field<2>(trees).averageDepth() << std::endl;

	typedef std::vector<TObjectIterator> TObjectHits;

	// contain test
	//
	for (unsigned i = 0; i < numberOfContainValidations; ++i)
	{
		TPoint target = bounds.random(generator);
		TObjectHits bruteHits;
		bool bruteContains = false;
		for (TObjectIterator obj = objectBegin; obj != objectEnd; ++obj)
		{
			if (obj->contains(target))
			{
				bruteContains = true;
				bruteHits.push_back(obj);
			}
		}
		std::sort(bruteHits.begin(), bruteHits.end());

		object_trees::ContainValidityTest<TPoint, TObjectHits> test(target, bruteHits, bruteContains);
		meta::tuple::forEach(trees, test);
	}

	// intersection test
	//
	for (unsigned i = 0; i < numberOfIntersectionValidations; ++i)
	{
		TPoint support = bounds.random(generator);
		TVector direction = TVector::random(generator);
		TRay ray(support, direction);

		T bruteT = TNumTraits::infinity;
		TObjectIterator bruteHit = objectEnd;
		for (TObjectIterator obj = objectBegin; obj != objectEnd; ++obj)
		{
			T t;
			const prim::Result result = prim::intersect(*obj, ray, t);
			if (result != prim::rNone && t < bruteT)
			{
				bruteHit = obj;
				bruteT = t;
			}
		}

		object_trees::IntersectionValidityTest<TRay, TObjectIterator> test(ray, bruteHit, bruteT);
		meta::tuple::forEach(trees, test);
	}

	// nearest neighbour test
	//
	for (unsigned i = 0; i < numberOfNearestNeighbourValidations; ++i)
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

		object_trees::NearestNeighbourValidityTest<TPoint, TObjectIterator> test(point, bruteNearest, bruteSqrDist);
		meta::tuple::forEach(trees, test);
	}

	// range search test
	//
	typedef object_trees::Neighbour<TObjectIterator, T> TNeighbour;
	typedef std::vector<TNeighbour> TNeighbours;
	TNeighbours bruteHits;
	for (unsigned i = 0; i < numberOfRangeSearchValidations; ++i)
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

		object_trees::RangeSearchValidityTest<TPoint, TNeighbours> test(target, bruteHits, maxRangeRadius, maxRangeCount);
		meta::tuple::forEach(trees, test);
	}



	// SPEED TESTS
	//
	LASS_COUT << "object tree speed tests: " << typeid(T).name() << " " << dim << "D\n";
	util::Clock clock;
	util::StopWatch stopWatch(clock);

	typedef std::vector<TPoint> TPointTargets;
	TPointTargets pointTargets;
	for (size_t i = 0; i < numberOfPointTestTargets; ++i)
	{
		pointTargets.push_back(bounds.random(generator));
	}

	typedef std::vector<TRay> TRayTargets;
	TRayTargets rayTargets;
	for (size_t i = 0; i < numberOfRayTestTargets; ++i)
	{
		TPoint support = bounds.random(generator);
		TVector direction = TVector::random(generator);
		rayTargets.push_back(TRay(support, direction));
	}

	LASS_COUT << "contain tests:\n";
	object_trees::ContainSpeedTest<TPointTargets> containSpeedTest(
		pointTargets, stopWatch, numberOfContainSpeedTestRuns);
	meta::tuple::forEach(trees, containSpeedTest);

	LASS_COUT << "intersection tests:\n";
	object_trees::IntersectionSpeedTest<TRayTargets> intersectionSpeedTest(
		rayTargets, stopWatch, numberOfIntersectionSpeedTestRuns);
	meta::tuple::forEach(trees, intersectionSpeedTest);

	LASS_COUT << "nearest neighbour tests:\n";
	object_trees::NearestNeighbourSpeedTest<TPointTargets> nearestNeighbourSpeedTest(
		pointTargets, stopWatch, numberOfNearestNeighbourSpeedTestRuns);
	meta::tuple::forEach(trees, nearestNeighbourSpeedTest);

	LASS_COUT << "range search tests:\n";
	object_trees::RangeSearchSpeedTest<TPointTargets, T> rangeSearchSpeedTest(
		pointTargets, maxRangeRadius, maxRangeCount, stopWatch, numberOfRangeSearchSpeedTestRuns);
	meta::tuple::forEach(trees, rangeSearchSpeedTest);
}



template <typename TPoint>
void testSpatKdTree()
{
	typedef typename TPoint::TValue TValue;
	typedef typename meta::Select
	<
		meta::Bool< TPoint::dimension == 2 >,
		prim::Aabb2D<TValue>,
		prim::Aabb3D<TValue>
	>::Type TAabb;
	typedef const TPoint* TPointIterator;

	// set bounds
	//
	TPoint min;
	TPoint max;
	for (unsigned i = 0; i < TPoint::dimension; ++i)
	{
		min[i] = TValue(-1000);
		max[i] = TValue(+1000);
	}
	const TAabb bounds(min, max);

	// generate points
	//
	const unsigned n = 10000;
	num::RandomMT19937 generator;
	TPoint points[n];
	for (unsigned i = 0; i < n; ++i)
	{
		points[i] = bounds.random(generator);
	}

	// make single tree, and do diagnostics on itµ
	//
	typedef spat::KdTree<TPoint> TKdTree;
	TKdTree tree(points, points + n);
	tree.diagnostics();

	// make tree set
	//
	typedef spat::DefaultObjectTraits<TPoint, TAabb, meta::NullType, TPointIterator> TPointTraits;
	typedef typename meta::type_list::Make<
		TKdTree,
		spat::AabbTree<TPoint, TPointTraits, spat::DefaultSplitHeuristics<32> >,
		spat::AabpTree<TPoint, TPointTraits, spat::DefaultSplitHeuristics<8> >
		//spat::QuadTree<TPoint, TPointTraits> // apparently, QuadTrees are no good idea for points
	>::Type TTreeTypes;
	typedef meta::Tuple<TTreeTypes> TTrees;
	TTrees trees;
	object_trees::TreeConstructor<TPointIterator> construct(points, points + n);
	meta::tuple::forEach(trees, construct);

	// VALIDATION TESTS
	//
	const unsigned nNearestTests = 1000;
	for (unsigned i = 0; i < nNearestTests; ++i)
	{
		TPoint target = bounds.random(generator);

		// naive nearest
		//
		size_t naiveNearest = 0;
		TValue naiveSqrDistance = squaredDistance(target, points[0]);
		for (unsigned k = 1; k < n; ++k)
		{
			const TValue sqrDist = squaredDistance(target, points[k]);
			if (sqrDist < naiveSqrDistance)
			{
				naiveNearest = k;
				naiveSqrDistance = sqrDist;
			}
		}

		// kd tree nearest
		//
		TPoint kdTreeNearest = tree.nearestNeighbour(target).position();

		LASS_TEST_CHECK_EQUAL(kdTreeNearest, points[naiveNearest]);

		object_trees::NearestNeighbourValidityTest<TPoint, TPointIterator> test(target, &points[naiveNearest], naiveSqrDistance);
		meta::tuple::forEach(trees, test);
	}

	// SPEED TESTS
	//
	util::Clock clock;
	util::StopWatch stopWatch(clock);

	const size_t nTargets = 1000;
	const size_t nRuns = 10;
	const TValue maxRadius = 50;
	const size_t maxCount = 10;
	typedef std::vector<TPoint> TTargets;
	TTargets targets;
	for (size_t i = 0; i < nTargets; ++i)
	{
		targets.push_back(bounds.random(generator));
	}

	LASS_COUT << "nearest neighbour tests:\n";
	object_trees::NearestNeighbourSpeedTest<TTargets> nearestNeighbourSpeedTest(
		targets, stopWatch, nRuns);
	meta::tuple::forEach(trees, nearestNeighbourSpeedTest);

	LASS_COUT << "range search tests:\n";
	object_trees::RangeSearchSpeedTest<TTargets, TValue> rangeSearchSpeedTest(
		targets, maxRadius, maxCount, stopWatch, nRuns);
	meta::tuple::forEach(trees, rangeSearchSpeedTest);	
}



}

}

#endif

// EOF
