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

namespace lass
{
namespace spat
{

template
<
	class ObjectType,
	class ObjectTraits = DefaultObjectTraits<ObjectType, typename ObjectType::TAabb, typename ObjectType::TRay>
>
class AabpTree
{
public:

	typedef AabpTree<ObjectType, ObjectTraits> TSelf;

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
	typedef typename TObjectTraits::TParam TReference;
	typedef typename TObjectTraits::TParam TConstReference;

	enum { dimension = TObjectTraits::dimension };

	typedef std::vector<TObjectIterator> TObjectIterators;

	AabpTree();
	AabpTree(TObjectIterator iBegin, TObjectIterator iEnd);

	void reset(TObjectIterator iBegin, TObjectIterator iEnd);

	bool contains(const TPoint& iPoint) const;
	template <typename OutputIterator> 
	OutputIterator find(const TPoint& iPoint, OutputIterator iFirst) const;
	TObjectIterator intersect(const TRay& iRay, TReference oT) const;

	void swap(TSelf& iOther);
	const bool isEmpty() const;
	void clear();

private:

	typedef size_t TAxis;

	enum { leafNode_ = TAxis(-1) };

	struct Node
	{
        TAxis split;
        union
        {
            struct
            {
                TValue leftBound;
                TValue rightBound;
            };
            TObjectIterator object;
        };
	};

	typedef std::vector<Node> TNodes;
	typedef typename TNodes::iterator TNodeIterator;

    struct BoundedObject
    {
        TAabb aabb;
        TObjectIterator object;
		BoundedObject(const TAabb& iAabb, TObjectIterator iObject): aabb(iAabb), object(iObject) {}
    };

	typedef std::vector<BoundedObject> TBoundedObjects;
	typedef typename TBoundedObjects::iterator TBoundedObjectIterator;

	class LessDim
	{
	public:
		LessDim(TAxis iSplit): split_(iSplit) {}
		bool operator()(const BoundedObject& iA, const BoundedObject& iB) const
		{
            return TObjectTraits::component(TObjectTraits::min(iA.aabb), split_) 
                < TObjectTraits::component(TObjectTraits::min(iB.aabb), split_);
		}
	private:
		TAxis split_;
	};

	void balance(size_t iIndex, const TAabb& iAabb,
        TBoundedObjectIterator iBegin, TBoundedObjectIterator iEnd);
	TAabb findAabb(TBoundedObjectIterator iBegin, TBoundedObjectIterator iEnd) const;
	TAxis findSplitAxis(const TAabb& iAabb) const;
	void assignLeaf(size_t iIndex, TBoundedObjectIterator iObject);
	void assignInternal(size_t iIndex, TAxis iSplit, TParam iLeftBound, TParam iRightBound);

	bool doContains(size_t iIndex, const TPoint& iPoint) const;
	template <typename OutputIterator> 
	OutputIterator doFind(size_t iIndex, const TPoint& iPoint, OutputIterator iFirst) const;
	TObjectIterator doIntersect(size_t iIndex, const TRay& iRay, TReference ioTNear, TReference ioTFar) const;

	TNodes heap_;
	TAabb aabb_;
	TObjectIterator begin_;
	TObjectIterator end_;
	size_t size_;
};


}

}

#include "aabp_tree.inl"

#endif

// EOF
