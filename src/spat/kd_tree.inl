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



/** Constructs a k-d tree from objects in range [iBegin, iEnd).
 *  @warning [iBegin, iEnd) must stay a valid range during the entire lifespan of the k-d tree!
 */
template <class O, class OT>
KdTree<O, OT>::KdTree(TObjectIterator iBegin, TObjectIterator iEnd):
	begin_(iBegin),
	end_(iEnd)
{
	size_ = std::distance(begin_, end_);
	heap_.resize(size_, end_);
	splits_.resize(size_, dummyAxis_);

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



/** Resets to a new k-d tree of objects in range [iBegin, iEnd).
 *  @warning [iBegin, iEnd) must stay a valid range during the entire lifespan of the k-d tree!
 */
template <class O, class OT>
void KdTree<O, OT>::reset(TObjectIterator iBegin, TObjectIterator iEnd)
{
	TSelf temp(iBegin, iEnd);
	swap(temp);
}



/** Locates the object that's nearest to a target position
 */
template <class O, class OT>
typename KdTree<O, OT>::Neighbour
KdTree<O, OT>::nearestNeighbour(const TPoint& iTarget) const
{
	if (isEmpty())
	{
		LASS_THROW("can't locate nearest neighbour in empty KdTree");
	}

	const size_t deepNode = findNode(iTarget, 0);
	const TPoint deepPivot = TObjectTraits::position(heap_[deepNode]);
	const TValue maxSqrRadius = this->squaredDistance(iTarget, deepPivot);
	Neighbour result(heap_[deepNode], maxSqrRadius);
	doNearestNeighbour(iTarget, result, 0);
	return result;
}



/** Locates objects within a spherical range around a target position.
 *
 *	@deprecated USE OVERLOADS WITH ITERATORS INSTEAD
 *
 *  @param iTarget [in] the center of the spherical range
 *  @param iMaxRadius [in] the radius of the range
 *  @param iMaxCount [in] the maximum number of objects to be returned.
 *		@arg If this is zero, then all objects in the range are returned.
 *		@arg If this is non-zero, then up to @a iMaxCount objects are returned.
 *			These will be the ones closest to @a iTarget
 *	@param oNeighbourhood [out] a std::vector that will be filled with the found objects.
 *			The vector will be @b cleared before use.
 *  @return the squared distance between @a iTarget and the furthest found object.
 */
template <class O, class OT>
typename KdTree<O, OT>::TValue
KdTree<O, OT>::rangeSearch(const TPoint& iTarget, TParam iMaxRadius, size_t iMaxCount,
						   TNeighbourhood& oNeighbourhood) const
{
	if (isEmpty())
	{
		LASS_THROW("can't perform range search in empty KdTree");
	}

	if (iMaxCount == 0)
	{
		oNeighbourhood.clear();
		rangeSearch(iTarget, iMaxRadius, std::back_inserter(oNeighbourhood));
		
		// oNeighbourhood is not a heap, find maximum squared distance
		TValue maxSquaredDistance = TValue();
		const typename TNeighbourhood::const_iterator end = oNeighbourhood.end();
		for (typename TNeighbourhood::const_iterator i = oNeighbourhood.begin(); i != end; ++i)
		{
			maxSquaredDistance = std::max(maxSquaredDistance, i->squaredDistance());
		}
		return maxSquaredDistance;
	}

	iMaxCount = std::min(iMaxCount, size_);
	oNeighbourhood.resize(iMaxCount + 1);

	typename TNeighbourhood::iterator last = rangeSearch(
		iTarget, iMaxRadius, iMaxCount, oNeighbourhood.begin());
	oNeighbourhood.erase(last, oNeighbourhood.end());
	
	if (oNeighbourhood.empty())
	{
		return TValue();
	}
	return oNeighbourhood.front().squaredDistance();
}



/** Find all objects in a radius of @a iMaxRadius of @a iTarget.
 *  @param iTarget [in] center of range.
 *	@param iMaxRadius [in] radius of range
 *	@param iFirst [in] output iterator dereferencable to Neighbour.
 *	@return output iterator @e last so that [@a iFirst, last) is the range of all found objects.
 *
 *	@note
 *		The range starting at @a iFirst must be large enough to contain all found objects.  
 *		But since this number is probably unknown beforehand, you better use one of those
 *		inserter kind of iterators to add the results to a dynamic sized container. 
 *
 *	@note
 *		If you wish to use a fixed sized range, you best use the other rangeSearch overload
 *		taking a random access iterator and an extra parameter @a iMaxCount.
 */
template <class O, class OT>
template <typename OutputIterator>
OutputIterator
KdTree<O, OT>::rangeSearch(const TPoint& iTarget, TParam iMaxRadius, OutputIterator iFirst) const
{
	if (isEmpty() || iMaxRadius == 0)
	{
		return iFirst;
	}
	const TValue squaredRadius = iMaxRadius * iMaxRadius;
	return doRangeSearch(iTarget, squaredRadius, iFirst, 0);
}



/** Find up to a fixed number of objects in a radius of @a iMaxRadius of @a iTarget.
 *  @param iTarget [in] center of range.
 *	@param iMaxRadius [in] radius of range
 *	@param iMaxCount [in] maximum number of objects to be found.
 *	@param iFirst [in] random access iterator dereferencable to Neighbour, 
 *		[@a iFirst, @a iFirst + @a iMaxCount + 1) must be a valid range.
 *	@return output iterator @e last so that [@a iFirst, last) is the range of all found objects.
 *
 *	@note
 *		This overload will search for a maximum number of @a iMaxCount objects at a maximum
 *		distance of @a iMaxRadius of the center @a iTarget.  When more than @a iMaxCount objects
 *		are within this distance, the closest objects will be selected.
 *		To select the closest objects, a heap is constructed on the iterator range, which is
 *		why random access iterators are required instead of regular output iterators.  This
 *		is also why there's need of an extra position in the range pointed to by @a iFirst:
 *		there's need of an extra position to swap in/out new/old objects.  That's why you
 *		must make sure [@a iFirst, @a iFirst + @a iMaxCount + 1) is a valid range.
 *
 *	@note
 *		If you wish to find all points within the range of @a iMaxRadius, you better use the
 *		overload with the regular output iterator and without @a iMaxCount.
 */
template <class O, class OT>
template <typename RandomAccessIterator>
RandomAccessIterator
KdTree<O, OT>::rangeSearch(const TPoint& iTarget, TParam iMaxRadius, size_t iMaxCount,
		RandomAccessIterator iFirst) const
{
	if (isEmpty() || iMaxRadius == 0)
	{
		return iFirst;
	}
	const TValue squaredRadius = iMaxRadius * iMaxRadius;
	return doRangeSearch(iTarget, squaredRadius, iMaxCount, iFirst, iFirst, 0);
}



/** Swap the representation of two k-d trees.
 */
template <class O, class OT>
void KdTree<O, OT>::swap(TSelf& iOther)
{
	std::swap(begin_, iOther.begin_);
	std::swap(end_, iOther.end_);
	heap_.swap(iOther.heap_);
	splits_.swap(iOther.splits_);
	std::swap(size_, iOther.size_);
}



/** returns true if there are no objects in the k-d tree
 */
template <class O, class OT>
const bool KdTree<O, OT>::isEmpty() const
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
KdTree<O, OT>::Neighbour::Neighbour(TObjectIterator iObject, TValue iSquaredDistance):
	object_(iObject),
	squaredDistance_(iSquaredDistance)
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
KdTree<O, OT>::Neighbour::operator<(const Neighbour& iOther) const
{
	return squaredDistance_ < iOther.squaredDistance_;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <class O, class OT>
void KdTree<O, OT>::balance(size_t iNode, TIteratorIterator iBegin, TIteratorIterator iEnd)
{
	if (iEnd == iBegin)
	{
		return;
	}

	if (iEnd == iBegin + 1)
	{
		// exactly one element in content
		assignNode(iNode, *iBegin, dummyAxis_);
		return;
	}

	// more than one, do the balance
	//
	const TAxis split = findSplitAxis(iBegin, iEnd);
	const size_t size = iEnd - iBegin;;
	const TIteratorIterator median = iBegin + size / 2;
	std::nth_element(iBegin, median, iEnd, LessDim(split));
	assignNode(iNode, *median, split);

	balance(2 * iNode + 1, iBegin, median);
	balance(2 * iNode + 2, median + 1, iEnd);
}



template <class O, class OT>
typename KdTree<O, OT>::TAxis
KdTree<O, OT>::findSplitAxis(TIteratorIterator iBegin, TIteratorIterator iEnd) const
{
	TPoint min = TObjectTraits::position(*iBegin);
	TPoint max = min;

	for (TIteratorIterator i = iBegin + 1; i != iEnd; ++i)
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
inline void KdTree<O, OT>::assignNode(size_t iNode, TObjectIterator iObject, TAxis iSplitAxis)
{
	if (heap_.size() <= iNode)
	{
		heap_.resize(iNode + 1, end_);
		splits_.resize(iNode + 1, dummyAxis_);
	}
	heap_[iNode] = iObject;
	splits_[iNode] = iSplitAxis;
}



template <class O, class OT>
size_t KdTree<O, OT>::findNode(const TPoint& iTarget, size_t iStartNode) const
{
	const size_t size = heap_.size();
	if (iStartNode >= size || heap_[iStartNode] == end_)
	{
		return size;
	}

	const TAxis split = splits_[iStartNode];
	if (split == dummyAxis_)
	{
		return iStartNode;
	}

	const TPoint pivot = TObjectTraits::position(heap_[iStartNode]);
	const TValue delta = iTarget[split] - pivot[split]; // distance to splitting plane
	const bool isLeftSide = delta < TValue();
	const size_t result = findNode(iTarget, 2 * iStartNode + (isLeftSide ? 1 : 2));
	return result != size ? result : iStartNode;
}



template <class O, class OT>
void KdTree<O, OT>::doNearestNeighbour(const TPoint& iTarget, Neighbour& ioNearest, size_t iNode) const
{
	if (iNode >= heap_.size() || heap_[iNode] == end_)
	{
		return;
	}

	const TPoint pivot = TObjectTraits::position(heap_[iNode]);
	const TAxis split = splits_[iNode];
	if (split != dummyAxis_)
	{
		const TValue delta = iTarget[split] - pivot[split]; // distance to splitting plane
		if (delta < TValue())
		{
			// we are left of the plane - search left node first
			doNearestNeighbour(iTarget, ioNearest, 2 * iNode + 1);
			if (num::sqr(delta) < ioNearest.squaredDistance())
			{
				doNearestNeighbour(iTarget, ioNearest, 2 * iNode + 2);
			}
		}
		else
		{
			// we are right of the plane - search right node first
			doNearestNeighbour(iTarget, ioNearest, 2 * iNode + 2);
			if (num::sqr(delta) < ioNearest.squaredDistance())
			{
				doNearestNeighbour(iTarget, ioNearest, 2 * iNode + 1);
			}
		}
	}

	const TValue sqrDistance = this->squaredDistance(pivot, iTarget);
	if (sqrDistance < ioNearest.squaredDistance())
	{
		ioNearest = Neighbour(heap_[iNode], sqrDistance);
	}
}



template <class O, class OT>
template <typename OutputIterator>
OutputIterator KdTree<O, OT>::doRangeSearch(
		const TPoint& iTarget, TParam iSquaredRadius,
		OutputIterator iOutput, size_t iNode) const
{
	if (iNode >= heap_.size() || heap_[iNode] == end_)
	{
		return iOutput;
	}

	const TPoint pivot = TObjectTraits::position(heap_[iNode]);
	const TAxis split = splits_[iNode];
	if (split != dummyAxis_)
	{
		const TValue delta = iTarget[split] - pivot[split]; // distance to splitting plane
		if (delta < TValue())
		{
			// we are left of the plane - search left node first
			iOutput = doRangeSearch(iTarget, iSquaredRadius, iOutput, 2 * iNode + 1);
			if (num::sqr(delta) < iSquaredRadius)
			{
				iOutput = doRangeSearch(iTarget, iSquaredRadius, iOutput, 2 * iNode + 2);
			}
		}
		else
		{
			// we are right of the plane - search right node first
			iOutput = doRangeSearch(iTarget, iSquaredRadius, iOutput, 2 * iNode + 2);
			if (num::sqr(delta) < iSquaredRadius)
			{
				iOutput = doRangeSearch(iTarget, iSquaredRadius, iOutput, 2 * iNode + 1);
			}
		}
	}

	const TValue sqrDistance = squaredDistance(pivot, iTarget);
	if (sqrDistance < iSquaredRadius)
	{
		*iOutput++ = Neighbour(heap_[iNode], sqrDistance);
	}
	return iOutput;
}



template <class O, class OT>
template <typename RandomIterator>
RandomIterator KdTree<O, OT>::doRangeSearch(
		const TPoint& iTarget, TReference ioSquaredRadius, size_t iMaxCount,
		RandomIterator iFirst, RandomIterator iLast, size_t iNode) const
{
	if (iNode >= heap_.size() || heap_[iNode] == end_)
	{
		return iLast;
	}

	const TPoint pivot = TObjectTraits::position(heap_[iNode]);
	const TAxis split = splits_[iNode];
	if (split != dummyAxis_)
	{
		const TValue delta = iTarget[split] - pivot[split]; // distance to splitting plane
		if (delta < TValue())
		{
			// we are left of the plane - search left node first
			iLast = doRangeSearch(
				iTarget, ioSquaredRadius, iMaxCount, iFirst, iLast, 2 * iNode + 1);
			if (num::sqr(delta) < ioSquaredRadius)
			{
				iLast = doRangeSearch(
					iTarget, ioSquaredRadius, iMaxCount, iFirst, iLast, 2 * iNode + 2);
			}
		}
		else
		{
			// we are right of the plane - search right node first
			iLast = doRangeSearch(
				iTarget, ioSquaredRadius, iMaxCount, iFirst, iLast, 2 * iNode + 2);
			if (num::sqr(delta) < ioSquaredRadius)
			{
				iLast = doRangeSearch(
					iTarget, ioSquaredRadius, iMaxCount, iFirst, iLast, 2 * iNode + 1);
			}
		}
	}

	const TValue sqrDistance = squaredDistance(pivot, iTarget);
	if (sqrDistance < ioSquaredRadius)
	{
		*iLast++ = Neighbour(heap_[iNode], sqrDistance);
		std::push_heap(iFirst, iLast);
		LASS_ASSERT(iLast >= iFirst);
		if (static_cast<size_t>(iLast - iFirst) > iMaxCount)
		{
			std::pop_heap(iFirst, iLast);
			--iLast;
			ioSquaredRadius = iFirst->squaredDistance();
		}
	}
	return iLast;
}



template <class O, class OT> inline
typename KdTree<O, OT>::TValue
KdTree<O, OT>::squaredDistance(const TPoint& iA, const TPoint& iB)
{
	TValue result = TValue();
	for (unsigned k = 0; k < dimension; ++k)
	{
		result += num::sqr(iA[k] - iB[k]);
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
		Visitor(const TObjectIterators& iHeap, const TAxes& iSplits, TObjectIterator iEnd):
			xml_("kdtree.xml", "diagnostics"),
			heap_(iHeap),
			splits_(iSplits),
			end_(iEnd)
		{
		}

		void visit(size_t iNode, const TAabb& iAabb)
		{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
			using lass::prim::operator<<;
#endif
			xml_ << iAabb;

			if (iNode >= heap_.size() || heap_[iNode] == end_)
			{
				return;
			}

			const typename TObjectTraits::TPoint pivot = TObjectTraits::position(heap_[iNode]);
			xml_ << pivot;

			const TAxis split = splits_[iNode];
			if (split == dummyAxis_)
			{
				return;
			}

			TAabb less = iAabb;
			typename TAabb::TPoint max = less.max();
			max[split] = pivot[split];
			less.setMax(max);
			visit(2 * iNode + 1, less);

			TAabb greater = iAabb;
			typename TAabb::TPoint min = greater.min();
			min[split] = pivot[split];
			greater.setMin(min);
			visit(2 * iNode + 2, greater);
		}

	private:
		io::XmlOFile xml_;
		const TObjectIterators& heap_;
		const TAxes& splits_;
		TObjectIterator end_;
	};

	TAabb aabb;
	for (TObjectIterator i = begin_; i != end_; ++i)
	{
		aabb += TObjectTraits::position(i);
	}

	Visitor visitor(heap_, splits_, end_);
	visitor.visit(0, aabb);
}
#endif



// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
