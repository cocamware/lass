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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_AABB_TREE_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_AABB_TREE_INL

#include "spat_common.h"
#include "aabb_tree.h"

#if LASS_HAVE_STDDEF_H
#	include <stddef.h>
#endif

namespace lass
{
namespace spat
{

// --- public --------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
AabbTree<O, OT, SH>::AabbTree(const TSplitHeuristics& heuristics):
	SH(heuristics),
	objects_(),
	nodes_(),
	end_(new TObjectIterator)
{
}



template <typename O, typename OT, typename SH>
AabbTree<O, OT, SH>::AabbTree(TObjectIterator first, TObjectIterator last, const TSplitHeuristics& heuristics):
	SH(heuristics),
	objects_(),
	nodes_(),
	end_(new TObjectIterator(last))
{
	if (first != last)
	{
		TInputs inputs;
		for (TObjectIterator i = first; i != last; ++i)
		{
			inputs.push_back(Input(TObjectTraits::objectAabb(i), i));
		}
		balance(inputs.begin(), inputs.end());
	}
}



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::reset(TObjectIterator first, TObjectIterator last)
{
	TSelf temp(first, last, static_cast<const SH&>(*this));
	swap(temp);
}



template <typename O, typename OT, typename SH> inline
const typename AabbTree<O, OT, SH>::TAabb
AabbTree<O, OT, SH>::aabb() const
{
	if (isEmpty())
	{
		return TObjectTraits::aabbEmpty();
	}
	return nodes_[0].aabb();
}



/** Check whether there's any object in the tree that contains @a point.
 */
template <typename O, typename OT, typename SH> inline
bool AabbTree<O, OT, SH>::contains(const TPoint& point, const TInfo* info) const
{
	if (isEmpty())
	{
		return false;
	}
	return doContains(0, point, info);
}



/** Find all objects that contain @a point.
 */
template <typename O, typename OT, typename SH>
template <typename OutputIterator> inline
OutputIterator AabbTree<O, OT, SH>::find(const TPoint& point, OutputIterator result, const TInfo* info) const
{
	if (isEmpty())
	{
		return result;
	}
	return doFind(0, point, result, info);
}



/** Find all objects that intersect (overlap) with @a box.
 */
template <typename O, typename OT, typename SH>
template <typename OutputIterator> inline
OutputIterator AabbTree<O, OT, SH>::find(const TAabb& box, OutputIterator result, const TInfo* info) const
{
	if (isEmpty())
	{
		return result;
	}
	return doFind(0, box, result, info);
}



/** Find all objects that have an intersection with @a ray between @a tMin and @a tMax.
 */
template <typename O, typename OT, typename SH>
template <typename OutputIterator> inline
OutputIterator AabbTree<O, OT, SH>::find(const TRay& ray, TParam tMin, TParam tMax, OutputIterator result, const TInfo* info) const
{
	if (isEmpty())
	{
		return result;
	}
	return doFind(0, ray, tMin, tMax, result, info);
}



template <typename O, typename OT, typename SH>
typename AabbTree<O, OT, SH>::TObjectIterator inline
AabbTree<O, OT, SH>::intersect(const TRay& ray, TReference t, TParam tMin, const TInfo* info) const
{
	const TVector& dir = ray.direction();
	const TVector invDir = TObjectTraits::vectorReciprocal(dir);
	TValue tDummy;
	if (isEmpty() || !volumeIntersect(nodes_.front().aabb(), ray, invDir, tDummy, tMin))
	{
		return *end_;
	}
	return doIntersect(0, ray, invDir, t, tMin, info);
}



template <typename O, typename OT, typename SH>
bool inline AabbTree<O, OT, SH>::intersects(const TRay& ray, TParam tMin, TParam tMax, const TInfo* info) const
{
	if (isEmpty())
	{
		return false;
	}
	const TVector& dir = ray.direction();
	const TVector invDir = TObjectTraits::vectorReciprocal(dir);
#if 1
	int stack[32];
	size_t stackSize = 0;
	stack[stackSize++] = 0;
	while (stackSize > 0)
	{
		const int index = stack[--stackSize];
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
		const Node& node = nodes_[index];

		if (!volumeIntersects(node.aabb(), ray, invDir, tMin, tMax))
		{
			continue;
		}
		if (node.isInternal())
		{
			stack[stackSize++] = node.right();
			stack[stackSize++] = index + 1;
			continue;
		}
		for (int i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectIntersects(objects_[i], ray, tMin, tMax, info))
			{
				return true;
			}
		}
	}
	return false;
#else
	return doIntersects(0, ray, tMin, tMax, info);
#endif
}



template <typename O, typename OT, typename SH>
const typename AabbTree<O, OT, SH>::Neighbour
AabbTree<O, OT, SH>::nearestNeighbour(const TPoint& point, const TInfo* info) const
{
	Neighbour nearest(*end_, std::numeric_limits<TValue>::infinity());
	if (!isEmpty())
	{
		doNearestNeighbour(0, point, info, nearest);
	}
	return nearest;
}



template <class O, class OT, typename SH>
template <typename RandomAccessIterator>
RandomAccessIterator
AabbTree<O, OT, SH>::rangeSearch(
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
void AabbTree<O, OT, SH>::swap(TSelf& other)
{
	SH::swap(other);
	nodes_.swap(other.nodes_);
	objects_.swap(other.objects_);
	std::swap(end_, other.end_);
}



template <typename O, typename OT, typename SH>
bool AabbTree<O, OT, SH>::isEmpty() const
{
	return objects_.empty();
}



template <typename O, typename OT, typename SH>
const typename AabbTree<O, OT, SH>::TObjectIterator
AabbTree<O, OT, SH>::end() const
{
	return *end_;
}



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::reset()
{
	TSelf temp(static_cast<const SH&>(*this));
	swap(temp);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
int AabbTree<O, OT, SH>::balance(TInputIterator first, TInputIterator last)
{
	const SplitInfo<OT> split = TSplitHeuristics::template split<OT>(first, last);	
	if (split.axis < 0)
	{
		return addLeafNode(split.aabb, first, last);
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

	const int node = addInternalNode(split.aabb);
	const int LASS_UNUSED(left) = balance(first, middle);
	LASS_ASSERT(left == node + 1);
	const int right = balance(middle, last);
	nodes_[node].right() = right;
	return node;
}



template <typename O, typename OT, typename SH>
int AabbTree<O, OT, SH>::addLeafNode(
		const TAabb& aabb, TInputIterator first, TInputIterator last)
{
	const int begin = static_cast<int>(objects_.size());
	while (first != last)
	{
		objects_.push_back((first++)->object);
	}
	const int end = static_cast<int>(objects_.size());
	
	LASS_ASSERT(begin >= 0 && end >= 0);
	nodes_.push_back(Node(aabb, begin, end));

	LASS_ASSERT(static_cast<int>(nodes_.size()) > 0);
	return static_cast<int>(nodes_.size()) - 1;
}



template <typename O, typename OT, typename SH>
int AabbTree<O, OT, SH>::addInternalNode(const TAabb& aabb)
{
	nodes_.push_back(Node(aabb));

	LASS_ASSERT(static_cast<int>(nodes_.size()) > 0);
	return static_cast<int>(nodes_.size()) - 1;
}



template <typename O, typename OT, typename SH>
bool AabbTree<O, OT, SH>::doContains(int index, const TPoint& point, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	if (!TObjectTraits::aabbContains(node.aabb(), point))
	{
		return false;
	}
	if (node.isInternal())
	{
		return doContains(index + 1, point, info) || doContains(node.right(), point, info);
	}
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



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabbTree<O, OT, SH>::doFind(int index, const TPoint& point, OutputIterator result, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	if (!TObjectTraits::aabbContains(node.aabb(), point))
	{
		return result;
	}
	if (node.isInternal())
	{
		result = doFind(index + 1, point, result, info);
		return doFind(node.right(), point, result, info);
	}
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



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabbTree<O, OT, SH>::doFind(int index, const TAabb& box, OutputIterator result, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	if (!TObjectTraits::aabbIntersects(node.aabb(), box))
	{
		return result;
	}
	if (node.isInternal())
	{
		result = doFind(index + 1, box, result, info);
		return doFind(node.right(), box, result, info);
	}
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



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabbTree<O, OT, SH>::doFind(int index, const TRay& ray, TParam tMin, TParam tMax, OutputIterator result, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	if (!volumeIntersects(node.aabb(), ray, tMin, tMax))
	{
		return result;
	}
	if (node.isInternal())
	{
		result = doFind(index + 1, ray, tMin, tMax, result, info);
		return doFind(node.right(), ray, tMin, tMax, result, info);
	}
	for (int i = node.first(); i != node.last(); ++i)
	{
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
		if (TObjectTraits::objectIntersects(objects_[i], ray, tMin, tMax, info))
		{
			*result++ = objects_[i];
		}
	}
	return result;
}



template <typename O, typename OT, typename SH>
typename AabbTree<O, OT, SH>::TObjectIterator 
AabbTree<O, OT, SH>::doIntersect(int index, const TRay& ray, const TVector& invDir, TReference t, TParam tMin, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		TValue tBest = 0;
		TObjectIterator best = *end_;
		for (int i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			TValue tCandidate = 0;
			if (TObjectTraits::objectIntersect(objects_[i], ray, tCandidate, tMin, info))
			{
				if (best == *end_ || tCandidate < tBest)
				{
					tBest = tCandidate;
					best = objects_[i];
				}
			}
		}
		if (best != *end_)
		{
			t = tBest;
		}
		return best;
	}

	int left = index + 1;
	int right = node.right();
	TValue tLeftBox = 0, tRightBox = 0;
	const bool hitsLeft = volumeIntersect(nodes_[left].aabb(), ray, invDir, tLeftBox, tMin);
	const bool hitsRight = volumeIntersect(nodes_[right].aabb(), ray, invDir, tRightBox, tMin);
	
	if (!hitsLeft)
	{
		return hitsRight ? doIntersect(right, ray, invDir, t, tMin, info) : *end_;
	}
	if (!hitsRight)
	{
		return doIntersect(left, ray, invDir, t, tMin, info);
	}

	// ok, we intersect both childs. Visit the box that is nearest first.
	if (tRightBox < tLeftBox)
	{
		std::swap(tLeftBox, tRightBox);
		std::swap(left, right);
	}

	TValue tLeft;
	const TObjectIterator leftBest = doIntersect(left, ray, invDir, tLeft, tMin, info);
	if (leftBest == *end_)
	{
		return doIntersect(right, ray, invDir, t, tMin, info);
	}

	if (tRightBox <= tLeft)
	{
		// right node might still have a closer hit.
		TValue tRight;
		const TObjectIterator rightBest = doIntersect(right, ray, invDir, tRight, tMin, info);
		if (rightBest != *end_ && tRight < tLeft)
		{
			t = tRight;
			return rightBest;
		}
	}

	t = tLeft;
	return leftBest;
}



template <typename O, typename OT, typename SH>
bool AabbTree<O, OT, SH>::doIntersects(
		int index, const TRay& ray, TParam tMin, const TParam tMax, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	if (!volumeIntersects(node.aabb(), ray, tMin, tMax))
	{
		return false;
	}
	if (node.isInternal())
	{
		return doIntersects(index + 1, ray, tMin, tMax, info)
			|| doIntersects(node.right(), ray, tMin, tMax, info);
	}
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



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::doNearestNeighbour(
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
			const TValue squaredDistance = TObjectTraits::objectSquaredDistance(objects_[i], target, info);
			if (squaredDistance < best.squaredDistance())
			{
				best = Neighbour(objects_[i], squaredDistance);
			}
		}
	}
	else
	{
		int children[2];
		TValue squaredDistances[2];
		getChildren(index, target, children, squaredDistances);
		if (squaredDistances[0] < best.squaredDistance())
		{
			doNearestNeighbour(children[0], target, info, best);
			if (squaredDistances[1] < best.squaredDistance())
			{
				doNearestNeighbour(children[1], target, info, best);
			}
		}
	}
}



template <typename O, typename OT, typename SH>
template <typename RandomIterator>
RandomIterator AabbTree<O, OT, SH>::doRangeSearch(
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
	TValue squaredDistances[2];
	getChildren(index, target, children, squaredDistances);
	if (squaredDistances[0] < squaredRadius)
	{
		last = doRangeSearch(children[0], target, squaredRadius, maxCount, first, last, info);
		if (squaredDistances[1] < squaredRadius)
		{
			last = doRangeSearch(children[1], target, squaredRadius, maxCount, first, last, info);
		}
	}

	return last;
}



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::getChildren(
		int index, const TPoint& target, int indices[2], TValue squaredDistances[2]) const
{
	const Node& node = nodes_[index];
	indices[0] = index + 1;
	indices[1] = node.right();

	for (size_t i = 0; i < 2; ++i)
	{
		const Node& child = nodes_[indices[i]];
		squaredDistances[i] = 0;
		const TPoint& min = TObjectTraits::aabbMin(child.aabb());
		const TPoint& max = TObjectTraits::aabbMax(child.aabb());
		for (size_t k = 0; k < dimension; ++k)
		{
			const TValue x = TObjectTraits::coord(target, k);
			const TValue d = std::max(TObjectTraits::coord(min, k) - x, x - TObjectTraits::coord(max, k));
			if (d > 0)
			{
				squaredDistances[i] += d * d;
			}
		}
	}

	if (squaredDistances[1] < squaredDistances[0])
	{
		std::swap(squaredDistances[0], squaredDistances[1]);
		std::swap(indices[0], indices[1]);
	}
}



template <typename O, typename OT, typename SH>
bool AabbTree<O, OT, SH>::volumeIntersect(const TAabb& box, const TRay& ray, const TVector& invDir, TReference t, TParam tMin) const
{
	if (TObjectTraits::aabbContains(box, TObjectTraits::rayPoint(ray, tMin)))
	{
		t = tMin;
		return true;
	}
	return TObjectTraits::aabbIntersect(box, ray, invDir, t, tMin);
}



template <typename O, typename OT, typename SH>
bool AabbTree<O, OT, SH>::volumeIntersects(const TAabb& box, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax) const
{
	TValue t = 0;
	return volumeIntersect(box, ray, invDir, t, tMin) && t <= tMax;
}


// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
