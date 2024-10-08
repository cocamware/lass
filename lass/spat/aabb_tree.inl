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
 *	Copyright (C) 2004-2024 the Initial Developer.
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

#include <cstddef>

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
		std::ptrdiff_t n = last - first;
		if (n < 0)
		{
			LASS_THROW("AabbTree: invalid range");
		}
		if (static_cast<size_t>(n) > static_cast<size_t>(Node::sentinelInternal))
		{
			LASS_THROW("AabbTree: too many objects");
		}
		TInputs inputs;
		inputs.reserve(static_cast<size_t>(n));
		for (TObjectIterator i = first; i != last; ++i)
		{
			inputs.push_back(Input(TObjectTraits::objectAabb(i), i));
		}
		balance(inputs.begin(), inputs.end());
	}
}


template <typename O, typename OT, typename SH>
AabbTree<O, OT, SH>::AabbTree(TSelf&& other) noexcept:
	SH(std::forward<TSplitHeuristics>(other)),
	objects_(std::move(other.objects_)),
	nodes_(std::move(other.nodes_)),
	end_(std::move(other.end_))
{
}


template <typename O, typename OT, typename SH>
AabbTree<O, OT, SH>& AabbTree<O, OT, SH>::operator=(TSelf&& other) noexcept
{
	TSplitHeuristics::operator=(std::forward<TSplitHeuristics>(other));
	objects_ = std::move(other.objects_);
	nodes_ = std::move(other.nodes_);
	end_ = std::move(other.end_);
	return *this;
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
	const TVector& dir = ray.direction();
	const TVector invDir = TObjectTraits::vectorReciprocal(dir);
#if LASS_SPAT_AABB_TREE_STACK_SIZE
	TIndex stack[LASS_SPAT_AABB_TREE_STACK_SIZE];
	TIndex stackSize = 0;
	stack[stackSize++] = 0;
	while (stackSize > 0)
	{
		const TIndex index = stack[--stackSize];
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index < nodes_.size());
		const Node& node = nodes_[index];

		if (!volumeIntersects(node.aabb(), ray, invDir, tMin, tMax))
		{
			continue;
		}
		if (node.isInternal())
		{
			if (stackSize + 2 >= LASS_SPAT_AABB_TREE_STACK_SIZE)
			{
				// fall back to recursive implementation
				result = doFind(index + 1, ray, invDir, tMin, tMax, result, info);
				result = doFind(node.right(), ray, invDir, tMin, tMax, result, info);
				continue;
			}
			LASS_ASSERT(stackSize + 2 < LASS_SPAT_AABB_TREE_STACK_SIZE);
			stack[stackSize++] = node.right();
			stack[stackSize++] = index + 1;
			continue;
		}
		for (TIndex i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectIntersects(objects_[i], ray, tMin, tMax, info))
			{
				*result++ = objects_[i];
			}
		}
	}
	return result;
#else
	return doFind(0, ray, invDir, tMin, tMax, result, info);
#endif
}



template <typename O, typename OT, typename SH>
typename AabbTree<O, OT, SH>::TObjectIterator inline
AabbTree<O, OT, SH>::intersect(const TRay& ray, TReference t, TParam tMin, const TInfo* info) const
{
	const TVector& dir = ray.direction();
	const TVector invDir = TObjectTraits::vectorReciprocal(dir);
	TValue tRoot;
	if (isEmpty() || !volumeIntersect(nodes_.front().aabb(), ray, invDir, tRoot, tMin))
	{
		return *end_;
	}
#if LASS_SPAT_AABB_TREE_STACK_SIZE
	struct Visit
	{
		TIndex index;
		TValue tNear;
		Visit(TIndex index = 0, TValue tNear = 0) : index(index), tNear(tNear) {}
	};
	Visit stack[LASS_SPAT_AABB_TREE_STACK_SIZE];
	TIndex stackSize = 0;
	stack[stackSize++] = Visit(0, tRoot);
	TValue tBest = 0;
	TObjectIterator best = *end_;
	while (stackSize > 0)
	{
		const Visit visit = stack[--stackSize];
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(visit.index < nodes_.size());
		const Node& node = nodes_[visit.index];

		if (best != *end_ && tBest < visit.tNear)
		{
			continue; // we already have a closer hit than what this node can offer
		}

		if (stackSize + 2 >= LASS_SPAT_AABB_TREE_STACK_SIZE)
		{
			// fall back to recursive implementation
			TValue tb = 0;
			TObjectIterator b = doIntersect(0, ray, invDir, tb, tMin, info);
			if (best == *end_ || tb < tBest)
			{
				tBest = tb;
				best = b;
			}
			continue;
		}
		
		if (node.isLeaf())
		{
			for (TIndex i = node.first(); i != node.last(); ++i)
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
			continue;
		}

		TIndex left = visit.index + 1;
		TIndex right = node.right();
		TValue tLeftBox = 0, tRightBox = 0;
		const bool hitsLeft = volumeIntersect(nodes_[left].aabb(), ray, invDir, tLeftBox, tMin);
		const bool hitsRight = volumeIntersect(nodes_[right].aabb(), ray, invDir, tRightBox, tMin);

		if (hitsLeft)
		{
			if (hitsRight)
			{
				// ok, we intersect both childs. Visit the box that is nearest first.
				if (tRightBox < tLeftBox)
				{
					std::swap(tLeftBox, tRightBox);
					std::swap(left, right);
				}
				// left is nearest, so push right first (it'll be visited as last)
				stack[stackSize++] = Visit(right, tRightBox);
				stack[stackSize++] = Visit(left, tLeftBox);
			}
			else
			{
				stack[stackSize++] = Visit(left, tLeftBox);
				continue;
			}
		}
		else if (hitsRight)
		{
			stack[stackSize++] = Visit(right, tRightBox);
		}
	}
	if (best != *end_)
	{
		t = tBest;
	}
	return best;
#else
	return doIntersect(0, ray, invDir, t, tMin, info);
#endif
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
#if LASS_SPAT_AABB_TREE_STACK_SIZE
	TIndex stack[LASS_SPAT_AABB_TREE_STACK_SIZE];
	TIndex stackSize = 0;
	stack[stackSize++] = 0;
	while (stackSize > 0)
	{
		const TIndex index = stack[--stackSize];

		if (stackSize + 2 >= LASS_SPAT_AABB_TREE_STACK_SIZE)
		{
			// fall back to recursive implementation
			if (doIntersects(index, ray, invDir, tMin, tMax, info))
			{
				return true;
			}
			continue;
		}

		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index < nodes_.size());
		const Node& node = nodes_[index];

		if (!volumeIntersects(node.aabb(), ray, invDir, tMin, tMax))
		{
			continue;
		}

		if (node.isLeaf())
		{
			for (TIndex i = node.first(); i != node.last(); ++i)
			{
				LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
				if (TObjectTraits::objectIntersects(objects_[i], ray, tMin, tMax, info))
				{
					return true;
				}
			}
			continue;
		}

		LASS_ASSERT(stackSize + 2 < LASS_SPAT_AABB_TREE_STACK_SIZE);
		stack[stackSize++] = node.right();
		stack[stackSize++] = index + 1;
	}
	return false;
#else
	return doIntersects(0, ray, invDir, tMin, tMax, info);
#endif
}



template <typename O, typename OT, typename SH>
const typename AabbTree<O, OT, SH>::Neighbour
AabbTree<O, OT, SH>::nearestNeighbour(const TPoint& point, const TInfo* info) const
{
	Neighbour nearest(*end_, std::numeric_limits<TValue>::infinity());
	if (isEmpty())
	{
		return nearest;
	}
#if LASS_SPAT_AABB_TREE_STACK_SIZE
	struct Visit
	{
		TIndex index;
		TValue squaredDistance;
		Visit(TIndex index = 0, TValue squaredDistance = 0) : index(index), squaredDistance(squaredDistance) {}
	};
	Visit stack[LASS_SPAT_AABB_TREE_STACK_SIZE];
	TIndex stackSize = 0;
	stack[stackSize++] = Visit(0, 0);
	while (stackSize > 0)
	{
		const Visit visit = stack[--stackSize];
		if (visit.squaredDistance >= nearest.squaredDistance())
		{
			continue; // we already have a closer hit than what this node can offer
		}

		if (stackSize + 2 >= LASS_SPAT_AABB_TREE_STACK_SIZE)
		{
			// fall back to recursive implementation
			doNearestNeighbour(visit.index, point, info, nearest);
			continue;
		}

		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(nearest.squaredDistance() >= 0);
		LASS_ASSERT(visit.index < nodes_.size());
		const Node& node = nodes_[visit.index];

		if (node.isLeaf())
		{
			for (TIndex i = node.first(); i != node.last(); ++i)
			{
				LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
				const TValue squaredDistance = TObjectTraits::objectSquaredDistance(objects_[i], point, info);
				if (squaredDistance < nearest.squaredDistance())
				{
					nearest = Neighbour(objects_[i], squaredDistance);
				}
			}
			continue;
		}

		TIndex children[2];
		TValue squaredDistances[2];
		getChildren(visit.index, point, children, squaredDistances);
		// children[0] is the nearest child, so push it as last (it'll be visited first)
		stack[stackSize++] = Visit(children[1], squaredDistances[1]);
		stack[stackSize++] = Visit(children[0], squaredDistances[0]);
	}
#else
	doNearestNeighbour(0, point, info, nearest);
#endif
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
#if LASS_SPAT_AABB_TREE_STACK_SIZE
	RandomAccessIterator last = first;
	struct Visit
	{
		TIndex index;
		TValue squaredDistance;
		Visit(TIndex index = 0, TValue squaredDistance = 0) : index(index), squaredDistance(squaredDistance) {}
	};
	Visit stack[LASS_SPAT_AABB_TREE_STACK_SIZE];
	TIndex stackSize = 0;
	stack[stackSize++] = Visit(0, 0);
	while (stackSize > 0)
	{
		const Visit visit = stack[--stackSize];
		if (visit.squaredDistance >= squaredRadius)
		{
			continue; // node is too far away
		}

		if (stackSize + 2 >= LASS_SPAT_AABB_TREE_STACK_SIZE)
		{
			// fall back to recursive implementation
			last = doRangeSearch(visit.index, target, squaredRadius, maxCount, first, last, info);
			continue;
		}

		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(visit.index < nodes_.size());
		const Node& node = nodes_[visit.index];

		if (node.isLeaf())
		{
			for (TIndex i = node.first(); i != node.last(); ++i)
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
			continue;
		}

		TIndex children[2];
		TValue squaredDistances[2];
		getChildren(visit.index, target, children, squaredDistances);
		// children[0] is the nearest child, so push it as last (it'll be visited first)
		stack[stackSize++] = Visit(children[1], squaredDistances[1]);
		stack[stackSize++] = Visit(children[0], squaredDistances[0]);
	}
	return last;
#else
	return doRangeSearch(0, target, squaredRadius, maxCount, first, first, info);
#endif
}



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::swap(TSelf& other)
{
	SH::swap(other);
	nodes_.swap(other.nodes_);
	objects_.swap(other.objects_);
	end_.swap(other.end_);
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
typename AabbTree<O, OT, SH>::TIndex
AabbTree<O, OT, SH>::balance(TInputIterator first, TInputIterator last)
{
	const SplitInfo<OT> split = TSplitHeuristics::template split<OT>(first, last);	
	if (split.isLeaf())
	{
		return addLeafNode(split.aabb, first, last);
	}

	TInputIterator middle = std::partition(first, last, impl::Splitter<TObjectTraits>(split));
	if (middle == first || middle == last)
	{
		const std::ptrdiff_t halfSize = (last - first) / 2;
		LASS_ASSERT(halfSize > 0);
		middle = first + halfSize;
		std::nth_element(first, middle, last, impl::LessAxis<TObjectTraits>(split.axis));
	}
	LASS_ASSERT(middle != first && middle != last);

	const TIndex node = addInternalNode(split.aabb);
	[[maybe_unused]] const TIndex left = balance(first, middle);
	LASS_ASSERT(left == node + 1);
	const TIndex right = balance(middle, last);
	nodes_[node].setRight(right);
	return node;
}



template <typename O, typename OT, typename SH>
typename AabbTree<O, OT, SH>::TIndex
AabbTree<O, OT, SH>::addLeafNode(
		const TAabb& aabb, TInputIterator first, TInputIterator last)
{
	LASS_ASSERT(objects_.size() <= Node::sentinelInternal);
	const TIndex begin = static_cast<TIndex>(objects_.size());
	while (first != last)
	{
		objects_.push_back((first++)->object);
	}
	LASS_ASSERT(objects_.size() <= Node::sentinelInternal);
	const TIndex end = static_cast<TIndex>(objects_.size());
	nodes_.push_back(Node(aabb, begin, end));

	LASS_ASSERT(nodes_.size() > 0);
	return static_cast<TIndex>(nodes_.size() - 1);
}



template <typename O, typename OT, typename SH>
typename AabbTree<O, OT, SH>::TIndex
AabbTree<O, OT, SH>::addInternalNode(const TAabb& aabb)
{
	nodes_.push_back(Node(aabb));
	LASS_ASSERT(objects_.size() <= Node::sentinelInternal);
	return static_cast<TIndex>(nodes_.size() - 1);
}



template <typename O, typename OT, typename SH>
bool AabbTree<O, OT, SH>::doContains(TIndex rootIndex, const TPoint& point, const TInfo* info) const
{
#if LASS_SPAT_AABB_TREE_STACK_SIZE
	TIndex stack[LASS_SPAT_AABB_TREE_STACK_SIZE];
	TIndex stackSize = 0;
	stack[stackSize++] = rootIndex;
	while (stackSize > 0)
	{
		const TIndex index = stack[--stackSize];
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index < nodes_.size());
		const Node& node = nodes_[index];

		if (!TObjectTraits::aabbContains(node.aabb(), point))
		{
			continue;
		}
		if (node.isInternal())
		{
			if (stackSize + 2 >= LASS_SPAT_AABB_TREE_STACK_SIZE)
			{
				// fall back to recursive implementation
				if (doContains(index + 1, point, info) || doContains(node.right(), point, info))
				{
					return true;
				}
				continue;
			}
			LASS_ASSERT(stackSize + 2 < LASS_SPAT_AABB_TREE_STACK_SIZE);
			stack[stackSize++] = node.right();
			stack[stackSize++] = index + 1;
			continue;
		}
		for (TIndex i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectContains(objects_[i], point, info))
			{
				return true;
			}
		}
	}
#else
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(rootIndex < nodes_.size());
	const Node& node = nodes_[rootIndex];

	if (!TObjectTraits::aabbContains(node.aabb(), point))
	{
		return false;
	}
	if (node.isInternal())
	{
		return doContains(rootIndex + 1, point, info) || doContains(node.right(), point, info);
	}
	for (TIndex i = node.first(); i != node.last(); ++i)
	{
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
		if (TObjectTraits::objectContains(objects_[i], point, info))
		{
			return true;
		}
	}
#endif
	return false;
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabbTree<O, OT, SH>::doFind(TIndex rootIndex, const TPoint& point, OutputIterator result, const TInfo* info) const
{
#if LASS_SPAT_AABB_TREE_STACK_SIZE
	TIndex stack[LASS_SPAT_AABB_TREE_STACK_SIZE];
	TIndex stackSize = 0;
	stack[stackSize++] = rootIndex;
	while (stackSize > 0)
	{
		const TIndex index = stack[--stackSize];
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index < nodes_.size());
		const Node& node = nodes_[index];

		if (!TObjectTraits::aabbContains(node.aabb(), point))
		{
			continue;
		}
		if (node.isInternal())
		{
			if (stackSize + 2 >= LASS_SPAT_AABB_TREE_STACK_SIZE)
			{
				// fall back to recursive implementation
				result = doFind(index + 1, point, result, info);
				result = doFind(node.right(), point, result, info);
				continue;
			}
			LASS_ASSERT(stackSize + 2 < LASS_SPAT_AABB_TREE_STACK_SIZE);
			stack[stackSize++] = node.right();
			stack[stackSize++] = index + 1;
			continue;
		}
		for (TIndex i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectContains(objects_[i], point, info))
			{
				*result++ = objects_[i];
			}
		}
	}
	return result;
#else
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(rootIndex < nodes_.size());
	const Node& node = nodes_[rootIndex];

	if (!TObjectTraits::aabbContains(node.aabb(), point))
	{
		return result;
	}
	if (node.isInternal())
	{
		result = doFind(rootIndex + 1, point, result, info);
		return doFind(node.right(), point, result, info);
	}
	for (TIndex i = node.first(); i != node.last(); ++i)
	{
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
		if (TObjectTraits::objectContains(objects_[i], point, info))
		{
			*result++ = objects_[i];
		}
	}
	return result;
#endif
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabbTree<O, OT, SH>::doFind(TIndex rootIndex, const TAabb& box, OutputIterator result, const TInfo* info) const
{
#if LASS_SPAT_AABB_TREE_STACK_SIZE
	TIndex stack[LASS_SPAT_AABB_TREE_STACK_SIZE];
	TIndex stackSize = 0;
	stack[stackSize++] = rootIndex;
	while (stackSize > 0)
	{
		const TIndex index = stack[--stackSize];
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index < nodes_.size());
		const Node& node = nodes_[index];

		if (!TObjectTraits::aabbIntersects(node.aabb(), box))
		{
			continue;
		}
		if (node.isInternal())
		{
			if (stackSize + 2 >= LASS_SPAT_AABB_TREE_STACK_SIZE)
			{
				// fall back to recursive implementation
				result = doFind(index + 1, box, result, info);
				result = doFind(node.right(), box, result, info);
				continue;
			}
			LASS_ASSERT(stackSize + 2 < LASS_SPAT_AABB_TREE_STACK_SIZE);
			stack[stackSize++] = node.right();
			stack[stackSize++] = index + 1;
			continue;
		}
		for (TIndex i = node.first(); i != node.last(); ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectIntersects(objects_[i], box, info))
			{
				*result++ = objects_[i];
			}
		}
	}
	return result;
#else
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(rootIndex < nodes_.size());
	const Node& node = nodes_[rootIndex];

	if (!TObjectTraits::aabbIntersects(node.aabb(), box))
	{
		return result;
	}
	if (node.isInternal())
	{
		result = doFind(rootIndex + 1, box, result, info);
		return doFind(node.right(), box, result, info);
	}
	for (TIndex i = node.first(); i != node.last(); ++i)
	{
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
		if (TObjectTraits::objectIntersects(objects_[i], box, info))
		{
			*result++ = objects_[i];
		}
	}
	return result;
#endif
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabbTree<O, OT, SH>::doFind(TIndex index, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax, OutputIterator result, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index < nodes_.size());
	const Node& node = nodes_[index];

	if (!volumeIntersects(node.aabb(), ray, invDir, tMin, tMax))
	{
		return result;
	}
	if (node.isInternal())
	{
		result = doFind(index + 1, ray, invDir, tMin, tMax, result, info);
		return doFind(node.right(), ray, invDir, tMin, tMax, result, info);
	}
	for (TIndex i = node.first(); i != node.last(); ++i)
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
AabbTree<O, OT, SH>::doIntersect(TIndex index, const TRay& ray, const TVector& invDir, TReference t, TParam tMin, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index < nodes_.size());
	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		TValue tBest = 0;
		TObjectIterator best = *end_;
		for (TIndex i = node.first(); i != node.last(); ++i)
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

	TIndex left = index + 1;
	TIndex right = node.right();
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
		TIndex index, const TRay& ray, const TVector& invDir, TParam tMin, const TParam tMax, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(index < nodes_.size());
	const Node& node = nodes_[index];

	if (!volumeIntersects(node.aabb(), ray, invDir, tMin, tMax))
	{
		return false;
	}
	if (node.isInternal())
	{
		return doIntersects(index + 1, ray, invDir, tMin, tMax, info)
			|| doIntersects(node.right(), ray, invDir, tMin, tMax, info);
	}
	for (TIndex i = node.first(); i != node.last(); ++i)
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
		TIndex index, const TPoint& target, const TInfo* info, Neighbour& best) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(best.squaredDistance() >= 0);

	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		for (TIndex i = node.first(); i != node.last(); ++i)
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
		TIndex children[2];
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
	TIndex index, const TPoint& target, TReference squaredRadius, size_t maxCount,
	RandomIterator first, RandomIterator last, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(squaredRadius >= 0);

	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		for (TIndex i = node.first(); i != node.last(); ++i)
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

	TIndex children[2];
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
		TIndex index, const TPoint& target, TIndex indices[2], TValue squaredDistances[2]) const
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
