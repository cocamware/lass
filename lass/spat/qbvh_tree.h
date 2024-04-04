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
 *	Copyright (C) 2024 the Initial Developer.
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



/** @class lass::spat::QbvhTree
 *  @brief an 4-ary AABB tree
 *  @author Bram de Greve [BdG]
 * 
 *  This BVH is much like an AABB tree, but with a branching factor of 4. It is
 *  constructed in similar fashion, by recursively splitting the objects into
 *  2 sets along an axis, but then the sets are split again, totalling 4 sets per
 *  node.
 * 
 *  This tree, when used with SAHSplitHeuristics, should be the fastest for ray
 *  intersection tests. For other operations, it's generally faster than AabbTree
 *  if AVX is available. But as always, YMMV.
 *
 *  The QbvhTree does NOT own the objects.  You must keep them yourself!
 * 
 *  @note H. Dammertz, J. Hanika, and A. Keller. Shallow bounding volume hierarchies 
 *    for fast SIMD ray tracing of incoherent rays. In Proceedings of the Nineteenth 
 *    Eurographics conference on Rendering 2008 (EGSR '08). Eurographics Association, 
 *    Goslar, DEU, 1225–1233. https://doi.org/10.1111/j.1467-8659.2008.01261.x
 * 
 *  @note T. Barnes. Fast, Branchless Ray/Bounding Box Intersections, Part 3: Boundaries
 *    https://tavianator.com/2022/ray_box_boundary.html
 * 
 */

#pragma once

#include "spat_common.h"
#include "default_object_traits.h"
#include "split_heuristics.h"

#if LASS_HAVE_AVX
#include <immintrin.h>
#include <xmmintrin.h>
#endif

namespace lass
{
namespace spat
{
namespace impl::qbvh
{

template <typename T, size_t D>
struct Point
{
	T x[D];
};

template <typename T, size_t D>
struct Ray
{
	T support[D];
	T invDir[D];
	int sign[D];
};
template <typename T, size_t D>
struct Aabb
{
	T corners[D][2];
};

template <typename T, size_t D>
struct QAabb
{
	T corners[4][D][2];
	void set(size_t k, const Aabb<T, D>& aabb);
};

#if LASS_HAVE_AVX

template <size_t D>
struct alignas(16) QAabb<float, D>
{
	__m128 corners[D][2];
	void set(size_t k, const Aabb<float, D>& aabb);
};

template <size_t D>
struct alignas(32) QAabb<double, D>
{
	__m256d corners[D][2];
	void set(size_t k, const Aabb<double, D>& aabb);
};

#endif

}


template
<
	typename ObjectType,
	typename ObjectTraits = DefaultObjectTraits<ObjectType>,
	typename SplitHeuristics = DefaultSplitHeuristics
>
class QbvhTree: public SplitHeuristics
{
public:

	using TSelf = QbvhTree<ObjectType, ObjectTraits, SplitHeuristics>;

	using TObject = ObjectType;
	using TObjectTraits = ObjectTraits;
	using TSplitHeuristics = SplitHeuristics;

	using TObjectIterator = typename TObjectTraits::TObjectIterator;
	using TObjectReference = typename TObjectTraits::TObjectReference;
	using TAabb = typename TObjectTraits::TAabb;
	using TRay = typename TObjectTraits::TRay;
	using TPoint = typename TObjectTraits::TPoint;
	using TVector = typename TObjectTraits::TVector;
	using TValue = typename TObjectTraits::TValue;
	using TParam = typename TObjectTraits::TParam;
	using TReference = typename TObjectTraits::TReference;
	using TConstReference = typename TObjectTraits::TConstReference;
	using TInfo = typename TObjectTraits::TInfo;

	using TNumTraits = num::NumTraits<TValue>;

	using TObjectIterators = std::vector<TObjectIterator>;

	class Neighbour
	{
	public:
		Neighbour() {}
		Neighbour(TObjectIterator object, TValue squaredDistance):
			object_(object), squaredDistance_(squaredDistance) {}
		TObjectIterator object() const { return object_; }
		TValue squaredDistance() const { return squaredDistance_; }
		TObjectIterator operator->() const { return object_; }
		TObjectReference operator*() const { return TObjectTraits::object(object_); }
		bool operator<(const Neighbour& other) const { return squaredDistance_ < other.squaredDistance_; }
	private:
		TObjectIterator object_;
		TValue squaredDistance_;
	};

	constexpr static size_t dimension = TObjectTraits::dimension;
	constexpr static size_t defaultMaxObjectsPerLeaf = 1;
	constexpr static size_t defaultMaxDepth = 20;

	QbvhTree(TSplitHeuristics heuristics = TSplitHeuristics(defaultMaxObjectsPerLeaf, defaultMaxDepth));
	QbvhTree(TObjectIterator first, TObjectIterator last, TSplitHeuristics heuristics = TSplitHeuristics(defaultMaxObjectsPerLeaf, defaultMaxDepth));
	QbvhTree(TSelf&& other) noexcept;

	TSelf& operator=(TSelf&& other) noexcept;

	void reset();
	void reset(TObjectIterator first, TObjectIterator last);

	const TAabb aabb() const;

	bool contains(const TPoint& point, const TInfo* info = 0) const;

	template <typename OutputIterator> 
	OutputIterator find(const TPoint& point, OutputIterator result, const TInfo* info = 0) const;
	template <typename OutputIterator>
	OutputIterator find(const TAabb& box, OutputIterator result, const TInfo* info = 0) const;
	template <typename OutputIterator>
	OutputIterator find(const TRay& ray, TParam tMin, TParam tMax, OutputIterator result, const TInfo* info = 0) const;

	TObjectIterator intersect(const TRay& ray, TReference t, TParam tMin = 0, const TInfo* info = 0) const;
	bool intersects(const TRay& ray, TParam tMin = 0, TParam tMax = std::numeric_limits<TValue>::infinity(), const TInfo* info = 0) const;

	const Neighbour nearestNeighbour(const TPoint& point, const TInfo* info = 0) const;
	template <typename RandomIterator>
	RandomIterator rangeSearch(const TPoint& center, TParam maxRadius, size_t maxCount, RandomIterator first, const TInfo* info = 0) const;

	bool isEmpty() const;
	const TObjectIterator end() const;
	void swap(TSelf& other);

private:

	constexpr static size_t stackSize_ = 128;

	using TIndex = unsigned;
	using TAxis = int;

	struct Input
	{
		TAabb aabb;
		TObjectIterator object;
		Input(const TAabb& aabb, TObjectIterator object): aabb(aabb), object(object) {}
	};
	using TInputs = std::vector<Input>;
	using TInputIterator = typename TInputs::iterator;

	class Child
	{
	public:
		constexpr static size_t countBits = 4; ///< number of bits for object count
		constexpr static TIndex maxCount = 1 << countBits; ///< max number of objects per node
		constexpr static TIndex maxObject = static_cast<TIndex>(std::numeric_limits<int>::max()) >> countBits; ///< max object index
		constexpr static TIndex maxNode = static_cast<TIndex>(std::numeric_limits<int>::max()) - 1; ///< max node index

		/** Constructs an empty node */
		Child(): index_(0) {}

		/** Constructs an internal node */
		explicit Child(TIndex node):
			index_(static_cast<int>(node + 1))
		{ 
			LASS_ASSERT(index_ > 0);
		}

		/** Constructs a leaf node with the given range of objects. */
		Child(TIndex first, TIndex count):
			index_( -static_cast<int>((first << countBits) | (count - 1)) - 1 )
		{
			LASS_ASSERT(index_ < 0);
			LASS_ASSERT((first >= 0) && (count > 0 && count <= maxCount) && (first + count -1 <= maxObject));
		}

		bool isEmpty() const { return !index_; }
		bool isInternal() const { return index_ >  0; } ///< true if child is internal node
		bool isLeaf() const { return index_ < 0; } ///< true if child contains objects
		TIndex node() const { LASS_ASSERT(index_ > 0); return static_cast<TIndex>(index_ - 1); } ///< index of internal child node
		TIndex first() const { LASS_ASSERT(index_ < 0); return static_cast<TIndex>(-(index_ + 1)) >> countBits; } ///< index of first object in leaf node
		TIndex count() const { LASS_ASSERT(index_ < 0); return (static_cast<TIndex>(-(index_ + 1)) & (maxCount - 1)) + 1; } /// < number of objects in leaf node

	private:
		int index_;
	};

	using TPoint_ = impl::qbvh::Point<TValue, dimension>;
	using TRay_ = impl::qbvh::Ray<TValue, dimension>;
	using TAabb_ = impl::qbvh::Aabb<TValue, dimension>;
	using TQAabb_ = impl::qbvh::QAabb<TValue, dimension>;

	struct alignas(128) Node
	{
		TQAabb_ bounds;
		Child children[4];
		TAxis axis[3];
		int usedMask; ///< bit mask of non-empty children
	};

	using TNodes = std::vector<Node>;

	using TSplitInfo = SplitInfo<TObjectTraits>;

	Child balance(TInputIterator first, TInputIterator last, TAabb& bounds);
	TSplitInfo forceSplit(const TAabb& bounds);

	bool doContains(Child root, const TPoint& point, const TInfo* info) const;

	template <typename OutputIterator> 
	OutputIterator doFind(Child root, const TPoint& point, OutputIterator result, const TInfo* info) const;
	template <typename OutputIterator> 
	OutputIterator doFind(Child root, const TAabb& box, OutputIterator result, const TInfo* info) const;
	template <typename OutputIterator> 
	OutputIterator doFind(Child root, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax, OutputIterator result, const TInfo* info) const;

	TObjectIterator doIntersect(Child root, const TRay& ray, const TVector& invDir, TReference t, TParam tMin, const TInfo* info) const;
	bool doIntersects(Child root, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax, const TInfo* info) const;

	void doNearestNeighbour(Child root, const TPoint& point, const TInfo* info, Neighbour& best) const;
	template <typename RandomIterator>
	RandomIterator doRangeSearch(Child root, const TPoint& center, TReference squaredRadius, size_t maxCount, RandomIterator first, RandomIterator last, const TInfo* info) const;

	bool volumeIntersect(const TAabb& box, const TRay& ray, const TVector& invDir, TReference t, TParam tMin) const;
	bool volumeIntersects(const TAabb& box, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax) const;

	static TPoint_ makePoint(const TPoint& point);
	static TRay_ makeRay(const TRay& ray);
	static TAabb_ makeAabb(const TAabb& aabb);

	TAabb aabb_;
	TNodes nodes_;
	TObjectIterators objects_;
	std::unique_ptr<TObjectIterator> end_;
	Child root_;
};

} // namespace spat
} // namespace lass

#include "qbvh_tree.inl"
