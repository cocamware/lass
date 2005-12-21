/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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

template <class O, class OT>
KdTree<O, OT>::KdTree():
	begin_(),
	end_()
{
}



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



template <class O, class OT>
void KdTree<O, OT>::reset(TObjectIterator iBegin, TObjectIterator iEnd)
{
	TSelf temp(iBegin, iEnd);
	swap(temp);
}



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



template <class O, class OT>
typename KdTree<O, OT>::TValue
KdTree<O, OT>::rangeSearch(const TPoint& iTarget, TParam iMaxRadius, size_t iMaxCount,
						   TNeighbourhood& oNeighbourhood) const
{
	if (isEmpty())
	{
		LASS_THROW("can't perform range search in empty KdTree");
	}

	iMaxCount = std::min(iMaxCount, size_);

	LASS_ASSERT(iMaxRadius > TValue()); // no initial zero radius allowed
	LASS_ASSERT(iMaxCount > 0);
    
	TValue squaredRadius = iMaxRadius * iMaxRadius;
	oNeighbourhood.clear();
	oNeighbourhood.reserve(iMaxCount + 1);

	doRangeSearch(iTarget, squaredRadius, iMaxCount, oNeighbourhood, 0);
	return oNeighbourhood.empty() ? TValue() : oNeighbourhood.front().squaredDistance();
}


#pragma LASS_TODO("Integrate this lean and mean range search more properly, avoiding code duplication [Bramz]")
template <class O, class OT>
void KdTree<O, OT>::rangeSearchLeanAndMean(const TPoint& iTarget, TParam iMaxRadius, TNeighbourhood& oNeighbourhood) const
{
	if (isEmpty())
	{
		LASS_THROW("can't perform range search in empty KdTree");
	}

	LASS_ASSERT(iMaxRadius > TValue()); // no initial zero radius allowed
    
	TValue squaredRadius = iMaxRadius * iMaxRadius;
	oNeighbourhood.clear();

	doRangeSearch(iTarget, squaredRadius, oNeighbourhood, 0);
}



template <class O, class OT>
void KdTree<O, OT>::swap(TSelf& iOther)
{
	std::swap(begin_, iOther.begin_);
	std::swap(end_, iOther.end_);
	heap_.swap(iOther.heap_);
	splits_.swap(iOther.splits_);
	std::swap(size_, iOther.size_);
}



template <class O, class OT>
const bool KdTree<O, OT>::isEmpty() const
{
	return heap_.empty();
}



template <class O, class OT>
void KdTree<O, OT>::clear()
{
	TSelf temp;
	swap(temp);
}



// --- neighbour -----------------------------------------------------------------------------------

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
void KdTree<O, OT>::doRangeSearchLeanAndMean(const TPoint& iTarget,
											 TReference ioSquaredRadius,
											 TNeighbourhood& oNeighbourhood,
											 size_t iNode) const
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
			doRangeSearch(iTarget, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 1);
			if (num::sqr(delta) < ioSquaredRadius)
			{
				doRangeSearch(iTarget, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 2);
			}
		}
		else
		{
			// we are right of the plane - search right node first
			doRangeSearch(iTarget, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 2);
			if (num::sqr(delta) < ioSquaredRadius)
			{
				doRangeSearch(iTarget, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 1);
			}
		}
	}

	const TValue sqrDistance = squaredDistance(pivot, iTarget);
	if (sqrDistance < ioSquaredRadius)
	{
		oNeighbourhood.push_back(Neighbour(heap_[iNode], sqrDistance));
	}
}



template <class O, class OT>
void KdTree<O, OT>::doRangeSearch(const TPoint& iTarget,
								  TReference ioSquaredRadius,
								  size_t iMaxCount,
								  TNeighbourhood& oNeighbourhood,
								  size_t iNode) const
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
			doRangeSearch(iTarget, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 1);
			if (num::sqr(delta) < ioSquaredRadius)
			{
				doRangeSearch(iTarget, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 2);
			}
		}
		else
		{
			// we are right of the plane - search right node first
			doRangeSearch(iTarget, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 2);
			if (num::sqr(delta) < ioSquaredRadius)
			{
				doRangeSearch(iTarget, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 1);
			}
		}
	}

	const TValue sqrDistance = squaredDistance(pivot, iTarget);
	if (sqrDistance < ioSquaredRadius)
	{
		oNeighbourhood.push_back(Neighbour(heap_[iNode], sqrDistance));
		std::push_heap(oNeighbourhood.begin(), oNeighbourhood.end());
		if (oNeighbourhood.size() > iMaxCount)
		{
			std::pop_heap(oNeighbourhood.begin(), oNeighbourhood.end());
			ioSquaredRadius = oNeighbourhood.front().squaredDistance();
		}
	}
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
	typedef typename meta::Select<dimension == 2, prim::Aabb2D<TValue>, prim::Aabb3D<TValue> >::Type TAabb;
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
