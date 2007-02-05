/** @file
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



/** @class lass::spat::AabbTree
 *  @brief an AABB bounding volume tree that looks similar to a KdTree
 *  @author Bram de Greve [BdG]
 *
 *  the AabbTree does NOT own the objects.  You must keep them yourself!
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_AABB_TREE_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_AABB_TREE_H

//#define LASS_SPAT_AABB_TREE_DIAGNOSTICS

#include "spat_common.h"
#include "default_object_traits.h"
#include "split_heuristics.h"

namespace lass
{
namespace spat
{

template
<
	typename ObjectType,
	typename ObjectTraits = DefaultObjectTraits<ObjectType>,
	typename SplitHeuristics = DefaultSplitHeuristics<>
>
class AabbTree
{
public:

	typedef AabbTree<ObjectType, ObjectTraits> TSelf;

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

	enum { dimension = TObjectTraits::dimension };

	typedef std::vector<TObjectIterator> TObjectIterators;

	AabbTree();
	AabbTree(TObjectIterator first, TObjectIterator last);

	void reset(TObjectIterator first, TObjectIterator last);

	const TAabb aabb() const;

	bool contains(const TPoint& point, const TInfo* info = 0) const;
	template <typename OutputIterator> 
	OutputIterator find(const TPoint& point, OutputIterator result, const TInfo* info = 0) const;
	TObjectIterator intersect(const TRay& ray, TReference t, TParam tMin = 0, const TInfo* info = 0) const;
	bool intersects(const TRay& ray, TParam tMin = 0, 
		TParam tMax = std::numeric_limits<TValue>::infinity(), const TInfo* info = 0) const;

	void swap(TSelf& other);
	const bool isEmpty() const;
	void clear();

private:

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
		explicit Node(const TAabb& aabb): 
			aabb_(aabb), 
			first_(-1) 
		{
		}
		Node(const TAabb& aabb, int first, int last): 
			aabb_(aabb), 
			first_(first)
		{
			LASS_ASSERT(first >= 0 && last > first);
			last_ = last; // do union stuff here
		}

		const bool isInternal() const { return first_ < 0; }
		const TAabb& aabb() const { return aabb_; }
		const int right() const { LASS_ASSERT(isInternal()); return right_; }
		int& right() { LASS_ASSERT(isInternal()); return right_; }

		const bool isLeaf() const { return first_ >= 0; }
		const int first() const { LASS_ASSERT(isLeaf()); return first_; }
		const int last() const { LASS_ASSERT(isLeaf()); return last_; }
	private:
		TAabb aabb_; // both
		int first_; // ==-1:internal, >=0:leaf
		union 
		{
			int right_; // internal
			int last_; // leaf
		};
	};
	typedef std::deque<Node> TNodes;

	const int balance(TInputIterator first, TInputIterator last);
	const int addLeafNode(const TAabb& aabb, TInputIterator first, TInputIterator last);
	const int addInternalNode(const TAabb& aabb);

	bool doContains(int index, const TPoint& point, const TInfo* info) const;
	template <typename OutputIterator> 
	OutputIterator doFind(int index, const TPoint& point, OutputIterator first, const TInfo* info) const;
	TObjectIterator doIntersect(int index, const TRay& ray, TReference t, TParam tMin, const TInfo* info) const;
	bool doIntersects(int iIndex, const TRay& ray, TParam tMin, TParam tMax, const TInfo* info) const;

	TObjectIterators objects_;
	TNodes nodes_;
	TObjectIterator end_;
};


}

}

#include "aabb_tree.inl"

#endif

// EOF
