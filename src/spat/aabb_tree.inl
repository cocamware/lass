/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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
AabbTree<O, OT, SH>::AabbTree(TObjectIterator first, TObjectIterator last):
	objects_(),
	nodes_(),
	end_(last)
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
	TSelf temp(first, last);
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



template <typename O, typename OT, typename SH> inline
bool AabbTree<O, OT, SH>::contains(const TPoint& point, const TInfo* info) const
{
	if (isEmpty())
	{
		return false;
	}
	return doContains(0, point, info);
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator> inline
OutputIterator AabbTree<O, OT, SH>::find(
		const TPoint& point, OutputIterator result, const TInfo* info) const
{
	if (isEmpty())
	{
		return result;
	}
	return doFind(0, point, result, info);
}



template <typename O, typename OT, typename SH>
typename AabbTree<O, OT, SH>::TObjectIterator inline
AabbTree<O, OT, SH>::intersect(const TRay& ray, TReference t, TParam tMin, const TInfo* info) const
{
	if (isEmpty())
	{
		return end_;
	}
	return doIntersect(0, ray, t, tMin, info);
}



template <typename O, typename OT, typename SH>
bool inline AabbTree<O, OT, SH>::intersects(const TRay& ray, TParam tMin, TParam tMax, const TInfo* info) const
{
	if (isEmpty())
	{
		return false;
	}
	return doIntersects(0, ray, tMin, tMax, info);
}



template <typename O, typename OT, typename SH>
const typename AabbTree<O, OT, SH>::Neighbour
AabbTree<O, OT, SH>::nearestNeighbour(const TPoint& point, const TInfo* info) const
{
	Neighbour nearest(end_, std::numeric_limits<TValue>::infinity());
	if (!isEmpty())
	{
		doNearestNeighbour(0, point, info, nearest);
	}
	return nearest;
}



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::swap(TSelf& other)
{
	nodes_.swap(other.nodes_);
	objects_.swap(other.objects_);
	std::swap(end_, other.end_);
}



template <typename O, typename OT, typename SH>
const bool AabbTree<O, OT, SH>::isEmpty() const
{
	return objects_.empty();
}



template <typename O, typename OT, typename SH>
const typename AabbTree<O, OT, SH>::TObjectIterator
AabbTree<O, OT, SH>::end() const
{
	return end_;
}



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::reset()
{
	TSelf temp;
	swap(temp);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
const int AabbTree<O, OT, SH>::balance(TInputIterator first, TInputIterator last)
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
	const int left = balance(first, middle);
	LASS_ASSERT(left == node + 1);
	const int right = balance(middle, last);
	nodes_[node].right() = right;
	return node;
}



template <typename O, typename OT, typename SH>
const int AabbTree<O, OT, SH>::addLeafNode(
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
const int AabbTree<O, OT, SH>::addInternalNode(const TAabb& aabb)
{
	nodes_.push_back(Node(aabb));

	LASS_ASSERT(static_cast<int>(nodes_.size()) > 0);
	return static_cast<int>(nodes_.size()) - 1;
}



template <typename O, typename OT, typename SH>
bool AabbTree<O, OT, SH>::doContains(int index, const TPoint& point, const TInfo* info) const
{
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
		if (TObjectTraits::objectContains(objects_[i], point, info))
		{
			return true;
		}
	}
	return false;
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabbTree<O, OT, SH>::doFind(
		int index, const TPoint& point, OutputIterator result, const TInfo* info) const
{
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
		if (TObjectTraits::objectContains(objects_[i], point, info))
		{
			*result++ = objects_[i];
		}
	}
	return result;
}



template <typename O, typename OT, typename SH>
typename AabbTree<O, OT, SH>::TObjectIterator 
AabbTree<O, OT, SH>::doIntersect(
		int index, const TRay& ray, TReference t, TParam tMin, const TInfo* info) const
{
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	TValue tDummy;
	if (!TObjectTraits::aabbIntersect(node.aabb(), ray, tDummy, tMin))
	{
		return end_;
	}

	if (node.isInternal())
	{
		TValue tLeft;
		TObjectIterator left = doIntersect(index + 1, ray, tLeft, tMin, info);
		TValue tRight;
		TObjectIterator right = doIntersect(node.right(), ray, tRight, tMin, info);

		if (left != end_ && (right == end_ || tLeft < tRight))
		{
			t = tLeft;
			return left;
		}
		if (right != end_)
		{
			LASS_ASSERT(left == end_ || !(tLeft < tRight));
			t = tRight;
			return right;
		}
		return end_;
	}

	TValue tBest = 0;
	TObjectIterator best = end_;
	for (int i = node.first(); i != node.last(); ++i)
	{
		TValue tCandidate;
		if (TObjectTraits::objectIntersect(objects_[i], ray, tCandidate, tMin, info))
		{
			if (best == end_ || tCandidate < tBest)
			{
				tBest = tCandidate;
				best = objects_[i];
			}
		}
	}
	if (best != end_)
	{
		t = tBest;
	}
	return best;
}



template <typename O, typename OT, typename SH>
bool AabbTree<O, OT, SH>::doIntersects(
		int index, const TRay& ray, TParam tMin, const TParam tMax, const TInfo* info) const
{
	LASS_ASSERT(index >= 0 && static_cast<size_t>(index) < nodes_.size());
	const Node& node = nodes_[index];

	TValue tDummy;
	if (!TObjectTraits::aabbIntersect(node.aabb(), ray, tDummy, tMin))
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
		if (TObjectTraits::objectIntersects(objects_[i], ray, tMin, tMax, info))
		{
			return true;
		}
	}
	return false;
}



template <typename O, typename OT, typename SH>
void AabbTree<O, OT, SH>::doNearestNeighbour(
		int index, const TPoint& point, const TInfo* info, Neighbour& best) const
{
	LASS_ASSERT(best.squaredDistance() >= 0);

	const Node& node = nodes_[index];

	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			const TValue squaredDistance = TObjectTraits::objectSquaredDistance(objects_[i], point, info);
			if (squaredDistance < best.squaredDistance())
			{
				best = Neighbour(objects_[i], squaredDistance);
			}
		}
	}
	else
	{
		// find distances to children first
		const size_t children[2] = { index + 1, node.right() };
		TValue sqrDistAabb[2] = { 0, 0 };
		for (size_t i = 0; i < 2; ++i)
		{
			const Node& child = nodes_[children[i]];
			const TPoint& min = TObjectTraits::aabbMin(child.aabb());
			const TPoint& max = TObjectTraits::aabbMax(child.aabb());
			for (size_t k = 0; k < dimension; ++k)
			{
				const TValue x = TObjectTraits::coord(point, k);
				const TValue d = std::max(TObjectTraits::coord(min, k) - x, x - TObjectTraits::coord(max, k));
				if (d > 0)
				{
					sqrDistAabb[i] += d * d;
				}
			}
		}

		// visit children, but visit closest one first =)
		//
		if (sqrDistAabb[0] < sqrDistAabb[1] && sqrDistAabb[0] < best.squaredDistance())
		{
			doNearestNeighbour(index + 1, point, info, best);
			if (sqrDistAabb[1] < best.squaredDistance())
			{
				doNearestNeighbour(node.right(), point, info, best);
			}
		}
		else if (sqrDistAabb[1] < best.squaredDistance())
		{
			doNearestNeighbour(node.right(), point, info, best);
			if (sqrDistAabb[0] < best.squaredDistance())
			{
				doNearestNeighbour(index + 1, point, info, best);
			}
		}
	}
}



// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
