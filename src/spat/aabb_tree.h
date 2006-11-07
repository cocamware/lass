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
	AabbTree(TObjectIterator iBegin, TObjectIterator iEnd);

	void reset(TObjectIterator iBegin, TObjectIterator iEnd);

	const TAabb aabb() const;

	bool contains(const TPoint& iPoint, const TInfo* iInfo = 0) const;
	template <typename OutputIterator> 
	OutputIterator find(const TPoint& iPoint, OutputIterator iResult, const TInfo* iInfo = 0) const;
	TObjectIterator intersect(const TRay& iRay, TReference oT, TParam iMinT = 0, 
		const TInfo* iInfo = 0) const;
	bool intersects(const TRay& iRay, TParam iMinT = 0, 
		TParam iMaxT = std::numeric_limits<TValue>::infinity(), const TInfo* iInfo = 0) const;

	void swap(TSelf& iOther);
	const bool isEmpty() const;
	void clear();

private:

	typedef std::pair<TObjectIterator, TAabb> TInput;
	typedef std::vector<TInput> TInputs;
	typedef typename TInputs::iterator TInputIterator;

	class Node
	{
	public:
		explicit Node(const TAabb& iAabb): 
			aabb_(iAabb), 
			first_(-1) 
		{
		}
		Node(const TAabb& iAabb, int iFirst, int iLast): 
			aabb_(iAabb), 
			first_(iFirst)
		{
			LASS_ASSERT(iFirst >= 0 && iLast > iFirst);
			last_ = iLast; // do union stuff here
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
	typedef std::vector<TObjectIterator> TObjectIterators;

	const int balance(TInputIterator iFirst, TInputIterator iLast);
	const int addLeafNode(const TAabb& iAabb, TInputIterator iFirst, TInputIterator Last);
	const int addInternalNode(const TAabb& iAabb);

	bool doContains(int iIndex, const TPoint& iPoint, const TInfo* iInfo) const;
	template <typename OutputIterator> 
	OutputIterator doFind(int iIndex, const TPoint& iPoint, OutputIterator iFirst, const TInfo* iInfo) const;
	TObjectIterator doIntersect(int iIndex, const TRay& iRay, TReference oT, TParam iMinT, const TInfo* iInfo) const;
	bool doIntersects(int iIndex, const TRay& iRay, TParam iMinT, TParam iMaxT, const TInfo* iInfo) const;

	TObjectIterators objects_;
	TNodes nodes_;
	TObjectIterator end_;
};


}

}

#include "aabb_tree.inl"

#endif

// EOF
