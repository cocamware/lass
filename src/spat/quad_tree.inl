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

template <typename O, typename OT>
QuadTree<O, OT>::QuadTree(size_t maxSize, size_t maxLevel):
	root_(0),
	maxSize_(maxSize),
	maxLevel_(maxLevel)
{
}



template <typename O, typename OT>
QuadTree<O, OT>::~QuadTree()
{
	delete root_;
}

/*
template
<
	class ObjectType,
	typename ObjectTraits
>
QuadTree<O, OT>::QuadTree(const TPoint& center,const TVector& extents, size_t maxSize, size_t maxLevel):
	root_(new QuadNode(center, extents)),
	maxSize_(maxSize),
	maxLevel_(maxLevel)
{
}

template
<
	class ObjectType,
	typename ObjectTraits
>
QuadTree<O, OT>::QuadTree(const TAabb& box, size_t maxSize, size_t maxLevel):
	maxSize_(maxSize),
	maxLevel_(maxLevel)
{
	TVector extents = box.max()-box.min();
	extents *= 0.5;
	root_ = new QuadNode(box.center(), extents);
}
*/

template <typename O, typename OT>
QuadTree<O, OT>::QuadTree(TObjectIterator first, TObjectIterator last, size_t maxSize, size_t maxLevel):
	root_(0),
	end_(last),
	maxSize_(maxSize),
	maxLevel_(maxLevel)
{
	aabb_ = TObjectTraits::aabbEmpty();
	for (TObjectIterator i = first; i != last; ++i)
	{
		aabb_ = TObjectTraits::aabbJoin(aabb_, TObjectTraits::objectAabb(i));
	}

	const TPoint min = TObjectTraits::aabbMin(aabb_);
	const TPoint max = TObjectTraits::aabbMax(aabb_);
	TPoint center;
	TVector extents;
	for (size_t k = 0; k < dimension; ++k)
	{
		TObjectTraits::coord(center, k, (TObjectTraits::coord(max, k) + TObjectTraits::coord(min, k)) / 2);
		TObjectTraits::coord(extents, k, (TObjectTraits::coord(max, k) - TObjectTraits::coord(min, k)) / 2);
	}

	root_ = new QuadNode(center, extents);
	while (first != last)
	{
		add(first++);
	}
}



template <typename O, typename OT>
void QuadTree<O, OT>::reset()
{
	QuadTree temp;
	swap(temp);
}



template <typename O, typename OT>
void QuadTree<O, OT>::reset(TObjectIterator first, TObjectIterator last)
{
	QuadTree temp(first, last);
	swap(temp);
}



template <typename O, typename OT>
void QuadTree<O, OT>::add(TObjectIterator object)
{
	if (!TObjectTraits::aabbContains(aabb_, TObjectTraits::objectAabb(object)))
	{
		LASS_THROW("object not within QuadTree");
	}
	root_->add(object, maxSize_, maxLevel_);
}



template <typename O, typename OT>
const bool QuadTree<O, OT>::contains(const TPoint& point, const TInfo* info = 0) const
{
	if (!root_ || !TObjectTraits::aabbContains(aabb_, point))
	{
		return false;
	}

	QuadNode* node = root_;
	while (!node->leaf)
	{
		node = node->node[findSubNode(node->center, point)];
		LASS_ASSERT(node); // if it's not a leaf, there should be a child node
	}

	for (TObjectIterators::const_iterator i = node->data.begin(); i != node->data.end(); ++i)
	{
		if (TObjectTraits::objectContains(*i, point, info))
		{
			return true;
		}
	}
	return false;
}



template <typename O, typename OT>
template <typename OutputIterator>
OutputIterator QuadTree<O, OT>::find(const TPoint& point, OutputIterator result, const TInfo* info = 0) const
{
	if (!root_ || !TObjectTraits::aabbContains(aabb_, point))
	{
		return result;
	}

	QuadNode* node = root_;
	while (!node->leaf)
	{
		node = node->node[findSubNode(node->center, point)];
		LASS_ASSERT(node); // if it's not a leaf, there should be a child node
	}

	for (TObjectIterators::const_iterator i = node->data.begin(); i != node->data.end(); ++i)
	{
		if (TObjectTraits::objectContains(*i, point, info))
		{
			*result++ = *i;
		}
	}
	return result;
}



template <typename O, typename OT>
const typename QuadTree<O, OT>::TObjectIterator 
QuadTree<O, OT>::intersect(
		const TRay& ray, TReference t, TParam tMin, const TInfo* info) const
{
	if (!root_)
	{
		return end_;
	}

#ifdef LASS_TEST_SPAT_OBJECT_TREES_DIAGNOSTICS
	if (info) (*(io::XmlOStream*)info) << ray;
#endif

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

	return doIntersect(root_, ray, t, tMin, info, tNear, tFar, flipMask);
}



template <typename O, typename OT>
const bool QuadTree<O, OT>::intersects(
		const TRay& ray, TParam tMin, TParam tMax, const TInfo* info) const
{
	if (!root_)
	{
		return false;
	}

	const TPoint min = TObjectTraits::aabbMin(aabb_);
	const TPoint max = TObjectTraits::aabbMax(aabb_);
	const TVector size = subtract(max, min);
	TPoint support = TObjectTraits::raySupport(ray);
	TVector direction = TObjectTraits::rayDirection(ray);
	const size_t flipMask = forcePositiveDirection(size, support, direction);
	const TVector reciprocalDirection = TObjectTraits::vectorReciprocal(direction);
	const TVector tNear;
	const TVector tFar;
	nearAndFar(min, max, support, reciprocalDirection, tNear, tFar);
	const TValue tNearMax = maxComponent(tNear);
	const TValue tFarMin = minComponent(tFar);
	if (tFarMin < tNearMax || tFarMin <= tMin)
	{
		return false;
	}

	return doIntersects(node, ray, tMin, tMax, info, tNear, tFar, flipMask);
}



template <typename O, typename OT>
const typename QuadTree<O, OT>::Neighbour
QuadTree<O, OT>::nearestNeighbour(const TPoint& point, const TInfo* info) const
{
	Neighbour nearest(end_, std::numeric_limits<TValue>::infinity());
	if (root_)
	{
		doNearestNeighbour(root_, point, info, nearest);
	}
	return nearest;
}



template <typename O, typename OT>
size_t QuadTree<O, OT>::objectCount() const
{
	return root_ ? root_->objectCount() : 0;
}



template <typename O, typename OT>
const size_t QuadTree<O, OT>::depth() const
{
	return root_ ? root_->depth() : 0;
}




template <typename O, typename OT>
const typename QuadTree<O, OT>::TValue
QuadTree<O, OT>::averageDepth() const
{
	return root_ ? root_->averageDepth() : 0;
}



template <typename O, typename OT>
void QuadTree<O, OT>::swap(QuadTree& other)
{
	std::swap(aabb_, other.aabb_);
	std::swap(root_, other.root_);
	std::swap(end_, other.end_);
	std::swap(maxSize_, other.maxSize_);
	std::swap(maxLevel_, other.maxLevel_);
}



template <typename O, typename OT>
const typename QuadTree<O, OT>::TObjectIterator
QuadTree<O, OT>::end() const
{
	return end_;
}



// --- private -------------------------------------------------------------------------------------

template <typename O, typename OT>
const typename QuadTree<O, OT>::TObjectIterator 
QuadTree<O, OT>::doIntersect(
		const QuadNode* node,
		const TRay& ray, TReference t, TParam tMin, const TInfo* info,
		const TVector& tNear, const TVector& tFar, size_t flipMask) const
{
	if (!node || minComponent(tFar) < tMin)
	{
		return end_;
	}

#ifdef LASS_TEST_SPAT_OBJECT_TREES_DIAGNOSTICS
	if (info) (*(io::XmlOStream*)info) << node->aabb();
#endif

	if (node->leaf)
	{
		LASS_ASSERT(std::count(node->node, node->node + subNodeCount, static_cast<QuadNode*>(0)) == subNodeCount);

		const size_t n = node->data.size();
		TValue tBest = 0;
		TObjectIterator best = end_;
		for (size_t i = 0; i < n; ++i)
		{
			TValue tCandidate;
			if (TObjectTraits::objectIntersect(node->data[i], ray, tCandidate, tMin, info))
			{
				LASS_ASSERT(tCandidate > tMin);
				if (best == end_ || tCandidate < tBest)
				{
					LASS_ASSERT(tCandidate > tMin);
					best = node->data[i];
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
		QuadNode* const child = node->node[i ^ flipMask];
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



template <typename O, typename OT>
const bool QuadTree<O, OT>::doIntersects(
		const QuadNode* node,
		const TRay& ray, TParam tMin, TParam tMax, const TInfo* info,
		const TVector& tNear, const TVector& tFar, size_t flipMask) const
{
	if (!node || minComponent(tFar) < tMin || maxComponent(tNear) > tMax)
	{
		return false;
	}

	if (node->leaf)
	{
		LASS_ASSERT(std::count(node->node, node->node + subNodeCount, static_cast<QuadNode*>(0)) == subNodeCount);

		const size_t n = node->data.size();
		TValue tBest = 0;
		TObjectIterator best = end_;
		for (size_t i = 0; i < n; ++i)
		{
			TValue tCandidate;
			if (TObjectTraits::objectIntersects(node->data[i], ray, tMin, tMax, info))
			{
				return true;
			}
		}

		return false;
	}

	const TVector tMiddle = middle(tNear, tFar);
	size_t i = entryNode(tNear);
	do
	{
		QuadNode* const child = node->node[i ^ flipMask];
		TVector tChildNear = tNear;
		TVector tChildFar = tFar;
		childNearAndFar(tNearChild, tFarChild, tMiddle, i);
		TValue t;
		if (doIntersects(child, ray, t, tMin, tMax, info, tChildNear, tChildFar, flipMask))
		{
			return true;
		}
		i = nextNode(i, tChildFar);
	}
	while (i != size_t(-1));

	return false;
}



template <typename O, typename OT>
void QuadTree<O, OT>::doNearestNeighbour(
		const QuadNode* node, const TPoint& point, const TInfo* info, Neighbour& best) const
{
	if (node->leaf)
	{
		const size_t n = node->data.size();
		for (size_t i = 0; i < n; ++i)
		{
			const TValue squaredDistance = 
				TObjectTraits::objectSquaredDistance(node->data[i], point, info);
			if (squaredDistance < best.squaredDistance())
			{
				best = Neighbour(node->data[i], squaredDistance);
			}
		}
	}
	else
	{
		// first, determine squared distances to children and find closest one.
		TValue sqrNodeDists[subNodeCount];
		size_t nearestNode = 0;
		for (size_t i = 0; i < subNodeCount; ++i)
		{
			sqrNodeDists[i] = 0;
			const TPoint& center = node->node[i]->center;
			const TVector& extents = node->node[i]->extents;
			for (size_t k = 0; k < dimension; ++k)
			{
				const TValue d = num::abs(TObjectTraits::coord(center, k) - TObjectTraits::coord(point, k)) -
					TObjectTraits::coord(extents, k);
				if (d > 0)
				{
					sqrNodeDists[i] += d * d;
				}
			}
			if (sqrNodeDists[i] < sqrNodeDists[nearestNode])
			{
				nearestNode = i;
			}
		}

		// visit closest node first, and then the others.
		if (sqrNodeDists[nearestNode] < best.squaredDistance())
		{
			doNearestNeighbour(node->node[nearestNode], point, info, best);
		}
		for (size_t i = 0; i < subNodeCount; ++i)
		{
			if (sqrNodeDists[i] < best.squaredDistance() && i != nearestNode)
			{
				doNearestNeighbour(node->node[i], point, info, best);
			}
		}
	}
}



// --- QuadNode ------------------------------------------------------------------------------------

template <typename O, typename OT>
QuadTree<O, OT>::QuadNode::QuadNode() : leaf(true), level(0), listSize(0)
{
}



template <typename O, typename OT>
QuadTree<O, OT>::QuadNode::QuadNode( const TPoint& center, const TVector& extents) :
	center(center), extents(extents), leaf(true)
{
	std::fill(node, node + subNodeCount, static_cast<QuadNode*>(0));
}



template <typename O, typename OT>
QuadTree<O, OT>::QuadNode::~QuadNode()
{
	// destruct in reverse order
	size_t i = subNodeCount;
	while (i > 0)
	{
		delete node[--i];
	}
}



template <typename O, typename OT>
size_t QuadTree<O, OT>::QuadNode::objectCount() const
{
	size_t cumulCount = data.size();
	for (size_t i=0;i<subNodeCount;++i)
	{
		if (node[i])
			cumulCount += node[i]->objectCount();
	}
	return cumulCount;
}



template <typename O, typename OT>
void QuadTree<O, OT>::QuadNode::add(
		TObjectIterator object, size_t maxSize, size_t maxLevel, size_t level, bool mayDecompose)
{
	if (leaf)
	{
		data.push_back(object);
		if (mayDecompose && data.size() > maxSize && level < maxLevel)
		{
			decompose(maxSize, maxLevel, level);
		}
	}
	else
	{
		for (size_t i=0;i<subNodeCount;++i)
		{
			if (TObjectTraits::objectOverlaps(object, node[i]->aabb()))
			{
				node[i]->add(object, maxSize, maxLevel, level + 1);
			}
		}
	}
}



template <typename O, typename OT>
typename QuadTree<O, OT>::TAabb QuadTree<O, OT>::QuadNode::aabb() const
{
	/* Reconstruction of the aabb everytime an object gets inserted to the quadnode
	*  Altough this is not very efficient, it is only needed during construction.  Better
	*  storage via center and dx,dy can give more efficient lookup code which is the main
	*  interest.  Storing the aabb would give too much overhead in comparison with the gain
	*  in speed.
	*/
	//std::cout << "center:"<<center << std::endl;
	//std::cout << "extents:"<<extents << std::endl;
	TAabb result(center-extents,center+extents);
	//std::cout << "result:"<<result << std::endl;
	return result;
}



template <typename O, typename OT>
void QuadTree<O, OT>::QuadNode::decompose(size_t maxSize, size_t maxLevel, size_t level)
{
	if (leaf) // only decompose leaf nodes, others are already decomposed
	{
		buildSubNodes(this);
		leaf = false;

		TAabb nodeAabb[subNodeCount];       // cache node aabb
		for (size_t i=0;i<subNodeCount;++i)
			nodeAabb[i] = node[i]->aabb();

		const size_t maxCopies = subNodeCount * data.size() * 2 / 3;
		size_t copies = 0;
		for (typename TObjectIterators::iterator vit = data.begin(); vit != data.end(); ++vit)
		{
			const TAabb tempAabb = TObjectTraits::objectAabb( *vit );
			
			/* for each object we test wether it is contained in one of the
			*  subnodes of the quadnode.  If it is even partially in one then move
			*  the object down the tree, but only one level
			*/
			for (size_t i = 0; i < subNodeCount; ++i)
			{
				if (TObjectTraits::objectOverlaps(*vit, nodeAabb[i]))
				{
					node[i]->add(*vit, maxSize, maxLevel, level + 1, false);
					++copies;
				}
			}
		}

		data.clear();
	}
}



template <typename O, typename OT>
void QuadTree<O, OT>::QuadNode::absorb()
{
	if (!leaf)
	{
		for (size_t i=0;i<subNodeCount;++i)
		{
			node[i]->absorb();
			std::copy(node[i]->data.begin(), node[i]->data.end(), std::back_inserter(data));
			delete node[i];
		}
		std::sort(data.begin(), data.end());
		TObjectIterators::iterator last = std::unique(data.begin(), data.end());
		data.erase(last, data.end());
		leaf = true;
	}
}



template <typename O, typename OT>
size_t QuadTree<O, OT>::QuadNode::depth() const
{
	if (leaf)
		return 1;

	size_t depth=node[0]->depth();
	for (size_t i=1;i<subNodeCount;++i)
		depth = std::max(depth,node[i]->depth());
	return depth + 1;
}



template <typename O, typename OT>
const typename QuadTree<O, OT>::TValue 
QuadTree<O, OT>::QuadNode::averageDepth() const
{
	if (leaf)
		return 1;

	TValue depth = 0;
	for (size_t i = 1; i < subNodeCount; ++i)
		depth += node[i]->averageDepth();
	return depth / subNodeCount + 1;
}



}
}

#endif

// EOF
