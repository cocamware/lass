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
 *	Copyright (C) 2004-2024 the Initial Developer.
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
#include "../util/shared_ptr.h"

namespace lass
{
namespace spat
{

template <typename ObjectType, typename IteratorType=const ObjectType*>
struct KdTreeObjectTraits
{
	typedef ObjectType TPoint;
	typedef IteratorType TObjectIterator;
	typedef const ObjectType& TObjectReference;
	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	enum { dimension = TPoint::dimension };

	static const TPoint& position(TObjectIterator object) { return *object; }
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
	typedef typename TObjectTraits::TReference TReference;
	typedef typename TObjectTraits::TConstReference TConstReference;

	enum { dimension = TObjectTraits::dimension };

	class Neighbour
	{
	public:
		Neighbour();
		Neighbour(TObjectIterator object, TValue squaredDistance);

		TObjectIterator object() const;
		TPoint position() const;
		TValue squaredDistance() const;

		TObjectIterator operator->() const;
		TObjectReference operator*() const;
		bool operator<(const Neighbour& other) const;
	private:
		TObjectIterator object_;
		TValue squaredDistance_;
	};

	typedef std::vector<Neighbour> TNeighbourhood;

	KdTree();
	KdTree(TObjectIterator first, TObjectIterator last);
	KdTree(TSelf&& other) noexcept;

	TSelf& operator=(TSelf&& other) noexcept;

	void reset();
	void reset(TObjectIterator first, TObjectIterator last);

	Neighbour nearestNeighbour(const TPoint& target) const;	
	Neighbour nearestNeighbour(const TPoint& target, TParam maxRadius) const;	
	TValue rangeSearch(const TPoint& target, TParam maxRadius, size_t maxCount,
			TNeighbourhood& neighbourhood) const;

	template <typename OutputIterator> 
	OutputIterator rangeSearch(const TPoint& target, TParam maxRadius, 
			OutputIterator first) const;	
	template <typename RandomIterator>
	RandomIterator rangeSearch(const TPoint& target, TParam maxRadius, size_t maxCount,
			RandomIterator first) const;	

	void swap(TSelf& other);
	bool isEmpty() const;
	void clear();

	const TObjectIterator end() const;

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
		LessDim(TAxis split): split_(split) {}
		bool operator()(const TObjectIterator& a, const TObjectIterator& b) const
		{
			return TObjectTraits::position(a)[split_] < TObjectTraits::position(b)[split_];
		}
	private:
		TAxis split_;
	};

	class Node
	{
	public:
		Node(TObjectIterator object, const TPoint& position = TPoint(), TAxis axis = dummyAxis_): 
			position_(position), object_(object), axis_(axis) {}
		const TObjectIterator& object() const { return object_; }
		const TPoint& position() const { return position_; }
		TAxis axis() const { return axis_; }
	private:
		TPoint position_;
		TObjectIterator object_;
		TAxis axis_;
	};
	typedef std::vector<Node> TNodes;

	void buildHeap();
	void balance(size_t index, TIteratorIterator first, TIteratorIterator last);
	TAxis findSplitAxis(TIteratorIterator first, TIteratorIterator last) const;
	void assignNode(size_t index, TObjectIterator object, TAxis splitAxis);
	size_t findNode(size_t index, const TPoint& target) const;

	void doNearestNeighbour(size_t index, const TPoint& target, Neighbour& best) const;

	template <typename OutputIterator>
	OutputIterator doRangeSearch(size_t index, const TPoint& target, 
		TParam squaredRadius, OutputIterator output) const;
	template <typename RandomIterator>
	RandomIterator doRangeSearch(size_t index, const TPoint& target, TReference squaredRadius, 
		size_t maxCount, RandomIterator first, RandomIterator last) const;

	static TValue squaredDistance(const TPoint& a, const TPoint& b);

	TNodes heap_;
	std::unique_ptr<TObjectIterator> end_;
};


}

}

#include "kd_tree.inl"

#endif

// EOF
