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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_AABB_TREE_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_AABB_TREE_INL

#include "spat_common.h"
#include "aabb_tree.h"

namespace lass
{
namespace spat
{

// --- public --------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
AabbTree<O, OT, SH>::AabbTree():
	objects_(),
	nodes_(),
	end_()
{
}



template <typename O, typename OT, typename SH>
AabbTree<O, OT, SH>::AabbTree(TObjectIterator iBegin, TObjectIterator iEnd):
	objects_(),
	nodes_(),
	end_(iEnd)
{
	TInputs inputs;
	for (TObjectIterator i = iBegin; i != iEnd; ++i)
	{
		inputs.push_back(std::make_pair(i, TObjectTraits::aabb(i)));
	}
	if (iBegin != iEnd)
	{
		balance(inputs.begin(), inputs.end());
	}
}



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::reset(TObjectIterator iBegin, TObjectIterator iEnd)
{
	TSelf temp(iBegin, iEnd);
	swap(temp);
}



template <typename O, typename OT, typename SH> inline
const typename AabbTree<O, OT, SH>::TAabb
AabbTree<O, OT, SH>::aabb() const
{
	if (isEmpty())
	{
		return TObjectTraits::emptyAabb();
	}
	return nodes_[0].aabb;
}



template <typename O, typename OT, typename SH> inline
bool AabbTree<O, OT, SH>::contains(const TPoint& iPoint, const TInfo* iInfo) const
{
	if (isEmpty())
	{
		return false;
	}
	return doContains(0, iPoint, iInfo);
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator> inline
OutputIterator AabbTree<O, OT, SH>::find(const TPoint& iPoint, OutputIterator iFirst, 
									 const TInfo* iInfo) const
{
	if (isEmpty())
	{
		return iFirst;
	}
	return doFind(0, iPoint, iFirst, iInfo);
}



template <typename O, typename OT, typename SH>
typename AabbTree<O, OT, SH>::TObjectIterator inline
AabbTree<O, OT, SH>::intersect(const TRay& iRay, TReference oT, TParam iMinT, const TInfo* iInfo) const
{
	if (isEmpty())
	{
		return end_;
	}
	return doIntersect(0, iRay, oT, iMinT, iInfo);
}



template <typename O, typename OT, typename SH>
bool inline AabbTree<O, OT, SH>::intersects(const TRay& iRay, TParam iMinT, TParam iMaxT, const TInfo* iInfo) const
{
	if (isEmpty())
	{
		return false;
	}
	return doIntersects(0, iRay, iMinT, iMaxT, iInfo);
}



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::swap(TSelf& iOther)
{
	nodes_.swap(iOther.nodes_);
	objects_.swap(iOther.objects_);
	std::swap(end_, iOther.end_);
}



template <typename O, typename OT, typename SH>
const bool AabbTree<O, OT, SH>::isEmpty() const
{
	return objects_.empty();
}



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::clear()
{
	TSelf temp;
	swap(temp);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
const int AabbTree<O, OT, SH>::balance(TInputIterator iFirst, TInputIterator iLast)
{
	const SplitInfo<OT> split = TSplitHeuristics::split<OT>(iFirst, iLast);	
	if (split.axis < 0)
	{
		return addLeafNode(split.aabb, iFirst, iLast);
	}

	TInputIterator middle = std::partition(iFirst, iLast, impl::Splitter<TObjectTraits>(split));
	
	if (middle == iFirst || middle == iLast)
	{
		const ptrdiff_t halfSize = (iLast - iFirst) / 2;
		middle = iFirst + halfSize;
		std::nth_element(iFirst, middle, iLast, impl::LessAxis<TObjectTraits>(split.axis));
	}

	LASS_ASSERT(middle != iFirst && middle != iLast);

	const int node = addInternalNode(split.aabb);
	const int left = balance(iFirst, middle);
	LASS_ASSERT(left == node + 1);
	const int right = balance(middle, iLast);
	nodes_[node].right() = right;
	return node;
}



template <typename O, typename OT, typename SH>
const int AabbTree<O, OT, SH>::addLeafNode(
		const TAabb& iAabb, TInputIterator iFirst, TInputIterator iLast)
{
	const int first = objects_.size();
	while (iFirst != iLast)
	{
		TObjectIterator object = iFirst->first;
		objects_.push_back(object);
		++iFirst;
	}
	const int last = objects_.size();

	nodes_.push_back(Node(iAabb, first, last));
	return nodes_.size() - 1;
}



template <typename O, typename OT, typename SH>
const int AabbTree<O, OT, SH>::addInternalNode(const TAabb& iAabb)
{
	nodes_.push_back(Node(iAabb));
	return nodes_.size() - 1;
}



template <typename O, typename OT, typename SH>
bool AabbTree<O, OT, SH>::doContains(int iIndex, const TPoint& iPoint, const TInfo* iInfo) const
{
	LASS_ASSERT(iIndex < nodes_.size());
	const Node& node = nodes_[iIndex];

	if (!TObjectTraits::contains(node.aabb(), iPoint))
	{
		return false;
	}
	if (node.isInternal())
	{
		return doContains(iIndex + 1, iPoint, iInfo) || doContains(node.right(), iPoint, iInfo);
	}
	for (int i = node.first(); i != node.last(); ++i)
	{
		if (TObjectTraits::contains(objects_[i], iPoint, iInfo))
		{
			return true;
		}
	}
	return false;
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabbTree<O, OT, SH>::doFind(int iIndex, const TPoint& iPoint, OutputIterator iOutput,
									   const TInfo* iInfo) const
{
	LASS_ASSERT(iIndex < nodes_.size());
	const Node& node = nodes_[iIndex];

	if (!TObjectTraits::contains(node.aabb(), iPoint))
	{
		return iOutput;
	}
	if (node.isInternal())
	{
		iOutput = doFind(iIndex + 1, iPoint, iOutput, iInfo);
		return doFind(node.right(), iPoint, iOutput, iInfo);
	}
	for (int i = node.first(); i != node.last(); ++i)
	{
		if (TObjectTraits::contains(objects_[i], iPoint, iInfo))
		{
			*iOutput++ = objects_[i];
		}
	}
	return iOutput;
}



template <typename O, typename OT, typename SH>
typename AabbTree<O, OT, SH>::TObjectIterator 
AabbTree<O, OT, SH>::doIntersect(
		int iIndex, const TRay& iRay, TReference oT, TParam iMin, const TInfo* iInfo) const
{
	LASS_ASSERT(iIndex < nodes_.size());
	const Node& node = nodes_[iIndex];

	TValue t;
	if (!TObjectTraits::intersect(node.aabb(), iRay, t, iMin))
	{
		return end_;
	}
	if (node.isInternal())
	{
		TValue tLeft;
		TObjectIterator left = doIntersect(iIndex + 1, iRay, tLeft, iMin, iInfo);
		TValue tRight;
		TObjectIterator right = doIntersect(node.right(), iRay, tRight, iMin, iInfo);

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

	TValue closestT = 0;
	TObjectIterator closestObject = end_;
	for (int i = node.first(); i != node.last(); ++i)
	{
		TValue t;
		if (TObjectTraits::intersect(objects_[i], iRay, t, iMin, iInfo))
		{
			if (closestObject == end_ || t < closestT)
			{
				closestT = t;
				closestObject = objects_[i];
			}
		}
	}
	if (closestObject != end_)
	{
		oT = closestT;
	}
	return closestObject;
}



template <typename O, typename OT, typename SH>
bool AabbTree<O, OT, SH>::doIntersects(
		int iIndex, const TRay& iRay, TParam iMin, const TParam iMaxT, const TInfo* iInfo) const
{
	LASS_ASSERT(iIndex < heap_.size());
	const Node& node = heap_[iIndex];

	TValue t;
	if (!TObjectTraits::intersect(node.aabb(), iRay, t, iMin))
	{
		return false;
	}
	if (node.isInternal())
	{
		return doIntersects(iIndex, iRay, iMin, iMaxT, iInfo)
			|| doIntersects(node.right(), iRay, iMin, iMaxT, iInfo);
	}
	for (int i = node.first(); i != node.last(); ++i)
	{
		if (TObjectTraits::intersects(objects_[i], iRay, iMin, iMaxT, iInfo))
		{
			return true;
		}
	}
	return false;
}



// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
