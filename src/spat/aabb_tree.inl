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
void AabbTree<O, OT, SH>::clear()
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



// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
