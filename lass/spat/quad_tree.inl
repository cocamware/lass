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



/** @class lass::spat::QuadTree
 *  A spatial container for generic objects.  The object needs a traits class which
 *  contains the necessary functions to perform the quad tree management for the
 *  particular ObjectType.  The traits class needs as a basis the following interface:
 *  <tt>
 *      static TAabb aabb(const TSimplePolygon3D& iP);
 *      static bool contains( const TSimplePolygon3D& iP, const TPoint& point)
 *  </tt>
 *  The above functions are only examples.  The dimensionality of the primitives must
 *  match but can be of any order.  So the quad tree can be used to classify in
 *  2 and 3 dimensions.  In three dimensions the more common name is OctTree.
 *
 *  Higher level divisions can in theory be supported but the dimensional specific
 *  part must be reimplemented.  Altough this is only 2 functions and could be written
 *  generally this is not yet available.
 *
 *  @brief a Quad tree for general objects
 *  @author Tom De Muer [TDM]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_QUAD_TREE_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_QUAD_TREE_INL

#include "spat_common.h"
#include "quad_tree.h"

namespace lass
{
namespace spat
{

// --- public --------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
QuadTree<O, OT, SH>::QuadTree(const TSplitHeuristics& heuristics):
	SH(heuristics),
	aabb_(),
	root_(0),
	end_(),
	nodesAllocator_(new TNodesAllocator(sizeof(QuadNode) * numChildren))
{
}

	

	/** empty quadtree with fixed bounding box
 */
template <typename O, typename OT, typename SH>
QuadTree<O, OT, SH>::QuadTree(const TAabb& aabb, const TSplitHeuristics& heuristics):
	SH(heuristics),
	nodesAllocator_(sizeof(QuadNode) * numChildren),
	aabb_(aabb),
	root_(0),
	end_(),
	nodesAllocator_(new TNodesAllocator(sizeof(QuadNode) * numChildren))
{
	root_ = new QuadNode(CenteredBox(aabb), *nodesAllocator_);
}

	

/** empty quadtree with fixed bounding box and end iterator.
 */
template <typename O, typename OT, typename SH>
QuadTree<O, OT, SH>::QuadTree(const TAabb& aabb, const TObjectIterator end, const TSplitHeuristics& heuristics):
	SH(heuristics),
	aabb_(aabb),
	root_(0),
	end_(end),
	nodesAllocator_(new TNodesAllocator(sizeof(QuadNode) * numChildren))
{
	root_ = new QuadNode(CenteredBox(aabb), *nodesAllocator_);
}



/** quadtree from objects, with computed bounding box
 */
template <typename O, typename OT, typename SH>
QuadTree<O, OT, SH>::QuadTree(TObjectIterator first, TObjectIterator last, const TSplitHeuristics& heuristics):
	SH(heuristics),
	root_(0),
	end_(last),
	nodesAllocator_(new TNodesAllocator(sizeof(QuadNode) * numChildren))
{
	aabb_ = TObjectTraits::aabbEmpty();
	for (TObjectIterator i = first; i != last; ++i)
	{
		aabb_ = TObjectTraits::aabbJoin(aabb_, TObjectTraits::objectAabb(i));
	}

	root_ = new QuadNode(CenteredBox(aabb_), *nodesAllocator_);
	while (first != last)
	{
		add(first++);
	}
}



template <typename O, typename OT, typename SH>
QuadTree<O, OT, SH>::~QuadTree()
{
	delete root_;
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::reset()
{
	QuadTree temp(aabb_, end_, *this);
	swap(temp);
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::reset(TObjectIterator first, TObjectIterator last)
{
	QuadTree temp(first, last, *this);
	swap(temp);
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::add(TObjectIterator object)
{
	LASS_ASSERT(root_);
	if (!TObjectTraits::aabbContains(aabb_, TObjectTraits::objectAabb(object)))
	{
		LASS_THROW("object not within bounding box of tree");
	}
	root_->add(object, *this);
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::remove(TObjectIterator object)
{
	LASS_ASSERT(root_);
	root_->remove(object);
}



template <typename O, typename OT, typename SH>
bool QuadTree<O, OT, SH>::contains(const TPoint& point, const TInfo* info) const
{
	LASS_ASSERT(root_);

	if (!TObjectTraits::aabbContains(aabb_, point))
	{
		return false;
	}

	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	QuadNode* node = root_;
	while (!node->isLeaf())
	{
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_NODE;
		node = &node->children[findSubNode(node->bounds.center, point)];
		LASS_ASSERT(node); // if it's not a leaf, there should be a child node
	}

	for (typename TObjectIterators::const_iterator i = node->data.begin(); i != node->data.end(); ++i)
	{
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
		if (TObjectTraits::objectContains(*i, point, info))
		{
			return true;
		}
	}
	return false;
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator QuadTree<O, OT, SH>::find(const TPoint& point, OutputIterator result, const TInfo* info) const
{
	LASS_ASSERT(root_);

	if (!TObjectTraits::aabbContains(aabb_, point))
	{
		return result;
	}

	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	QuadNode* node = root_;
	while (!node->isLeaf())
	{
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_NODE;
		node = &node->children[findSubNode(node->bounds.center, point)];
		LASS_ASSERT(node); // if it's not a leaf, there should be a child node
	}

	for (typename TObjectIterators::const_iterator i = node->data.begin(); i != node->data.end(); ++i)
	{
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
		if (TObjectTraits::objectContains(*i, point, info))
		{
			*result++ = *i;
		}
	}
	return result;
}



/** @warning As this algorithm visits multiple leaf nodes, it may find duplicate objects.
 *      This algorithm doesn't care.  But if you do, you can use insert_iterators to a set.
 */
template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator QuadTree<O, OT, SH>::find(const TAabb& box, OutputIterator result, const TInfo* info) const
{
	LASS_ASSERT(root_);
	if (!TObjectTraits::aabbIntersects(aabb_, box))
	{
		return result;
	}
	const CenteredBox centeredBox(box);
	return doFind(*root_, box, centeredBox, result, info);
}



template <typename O, typename OT, typename SH>
const typename QuadTree<O, OT, SH>::TObjectIterator 
QuadTree<O, OT, SH>::intersect(
		const TRay& ray, TReference t, TParam tMin, const TInfo* info) const
{
	LASS_ASSERT(root_);

	const TPoint min = TObjectTraits::aabbMin(aabb_);
	const TPoint max = TObjectTraits::aabbMax(aabb_);
	const TPoint center = middle(min, max);
	TPoint support = TObjectTraits::raySupport(ray);
	TVector direction = TObjectTraits::rayDirection(ray);
	const size_t flipMask = forcePositiveDirection(center, support, direction);
	const TVector reciprocalDirection = TObjectTraits::vectorReciprocal(direction);
	TVector tNear;
	TVector tFar;
	nearAndFar(min, max, support, reciprocalDirection, tNear, tFar);
	const TValue tNearMax = maxComponent(tNear);
	const TValue tFarMin = minComponent(tFar);
	if (tFarMin < tNearMax || tFarMin <= tMin)
	{
		return end_;
	}

	return doIntersect(*root_, ray, t, tMin, info, tNear, tFar, flipMask);
}



template <typename O, typename OT, typename SH>
bool QuadTree<O, OT, SH>::intersects(
		const TRay& ray, TParam tMin, TParam tMax, const TInfo* info) const
{
	LASS_ASSERT(root_);

	const TPoint min = TObjectTraits::aabbMin(aabb_);
	const TPoint max = TObjectTraits::aabbMax(aabb_);
	const TPoint center = middle(min, max);
	//const TVector size = halfDifference(max, min);
	TPoint support = TObjectTraits::raySupport(ray);
	TVector direction = TObjectTraits::rayDirection(ray);
	const size_t flipMask = forcePositiveDirection(center, support, direction);
	const TVector reciprocalDirection = TObjectTraits::vectorReciprocal(direction);
	TVector tNear;
	TVector tFar;
	nearAndFar(min, max, support, reciprocalDirection, tNear, tFar);
	const TValue tNearMax = maxComponent(tNear);
	const TValue tFarMin = minComponent(tFar);
	if (tFarMin < tNearMax || tFarMin <= tMin)
	{
		return false;
	}

	return doIntersects(*root_, ray, tMin, tMax, info, tNear, tFar, flipMask);
}



template <typename O, typename OT, typename SH>
const typename QuadTree<O, OT, SH>::Neighbour
QuadTree<O, OT, SH>::nearestNeighbour(const TPoint& point, const TInfo* info) const
{
	LASS_ASSERT(root_);
	Neighbour nearest(end_, std::numeric_limits<TValue>::infinity());
	doNearestNeighbour(*root_, point, info, nearest);
	return nearest;
}



template <typename O, typename OT, typename SH>
template <typename RandomAccessIterator>
RandomAccessIterator
QuadTree<O, OT, SH>::rangeSearch(
		const TPoint& target, TParam maxRadius, size_t maxCount, RandomAccessIterator first, 
		const TInfo* info) const
{
	LASS_ASSERT(root_);
	if (maxRadius == 0)
	{
		return first;
	}
	TValue squaredRadius = maxRadius * maxRadius;
	return doRangeSearch(*root_, target, squaredRadius, maxCount, first, first, info);
}



template <typename O, typename OT, typename SH>
size_t QuadTree<O, OT, SH>::objectCount() const
{
	LASS_ASSERT(root_);
	return root_->objectCount();
}



template <typename O, typename OT, typename SH>
const typename QuadTree<O, OT, SH>::TAabb& 
QuadTree<O, OT, SH>::aabb() const
{
	return aabb_;
}



template <typename O, typename OT, typename SH>
size_t QuadTree<O, OT, SH>::depth() const
{
	LASS_ASSERT(root_);
	return root_->depth();
}




template <typename O, typename OT, typename SH>
const typename QuadTree<O, OT, SH>::TValue
QuadTree<O, OT, SH>::averageDepth() const
{
	LASS_ASSERT(root_);
	return root_->averageDepth();
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::swap(QuadTree& other)
{
	SH::swap(other);
	nodesAllocator_.swap(other.nodesAllocator_);
	std::swap(aabb_, other.aabb_);
	std::swap(root_, other.root_);
	std::swap(end_, other.end_);
}



template <typename O, typename OT, typename SH>
const typename QuadTree<O, OT, SH>::TObjectIterator
QuadTree<O, OT, SH>::end() const
{
	return end_;
}



// --- private -------------------------------------------------------------------------------------


template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator QuadTree<O, OT, SH>::doFind(
	const QuadNode& node, const TAabb& box, const CenteredBox& centeredBox, OutputIterator output, 
	const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);

	if (node.isLeaf())
	{
		const typename TObjectIterators::const_iterator end = node.data.end();
		for (typename TObjectIterators::const_iterator i = node.data.begin(); i != end; ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectIntersects(*i, box, info))
			{
				*output++ = *i;
			}
		}
		return output;
	}
	for (size_t i = 0; i < numChildren; ++i)
	{
		if (node.children[i].bounds.intersects(centeredBox))
		{
			output = doFind(node.children[i], box, centeredBox, output, info);
		}
	}
	return output;
}



/**
 *	Reference: J. Revelles, C. Urena, and M. Lastra. An efficient parametric algorithm for octree 
 *	traversal. In Eighth International Conference in Central Europe on Computer Graphics, 
 *	Visualization and Interactive Digital Media (WSCG 2000), pages 212–-219, Plzen, Czech Republic, 
 *	2000.
 */
template <typename O, typename OT, typename SH>
const typename QuadTree<O, OT, SH>::TObjectIterator 
QuadTree<O, OT, SH>::doIntersect(
		const QuadNode& node,
		const TRay& ray, TReference t, TParam tMin, const TInfo* info,
		const TVector& tNear, const TVector& tFar, size_t flipMask) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	if (minComponent(tFar) < tMin)
	{
		return end_;
	}


	if (node.isLeaf())
	{
		const size_t n = node.data.size();
		TValue tBest = 0;
		TObjectIterator best = end_;
		for (size_t i = 0; i < n; ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			TValue tCandidate = 0;
			if (TObjectTraits::objectIntersect(node.data[i], ray, tCandidate, tMin, info))
			{
				LASS_ASSERT(tCandidate > tMin);
				if (best == end_ || tCandidate < tBest)
				{
					LASS_ASSERT(tCandidate > tMin);
					best = node.data[i];
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

	const TVector tMiddle = middle(tNear, tFar);
	TObjectIterator best = end_;
	TValue tBest = 0;
	size_t i = entryNode(tNear, tMiddle);
	do
	{
		const QuadNode& child = node.children[i ^ flipMask];
		TVector tChildNear = tNear;
		TVector tChildFar = tFar;
		childNearAndFar(tChildNear, tChildFar, tMiddle, i);
		
		TValue tCandidate;
		TObjectIterator candidate = doIntersect(
			child, ray, tCandidate, tMin, info, tChildNear, tChildFar, flipMask);
		if (candidate != end_)
		{
			if (best == end_ || tCandidate < tBest)
			{
				best = candidate;
				tBest = tCandidate;
			}
		}

		if (best != end_ && tBest < minComponent(tChildFar))
		{
			t = tBest;
			return best;
		}

		i = nextNode(i, tChildFar);
	}
	while (i != size_t(-1));

	if (best != end_)
	{
		t = tBest;
	}
	return best;
}



template <typename O, typename OT, typename SH>
bool QuadTree<O, OT, SH>::doIntersects(
		const QuadNode& node,
		const TRay& ray, TParam tMin, TParam tMax, const TInfo* info,
		const TVector& tNear, const TVector& tFar, size_t flipMask) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	if (minComponent(tFar) < tMin || maxComponent(tNear) > tMax)
	{
		return false;
	}

	if (node.isLeaf())
	{
		LASS_ASSERT(std::count(node->node, node->node + numChildren, static_cast<QuadNode*>(0)) == numChildren);

		const size_t n = node->data.size();
		for (size_t i = 0; i < n; ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			if (TObjectTraits::objectIntersects(node->data[i], ray, tMin, tMax, info))
			{
				return true;
			}
		}

		return false;
	}

	const TVector tMiddle = middle(tNear, tFar);
	size_t i = entryNode(tNear, tMiddle);
	do
	{
		const QuadNode& child = node.children[i ^ flipMask];
		TVector tChildNear = tNear;
		TVector tChildFar = tFar;
		childNearAndFar(tChildNear, tChildFar, tMiddle, i);
		if (doIntersects(child, ray, tMin, tMax, info, tChildNear, tChildFar, flipMask))
		{
			return true;
		}
		i = nextNode(i, tChildFar);
	}
	while (i != size_t(-1));

	return false;
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::doNearestNeighbour(
		const QuadNode& node, const TPoint& point, const TInfo* info, Neighbour& best) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	if (node.isLeaf())
	{
		const size_t n = node.data.size();
		for (size_t i = 0; i < n; ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			const TValue squaredDistance = 
				TObjectTraits::objectSquaredDistance(node.data[i], point, info);
			if (squaredDistance < best.squaredDistance())
			{
				best = Neighbour(node.data[i], squaredDistance);
			}
		}
	}
	else
	{
		// first, determine squared distances to children and find closest one.
		TValue sqrNodeDists[numChildren];
		size_t nearestNode = 0;
		for (size_t i = 0; i < numChildren; ++i)
		{
			sqrNodeDists[i] = node.children[i].bounds.sqrDistance(point);
			if (sqrNodeDists[i] < sqrNodeDists[nearestNode])
			{
				nearestNode = i;
			}
		}

		// visit closest node first, and then the others.
		if (sqrNodeDists[nearestNode] < best.squaredDistance())
		{
			doNearestNeighbour(node.children[nearestNode], point, info, best);
		}
		for (size_t i = 0; i < numChildren; ++i)
		{
			if (sqrNodeDists[i] < best.squaredDistance() && i != nearestNode)
			{
				doNearestNeighbour(node.children[i], point, info, best);
			}
		}
	}
}



template <typename O, typename OT, typename SH>
template <typename RandomIterator>
RandomIterator QuadTree<O, OT, SH>::doRangeSearch(
	const QuadNode& node, const TPoint& target, TReference squaredRadius, size_t maxCount,
	RandomIterator first, RandomIterator last, const TInfo* info) const
{
	LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
	LASS_ASSERT(squaredRadius >= 0);

	if (node.isLeaf())
	{
		const size_t n = node.data.size();
		for (size_t i = 0; i < n; ++i)
		{
			LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
			const TValue sqrDist = TObjectTraits::objectSquaredDistance(node.data[i], target, info);
			if (sqrDist < squaredRadius)
			{
				Neighbour candidate(node.data[i], sqrDist);
// TODO: use a push_heap_unique to avoid duplicates instead of naive search [Bramz]
// https://sourceforge.net/tracker2/?func=detail&aid=2517748&group_id=118315&atid=680768
				if (std::find(first, last, candidate) == last)
				{
					*last++ = candidate;
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
		}
		return last;
	}

	// first, determine squared distances to children and find closest one.
	TValue sqrNodeDists[numChildren];
	size_t nearestNode = 0;
	for (size_t i = 0; i < numChildren; ++i)
	{
		sqrNodeDists[i] = node.children[i].bounds.sqrDistance(target);
		if (sqrNodeDists[i] < sqrNodeDists[nearestNode])
		{
			nearestNode = i;
		}
	}

	if (sqrNodeDists[nearestNode] < squaredRadius)
	{
		last = doRangeSearch(node.children[nearestNode], target, squaredRadius, maxCount, first, last, info);
	}
	for (size_t i = 0; i < numChildren; ++i)
	{
		if (sqrNodeDists[i] < squaredRadius && i != nearestNode)
		{
			last = doRangeSearch(node.children[i], target, squaredRadius, maxCount, first, last, info);
		}
	}
	return last;
}



// --- CenteredBox ---------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
QuadTree<O, OT, SH>::CenteredBox::CenteredBox(const TPoint& center, const TVector& extents):
	center(center),
	extents(extents)
{
}



template <typename O, typename OT, typename SH>
QuadTree<O, OT, SH>::CenteredBox::CenteredBox(const TAabb& box)
{
	const TPoint min = TObjectTraits::aabbMin(box);
	const TPoint max = TObjectTraits::aabbMax(box);
	center = middle(min, max);
	extents = halfExtents(max, min);
}



template <typename O, typename OT, typename SH>
bool QuadTree<O, OT, SH>::CenteredBox::intersects(const CenteredBox& other) const
{
	for (size_t k = 0; k < dimension; ++k)
	{
		const TValue centerToCenter = num::abs(TObjectTraits::coord(center, k) - TObjectTraits::coord(other.center, k));
		const TValue totalExtents = TObjectTraits::coord(extents, k) + TObjectTraits::coord(other.extents, k);
		if (centerToCenter >= totalExtents)
		{
			return false;
		}
	}
	return true;
}



template <typename O, typename OT, typename SH>
const typename QuadTree<O, OT, SH>::TValue
QuadTree<O, OT, SH>::CenteredBox::sqrDistance(const TPoint& point) const
{
	TValue sqrDist = 0;
	for (size_t k = 0; k < dimension; ++k)
	{
		const TValue d = num::abs(TObjectTraits::coord(center, k) - TObjectTraits::coord(point, k)) -
			TObjectTraits::coord(extents, k);
		if (d > 0)
		{
			sqrDist += d * d;
		}
	}
	return sqrDist;
}



// --- QuadNode ------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
QuadTree<O, OT, SH>::QuadNode::QuadNode(const CenteredBox& bounds, TNodesAllocator& allocator):
	children(0),
	bounds(bounds), 
	allocator_(allocator)
{
}



template <typename O, typename OT, typename SH>
QuadTree<O, OT, SH>::QuadNode::~QuadNode()
{
	deleteChildren(numChildren);
}



template <typename O, typename OT, typename SH>
size_t QuadTree<O, OT, SH>::QuadNode::objectCount() const
{
	size_t cumulCount = data.size();
	if (!isLeaf())
	{
		for (size_t i=0;i<numChildren;++i)
		{
			cumulCount += children[i].objectCount();
		}
	}
	return cumulCount;
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::QuadNode::add(
		TObjectIterator object, const TSplitHeuristics& heuristics, size_t level, bool mayDecompose)
{
	if (isLeaf())
	{
		data.push_back(object);
		if (mayDecompose && data.size() > heuristics.maxObjectsPerLeaf() && level < heuristics.maxDepth())
		{
			decompose(heuristics, level);
		}
	}
	else
	{
		for (size_t i=0;i<numChildren;++i)
		{
			if (TObjectTraits::objectIntersects(object, children[i].aabb(), 0))
			{
				children[i].add(object, heuristics, level + 1);
			}
		}
	}
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::QuadNode::remove(
		TObjectIterator object)
{
	if (isLeaf())
	{
		typename TObjectIterators::iterator last = std::remove(data.begin(), data.end(), object);
		data.erase(last, data.end());
	}
	else
	{
		for (size_t i=0;i<numChildren;++i)
		{
			if (TObjectTraits::objectIntersects(object, children[i].aabb(), 0))
			{
				children[i].remove(object);
			}
		}
	}
}



template <typename O, typename OT, typename SH>
typename QuadTree<O, OT, SH>::TAabb QuadTree<O, OT, SH>::QuadNode::aabb() const
{
	/* Reconstruction of the aabb everytime an object gets inserted to the quadnode
	*  Altough this is not very efficient, it is only needed during construction.  Better
	*  storage via center and dx,dy can give more efficient lookup code which is the main
	*  interest.  Storing the aabb would give too much overhead in comparison with the gain
	*  in speed.
	*/
	return TAabb(bounds.center - bounds.extents, bounds.center + bounds.extents);
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::QuadNode::decompose(const TSplitHeuristics& heuristics, size_t level)
{
	if (!isLeaf())
	{
		return;
	}

	makeChildren();

	TAabb nodeAabb[numChildren];       // cache node aabb
	for (size_t i=0;i<numChildren;++i)
		nodeAabb[i] = children[i].aabb();

	//const size_t maxCopies = numChildren * data.size() * 2 / 3;
	size_t copies = 0;
	for (typename TObjectIterators::iterator vit = data.begin(); vit != data.end(); ++vit)
	{
		const TAabb tempAabb = TObjectTraits::objectAabb( *vit );
		
		/* for each object we test wether it is contained in one of the
		*  subnodes of the quadnode.  If it is even partially in one then move
		*  the object down the tree, but only one level
		*/
		for (size_t i = 0; i < numChildren; ++i)
		{
			if (TObjectTraits::objectIntersects(*vit, nodeAabb[i], 0))
			{
				children[i].add(*vit, heuristics, level + 1, false);
				++copies;
			}
		}
	}

	data.clear();
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::QuadNode::absorb()
{
	if (isLeaf())
	{
		return;
	}

	for (size_t i=0;i<numChildren;++i)
	{
		children[i].absorb();
		std::copy(children[i].data.begin(), children[i].data.end(), std::back_inserter(data));
	}
	deleteChildren(numChildren);

	std::sort(data.begin(), data.end());
	typename TObjectIterators::iterator last = std::unique(data.begin(), data.end());
	data.erase(last, data.end());
}



template <typename O, typename OT, typename SH>
size_t QuadTree<O, OT, SH>::QuadNode::depth() const
{
	if (isLeaf())
		return 1;

	size_t depth=node[0].depth();
	for (size_t i=1;i<numChildren;++i)
		depth = std::max(depth,children[i].depth());
	return depth + 1;
}



template <typename O, typename OT, typename SH>
const typename QuadTree<O, OT, SH>::TValue 
QuadTree<O, OT, SH>::QuadNode::averageDepth() const
{
	if (isLeaf())
		return 1;

	TValue depth = 0;
	for (size_t i = 1; i < numChildren; ++i)
		depth += children[i].averageDepth();
	return depth / numChildren + 1;
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::QuadNode::makeChildren()
{
	if (children)
	{
		return;
	}

	TVector newExtents(bounds.extents);
	for (size_t k = 0; k < dimension; ++k)
	{
		TObjectTraits::coord(newExtents, k, TObjectTraits::coord(newExtents, k) / 2);
	}

	children = static_cast<QuadNode*>(allocator_.allocate());
	size_t i = 0;
	try
	{
		for (i = 0; i < numChildren; ++i)
		{
			TPoint newCenter = bounds.center;
			for (size_t k = 0, mask = 1; k < dimension; ++k, mask *= 2)
			{
				TObjectTraits::coord(newCenter, k, TObjectTraits::coord(newCenter, k) + (i & mask ? 1 : -1) * TObjectTraits::coord(newExtents, k));
			}
			new (&children[i]) QuadNode(TCenteredBox(newCenter, newExtents), allocator_);
		}
	}
	catch (...)
	{
		deleteChildren(i);
		throw;
	}
}



template <typename O, typename OT, typename SH>
void QuadTree<O, OT, SH>::QuadNode::deleteChildren(size_t count)
{
	if (!children)
	{
		return;
	}
	while (count)
	{
		children[--count].~QuadNode();
	}
	allocator_.deallocate(children);
	children = 0;
}


}
}

#endif

// EOF
