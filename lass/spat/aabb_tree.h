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



/** @class lass::spat::AabbTree
 *  @brief an AABB bounding volume tree that looks similar to a KdTree
 *  @author Bram de Greve [BdG]
 *
 *  the AabbTree does NOT own the objects.  You must keep them yourself!
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_AABB_TREE_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_AABB_TREE_H

//#define LASS_SPAT_AABB_TREE_DIAGNOSTICS

#ifndef LASS_SPAT_AABB_TREE_STACK_SIZE
#	define LASS_SPAT_AABB_TREE_STACK_SIZE 32
#endif

#include "spat_common.h"
#include "default_object_traits.h"
#include "split_heuristics.h"
#include "../util/shared_ptr.h"

namespace lass
{
namespace spat
{

template
<
	typename ObjectType,
	typename ObjectTraits = DefaultObjectTraits<ObjectType>,
	typename SplitHeuristics = DefaultSplitHeuristics
>
class AabbTree: public SplitHeuristics
{
public:

	typedef AabbTree<ObjectType, ObjectTraits, SplitHeuristics> TSelf;

	typedef ObjectType TObject;
	typedef ObjectTraits TObjectTraits;
	typedef SplitHeuristics TSplitHeuristics;

	typedef typename TObjectTraits::TObjectIterator TObjectIterator;
	typedef typename TObjectTraits::TObjectReference TObjectReference;
	typedef typename TObjectTraits::TAabb TAabb;
	typedef typename TObjectTraits::TRay TRay;
	typedef typename TObjectTraits::TPoint TPoint;
	typedef typename TObjectTraits::TVector TVector;
	typedef typename TObjectTraits::TValue TValue;
	typedef typename TObjectTraits::TParam TParam;
	typedef typename TObjectTraits::TReference TReference;
	typedef typename TObjectTraits::TConstReference TConstReference;
	typedef typename TObjectTraits::TInfo TInfo;

	enum 
	{ 
		dimension = TObjectTraits::dimension,
		defaultMaxObjectsPerLeaf = 1,
		defaultMaxDepth = 20
	};

	typedef std::vector<TObjectIterator> TObjectIterators;

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

	AabbTree(const TSplitHeuristics& heuristics = TSplitHeuristics(defaultMaxObjectsPerLeaf, defaultMaxDepth));
	AabbTree(TObjectIterator first, TObjectIterator last, const TSplitHeuristics& heuristics = TSplitHeuristics(defaultMaxObjectsPerLeaf, defaultMaxDepth));
	AabbTree(TSelf&& other) noexcept;

	TSelf & operator=(TSelf&& other) noexcept;

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
	bool intersects(const TRay& ray, TParam tMin = 0, 
		TParam tMax = std::numeric_limits<TValue>::infinity(), const TInfo* info = 0) const;
	const Neighbour nearestNeighbour(const TPoint& point, const TInfo* info = 0) const;
	template <typename RandomIterator>
	RandomIterator rangeSearch(const TPoint& center, TParam maxRadius, size_t maxCount,
			RandomIterator first, const TInfo* info = 0) const;	

	bool isEmpty() const;
	const TObjectIterator end() const;
	void swap(TSelf& other);

private:

	typedef unsigned TIndex;
	struct Input
	{
		TAabb aabb;
		TObjectIterator object;
		Input(const TAabb& aabb, TObjectIterator object): aabb(aabb), object(object) {}
	};
	typedef std::vector<Input> TInputs;
	typedef typename TInputs::iterator TInputIterator;

	class Node
	{
	public:
		static constexpr TIndex sentinelInternal = std::numeric_limits<TIndex>::max();

		explicit Node(const TAabb& aabb):
			aabb_(aabb), 
			first_(sentinelInternal) 
		{
		}
		Node(const TAabb& aabb, TIndex first, TIndex last):
			aabb_(aabb), 
			first_(first)
		{
			LASS_ASSERT(first < sentinelInternal && last < sentinelInternal && last > first);
			last_ = last; // do union stuff here
		}

		bool isInternal() const { return first_ == sentinelInternal; }
		const TAabb& aabb() const { return aabb_; }
		TIndex right() const { LASS_ASSERT(isInternal()); return right_; }
		void setRight(TIndex right) { LASS_ASSERT(isInternal()); right_ = right; }

		bool isLeaf() const { return !isInternal(); }
		TIndex first() const { LASS_ASSERT(isLeaf()); return first_; }
		TIndex last() const { LASS_ASSERT(isLeaf()); return last_; }
	private:
		TAabb aabb_; // both
		TIndex first_; // ==sentinelInternal:internal, else:leaf
		union 
		{
			TIndex right_; // internal
			TIndex last_; // leaf
		};
	};
	typedef std::vector<Node> TNodes;

	TIndex balance(TInputIterator first, TInputIterator last);
	TIndex addLeafNode(const TAabb& aabb, TInputIterator first, TInputIterator last);
	TIndex addInternalNode(const TAabb& aabb);

	bool doContains(TIndex index, const TPoint& point, const TInfo* info) const;

	template <typename OutputIterator> 
	OutputIterator doFind(TIndex index, const TPoint& point, OutputIterator first, const TInfo* info) const;
	template <typename OutputIterator> 
	OutputIterator doFind(TIndex index, const TAabb& aabb, OutputIterator first, const TInfo* info) const;
	template <typename OutputIterator> 
	OutputIterator doFind(TIndex index, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax, OutputIterator first, const TInfo* info) const;

	TObjectIterator doIntersect(TIndex index, const TRay& ray, const TVector& invDir, TReference t, TParam tMin, const TInfo* info) const;
	bool doIntersects(TIndex iIndex, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax, const TInfo* info) const;
	void doNearestNeighbour(TIndex index, const TPoint& point, const TInfo* info, Neighbour& best) const;
	template <typename RandomIterator>
	RandomIterator doRangeSearch(TIndex index, const TPoint& point, TReference squaredRadius,
		size_t maxCount, RandomIterator first, RandomIterator last, const TInfo* info) const;

	void getChildren(TIndex index, const TPoint& target, TIndex indices[2], TValue squaredDistances[2]) const;
	bool volumeIntersect(const TAabb& box, const TRay& ray, const TVector& invDir, TReference t, TParam tMin) const;
	bool volumeIntersects(const TAabb& box, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax) const;

	TObjectIterators objects_;
	TNodes nodes_;
	std::unique_ptr<TObjectIterator> end_;
};


}

}

#include "aabb_tree.inl"

#endif

// EOF
