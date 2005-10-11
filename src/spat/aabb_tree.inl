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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_AABB_TREE_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_AABB_TREE_INL

#include "spat_common.h"
#include "aabb_tree.h"

namespace lass
{
namespace spat
{

// --- public --------------------------------------------------------------------------------------

template <class O, class OT>
AabbTree<O, OT>::AabbTree():
	begin_(),
	end_()
{
}



template <class O, class OT>
AabbTree<O, OT>::AabbTree(TObjectIterator iBegin, TObjectIterator iEnd):
	begin_(iBegin),
	end_(iEnd)
{
	size_ = std::distance(begin_, end_);
	heap_.resize(2 * size_, Node(TAabb(), end_));

	TNodes input;
	for (TObjectIterator i = begin_; i != end_; ++i)
	{
		input.push_back(Node(TObjectTraits::aabb(i), i));
	}

	balance(0, input.begin(), input.end());
}



template <class O, class OT>
void AabbTree<O, OT>::reset(TObjectIterator iBegin, TObjectIterator iEnd)
{
	TSelf temp(iBegin, iEnd);
	swap(temp);
}



template <class O, class OT> inline
bool AabbTree<O, OT>::contains(const TPoint& iPoint) const
{
	return doContains(0, iPoint);
}



template <class O, class OT>
template <typename OutputIterator> inline
OutputIterator AabbTree<O, OT>::find(const TPoint& iPoint, OutputIterator iFirst) const
{
	return doFind(0, iPoint, iFirst);
}



template <class O, class OT>
typename AabbTree<O, OT>::TObjectIterator inline
AabbTree<O, OT>::intersect(const TRay& iRay, TReference oT, TParam iMinT) const
{
	return doIntersect(0, iRay, oT, iMinT);
}



template <class O, class OT>
void AabbTree<O, OT>::swap(TSelf& iOther)
{
	std::swap(begin_, iOther.begin_);
	std::swap(end_, iOther.end_);
	heap_.swap(iOther.heap_);
	std::swap(size_, iOther.size_);
}



template <class O, class OT>
const bool AabbTree<O, OT>::isEmpty() const
{
	return heap_.empty();
}



template <class O, class OT>
void AabbTree<O, OT>::clear()
{
	TSelf temp;
	swap(temp);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <class O, class OT>
void AabbTree<O, OT>::balance(size_t iIndex, TNodeIterator iBegin, TNodeIterator iEnd)
{
	if (iEnd == iBegin)
	{
		return;
	}

	if (iEnd == iBegin + 1)
	{
		// exactly one element in content
		assignNode(iIndex, *iBegin);
		return;
	}

	// more than one, do the balance
	//
	const TAabb aabb = findAabb(iBegin, iEnd);
	assignNode(iIndex, Node(aabb, end_));

	const TAxis split = findSplitAxis(aabb);
	const ptrdiff_t halfSize = (iEnd - iBegin) / 2;
	const TNodeIterator median = iBegin + halfSize;
	std::nth_element(iBegin, median, iEnd, LessDim(split));

	balance(2 * iIndex + 1, iBegin, median);
	balance(2 * iIndex + 2, median, iEnd);

}



template <class O, class OT>
typename AabbTree<O, OT>::TAabb
AabbTree<O, OT>::findAabb(TNodeIterator iBegin, TNodeIterator iEnd) const
{
	TAabb result = iBegin->aabb;
	for (TNodeIterator i = iBegin + 1; i != iEnd; ++i)
	{
		result = TObjectTraits::join(result, i->aabb);
	}
	return result;
}



template <class O, class OT>
typename AabbTree<O, OT>::TAxis
AabbTree<O, OT>::findSplitAxis(const TAabb& iAabb) const
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
inline void AabbTree<O, OT>::assignNode(size_t iIndex, const Node& iNode)
{
	if (iIndex >= heap_.size())
	{
		heap_.resize(iIndex + 1, Node(TAabb(), end_));
	}
	heap_[iIndex] = iNode;
}



template <class O, class OT>
bool AabbTree<O, OT>::doContains(size_t iIndex, const TPoint& iPoint) const
{
	LASS_ASSERT(iIndex < heap_.size());
	const Node& node = heap_[iIndex];

	if (!TObjectTraits::contains(node.aabb, iPoint))
	{
		return false;
	}

	if (node.object == end_)
	{
		return doContains(2 * iIndex + 1, iPoint) || doContains(2 * iIndex + 2, iPoint);
	}

	return TObjectTraits::contains(node.object, iPoint);
}



template <class O, class OT>
template <typename OutputIterator>
OutputIterator AabbTree<O, OT>::doFind(size_t iIndex, const TPoint& iPoint, OutputIterator iFirst) const
{
	LASS_ASSERT(iIndex < heap_.size());
	const Node& node = heap_[iIndex];

	if (!TObjectTraits::contains(node.aabb, iPoint))
	{
		return iFirst;
	}

	if (node.object == end_)
	{
		OutputIterator temp = doFind(2 * iIndex + 1, iPoint, iFirst);
		return doFind(2 * iIndex + 2, iPoint, temp);
	}

	if (TObjectTraits::contains(node.object, iPoint))
	{
		*iFirst = node.object;
		++iFirst;
	}
	return iFirst;
}



template <class O, class OT>
typename AabbTree<O, OT>::TObjectIterator 
AabbTree<O, OT>::doIntersect(size_t iIndex, const TRay& iRay, TReference oT, TParam iMin) const
{
	LASS_ASSERT(iIndex < heap_.size());
	const Node& node = heap_[iIndex];

	TValue t;
	if (!TObjectTraits::intersect(node.aabb, iRay, t, iMin))
	{
		return end_;
	}

	if (node.object == end_)
	{
		TValue tLeft;
		TObjectIterator left = doIntersect(2 * iIndex + 1, iRay, tLeft, iMin);
		TValue tRight;
		TObjectIterator right = doIntersect(2 * iIndex + 2, iRay, tRight, iMin);

		if (left != end_)
		{
			if (right != end_ && tRight < tLeft)
			{
				oT = tRight;
				return right;
			}
			oT = tLeft;
			return left;
		}
		if (right != end_)
		{
			oT = tRight;
			return right;
		}
		return end_;
	}

	if (TObjectTraits::intersect(node.object, iRay, t, iMin))
	{
		oT = t;
		return node.object;
	}
	return end_;
}

// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
