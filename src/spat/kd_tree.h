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



/** @class lass::spat::KdTree
 *  @brief a KD tree for point-like objects
 *  @author Bram de Greve [BdG]
 *
 *  the KD tree does NOT own the objects.  You must keep them yourself!
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_KD_TREE_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_KD_TREE_H

//#define LASS_SPAT_KD_TREE_DIAGNOSTICS

#include "spat_common.h"

namespace lass
{
namespace spat
{

template <typename ObjectType>
struct KdTreeObjectTraits
{
	typedef const ObjectType* TObjectIterator;
	typedef const ObjectType& TObjectReference;
	typedef ObjectType TPoint;
	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	enum { dimension = TPoint::dimension };

	static const TPoint& position(TObjectIterator iObject) { return *iObject; }
};



template
<
	class ObjectType,
	class ObjectTraits = KdTreeObjectTraits<ObjectType>
>
class KdTree
{
public:

	typedef KdTree<ObjectType, ObjectTraits> TSelf;

	typedef ObjectType TObject;
	typedef ObjectTraits TObjectTraits;

	typedef typename TObjectTraits::TObjectIterator TObjectIterator;
	typedef typename TObjectTraits::TObjectReference TObjectReference;
	typedef typename TObjectTraits::TPoint TPoint;
	typedef typename TObjectTraits::TValue TValue;
	typedef typename TObjectTraits::TParam TParam;
	typedef typename TObjectTraits::TParam TReference;
	typedef typename TObjectTraits::TParam TConstReference;

	enum { dimension = TObjectTraits::dimension };

	class Neighbour
	{
	public:
		Neighbour();
		Neighbour(TObjectIterator iObject, TValue iSquaredDistance);

		TObjectIterator object() const;
		TPoint position() const;
		TValue squaredDistance() const;

		TObjectIterator operator->() const;
		TObjectReference operator*() const;
		bool operator<(const Neighbour& iOther) const;
	private:
		TObjectIterator object_;
		TValue squaredDistance_;
	};

	typedef std::vector<Neighbour> TNeighbourhood;

	KdTree();
	KdTree(TObjectIterator iBegin, TObjectIterator iEnd);

	void reset(TObjectIterator iBegin, TObjectIterator iEnd);

	Neighbour nearestNeighbour(const TPoint& iLocation) const;	
	TValue rangeSearch(const TPoint& iCenter, TParam iMaxRadius, size_t iMaxCount,
			TNeighbourhood& oNeighbourhood) const;

	template <typename OutputIterator> 
	OutputIterator rangeSearch(const TPoint& iCenter, TParam iMaxRadius, 
			OutputIterator iFirst) const;	
	template <typename RandomIterator>
	RandomIterator rangeSearch(const TPoint& iCenter, TParam iMaxRadius, size_t iMaxCount,
			RandomIterator iFirst) const;	

	void swap(TSelf& iOther);
	const bool isEmpty() const;
	void clear();

#ifdef LASS_SPAT_KD_TREE_DIAGNOSTICS
	void diagnostics();
#endif

private:

	typedef std::vector<TObjectIterator> TObjectIterators;
	typedef typename TObjectIterators::iterator TIteratorIterator;

	typedef unsigned char TAxis;
	typedef std::vector<TAxis> TAxes;

	enum { dummyAxis_ = TAxis(-1)} ;

	class LessDim
	{
	public:
		LessDim(TAxis iSplit): split_(iSplit) {}
		bool operator()(const TObjectIterator& iA, const TObjectIterator& iB) const
		{
			return TObjectTraits::position(iA)[split_] < TObjectTraits::position(iB)[split_];
		}
	private:
		TAxis split_;
	};

	void buildHeap();
	void balance(size_t iNode, TIteratorIterator iBegin, TIteratorIterator iEnd);
	TAxis findSplitAxis(TIteratorIterator iBegin, TIteratorIterator iEnd) const;
	void assignNode(size_t iNode, TObjectIterator iObject, TAxis iSplitAxis);
	size_t findNode(const TPoint& iTarget, size_t iStartNode) const;

	void doNearestNeighbour(const TPoint& iTarget, Neighbour& oNeighbour, size_t iNode) const;

	template <typename OutputIterator>
	OutputIterator doRangeSearch(const TPoint& iCenter, TParam iSquaredRadius, 
		OutputIterator iOutput, size_t iNode) const;
	template <typename RandomIterator>
	RandomIterator doRangeSearch(const TPoint& iCenter, TReference ioSquaredRadius, 
		size_t iMaxCount, RandomIterator iFirst, RandomIterator iLast, size_t iNode) const;

	static TValue squaredDistance(const TPoint& iA, const TPoint& iB);

	TObjectIterators heap_;
	TAxes splits_;
	TObjectIterator begin_;
	TObjectIterator end_;
	size_t size_;
};


}

}

#include "kd_tree.inl"

#endif

// EOF
