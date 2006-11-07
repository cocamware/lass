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
	aabb_(TObjectTraits::emptyAabb()),
	objects_(),
	nodes_(),
	end_()
{
}



template <typename O, typename OT, typename SH>
AabpTree<O, OT, SH>::AabpTree(TObjectIterator iBegin, TObjectIterator iEnd):
	aabb_(TObjectTraits::emptyAabb()),
	objects_(),
	nodes_(),
	end_(iEnd)
{
	TInputs inputs;
	for (TObjectIterator i = iBegin; i != iEnd; ++i)
	{
		TAabb aabb = TObjectTraits::aabb(i);
		aabb_ = TObjectTraits::join(aabb_, aabb);
		inputs.push_back(std::make_pair(i, aabb));
	}
	if (iBegin != iEnd)
	{
		balance(inputs.begin(), inputs.end());
	}
}



template <typename O, typename OT, typename SH>
void AabpTree<O, OT, SH>::reset(TObjectIterator iBegin, TObjectIterator iEnd)
{
	TSelf temp(iBegin, iEnd);
	swap(temp);
}



template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::TAabb& 
AabpTree<O, OT, SH>::aabb() const
{
	return aabb_;
}



template <typename O, typename OT, typename SH>
const bool AabpTree<O, OT, SH>::contains(const TPoint& iPoint, const TInfo* iInfo) const
{
	if (isEmpty() || !TObjectTraits::contains(aabb_, iPoint))
	{
		return false;
	}
	return doContains(0, iPoint, iInfo);
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabpTree<O, OT, SH>::find(const TPoint& iPoint, OutputIterator iResult,
									 const TInfo* iInfo) const
{
	if (isEmpty() || !TObjectTraits::contains(aabb_, iPoint))
	{
		return iResult;
	}
	return doFind(0, iPoint, iResult, iInfo);
}



template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::TObjectIterator
AabpTree<O, OT, SH>::intersect(const TRay& iRay, TReference oT, TParam iMinT, const TInfo* iInfo) const
{
	TValue tNear;
	if (isEmpty() || !TObjectTraits::intersect(aabb_, iRay, tNear, iMinT))
	{
		return end_;
	}
	TValue tFar;
	if (!TObjectTraits::intersect(aabb_, iRay, tFar, tNear))
	{
		tFar = tNear;
		tNear = iMinT;
	}
	const TVector reciprocalDirection = TObjectTraits::reciprocal(TObjectTraits::direction(iRay));
	TValue t;
	TObjectIterator hit = doIntersect(0, iRay, t, iMinT, iInfo, reciprocalDirection, tNear, tFar);
	if (hit != end_)
	{
		LASS_ASSERT(t > iMinT);
		oT = t;
		return hit;
	}
	return end_;
}



template <typename O, typename OT, typename SH>
const bool AabpTree<O, OT, SH>::intersects(
		const TRay& iRay, TParam iMinT, TParam iMaxT, const TInfo* iInfo) const
{
	TValue tNear;
	if (isEmpty() || !TObjectTraits::intersect(aabb_, iRay, tNear, iMinT))
	{
		return false;
	}
	TValue tFar;
	if (!TObjectTraits::intersect(aabb_, iRay, tFar, tNear))
	{
		tFar = tNear;
		tNear = iMinT;
	}
	if (tNear > iMax || tFar < iMin)
	{
		return false;
	}
	const TVector reciprocalDirection = TObjectTraits::reciprocal(TObjectTraits::direction(iRay));
	return doIntersects(0, iRay, iMinT, iMaxT, iInfo, reciprocalDirection, tNear, tFar);
}



template <typename O, typename OT, typename SH>
void AabpTree<O, OT, SH>::swap(TSelf& iOther)
{
	std::swap(aabb_, iOther.aabb_);
	nodes_.swap(iOther.nodes_);
	objects_.swap(iOther.objects_);
	std::swap(end_, iOther.end_);
}



template <typename O, typename OT, typename SH>
const bool AabpTree<O, OT, SH>::isEmpty() const
{
	return objects_.empty();
}



template <typename O, typename OT, typename SH>
void AabpTree<O, OT, SH>::clear()
{
	TSelf temp;
	swap(temp);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
const std::pair<int, typename AabpTree<O, OT, SH>::TAabb> 
AabpTree<O, OT, SH>::balance(TInputIterator iFirst, TInputIterator iLast)
{
	const SplitInfo<OT> split = TSplitHeuristics::split<OT>(iFirst, iLast);	
	if (split.axis < 0)
	{
		return std::make_pair(addLeafNode(iFirst, iLast), split.aabb);
	}

	class Splitter
	{
	public:
		Splitter(const SplitInfo<OT>& iSplit): split_(iSplit) {}
		bool operator()(const TInput& iInput) const
		{
			const TValue x = 
				(TObjectTraits::coordinate(TObjectTraits::min(iInput.second), split_.axis) +
				TObjectTraits::coordinate(TObjectTraits::max(iInput.second), split_.axis)) / 2;
			return x < split_.x;
		}			
	private:
		SplitInfo<OT> split_;
	};
	TInputIterator middle = std::partition(iFirst, iLast, Splitter(split));
	
	if (middle == iFirst || middle == iLast)
	{
		class LessDim
		{
		public:
			LessDim(int iAxis): axis_(iAxis) {}
			bool operator()(const TInput& a, const TInput& b) const
			{
				const TValue xa = 
					(TObjectTraits::coordinate(TObjectTraits::min(a.second), axis_) +
					TObjectTraits::coordinate(TObjectTraits::max(a.second), axis_)) / 2;
				const TValue xb = 
					(TObjectTraits::coordinate(TObjectTraits::min(b.second), axis_) +
					TObjectTraits::coordinate(TObjectTraits::max(b.second), axis_)) / 2;
				return xa < xb;
			}			
		private:
			int axis_;
		};
		const ptrdiff_t halfSize = (iLast - iFirst) / 2;
		middle = iFirst + halfSize;
		std::nth_element(iFirst, middle, iLast, LessDim(split.axis));
	}

	LASS_ASSERT(middle != iFirst && middle != iLast);

	const int index = addInternalNode(split.axis);
	const std::pair<int, TAabb> left = balance(iFirst, middle);
	const std::pair<int, TAabb> right = balance(middle, iLast);
	
	Node& node = nodes_[index];
	node.leftBound() = TObjectTraits::coordinate(TObjectTraits::max(left.second), split.axis);
	node.rightBound() = TObjectTraits::coordinate(TObjectTraits::min(right.second), split.axis);
	LASS_ASSERT(left.first == index + 1);
	node.right() = right.first;
	
	return std::make_pair(index, split.aabb);
}



template <typename O, typename OT, typename SH>
const int AabpTree<O, OT, SH>::addLeafNode(TInputIterator iFirst, TInputIterator iLast)
{
	const int first = objects_.size();
	while (iFirst != iLast)
	{
		TObjectIterator object = iFirst->first;
		objects_.push_back(object);
		++iFirst;
	}
	const int last = objects_.size();

	nodes_.push_back(Node(first, last));
	return nodes_.size() - 1;
}



template <typename O, typename OT, typename SH>
const int AabpTree<O, OT, SH>::addInternalNode(int iAxis)
{
	nodes_.push_back(Node(iAxis));
	return nodes_.size() - 1;
}



template <typename O, typename OT, typename SH>
const bool AabpTree<O, OT, SH>::doContains(
		int iIndex, const TPoint& iPoint, const TInfo* iInfo) const
{
	LASS_ASSERT(iIndex < nodes_.size());
	const Node& node = nodes_[iIndex];
	
	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			if (TObjectTraits::contains(objects_[i], iPoint, iInfo))
			{
				return true;
			}
		}
		return false;
	}

	const TValue x = TObjectTraits::coordinate(iPoint, node.axis());
	if (x <= node.leftBound() && doContains(iIndex + 1, iPoint, iInfo))
	{
		return true;
	}
	if (x >= node.rightBound() && doContains(node.right(), iPoint, iInfo))
	{
		return true;
	}
	return false;
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator AabpTree<O, OT, SH>::doFind(
		int iIndex, const TPoint& iPoint, OutputIterator iResult, const TInfo* iInfo) const
{
	LASS_ASSERT(iIndex < nodes_.size());
	const Node& node = nodes_[iIndex];

	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			if (TObjectTraits::contains(objects_[i], iPoint, iInfo))
			{
				*iResult++ = objects_[i];
			}
		}
		return iResult;
	}

	const TValue x = TObjectTraits::coordinate(iPoint, node.axis());
	if (x <= node.leftBound())
	{
		iResult = doFind(iIndex + 1, iPoint, iResult, iInfo);
	}
	if (x >= node.rightBound())
	{
		iResult = doFind(node.right(), iPoint, iResult, iInfo);
	}
	return iResult;
}



template <typename O, typename OT, typename SH>
const typename AabpTree<O, OT, SH>::TObjectIterator
AabpTree<O, OT, SH>::doIntersect(
		int iIndex, const TRay& iRay, TReference oT, TParam iTMin, const TInfo* iInfo,
		const TVector& iReciprocalDirection, TParam iTNear, TParam iTFar) const
{
	LASS_ASSERT(iIndex < nodes_.size());
	LASS_ASSERT(iTFar > iTNear);
	const Node& node = nodes_[iIndex];

	if (node.isLeaf())
	{
		TValue candidateT = 0;
		TObjectIterator candidate = end_;
		for (int i = node.first(); i != node.last(); ++i)
		{
			TValue t;
			if (TObjectTraits::intersect(objects_[i], iRay, t, iTMin, iInfo))
			{
				LASS_ASSERT(t > iTMin);
				if (candidate == end_ || t < candidateT)
				{
					candidate = objects_[i];
					candidateT = t;
				}
			}
		}
		if (candidate != end_)
		{
			oT = candidateT;
		}
		return candidate;
	}

	// check children
	const size_t leftIndex = iIndex + 1;
	const size_t rightIndex = node.right();
	const TValue s = TObjectTraits::coordinate(TObjectTraits::support(iRay), node.axis());
	const TValue d = TObjectTraits::component(TObjectTraits::direction(iRay), node.axis());
	const TValue invD = TObjectTraits::component(iReciprocalDirection, node.axis());
	const TValue tLeftBound = (node.leftBound() - s) * invD;
	const TValue tRightBound = (node.rightBound() - s) * invD;
	
	TValue tLeft = 0;
	TValue tRight = 0;
	TObjectIterator objectLeft = end_;
	TObjectIterator objectRight = end_;
	if (d > 0)
	{
		if (tLeftBound > iTNear)
		{
			objectLeft = doIntersect(leftIndex, iRay, tLeft, iTMin, iInfo,
				iReciprocalDirection, iTNear, std::min(tLeftBound, iTFar)); 
		}
		if (tRightBound < iTFar)
		{
			objectRight = doIntersect(rightIndex, iRay, tRight, iTMin, iInfo,
				iReciprocalDirection, std::max(tRightBound, iTNear), iTFar);
		}
	}
	else if (d < 0)
	{
		if (tLeftBound < iTFar)
		{
			objectLeft = doIntersect(leftIndex, iRay, tLeft, iTMin, iInfo,
				iReciprocalDirection, std::max(tLeftBound, iTNear), iTFar);
		}
		if (tRightBound > iTNear)
		{
			objectRight = doIntersect(rightIndex, iRay, tRight, iTMin, iInfo,
				iReciprocalDirection, iTNear, std::min(tRightBound, iTFar)); 
		}
	}
	else // if (d == TNumTraits::zero)
	{
		if (s <= node.leftBound())
		{
			objectLeft = doIntersect(leftIndex, iRay, tLeft, iTMin, iInfo,
				iReciprocalDirection, iTNear, iTFar);
		}
		if (s >= node.rightBound())
		{
			objectRight = doIntersect(rightIndex, iRay, tRight, iTMin, iInfo,
				iReciprocalDirection, iTNear, iTFar);
		}
	}
	
	// determine result
	if (objectLeft != end_)
	{
		if (objectRight != end_ && tRight < tLeft)
		{
			LASS_ASSERT(tRight > iTMin);
			oT = tRight;
			return objectRight;
		}
		else
		{
			LASS_ASSERT(tLeft > iTMin);
			oT = tLeft;
			return objectLeft;
		}
	}
	if (objectRight != end_)
	{
		LASS_ASSERT(tRight > iTMin);
		oT = tRight;
		return objectRight;
	}
	return end_;
}



template <typename O, typename OT, typename SH>
const bool AabpTree<O, OT, SH>::doIntersects(
		int iIndex, const TRay& iRay, TParam iTMin, TParam iTMax, const TInfo* iInfo,
		const TVector& iReciprocalDirection, TParam iTNear, TParam iTFar) const
{
	LASS_ASSERT(iIndex < nodes_.size());
	LASS_ASSERT(iTMax > iTMin);
	const Node& node = nodes_[iIndex];

	if (node.isLeaf())
	{
		for (int i = node.first(); i != node.last(); ++i)
		{
			if (TObjectTraits::intersects(objects_[i], iRay, iTMin, iTMax, iInfo))
			{
				return true;
			}
		}
		return false;
	}

	// check children
	const size_t leftIndex = 2 * iIndex + 1;
	const size_t rightIndex = leftIndex + 1;
	const TValue s = TObjectTraits::coordinate(TObjectTraits::support(iRay), node.axis());
	const TValue d = TObjectTraits::component(TObjectTraits::direction(iRay), node.axis());
	const TValue invD = TObjectTraits::component(iReciprocalDirection, node.axis());
	const TValue tLeftBound = (node.leftBound() - s) * invD;
	const TValue tRightBound = (node.rightBound() - s) * invD;
	
	if (d > 0)
	{
		if ((tLeftBound > iTNear) && doIntersects(leftIndex, iRay, iTMin, iTMax, iInfo,
				iReciprocalDirection, iTNear, std::min(tLeftBound, iTFar)))
		{
			return true;
		}
		if ((tRightBound < iTFar) && doIntersects(rightIndex, iRay, iTMin, iTMax, iInfo,
				iReciprocalDirection, std::max(tRightBound, iTNear), iTFar))
		{
			return true;
		}
	}
	else if (d < 0)
	{
		if ((tLeftBound < iTFar) && doIntersects(leftIndex, iRay, iTMin, iTMax, iInfo,
				iReciprocalDirection, std::max(tLeftBound, iTNear), iTFar))
		{
			return true;
		}
		if ((tRightBound > iTNear) && doIntersects(rightIndex, iRay, iTMin, iTMax, iInfo,
				iReciprocalDirection, iTNear, std::min(tRightBound, iTFar)))
		{
			return true;
		}
	}
	else // if (d == TNumTraits::zero)
	{
		if ((s <= node.leftBound) && doIntersects(rightIndex, iRay, iTMin, iTMax, iInfo,
				iReciprocalDirection, iTNear, iTFar))
		{
			return true;
		}
		if ((s >= node.rightBound) && doIntersects(rightIndex, iRay, iTMin, iTMax, iInfo,
				iReciprocalDirection, iTNear, iTFar))
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
