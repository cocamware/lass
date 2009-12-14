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
 *	Copyright (C) 2004-2009 the Initial Developer.
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
	aabb_(TObjectTraits::aabbEmpty()),
	objects_(),
	nodes_(),
	end_()
{
}



template <typename O, typename OT, typename SH>
AabpTree<O, OT, SH>::AabpTree(TObjectIterator first, TObjectIterator last):
	aabb_(TObjectTraits::aabbEmpty()),
	objects_(),
	nodes_(),
	end_(last)
{
	if (first != last)
	{
		TInputs inputs;
		for (TObjectIterator i = first; i != last; ++i)
		{
			TAabb aabb = TObjectTraits::objectAabb(i);
			aabb_ = TObjectTraits::aabbJoin(aabb_, aabb);
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
bool AabpTree<O, OT, SH>::contains(const TPoint& point, const TInfo* info) const
{
	if (isEmpty() || !TObjectTraits::aabbContains(aabb_, point))
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
	if (isEmpty() || !TObjectTraits::aabbContains(aabb_, point))
	{
		return result;
	}
	return doFind(0, point, result, info);
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabpTree<O, OT, SH>::find(
		const TAabb& box, OutputIterator result, const TInfo* info) const
{
	if (isEmpty() || !TObjectTraits::aabbIntersects(aabb_, box))
	{
		return result;
	}
	return doFind(0, box, result, info);
}



template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::TObjectIterator
AabpTree<O, OT, SH>::intersect(const TRay& ray, TReference t, TParam tMin, const TInfo* info) const
{
	TValue tNear;
	if (isEmpty() || !TObjectTraits::aabbIntersect(aabb_, ray, tNear, tMin))
	{
		return end_;
	}
	TValue tFar;
	if (!TObjectTraits::aabbIntersect(aabb_, ray, tFar, tNear))
	{
		tFar = tNear;
		tNear = tMin;
	}
	const TVector reciprocalDirection = TObjectTraits::vectorReciprocal(TObjectTraits::rayDirection(ray));
	TObjectIterator hit = doIntersect(0, ray, t, tMin, info, reciprocalDirection, tNear, tFar);
	LASS_ASSERT((t > tMin && t >= tNear * (1 - 1e-7f) && t <= tFar * (1 + 1e-7f)) || hit == end_);
	return hit;
}



template <typename O, typename OT, typename SH>
bool AabpTree<O, OT, SH>::intersects(
		const TRay& ray, TParam tMin, TParam tMax, const TInfo* info) const
{
	TValue tNear;
	if (isEmpty() || !TObjectTraits::aabbIntersect(aabb_, ray, tNear, tMin))
	{
		return false;
	}
	TValue tFar;
	if (!TObjectTraits::aabbIntersect(aabb_, ray, tFar, tNear))
	{
		tFar = tNear;
		tNear = tMin;
	}
	if (tNear > tMax || tFar < tMin)
	{
		return false;
	}
	const TVector reciprocalDirection = TObjectTraits::vectorReciprocal(TObjectTraits::rayDirection(ray));
	return doIntersects(0, ray, tMin, tMax, info, reciprocalDirection, tNear, tFar);
}



template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::Neighbour
AabpTree<O, OT, SH>::nearestNeighbour(const TPoint& target, const TInfo* info) const
{
	Neighbour nearest(end_, std::numeric_limits<TValue>::infinity());
	if (!isEmpty())
	{
		doNearestNeighbour(0, target, info, nearest);
	}
	return nearest;
}



template <class O, class OT, typename SH>
template <typename RandomAccessIterator>
RandomAccessIterator
AabpTree<O, OT, SH>::rangeSearch(
		const TPoint& target, TParam maxRadius, size_t maxCount, RandomAccessIterator first, 
		const TInfo* info) const
{
	if (isEmpty() || maxRadius == 0)
	{
		return first;
	}
	TValue squaredRadius = maxRadius * maxRadius;
	return doRangeSearch(0, target, squaredRadius, maxCount, first, first, info);
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
bool AabpTree<O, OT, SH>::isEmpty() const
{
	return objects_.empty();
}



template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::TObjectIterator
AabpTree<O, OT, SH>::end() const
{
	return end_;
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
	node.leftBound() = TObjectTraits::coord(TObjectTraits::aabbMax(left.aabb), split.axis);
	node.rightBound() = TObjectTraits::coord(TObjectTraits::aabbMin(right.aabb), split.axis);
	LASS_ASSERT(left.index == index + 1);
	node.right() = right.index;
	
	return BalanceResult(split.aabb, index);
}



template <typename O, typename OT, typename SH>
int AabpTree<O, OT, SH>::addLeafNode(TInputIterator first, TInputIterator last)
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
int AabpTree<O, OT, SH>::addInternalNode(int axis)
{
	nodes_.push_back(Node(axis));
	LASS_ASSERT(static_cast<int>(nodes_.size()) > 0);
	return static_cast<int>(nodes_.size()) - 1;
}



template <typename O, typename OT, typename SH>
bool AabpTree<O, OT, SH>::doContains(int index, const TPoint& point, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];
	
	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectContains(objects_[i], point, info))
			{
				return true;
			}
		}
		return false;
	}

	const TValue x = TObjectTraits::coord(point, node.axis());
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
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectContains(objects_[i], point, info))
			{
				*result++ = objects_[i];
			}
		}
		return result;
	}

	const TValue x = TObjectTraits::coord(point, node.axis());
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
template <typename OutputIterator>
OutputIterator AabpTree<O, OT, SH>::doFind(
		int index, const TAabb& box, OutputIterator result, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectIntersects(objects_[i], box, info))
			{
				*result++ = objects_[i];
			}
		}
		return result;
	}

	if (TObjectTraits::coord(TObjectTraits::aabbMin(box), node.axis()) <= node.leftBound())
	{
		result = doFind(index + 1, box, result, info);
	}
	if (TObjectTraits::coord(TObjectTraits::aabbMax(box), node.axis()) >= node.rightBound())
	{
		result = doFind(node.right(), box, result, info);
	}
	return result;
}



template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::TObjectIterator
AabpTree<O, OT, SH>::doIntersect(
		int index, const TRay& ray, TReference t, TParam tMin, const TInfo* info,
		const TVector& reciprocalDirection, TParam tNear, TParam tFar) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	LASS_ASSERT(tFar > tNear);
	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		TValue tBest = 0;
		TObjectIterator best = end_;
		for (int i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			TValue tCandidate = 0;
			if (TObjectTraits::objectIntersect(objects_[i], ray, tCandidate, tMin, info))
			{
				LASS_ASSERT(tCandidate > tMin);
				if (best == end_ || tCandidate < tBest)
				{
					LASS_ASSERT(tCandidate > tMin && tCandidate >= tNear * (1 - 1e-6f) && tCandidate <= tFar * (1 + 1e-6f));
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
	const TValue s = TObjectTraits::coord(TObjectTraits::raySupport(ray), node.axis());
	const TValue d = TObjectTraits::coord(TObjectTraits::rayDirection(ray), node.axis());
	const TValue invD = TObjectTraits::coord(reciprocalDirection, node.axis());
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
bool AabpTree<O, OT, SH>::doIntersects(
		int index, const TRay& ray, TParam tMin, TParam tMax, const TInfo* info,
		const TVector& reciprocalDirection, TParam tNear, TParam tFar) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	LASS_ASSERT(tMax > tMin);
	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectIntersects(objects_[i], ray, tMin, tMax, info))
			{
				return true;
			}
		}
		return false;
	}

	// check children
	const int leftIndex = index + 1;
	const int rightIndex = node.right();
	const TValue s = TObjectTraits::coord(TObjectTraits::raySupport(ray), node.axis());
	const TValue d = TObjectTraits::coord(TObjectTraits::rayDirection(ray), node.axis());
	const TValue invD = TObjectTraits::coord(reciprocalDirection, node.axis());
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



template <typename O, typename OT, typename SH>
void AabpTree<O, OT, SH>::doNearestNeighbour(
		int index, const TPoint& target, const TInfo* info, Neighbour& best) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(best.squaredDistance() >= 0);

	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			const TValue squaredDistance = 
				TObjectTraits::objectSquaredDistance(objects_[i], target, info);
			if (squaredDistance < best.squaredDistance())
			{
				best = Neighbour(objects_[i], squaredDistance);
			}
		}
	}
	else
	{
		int children[2];
		TValue signedDist[2];
		getChildren(index, target, children, signedDist);
		if (signedDist[0] <= 0 || (signedDist[0] * signedDist[0]) < best.squaredDistance())
		{
			doNearestNeighbour(children[0], target, info, best);
			if (signedDist[1] <= 0 || (signedDist[1] * signedDist[1]) < best.squaredDistance())
			{
				doNearestNeighbour(children[1], target, info, best);
			}
		}
	}
}



template <typename O, typename OT, typename SH>
template <typename RandomIterator>
RandomIterator AabpTree<O, OT, SH>::doRangeSearch(
	int index, const TPoint& target, TReference squaredRadius, size_t maxCount,
	RandomIterator first, RandomIterator last, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(squaredRadius >= 0);

	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			const TValue sqrDist = TObjectTraits::objectSquaredDistance(objects_[i], target, info);
			if (sqrDist < squaredRadius)
			{
				*last++ = Neighbour(objects_[i], sqrDist);
				std::push_heap(first, last);
				LASS_ASSERT(last >= first);
				if (static_cast<size_t>(last - first) > maxCount)
				{
					std::pop_heap(first, last);
					--last;
					squaredRadius = first->squaredDistance();
				}
			}
		}
		return last;
	}
	
	int children[2];
	TValue signedDist[2];
	getChildren(index, target, children, signedDist);
	if (signedDist[0] <= 0 || (signedDist[0] * signedDist[0]) < squaredRadius)
	{
		last = doRangeSearch(children[0], target, squaredRadius, maxCount, first, last, info);
		if (signedDist[1] <= 0 || (signedDist[1] * signedDist[1]) < squaredRadius)
		{
			last = doRangeSearch(children[1], target, squaredRadius, maxCount, first, last, info);
		}
	}
	return last;
}



template <typename O, typename OT, typename SH>
void AabpTree<O, OT, SH>::getChildren(
		int index, const TPoint& target, int indices[2], TValue signedDistances[2]) const
{
	const Node& node = nodes_[index];
	indices[0] = index + 1;
	indices[1] = node.right();
	const TValue x = TObjectTraits::coord(target, node.axis());
	signedDistances[0] = x - node.leftBound(); 
	signedDistances[1] = node.rightBound() - x; 

	if (signedDistances[1] < signedDistances[0])
	{
		std::swap(signedDistances[0], signedDistances[1]);
		std::swap(indices[0], indices[1]);
	}
}



// --- free ----------------------------------------------------------------------------------------

}

}

#endif

// EOF
