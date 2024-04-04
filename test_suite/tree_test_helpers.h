/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TREE_TEST_HELPERS_H
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TREE_TEST_HELPERS_H

#include "test_common.h"
#include "../lass/spat/aabb_tree.h"
#include "../lass/spat/aabp_tree.h"
#include "../lass/spat/quad_tree.h"
#include "../lass/spat/qbvh_tree.h"
#include "../lass/spat/kd_tree.h"
#include "../lass/meta/select.h"
#include "../lass/meta/tuple.h"
#include "../lass/meta/type_list.h"
#include "../lass/num/distribution.h"
#include "../lass/num/random.h"
#include "../lass/prim/aabb_2d.h"
#include "../lass/prim/aabb_3d.h"
#include "../lass/prim/ray_2d.h"
#include "../lass/prim/ray_3d.h"
#include "../lass/prim/sphere_3d.h"
#include "../lass/prim/triangle_2d.h"
#include "../lass/util/clock.h"
#include "../lass/util/stop_watch.h"
#include "../lass/io/xml_o_file.h"
#include "../lass/io/matlab_o_stream.h"

namespace lass
{
namespace test
{
namespace tree_test_helpers
{

template <size_t maxSize, size_t maxDepth> struct StaticSplitHeuristics: spat::DefaultSplitHeuristics
{
	StaticSplitHeuristics(): spat::DefaultSplitHeuristics(maxSize, maxDepth) {}
};



template <typename Vector, typename T, typename RandomGenerator> 
Vector randomExtents(T maxExtents, RandomGenerator& random)
{
	Vector v;

	std::uniform_real_distribution<T> distribution(0, maxExtents);
	for (size_t k = 0; k < Vector::dimension; ++k)
	{
		v[k] = distribution(random);
	}
	return v;
}



template <typename T, typename RandomGenerator>
std::pair<prim::Ray2D<T>, T> generateTestRay(const prim::Aabb2D<T>& bounds, RandomGenerator& rng)
{
	std::uniform_real_distribution<T> uniform;
	std::uniform_int_distribution<size_t> randomAxis(0, 1);
	const auto support = bounds.random(rng);
	const auto target = bounds.random(rng);
	auto direction = target - support;

	const T u = uniform(rng);
	if (u < 0.1f)
	{
		// make sure the ray is parallel to one axis.
		const size_t axis = randomAxis(rng);
		direction[axis] = 0;
	}

	const auto length = direction.norm();
	direction /= length;
	return std::make_pair(prim::Ray2D<T>(support, direction), length);
}



template <typename T, typename RandomGenerator>
std::pair<prim::Ray3D<T>, T> generateTestRay(const prim::Aabb3D<T>& bounds, RandomGenerator& rng)
{
	std::uniform_real_distribution<T> uniform;
	std::uniform_int_distribution<size_t> randomAxis(0, 2);
	const auto support = bounds.random(rng);
	const auto target = bounds.random(rng);
	auto direction = target - support;

	const T u = uniform(rng);
	if (u < 0.1f)
	{
		// make sure the ray is parallel to one axis-aligned plane.
		const size_t axis = randomAxis(rng);
		direction[axis] = 0;
	}
	else if (u < 0.2f)
	{
		// make sure the ray is parallel to one axis.
		const size_t axis = randomAxis(rng);
		direction[axis] = 0;
		direction[(axis + 1) % 3] = 0;
	}

	const auto length = direction.norm();
	direction /= length;
	return std::make_pair(prim::Ray3D<T>(support, direction), length);
}



template <typename T, typename RandomGenerator, typename OuputIterator>
OuputIterator generateObjects(
		const prim::Aabb2D<T>& iBound, T iMaxSize, RandomGenerator& iRandom,size_t iNumber, OuputIterator iObjects)
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
		std::uniform_real_distribution<T> radiusDistribution(0, maxRadius);
		const T radius = radiusDistribution(iRandom);
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
	bool operator<(const Neighbour& other) const { return squaredDistance_ < other.squaredDistance_; }
	template <typename Other> bool operator==(const Other& other) const
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
		tree.find(target_, std::inserter(treeHits, treeHits.begin()));
		LASS_TEST_CHECK_EQUAL(treeContains, bruteContains_);
		LASS_TEST_CHECK(treeHits.size() == bruteHits_.size() && std::equal(treeHits.begin(), treeHits.end(), bruteHits_.begin()));
	}
private:
	Point target_;
	const ObjectHits& bruteHits_;
	bool bruteContains_;
};



template <typename Aabb, typename ObjectHits>
class AabbFindValidityTest
{
public:
	AabbFindValidityTest(const Aabb& box, const ObjectHits& bruteHits): 
		box_(box), bruteHits_(bruteHits)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		ObjectHits treeHits;
		tree.find(box_, std::inserter(treeHits, treeHits.begin()));
		LASS_TEST_CHECK(treeHits.size() == bruteHits_.size() && std::equal(treeHits.begin(), treeHits.end(), bruteHits_.begin()));
	}
private:
	Aabb box_;
	const ObjectHits& bruteHits_;
};



template <typename Ray, typename ObjectHits>
class RayFindValidityTest
{
public:
	typedef typename Ray::TValue TValue;
	RayFindValidityTest(const Ray& ray, const ObjectHits& bruteHits, TValue tMin, TValue tMax):
		ray_(ray), bruteHits_(bruteHits), tMin_(tMin), tMax_(tMax)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		ObjectHits treeHits;
		tree.find(ray_, tMin_, tMax_, std::inserter(treeHits, treeHits.begin()));
		LASS_TEST_CHECK(treeHits.size() == bruteHits_.size() && std::equal(treeHits.begin(), treeHits.end(), bruteHits_.begin()));
	}
private:
	Ray ray_;
	const ObjectHits& bruteHits_;
	TValue tMin_;
	TValue tMax_;
};



template <typename Ray, typename ObjectIterator>
class IntersectionValidityTest
{
public:
	struct Hit
	{
		typename Ray::TValue t;
		ObjectIterator obj;
		bool intersects;
	};

	typedef typename Ray::TValue TValue;
	IntersectionValidityTest(const Ray& ray, Hit brute1, Hit brute2):
		ray_(ray), brute1_(brute1), brute2_(brute2)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		typedef typename Ray::TNumTraits TNumTraits;

		Hit tree1 { TNumTraits::infinity, tree.end(), false };
		tree1.obj = tree.intersect(ray_, tree1.t);
		tree1.intersects = tree.intersects(ray_);

		LASS_TEST_CHECK_EQUAL(tree1.obj, brute1_.obj);
		if (tree1.obj == tree.end())
		{
			LASS_TEST_CHECK_EQUAL(tree1.t, brute1_.t);
		}
		else
		{
			LASS_TEST_CHECK_CLOSE(tree1.t, brute1_.t, TValue(1e-5f));
		}
		LASS_TEST_CHECK_EQUAL(tree1.intersects, brute1_.intersects);

		// test the second intersection

		Hit tree2 { TNumTraits::infinity, tree.end(), false };
		tree2.obj = tree.intersect(ray_, tree2.t, tree1.t);
		tree2.intersects = tree.intersects(ray_, tree1.t);

		LASS_TEST_CHECK_EQUAL(tree2.obj, brute2_.obj);
		if (tree2.obj == tree.end())
		{
			LASS_TEST_CHECK_EQUAL(tree2.t, brute2_.t);
		}
		else
		{
			LASS_TEST_CHECK_CLOSE(tree2.t, brute2_.t, TValue(1e-5f));
		}
		LASS_TEST_CHECK_EQUAL(tree2.intersects, brute2_.intersects);
	}
private:
	const Ray& ray_;
	Hit brute1_;
	Hit brute2_;
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
			typename Tree::Neighbour LASS_UNUSED(temp) = tree.nearestNeighbour(point_);
		}
		LASS_TEST_CHECK_EQUAL(treeNearest.object(), bruteNearest_);
		if (bruteSqrDist_ == Point::TNumTraits::infinity)
		{
			LASS_TEST_CHECK_EQUAL(treeNearest.squaredDistance(), bruteSqrDist_);
		}
		else
		{
			LASS_TEST_CHECK_CLOSE(treeNearest.squaredDistance(), bruteSqrDist_, TValue(1e-4f));
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
		std::sort_heap(treeHits.begin(), last);
		LASS_TEST_CHECK(closeNeighbourhood(bruteHits_.begin(), bruteHits_.end(), treeHits.begin(), last, 1e-5f));
	}
private:

	/** @pre the ranges [first1, last1) and [first2, last2) are both expected to be sorted in the 
	 *		same way so that first1[k] can be matched with first2[k].
	 */
	template <typename RandomIt1, typename RandomIt2>
	static bool closeNeighbourhood(
			RandomIt1 first1, RandomIt1 last1, RandomIt2 first2, RandomIt2 last2, TValue tolerance)
	{
		if ((last1 - first1) != (last2 - first2))
		{
			return false;
		}
		while (first1 != last1)
		{
			if (first1->object() != first2->object())
			{
				return false;
			}
			if (!num::almostEqual(first1->squaredDistance(), first2->squaredDistance(), tolerance))
			{
				return false;
			}
			++first1;
			++first2;
		}
		return true;
	}

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
		const util::Clock::TTime time = stopWatch_.stop() / static_cast<util::Clock::TTime>(numberOfRuns_ * targets_.size());
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time << " (" << hits << ")" << std::endl;
	}
private:
	const Targets& targets_;
	util::StopWatch& stopWatch_;
	size_t numberOfRuns_;
};



template <typename Targets>
class FindSpeedTest
{
public:
	FindSpeedTest(const Targets& targets, util::StopWatch& stopWatch, size_t numberOfRuns):
		targets_(targets), stopWatch_(stopWatch), numberOfRuns_(numberOfRuns)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		size_t hits = 0;
		stopWatch_.restart();
		std::set<typename Tree::TObjectIterator> loot;
		for (size_t k = 0; k < numberOfRuns_; ++k)
		{
			const typename Targets::const_iterator end = targets_.end();
			for (typename Targets::const_iterator i = targets_.begin(); i != end; ++i)
			{
				loot.clear();
				tree.find(*i, std::inserter(loot, loot.begin()));
				hits += loot.size();
			}
		}
		const util::Clock::TTime time = stopWatch_.stop() / static_cast<util::Clock::TTime>(numberOfRuns_ * targets_.size());
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time << " (" << hits << ")" << std::endl;
	}
private:
	const Targets& targets_;
	util::StopWatch& stopWatch_;
	size_t numberOfRuns_;
};



template <typename Targets>
class RayFindSpeedTest
{
public:
	RayFindSpeedTest(const Targets& targets, util::StopWatch& stopWatch, size_t numberOfRuns):
		targets_(targets), stopWatch_(stopWatch), numberOfRuns_(numberOfRuns)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		size_t hits = 0;
		stopWatch_.restart();
		std::set<typename Tree::TObjectIterator> loot;
		for (size_t k = 0; k < numberOfRuns_; ++k)
		{
			const typename Targets::const_iterator end = targets_.end();
			for (typename Targets::const_iterator i = targets_.begin(); i != end; ++i)
			{
				loot.clear();
				tree.find(i->first, 0, i->second, std::inserter(loot, loot.begin()));
				hits += loot.size();
			}
		}
		const util::Clock::TTime time = stopWatch_.stop() / static_cast<util::Clock::TTime>(numberOfRuns_ * targets_.size());
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time << " (" << hits << ")" << std::endl;
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
				if (tree.intersect(i->first, t) != tree.end())
				{
					total += t;
				}
			}
		}
		const util::Clock::TTime time = stopWatch_.stop() / static_cast<util::Clock::TTime>(numberOfRuns_ * targets_.size());
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time << " (" << total << ")" << std::endl;
	}
private:
	const Targets& targets_;
	util::StopWatch& stopWatch_;
	size_t numberOfRuns_;
};



template <typename Targets>
class IntersectionsSpeedTest
{
public:
	IntersectionsSpeedTest(const Targets& targets, util::StopWatch& stopWatch, size_t numberOfRuns):
		targets_(targets), stopWatch_(stopWatch), numberOfRuns_(numberOfRuns)
	{
	}
	template <typename Tree> void operator()(const Tree& tree) const
	{
		size_t hits = 0;
		stopWatch_.restart();
		for (size_t k = 0; k < numberOfRuns_; ++k)
		{
			const typename Targets::const_iterator targetEnd = targets_.end();
			for (typename Targets::const_iterator i = targets_.begin(); i != targetEnd; ++i)
			{
				hits += tree.intersects(i->first) ? 1 : 0;
			}
		}
		const util::Clock::TTime time = stopWatch_.stop() / static_cast<util::Clock::TTime>(numberOfRuns_ * targets_.size());
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time << " (" << hits << ")" << std::endl;
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
		const util::Clock::TTime time = stopWatch_.stop() / static_cast<util::Clock::TTime>(numberOfRuns_ * targets_.size());
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time << " (" << total << ")" << std::endl;
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
				count += static_cast<size_t>(last - hits.begin());
			}
		}
		const util::Clock::TTime time = stopWatch_.stop();
		LASS_COUT << std::string(typeid(tree).name()).substr(0, 60) << ": " << time
			<< " (" << count << " hits)" << std::endl;
	}
private:
	const Targets& targets_;
	T maxRadius_;
	size_t maxCount_;
	util::StopWatch& stopWatch_;
	size_t numberOfRuns_;
};

}
}
}

#endif

// EOF
