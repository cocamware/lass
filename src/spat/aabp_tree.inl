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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_AABP_TREE_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_AABP_TREE_INL

#include "spat_common.h"
#include "aabp_tree.h"

namespace lass
{
namespace spat
{

// --- public --------------------------------------------------------------------------------------

template <class O, class OT>
AabpTree<O, OT>::AabpTree():
	begin_(),
	end_()
{
}



template <class O, class OT>
AabpTree<O, OT>::AabpTree(TObjectIterator iBegin, TObjectIterator iEnd):
	begin_(iBegin),
	end_(iEnd)
{
	size_ = std::distance(begin_, end_);
	heap_.resize(2 * size_);

	TBoundedObjects input;
    input.reserve(size_);
	for (TObjectIterator i = begin_; i != end_; ++i)
	{
		input.push_back(BoundedObject(TObjectTraits::aabb(i), i));
	}

	aabb_ = findAabb(input.begin(), input.end());
	balance(0, aabb_, input.begin(), input.end());
}



template <class O, class OT>
void AabpTree<O, OT>::reset(TObjectIterator iBegin, TObjectIterator iEnd)
{
	TSelf temp(iBegin, iEnd);
	swap(temp);
}



template <class O, class OT>
bool AabpTree<O, OT>::contains(const TPoint& iPoint) const
{
	if (!TObjectTraits::contains(aabb_, iPoint))
	{
		return false;
	}
	return doContains(0, iPoint);
}



template <class O, class OT>
template <typename OutputIterator>
OutputIterator AabpTree<O, OT>::find(const TPoint& iPoint, OutputIterator iFirst) const
{
	if (!TObjectTraits::contains(aabb_, iPoint))
	{
		return iFirst;
	}
	return doFind(0, iPoint, iFirst);
}



template <class O, class OT>
typename AabpTree<O, OT>::TObjectIterator
AabpTree<O, OT>::intersect(const TRay& iRay, TReference oT, TParam iMin) const
{
	TValue tNear;
	if (TObjectTraits::intersect(aabb_, iRay, tNear, iMin))
	{
		TValue tFar;
		if (!TObjectTraits::intersect(aabb_, iRay, tFar, tNear))
		{
			tFar = tNear;
			tNear = 0;
		}
		TValue t;
		TObjectIterator hit = doIntersect(0, iRay, t, tNear, tFar);
		if (hit != end_)
		{
			LASS_ASSERT(t > iMin);
			oT = t;
			return hit;
		}
	}
	return end_;
}



template <class O, class OT>
void AabpTree<O, OT>::swap(TSelf& iOther)
{
	std::swap(begin_, iOther.begin_);
	std::swap(end_, iOther.end_);
	heap_.swap(iOther.heap_);
	std::swap(aabb_, iOther.aabb_);
	std::swap(size_, iOther.size_);
}



template <class O, class OT>
const bool AabpTree<O, OT>::isEmpty() const
{
	return heap_.empty();
}



template <class O, class OT>
void AabpTree<O, OT>::clear()
{
	TSelf temp;
	swap(temp);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <class O, class OT>
void AabpTree<O, OT>::balance(size_t iIndex, const TAabb& iAabb,
                              TBoundedObjectIterator iBegin, TBoundedObjectIterator iEnd)
{
	if (iEnd == iBegin)
	{
		return;
	}

	if (iEnd == iBegin + 1)
	{
		// exactly one element in content
		assignLeaf(iIndex, iBegin);
		return;
	}

	// more than one, do the balance
	//
	const TAxis split = findSplitAxis(iAabb);
	const ptrdiff_t halfSize = (iEnd - iBegin) / 2;
	const TBoundedObjectIterator median = iBegin + halfSize;
	std::nth_element(iBegin, median, iEnd, LessDim(split));

	const TAabb aabbLeft = findAabb(iBegin, median);
	const TAabb aabbRight = findAabb(iBegin, iEnd);
    const TValue boundLeft = TObjectTraits::coordinate(TObjectTraits::max(aabbLeft), split);
    const TValue boundRight = TObjectTraits::coordinate(TObjectTraits::min(aabbRight), split);

	assignInternal(iIndex, split, boundLeft, boundRight);
	balance(2 * iIndex + 1, aabbLeft, iBegin, median);
	balance(2 * iIndex + 2, aabbRight, median, iEnd);
}



template <class O, class OT>
typename AabpTree<O, OT>::TAabb
AabpTree<O, OT>::findAabb(TBoundedObjectIterator iBegin, TBoundedObjectIterator iEnd) const
{
	TAabb result = iBegin->aabb;     
	for (TBoundedObjectIterator i = iBegin + 1; i != iEnd; ++i)
	{
        result = TObjectTraits::join(result, i->aabb);
	}
	return result;
}



template <class O, class OT>
typename AabpTree<O, OT>::TAxis
AabpTree<O, OT>::findSplitAxis(const TAabb& iAabb) const
{
    const TPoint min = TObjectTraits::min(iAabb);
	const TPoint max = TObjectTraits::max(iAabb);
	TAxis axis = 0;
    TValue maxDistance = TObjectTraits::coordinate(max, 0) - TObjectTraits::coordinate(min, 0);
	for (TAxis k = 1; k < dimension; ++k)
	{
		const TValue distance = TObjectTraits::coordinate(max, k) - TObjectTraits::coordinate(min, k);
		if (distance > maxDistance)
		{
			axis = k;
			maxDistance = distance;
		}
	}

	return axis;
}




template <class O, class OT>
inline void AabpTree<O, OT>::assignLeaf(size_t iIndex, TBoundedObjectIterator iObject)
{
	if (iIndex >= heap_.size())
	{
		heap_.resize(iIndex + 1);
	}
	heap_[iIndex].split = leafNode_;
    heap_[iIndex].object = iObject->object;
}




template <class O, class OT>
inline void AabpTree<O, OT>::assignInternal(size_t iIndex, TAxis iSplitAxis, TParam iLeftBound, TParam iRightBound)
{
	if (iIndex >= heap_.size())
	{
		heap_.resize(iIndex + 1);
	}
	heap_[iIndex].split = iSplitAxis;
    heap_[iIndex].leftBound = iLeftBound;
    heap_[iIndex].rightBound = iRightBound;
}



template <class O, class OT>
bool AabpTree<O, OT>::doContains(size_t iIndex, const TPoint& iPoint) const
{
	LASS_ASSERT(iIndex < heap_.size());
	const Node& node = heap_[iIndex];

    if (node.split == leafNode_)
    {
        // check object in leaf node
        return TObjectTraits::contains(node.object, iPoint);
    }

    // try both childs
    //
	const TValue x = TObjectTraits::coordinate(iPoint, node.split);
    if (x <= node.leftBound && doContains(2 * iIndex + 1, iPoint))
    {
        return true;
    }
    if (x >= node.rightBound && doContains(2 * iIndex + 2, iPoint))
    {
        return true;
    }
    return false;
}



template <class O, class OT>
template <typename OutputIterator>
OutputIterator AabpTree<O, OT>::doFind(size_t iIndex, const TPoint& iPoint, 
									   OutputIterator iFirst) const
{
	LASS_ASSERT(iIndex < heap_.size());
	const Node& node = heap_[iIndex];

    if (node.split == leafNode_)
    {
        // check object in leaf node
        if (TObjectTraits::contains(node.object, iPoint))
		{
			*iFirst = node.object;
			++iFirst;
		}
    }
	else
	{
		// try both childs
		const TValue x = TObjectTraits::coordinate(iPoint, node.split);
		if (x <= node.leftBound)
		{
			iFirst = doFind(2 * iIndex + 1, iPoint, iFirst);
		}
		if (x >= node.rightBound)
		{
			iFirst = doFind(2 * iIndex + 2, iPoint, iFirst);
		}
	}
    return iFirst;
}



template <class O, class OT>
typename AabpTree<O, OT>::TObjectIterator
AabpTree<O, OT>::doIntersect(size_t iIndex, const TRay& iRay, TReference oT, TParam iTMin, TParam iTMax) const
{
	LASS_ASSERT(iIndex < heap_.size());
	LASS_ASSERT(iTMax > iTMin);
	const Node& node = heap_[iIndex];

    if (node.split == leafNode_)
    {
        // check object in leaf node
		TValue t;
		if (TObjectTraits::intersect(node.object, iRay, t, iTMin))
		{
			LASS_ASSERT(t > iTMin && t < iTMax);
			oT = t;
			return node.object;
		}
		return end_;
    }
	else
	{
		// check children
		const size_t leftIndex = 2 * iIndex + 1;
		const size_t rightIndex = leftIndex + 1;
		const TValue s = TObjectTraits::coordinate(TObjectTraits::support(iRay), node.split);
		const TValue d = TObjectTraits::component(TObjectTraits::direction(iRay), node.split);
		const TValue tLeftBound = (node.leftBound - s) / d;
		const TValue tRightBound = (node.rightBound - s) / d;
		
		TValue tLeft;
		TValue tRight;
		TObjectIterator objectLeft;
		TObjectIterator objectRight;
		if (d > 0)
		{
			objectLeft = tLeftBound > iTMin ?
				doIntersect(leftIndex, iRay, tLeft, iTMin, std::min(tLeftBound, iTMax)) : end_;
			objectRight = tRightBound < iTMax ?
				doIntersect(rightIndex, iRay, tRight, std::max(tRightBound, iTMin), iTMax) : end_;
		}
		else if (d < 0)
		{
			objectLeft = tLeftBound < iTMax ?
				doIntersect(leftIndex, iRay, tLeft, std::max(tLeftBound, iTMin), iTMax) : end_;
			objectRight = tRightBound > iTMin ?
				doIntersect(rightIndex, iRay, tRight, iTMin, std::min(tRightBound, iTMax)) : end_;
		}
		else // if (d == TNumTraits::zero)
		{
			objectLeft = s <= node.leftBound ? 
				doIntersect(leftIndex, iRay, tLeft, iTMin, iTMax) : end_;
			objectRight = s >= node.rightBound ?
				doIntersect(rightIndex, iRay, tRight, iTMin, iTMax): end_;
		}
		
		// determine result
		if (objectLeft != end_)
		{
			if (objectRight != end_ && tRight < tLeft)
			{
				LASS_ASSERT(tRight > iTMin && tRight < iTMax);
				oT = tRight;
				return objectRight;
			}
			else
			{
				LASS_ASSERT(tLeft > iTMin && tLeft < iTMax);
				oT = tLeft;
				return objectLeft;
			}
		}
		if (objectRight != end_)
		{
			LASS_ASSERT(tRight > iTMin && tRight < iTMax);
			oT = tRight;
			return objectRight;
		}
		return end_;
	}
	LASS_ASSERT_UNREACHABLE;
}

// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
