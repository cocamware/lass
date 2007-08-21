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

	void reset();
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
