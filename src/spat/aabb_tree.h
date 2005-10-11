/** @file
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

namespace lass
{
namespace spat
{

template
<
	class ObjectType,
	class ObjectTraits = DefaultObjectTraits<ObjectType, typename ObjectType::TAabb, typename ObjectType::TRay>
>
class AabbTree
{
public:

	typedef AabbTree<ObjectType, ObjectTraits> TSelf;

	typedef ObjectType TObject;
	typedef ObjectTraits TObjectTraits;

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

	enum { dimension = TObjectTraits::dimension };

	typedef std::vector<TObjectIterator> TObjectIterators;

	AabbTree();
	AabbTree(TObjectIterator iBegin, TObjectIterator iEnd);

	void reset(TObjectIterator iBegin, TObjectIterator iEnd);

	bool contains(const TPoint& iPoint) const;
	template <typename OutputIterator> 
	OutputIterator find(const TPoint& iPoint, OutputIterator iFirst) const;
	TObjectIterator intersect(const TRay& iRay, TReference oT, TParam iMinT = 0) const;

	void swap(TSelf& iOther);
	const bool isEmpty() const;
	void clear();

private:

	struct Node
	{
		TAabb aabb;
		TObjectIterator object;
		Node(const TAabb& iAabb, TObjectIterator iObject): aabb(iAabb), object(iObject) {}
	};

	typedef std::vector<Node> TNodes;
	typedef typename TNodes::iterator TNodeIterator;

	typedef size_t TAxis;

	class LessDim
	{
	public:
		LessDim(TAxis iSplit): split_(iSplit) {}
		bool operator()(const Node& iA, const Node& iB) const
		{
			return TObjectTraits::coordinate(TObjectTraits::min(iA.aabb), split_)
				< TObjectTraits::coordinate(TObjectTraits::min(iB.aabb), split_);
		}
	private:
		TAxis split_;
	};

	void balance(size_t iIndex, TNodeIterator iBegin, TNodeIterator iEnd);
	TAabb findAabb(TNodeIterator iBegin, TNodeIterator iEnd) const;
	TAxis findSplitAxis(const TAabb& iAabb) const;
	void assignNode(size_t iIndex, const Node& iObject);

	bool doContains(size_t iIndex, const TPoint& iPoint) const;
	template <typename OutputIterator> 
	OutputIterator doFind(size_t iIndex, const TPoint& iPoint, OutputIterator iFirst) const;
	TObjectIterator doIntersect(size_t iIndex, const TRay& iRay, TReference oT, TParam iMinT) const;

	static TValue squaredDistance(const TPoint& iA, const TPoint& iB);

	TNodes heap_;
	TObjectIterator begin_;
	TObjectIterator end_;
	size_t size_;
};


}

}

#include "aabb_tree.inl"

#endif

// EOF
