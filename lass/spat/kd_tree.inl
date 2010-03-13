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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_KD_TREE_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_KD_TREE_INL

#include "spat_common.h"
#include "kd_tree.h"

#ifdef LASS_SPAT_KD_TREE_DIAGNOSTICS
#	include "../io/xml_o_file.h"
#	include "../meta/select.h"
#	include "../prim/aabb_2d.h"
#	include "../prim/aabb_3d.h"
#endif

namespace lass
{
namespace spat
{

// --- public --------------------------------------------------------------------------------------

/** Constructs an empty k-d tree
 */
template <class O, class OT>
KdTree<O, OT>::KdTree():
	begin_(),
	end_()
{
}



/** Constructs a k-d tree from objects in range [first, last).
 *  @warning [first, last) must stay a valid range during the entire lifespan of the k-d tree!
 */
template <class O, class OT>
KdTree<O, OT>::KdTree(TObjectIterator first, TObjectIterator last):
	begin_(first),
	end_(last)
{
	size_ = std::distance(begin_, end_);
	heap_.resize(size_, Node(end_));

	TObjectIterators input;
	for (TObjectIterator i = begin_; i != end_; ++i)
	{
		input.push_back(i);
	}

	balance(0, input.begin(), input.end());
}



/** Resets to an empty tree.
 */
template <class O, class OT>
void KdTree<O, OT>::reset()
{
	TSelf temp;
	swap(temp);
}



/** Resets to a new k-d tree of objects in range [first, last).
 *  @warning [first, last) must stay a valid range during the entire lifespan of the k-d tree!
 */
template <class O, class OT>
void KdTree<O, OT>::reset(TObjectIterator first, TObjectIterator last)
{
	TSelf temp(first, last);
	swap(temp);
}



/** Locates the object that's nearest to a target position
 */
template <class O, class OT>
typename KdTree<O, OT>::Neighbour
KdTree<O, OT>::nearestNeighbour(const TPoint& target) const
{
	return nearestNeighbour(target, std::numeric_limits<TValue>::infinity());
}



/** Locates the object that's nearest to a target position, within a maximum range
 */
template <class O, class OT>
typename KdTree<O, OT>::Neighbour
KdTree<O, OT>::nearestNeighbour(const TPoint& target, TParam maxRadius) const
{
	if (isEmpty())
	{
		LASS_THROW("can't locate nearest neighbour in empty KdTree");
	}

	Neighbour best(end_, maxRadius);
	doNearestNeighbour(0, target, best);
	return best;
}



/** Locates objects within a spherical range around a target position.
 *
 *	@deprecated USE OVERLOADS WITH ITERATORS INSTEAD
 *
 *  @param target [in] the center of the spherical range
 *  @param maxRadius [in] the radius of the range
 *  @param maxCount [in] the maximum number of objects to be returned.
 *		@arg If this is zero, then all objects in the range are returned.
 *		@arg If this is non-zero, then up to @a maxCount objects are returned.
 *			These will be the ones closest to @a target
 *	@param neighbourhood [out] a std::vector that will be filled with the found objects.
 *			The vector will be @b cleared before use.
 *  @return the squared distance between @a target and the furthest found object.
 */
template <class O, class OT>
typename KdTree<O, OT>::TValue
KdTree<O, OT>::rangeSearch(
		const TPoint& target, TParam maxRadius, size_t maxCount,
		TNeighbourhood& neighbourhood) const
{
	if (isEmpty())
	{
		LASS_THROW("can't perform range search in empty KdTree");
	}

	if (maxCount == 0)
	{
		neighbourhood.clear();
		rangeSearch(target, maxRadius, std::back_inserter(neighbourhood));
		
		// neighbourhood is not a heap, find maximum squared distance
		TValue maxSquaredDistance = TValue();
		const typename TNeighbourhood::const_iterator end = neighbourhood.end();
		for (typename TNeighbourhood::const_iterator i = neighbourhood.begin(); i != end; ++i)
		{
			maxSquaredDistance = std::max(maxSquaredDistance, i->squaredDistance());
		}
		return maxSquaredDistance;
	}

	maxCount = std::min(maxCount, size_);
	neighbourhood.resize(maxCount + 1);

	typename TNeighbourhood::iterator last = rangeSearch(
		target, maxRadius, maxCount, neighbourhood.begin());
	neighbourhood.erase(last, neighbourhood.end());
	
	if (neighbourhood.empty())
	{
		return TValue();
	}
	return neighbourhood.front().squaredDistance();
}



/** Find all objects in a radius of @a maxRadius of @a target.
 *  @param target [in] center of range.
 *	@param maxRadius [in] radius of range
 *	@param first [in] output iterator dereferencable to Neighbour.
 *	@return output iterator @e last so that [@a first, last) is the range of all found objects.
 *
 *	@note
 *		The range starting at @a first must be large enough to contain all found objects.  
 *		But since this number is probably unknown beforehand, you better use one of those
 *		inserter kind of iterators to add the results to a dynamic sized container. 
 *
 *	@note
 *		If you wish to use a fixed sized range, you best use the other rangeSearch overload
 *		taking a random access iterator and an extra parameter @a maxCount.
 */
template <class O, class OT>
template <typename OutputIterator>
OutputIterator
KdTree<O, OT>::rangeSearch(const TPoint& target, TParam maxRadius, OutputIterator first) const
{
	if (isEmpty() || maxRadius == 0)
	{
		return first;
	}
	const TValue squaredRadius = maxRadius * maxRadius;
	return doRangeSearch(0, target, squaredRadius, first);
}



/** Find up to a fixed number of objects in a radius of @a maxRadius of @a target.
 *  @param target [in] center of range.
 *	@param maxRadius [in] radius of range
 *	@param maxCount [in] maximum number of objects to be found.
 *	@param first [in] random access iterator dereferencable to Neighbour, 
 *		[@a first, @a first + @a maxCount + 1) must be a valid range.
 *	@return output iterator @e last so that [@a first, last) is the range of all found objects.
 *
 *	@note
 *		This overload will search for a maximum number of @a maxCount objects at a maximum
 *		distance of @a maxRadius of the center @a target.  When more than @a maxCount objects
 *		are within this distance, the closest objects will be selected.
 *		To select the closest objects, a heap is constructed on the iterator range, which is
 *		why random access iterators are required instead of regular output iterators.  This
 *		is also why there's need of an extra position in the range pointed to by @a first:
 *		there's need of an extra position to swap in/out new/old objects.  That's why you
 *		must make sure [@a first, @a first + @a maxCount + 1) is a valid range.
 *
 *	@note
 *		If you wish to find all points within the range of @a maxRadius, you better use the
 *		overload with the regular output iterator and without @a maxCount.
 */
template <class O, class OT>
template <typename RandomAccessIterator>
RandomAccessIterator
KdTree<O, OT>::rangeSearch(const TPoint& target, TParam maxRadius, size_t maxCount,
		RandomAccessIterator first) const
{
	if (isEmpty() || maxRadius == 0)
	{
		return first;
	}
	const TValue squaredRadius = maxRadius * maxRadius;
	return doRangeSearch(0, target, squaredRadius, maxCount, first, first);
}



/** Swap the representation of two k-d trees.
 */
template <class O, class OT>
void KdTree<O, OT>::swap(TSelf& other)
{
	std::swap(begin_, other.begin_);
	std::swap(end_, other.end_);
	heap_.swap(other.heap_);
	std::swap(size_, other.size_);
}



/** returns true if there are no objects in the k-d tree
 */
template <class O, class OT>
bool KdTree<O, OT>::isEmpty() const
{
	return heap_.empty();
}



/** resest the k-d tree to an empty one.
 */
template <class O, class OT>
void KdTree<O, OT>::clear()
{
	TSelf temp;
	swap(temp);
}



template <class O, class OT> inline
const typename KdTree<O, OT>::TObjectIterator
KdTree<O, OT>::end() const
{
	return end_;
}



// --- neighbour -----------------------------------------------------------------------------------

template <class O, class OT>
KdTree<O, OT>::Neighbour::Neighbour():
	object_(),
	squaredDistance_(0)
{
}



template <class O, class OT>
KdTree<O, OT>::Neighbour::Neighbour(TObjectIterator object, TValue squaredDistance):
	object_(object),
	squaredDistance_(squaredDistance)
{
}



template <class O, class OT>
inline typename KdTree<O, OT>::TObjectIterator
KdTree<O, OT>::Neighbour::object() const
{
	return object_;
}



template <class O, class OT>
inline typename KdTree<O, OT>::TPoint
KdTree<O, OT>::Neighbour::position() const
{
	return TObjectTraits::position(object_);
}



template <class O, class OT>
inline typename KdTree<O, OT>::TValue
KdTree<O, OT>::Neighbour::squaredDistance() const
{
	return squaredDistance_;
}



template <class O, class OT>
inline typename KdTree<O, OT>::TObjectReference
KdTree<O, OT>::Neighbour::operator*() const
{
	return *object_;
}



template <class O, class OT>
inline typename KdTree<O, OT>::TObjectIterator
KdTree<O, OT>::Neighbour::operator->() const
{
	return object_;
}



template <class O, class OT>
inline bool
KdTree<O, OT>::Neighbour::operator<(const Neighbour& other) const
{
	return squaredDistance_ < other.squaredDistance_;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <class O, class OT>
void KdTree<O, OT>::balance(size_t index, TIteratorIterator first, TIteratorIterator last)
{
	if (last == first)
	{
		return;
	}

	if (last == first + 1)
	{
		// exactly one element in content
		assignNode(index, *first, dummyAxis_);
		return;
	}

	// more than one, do the balance
	//
	const TAxis split = findSplitAxis(first, last);
	const size_t size = last - first;;
	const TIteratorIterator median = first + size / 2;
	std::nth_element(first, median, last, LessDim(split));
	assignNode(index, *median, split);

	balance(2 * index + 1, first, median);
	balance(2 * index + 2, median + 1, last);
}



template <class O, class OT>
typename KdTree<O, OT>::TAxis
KdTree<O, OT>::findSplitAxis(TIteratorIterator first, TIteratorIterator last) const
{
	TPoint min = TObjectTraits::position(*first);
	TPoint max = min;

	for (TIteratorIterator i = first + 1; i != last; ++i)
	{
		const TPoint position = TObjectTraits::position(*i);
		for (TAxis k = 0; k < dimension; ++k)
		{
			min[k] = std::min(min[k], position[k]);
			max[k] = std::max(max[k], position[k]);
		}
	}

	TAxis axis = 0;
	TValue maxDistance = max[0] - min[0];
	for (TAxis k = 1; k < dimension; ++k)
	{
		const TValue distance = max[k] - min[k];
		if (distance > maxDistance)
		{
			axis = k;
			maxDistance = distance;
		}
	}

	return axis;
}



template <class O, class OT>
inline void KdTree<O, OT>::assignNode(size_t index, TObjectIterator object, TAxis splitAxis)
{
	if (heap_.size() <= index)
	{
		heap_.resize(index + 1, Node(end_));
	}
	heap_[index] = Node(object, splitAxis);
}



template <class O, class OT>
size_t KdTree<O, OT>::findNode(size_t index, const TPoint& target) const
{
	const size_t size = heap_.size();
	if (index >= size || heap_[index].object() == end_)
	{
		return size;
	}
	const Node& node = heap_[index];

	const TAxis split = node.axis();
	if (split == dummyAxis_)
	{
		return index;
	}

	const TPoint pivot = node.position();
	const TValue delta = target[split] - pivot[split]; // distance to splitting plane
	const bool isLeftSide = delta < 0;
	const size_t result = findNode(2 * index + (isLeftSide ? 1 : 2), target);
	return result != size ? result : index;
}



template <class O, class OT>
void KdTree<O, OT>::doNearestNeighbour(size_t index, const TPoint& target, Neighbour& best) const
{
	if (index >= heap_.size() || heap_[index].object() == end_)
	{
		return;
	}
	const Node& node = heap_[index];
	const TPoint pivot = node.position();

	const TValue sqrDistance = squaredDistance(pivot, target);
	if (sqrDistance < best.squaredDistance())
	{
		best = Neighbour(node.object(), sqrDistance);
	}

	const TAxis split = node.axis();
	if (split != dummyAxis_)
	{
		const TValue delta = target[split] - pivot[split]; // distance to splitting plane
		if (delta < 0)
		{
			// we are left of the plane - search left node first
			doNearestNeighbour(2 * index + 1, target, best);
			if (num::sqr(delta) < best.squaredDistance())
			{
				doNearestNeighbour(2 * index + 2, target, best);
			}
		}
		else
		{
			// we are right of the plane - search right node first
			doNearestNeighbour(2 * index + 2, target, best);
			if (num::sqr(delta) < best.squaredDistance())
			{
				doNearestNeighbour(2 * index + 1, target, best);
			}
		}
	}
}



template <class O, class OT>
template <typename OutputIterator>
OutputIterator KdTree<O, OT>::doRangeSearch(
		size_t index, const TPoint& target, TParam squaredDistance,
		OutputIterator output) const
{
	if (index >= heap_.size() || heap_[index].object() == end_)
	{
		return output;
	}
	const Node& node = heap_[index];

	const TPoint pivot = node.position();
	const TAxis split = node.axis();
	if (split != dummyAxis_)
	{
		const TValue delta = target[split] - pivot[split]; // distance to splitting plane
		if (delta < TValue())
		{
			// we are left of the plane - search left node first
			output = doRangeSearch(2 * index + 1, target, squaredDistance, output);
			if (num::sqr(delta) < squaredDistance)
			{
				output = doRangeSearch(2 * index + 2, target, squaredDistance, output);
			}
		}
		else
		{
			// we are right of the plane - search right node first
			output = doRangeSearch(2 * index + 2, target, squaredDistance, output);
			if (num::sqr(delta) < squaredDistance)
			{
				output = doRangeSearch(2 * index + 1, target, squaredDistance, output);
			}
		}
	}

	const TValue sqrDistance = this->squaredDistance(pivot, target);
	if (sqrDistance < squaredDistance)
	{
		*output++ = Neighbour(node.object(), sqrDistance);
	}
	return output;
}



template <class O, class OT>
template <typename RandomIterator>
RandomIterator KdTree<O, OT>::doRangeSearch(
		size_t index, const TPoint& target, TReference squaredRadius, size_t maxCount,
		RandomIterator first, RandomIterator last) const
{
	if (index >= heap_.size() || heap_[index].object() == end_)
	{
		return last;
	}
	const Node& node = heap_[index];

	const TPoint pivot = node.position();
	const TAxis split = node.axis();
	if (split != dummyAxis_)
	{
		const TValue delta = target[split] - pivot[split]; // distance to splitting plane
		if (delta < TValue())
		{
			// we are left of the plane - search left node first
			last = doRangeSearch(
				2 * index + 1, target, squaredRadius, maxCount, first, last);
			if (num::sqr(delta) < squaredRadius)
			{
				last = doRangeSearch(
					 2 * index + 2, target, squaredRadius, maxCount, first, last);
			}
		}
		else
		{
			// we are right of the plane - search right node first
			last = doRangeSearch(
				2 * index + 2, target, squaredRadius, maxCount, first, last);
			if (num::sqr(delta) < squaredRadius)
			{
				last = doRangeSearch(
					2 * index + 1, target, squaredRadius, maxCount, first, last);
			}
		}
	}

	const TValue sqrDistance = squaredDistance(pivot, target);
	if (sqrDistance < squaredRadius)
	{
		*last++ = Neighbour(node.object(), sqrDistance);
		std::push_heap(first, last);
		LASS_ASSERT(last >= first);
		if (static_cast<size_t>(last - first) > maxCount)
		{
			std::pop_heap(first, last);
			--last;
			squaredRadius = first->squaredDistance();
		}
	}
	return last;
}



template <class O, class OT> inline
typename KdTree<O, OT>::TValue
KdTree<O, OT>::squaredDistance(const TPoint& a, const TPoint& b)
{
	TValue result = TValue();
	for (unsigned k = 0; k < dimension; ++k)
	{
		result += num::sqr(a[k] - b[k]);
	}
	return result;
}



#ifdef LASS_SPAT_KD_TREE_DIAGNOSTICS
template <class O, class OT>
void KdTree<O, OT>::diagnostics()
{
	typedef typename meta::Select< meta::Bool<dimension == 2>, prim::Aabb2D<TValue>, prim::Aabb3D<TValue> >::Type TAabb;
	class Visitor
	{
	public:
		Visitor(const TNodes& heap, TObjectIterator end):
			xml_("kdtree.xml", "diagnostics"),
			heap_(heap),
			end_(end)
		{
		}

		void visit(size_t index, const TAabb& aabb)
		{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
			using lass::prim::operator<<;
#endif
			xml_ << aabb;

			if (index >= heap_.size() || heap_[index].object() == end_)
			{
				return;
			}
			const Node& node = heap_[index];

			const typename TObjectTraits::TPoint pivot = node.position();
			xml_ << pivot;

			const TAxis split = node.axis();
			if (split == dummyAxis_)
			{
				return;
			}

			TAabb less = aabb;
			typename TAabb::TPoint max = less.max();
			max[split] = pivot[split];
			less.setMax(max);
			visit(2 * index + 1, less);

			TAabb greater = aabb;
			typename TAabb::TPoint min = greater.min();
			min[split] = pivot[split];
			greater.setMin(min);
			visit(2 * index + 2, greater);
		}

	private:
		io::XmlOFile xml_;
		const TNodes& heap_;
		TObjectIterator end_;
	};

	TAabb aabb;
	for (TObjectIterator i = begin_; i != end_; ++i)
	{
		aabb += TObjectTraits::position(i);
	}

	Visitor visitor(heap_, end_);
	visitor.visit(0, aabb);
}
#endif



// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
