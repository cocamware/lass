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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_AABP_TREE_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_AABP_TREE_INL

#include "spat_common.h"
#include "aabp_tree.h"

namespace lass
{
namespace spat
{

// --- public --------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
AabpTree<O, OT, SH>::AabpTree():
	aabb_(TObjectTraits::emptyAabb()),
	objects_(),
	nodes_(),
	end_()
{
}



template <typename O, typename OT, typename SH>
AabpTree<O, OT, SH>::AabpTree(TObjectIterator first, TObjectIterator last):
	aabb_(TObjectTraits::emptyAabb()),
	objects_(),
	nodes_(),
	end_(last)
{
	if (first != last)
	{
		TInputs inputs;
		for (TObjectIterator i = first; i != last; ++i)
		{
			TAabb aabb = TObjectTraits::aabb(i);
			aabb_ = TObjectTraits::join(aabb_, aabb);
			inputs.push_back(Input(aabb, i));
		}
		balance(inputs.begin(), inputs.end());
	}
}



template <typename O, typename OT, typename SH>
void AabpTree<O, OT, SH>::reset()
{
	TSelf temp;
	swap(temp);
}



template <typename O, typename OT, typename SH>
void AabpTree<O, OT, SH>::reset(TObjectIterator first, TObjectIterator last)
{
	TSelf temp(first, last);
	swap(temp);
}



template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::TAabb& 
AabpTree<O, OT, SH>::aabb() const
{
	return aabb_;
}



template <typename O, typename OT, typename SH>
const bool AabpTree<O, OT, SH>::contains(const TPoint& point, const TInfo* info) const
{
	if (isEmpty() || !TObjectTraits::contains(aabb_, point))
	{
		return false;
	}
	return doContains(0, point, info);
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabpTree<O, OT, SH>::find(
		const TPoint& point, OutputIterator result, const TInfo* info) const
{
	if (isEmpty() || !TObjectTraits::contains(aabb_, point))
	{
		return result;
	}
	return doFind(0, point, result, info);
}



template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::TObjectIterator
AabpTree<O, OT, SH>::intersect(const TRay& ray, TReference t, TParam minT, const TInfo* info) const
{
	TValue tNear;
	if (isEmpty() || !TObjectTraits::intersect(aabb_, ray, tNear, minT))
	{
		return end_;
	}
	TValue tFar;
	if (!TObjectTraits::intersect(aabb_, ray, tFar, tNear))
	{
		tFar = tNear;
		tNear = minT;
	}
	const TVector reciprocalDirection = TObjectTraits::reciprocal(TObjectTraits::direction(ray));
	TObjectIterator hit = doIntersect(0, ray, t, minT, info, reciprocalDirection, tNear, tFar);
	LASS_ASSERT(t > minT || hit == end_);
	return hit;
}



template <typename O, typename OT, typename SH>
const bool AabpTree<O, OT, SH>::intersects(
		const TRay& ray, TParam minT, TParam maxT, const TInfo* info) const
{
	TValue tNear;
	if (isEmpty() || !TObjectTraits::intersect(aabb_, ray, tNear, minT))
	{
		return false;
	}
	TValue tFar;
	if (!TObjectTraits::intersect(aabb_, ray, tFar, tNear))
	{
		tFar = tNear;
		tNear = minT;
	}
	if (tNear > maxT || tFar < minT)
	{
		return false;
	}
	const TVector reciprocalDirection = TObjectTraits::reciprocal(TObjectTraits::direction(ray));
	return doIntersects(0, ray, minT, maxT, info, reciprocalDirection, tNear, tFar);
}



template <typename O, typename OT, typename SH>
void AabpTree<O, OT, SH>::swap(TSelf& other)
{
	std::swap(aabb_, other.aabb_);
	nodes_.swap(other.nodes_);
	objects_.swap(other.objects_);
	std::swap(end_, other.end_);
}



template <typename O, typename OT, typename SH>
const bool AabpTree<O, OT, SH>::isEmpty() const
{
	return objects_.empty();
}



template <typename O, typename OT, typename SH>
void AabpTree<O, OT, SH>::clear()
{
	TSelf temp;
	swap(temp);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::BalanceResult 
AabpTree<O, OT, SH>::balance(TInputIterator first, TInputIterator last)
{
	const SplitInfo<OT> split = TSplitHeuristics::template split<OT>(first, last);	
	if (split.axis < 0)
	{
		return BalanceResult(split.aabb, addLeafNode(first, last));
	}

	TInputIterator middle = std::partition(first, last, impl::Splitter<TObjectTraits>(split));
	if (middle == first || middle == last)
	{
		const ptrdiff_t halfSize = (last - first) / 2;
		LASS_ASSERT(halfSize > 0);
		middle = first + halfSize;
		std::nth_element(first, middle, last, impl::LessAxis<TObjectTraits>(split.axis));
	}
	LASS_ASSERT(middle != first && middle != last);

	const int index = addInternalNode(split.axis);
	const BalanceResult left = balance(first, middle);
	const BalanceResult right = balance(middle, last);
	
	Node& node = nodes_[index];
	node.leftBound() = TObjectTraits::coordinate(TObjectTraits::max(left.aabb), split.axis);
	node.rightBound() = TObjectTraits::coordinate(TObjectTraits::min(right.aabb), split.axis);
	LASS_ASSERT(left.index == index + 1);
	node.right() = right.index;
	
	return BalanceResult(split.aabb, index);
}



template <typename O, typename OT, typename SH>
const int AabpTree<O, OT, SH>::addLeafNode(TInputIterator first, TInputIterator last)
{
	const int begin = static_cast<int>(objects_.size());
	while (first != last)
	{
		objects_.push_back((first++)->object);
	}
	const int end = static_cast<int>(objects_.size());
	
	LASS_ASSERT(begin >= 0 && end >= 0);
	nodes_.push_back(Node(begin, end));

	LASS_ASSERT(static_cast<int>(nodes_.size()) > 0);
	return static_cast<int>(nodes_.size()) - 1;
}



template <typename O, typename OT, typename SH>
const int AabpTree<O, OT, SH>::addInternalNode(int axis)
{
	nodes_.push_back(Node(axis));
	LASS_ASSERT(static_cast<int>(nodes_.size()) > 0);
	return static_cast<int>(nodes_.size()) - 1;
}



template <typename O, typename OT, typename SH>
const bool AabpTree<O, OT, SH>::doContains(int index, const TPoint& point, const TInfo* info) const
{
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];
	
	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			if (TObjectTraits::contains(objects_[i], point, info))
			{
				return true;
			}
		}
		return false;
	}

	const TValue x = TObjectTraits::coordinate(point, node.axis());
	if (x <= node.leftBound() && doContains(index + 1, point, info))
	{
		return true;
	}
	if (x >= node.rightBound() && doContains(node.right(), point, info))
	{
		return true;
	}
	return false;
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabpTree<O, OT, SH>::doFind(
		int index, const TPoint& point, OutputIterator result, const TInfo* info) const
{
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			if (TObjectTraits::contains(objects_[i], point, info))
			{
				*result++ = objects_[i];
			}
		}
		return result;
	}

	const TValue x = TObjectTraits::coordinate(point, node.axis());
	if (x <= node.leftBound())
	{
		result = doFind(index + 1, point, result, info);
	}
	if (x >= node.rightBound())
	{
		result = doFind(node.right(), point, result, info);
	}
	return result;
}



template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::TObjectIterator
AabpTree<O, OT, SH>::doIntersect(
		int index, const TRay& ray, TReference t, TParam tMin, const TInfo* info,
		const TVector& reciprocalDirection, TParam tNear, TParam tFar) const
{
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	LASS_ASSERT(tFar > tNear);
	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		TValue tBest = 0;
		TObjectIterator best = end_;
		for (int i = node.first(); i != node.last(); ++i)
		{
			TValue tCandidate;
			if (TObjectTraits::intersect(objects_[i], ray, tCandidate, tMin, info))
			{
				LASS_ASSERT(tCandidate > tMin);
				if (best == end_ || tCandidate < tBest)
				{
					best = objects_[i];
					tBest = tCandidate;
				}
			}
		}
		if (best != end_)
		{
			t = tBest;
		}
		return best;
	}

	// check children
	const int leftIndex = index + 1;
	const int rightIndex = node.right();
	const TValue s = TObjectTraits::coordinate(TObjectTraits::support(ray), node.axis());
	const TValue d = TObjectTraits::component(TObjectTraits::direction(ray), node.axis());
	const TValue invD = TObjectTraits::component(reciprocalDirection, node.axis());
	const TValue tLeftBound = (node.leftBound() - s) * invD;
	const TValue tRightBound = (node.rightBound() - s) * invD;
	
	TValue tLeft = 0;
	TValue tRight = 0;
	TObjectIterator objectLeft = end_;
	TObjectIterator objectRight = end_;
	if (d > 0)
	{
		if (tLeftBound > tNear)
		{
			objectLeft = doIntersect(leftIndex, ray, tLeft, tMin, info, reciprocalDirection, 
				tNear, std::min(tLeftBound, tFar));
		}
		if (tRightBound < tFar)
		{
			objectRight = doIntersect(rightIndex, ray, tRight, tMin, info, reciprocalDirection, 
				std::max(tRightBound, tNear), tFar);
		}
	}
	else if (d < 0)
	{
		if (tLeftBound < tFar)
		{
			objectLeft = doIntersect(leftIndex, ray, tLeft, tMin, info, reciprocalDirection, 
				std::max(tLeftBound, tNear), tFar);
		}
		if (tRightBound > tNear)
		{
			objectRight = doIntersect(rightIndex, ray, tRight, tMin, info, reciprocalDirection, 
				tNear, std::min(tRightBound, tFar)); 
		}
	}
	else // if (d == TNumTraits::zero)
	{
		if (s <= node.leftBound())
		{
			objectLeft = doIntersect(leftIndex, ray, tLeft, tMin, info, reciprocalDirection, 
				tNear, tFar);
		}
		if (s >= node.rightBound())
		{
			objectRight = doIntersect(rightIndex, ray, tRight, tMin, info, reciprocalDirection, 
				tNear, tFar);
		}
	}
	
	// determine result
	if (objectLeft != end_ && (objectRight == end_ || tLeft < tRight))
	{
		LASS_ASSERT(tLeft > tMin);
		t = tLeft;
		return objectLeft;
	}
	if (objectRight != end_)
	{
		LASS_ASSERT(objectLeft == end_ || !(tLeft < tRight));
		LASS_ASSERT(tRight > tMin);
		t = tRight;
		return objectRight;
	}
	return end_;
}



template <typename O, typename OT, typename SH>
const bool AabpTree<O, OT, SH>::doIntersects(
		int index, const TRay& ray, TParam tMin, TParam tMax, const TInfo* info,
		const TVector& reciprocalDirection, TParam tNear, TParam tFar) const
{
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	LASS_ASSERT(tMax > tMin);
	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			if (TObjectTraits::intersects(objects_[i], ray, tMin, tMax, info))
			{
				return true;
			}
		}
		return false;
	}

	// check children
	const int leftIndex = index + 1;
	const int rightIndex = node.right();
	const TValue s = TObjectTraits::coordinate(TObjectTraits::support(ray), node.axis());
	const TValue d = TObjectTraits::component(TObjectTraits::direction(ray), node.axis());
	const TValue invD = TObjectTraits::component(reciprocalDirection, node.axis());
	const TValue tLeftBound = (node.leftBound() - s) * invD;
	const TValue tRightBound = (node.rightBound() - s) * invD;
	
	if (d > 0)
	{
		if ((tLeftBound > tNear) && doIntersects(leftIndex, ray, tMin, tMax, info, 
				reciprocalDirection, tNear, std::min(tLeftBound, tFar)))
		{
			return true;
		}
		if ((tRightBound < tFar) && doIntersects(rightIndex, ray, tMin, tMax, info,
				reciprocalDirection, std::max(tRightBound, tNear), tFar))
		{
			return true;
		}
	}
	else if (d < 0)
	{
		if ((tLeftBound < tFar) && doIntersects(leftIndex, ray, tMin, tMax, info,
				reciprocalDirection, std::max(tLeftBound, tNear), tFar))
		{
			return true;
		}
		if ((tRightBound > tNear) && doIntersects(rightIndex, ray, tMin, tMax, info,
				reciprocalDirection, tNear, std::min(tRightBound, tFar)))
		{
			return true;
		}
	}
	else // if (d == TNumTraits::zero)
	{
		if ((s <= node.leftBound()) && doIntersects(rightIndex, ray, tMin, tMax, info,
				reciprocalDirection, tNear, tFar))
		{
			return true;
		}
		if ((s >= node.rightBound()) && doIntersects(rightIndex, ray, tMin, tMax, info,
				reciprocalDirection, tNear, tFar))
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
