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



/** @class lass::spat::AabpTree
 *  @brief an AABB bounding volume tree that looks similar to a KdTree
 *  @author Bram de Greve [BdG]
 *
 *  the AabbTree does NOT own the objects.  You must keep them yourself!
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_AABP_TREE_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_AABP_TREE_H

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
	class ObjectType,
	class ObjectTraits = DefaultObjectTraits<ObjectType>,
	typename SplitHeuristics = DefaultSplitHeuristics<>
>
class AabpTree
{
public:

	typedef AabpTree<ObjectType, ObjectTraits> TSelf;

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

	AabpTree();
	AabpTree(TObjectIterator iBegin, TObjectIterator iEnd);

	void reset(TObjectIterator iBegin, TObjectIterator iEnd);

	const TAabb& aabb() const;

	const bool contains(const TPoint& iPoint, const TInfo* iInfo = 0) const;
	template <typename OutputIterator> 
	OutputIterator find(const TPoint& iPoint, OutputIterator iResult, const TInfo* iInfo = 0) const;
	const TObjectIterator intersect(const TRay& iRay, TReference oT, TParam iMinT = 0, 
		const TInfo* iInfo = 0) const;
	const bool intersects(const TRay& iRay, TParam iMinT = 0, 
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
		Node(int iAxis):
			axis_(iAxis),
			first_(-1) 
		{
			LASS_ASSERT(iAxis >= 0 && iAxis < dimension);
		}
		Node(int iFirst, int iLast):
			axis_(-1),
			first_(iFirst)
		{
			LASS_ASSERT(iFirst >= 0 && iLast > iFirst);
			last_ = iLast; // do union stuff here
		}

		const bool isInternal() const { return first_ < 0; }
		TParam leftBound() const { LASS_ASSERT(isInternal()); return leftBound_; }
		TReference leftBound() { LASS_ASSERT(isInternal()); return leftBound_; }
		TParam rightBound() const { LASS_ASSERT(isInternal()); return rightBound_; }
		TReference rightBound() { LASS_ASSERT(isInternal()); return rightBound_; }
		const int axis() const { LASS_ASSERT(isInternal()); return axis_; }
		const int right() const { LASS_ASSERT(isInternal()); return right_; }
		int& right() { LASS_ASSERT(isInternal()); return right_; }

		const bool isLeaf() const { return first_ >= 0; }
		const int first() const { LASS_ASSERT(isLeaf()); return first_; }
		const int last() const { LASS_ASSERT(isLeaf()); return last_; }

	private:
		TValue leftBound_;	// internal
		TValue rightBound_;	// internal
		int axis_; // internal
		int first_; // ==-1:internal, >=0:leaf
		union 
		{
			int right_; // internal
			int last_; // leaf
		};
	};

	typedef std::vector<Node> TNodes;
	typedef std::vector<TObjectIterator> TObjectIterators;

	const std::pair<int, TAabb> balance(TInputIterator iFirst, TInputIterator iLast);
	const int addLeafNode(TInputIterator iFirst, TInputIterator iLast);
	const int addInternalNode(int iAxis);

	const bool doContains(int iIndex, const TPoint& iPoint, const TInfo* iInfo) const;
	template <typename OutputIterator> 
	OutputIterator doFind(int iIndex, const TPoint& iPoint, 
		OutputIterator iResult, const TInfo* iInfo) const;
	const TObjectIterator doIntersect(int iIndex, const TRay& iRay, TReference oT, TParam iTMin,
		const TInfo* iInfo, const TVector& iReciprocalDirection, TParam iTNear, TParam iTFar) const;
	const bool doIntersects(int iIndex, const TRay& iRay, TParam iTMin, TParam iMax,
		const TInfo* iInfo, const TVector& iReciprocalDirection, TParam iTNear, TParam iTFar) const;

	TAabb aabb_;
	TObjectIterators objects_;
	TNodes nodes_;
	TObjectIterator end_;
};

}

}

#include "aabp_tree.inl"

#endif

// EOF
