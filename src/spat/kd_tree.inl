/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

template <typename O, template <class> class OT>
KdTree<O, OT>::KdTree():
	begin_(),
    end_()
{
}



template <typename O, template <class> class OT>
KdTree<O, OT>::KdTree(TObjectIterator iBegin, TObjectIterator iEnd):
	begin_(iBegin),
    end_(iEnd)
{
    const size_t size = static_cast<size_t>(end_ - begin_);
    heap_.resize(size, end_);
    splits_.resize(size, dummyAxis_);

    TObjectIterators input;
    for (TObjectIterator i = begin_; i != end_; ++i)
    {
        input.push_back(i);
    }

    balance(0, input.begin(), input.end());
}



template <typename O, template <class> class OT>
void KdTree<O, OT>::reset(TObjectIterator iBegin, TObjectIterator iEnd)
{
    TSelf temp(iBegin, iEnd);
    swap(temp);
}



template <typename O, template <class> class OT>
void KdTree<O, OT>::swap(TSelf& iOther)
{
    std::swap(begin_, iOther.begin_);
    std::swap(end_, iOther.end_);
    heap_.swap(iOther.heap_);
    splits_.swap(iOther.splits_);
}



template <typename O, template <class> class OT>
typename KdTree<O, OT>::TValue
KdTree<O, OT>::neighbourhood(const TPoint& iCenter, TParam iMaxRadius, size_t iMaxCount,
                             TNeighbourhood& oNeighbourhood)
{
    LASS_ASSERT(iMaxRadius > TValue()); // no initial zero radius allowed
    LASS_ASSERT(iMaxCount > 0);

    TValue squaredRadius = iMaxRadius * iMaxRadius;
    oNeighbourhood.clear();
    oNeighbourhood.reserve(iMaxCount + 1);

    doNeighbourhood(iCenter, squaredRadius, iMaxCount, oNeighbourhood, 0);
    return oNeighbourhood.empty() ? TValue() : oNeighbourhood.front().squaredDistance();
}



// --- neighbour -----------------------------------------------------------------------------------

template <typename O, template <class> class OT>
KdTree<O, OT>::Neighbour::Neighbour(TObjectIterator iObject, TValue iSquaredDistance):
    object_(iObject),
    squaredDistance_(iSquaredDistance)
{
}



template <typename O, template <class> class OT>
inline typename KdTree<O, OT>::TObjectIterator
KdTree<O, OT>::Neighbour::object() const
{
    return object_;
}



template <typename O, template <class> class OT>
inline typename KdTree<O, OT>::TValue
KdTree<O, OT>::Neighbour::squaredDistance() const
{
    return squaredDistance_;
}



template <typename O, template <class> class OT>
inline bool
KdTree<O, OT>::Neighbour::operator<(const Neighbour& iOther) const
{
    return squaredDistance_ < iOther.squaredDistance_;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename O, template <class> class OT>
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



template <typename O, template <class> class OT>
typename KdTree<O, OT>::TAxis
KdTree<O, OT>::findSplitAxis(TIteratorIterator iBegin, TIteratorIterator iEnd) const
{
    TPoint min = TObjectTraits::position(*(*iBegin));
    TPoint max = min;

    for (TIteratorIterator i = iBegin + 1; i != iEnd; ++i)
    {
        const TPoint position = TObjectTraits::position(*(*i));
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



template <typename O, template <class> class OT>
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




template <typename O, template <class> class OT>
void KdTree<O, OT>::doNeighbourhood(const TPoint& iCenter, 
                                    TReference ioSquaredRadius, 
                                    size_t iMaxCount,
                                    TNeighbourhood& oNeighbourhood, 
                                    size_t iNode)
{
    if (iNode >= heap_.size() || heap_[iNode] == end_)
    {
        return;
    }

    const TPoint position = TObjectTraits::position(*heap_[iNode]);
    const TAxis split = splits_[iNode];
    if (split != dummyAxis_)
    {    
        const TValue delta = iCenter[split] - position[split]; // distance to splitting plane
        if (delta < TValue())
        {
            // we are left of the plane - search left node first
            doNeighbourhood(iCenter, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 1);
            if (num::sqr(delta) < ioSquaredRadius)
            {
                doNeighbourhood(iCenter, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 2);
            }
        }
        else
        {
            // we are right of the plane - search right node first
            doNeighbourhood(iCenter, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 2);
            if (num::sqr(delta) < ioSquaredRadius)
            {
                doNeighbourhood(iCenter, ioSquaredRadius, iMaxCount, oNeighbourhood, 2 * iNode + 1);
            }
        }
    }

    TValue squaredDistance = TValue();
    for (TAxis k = 0; k < dimension; ++k)
    {
        squaredDistance += num::sqr(position[k] - iCenter[k]);
    }

    if (squaredDistance < ioSquaredRadius)
    {
        oNeighbourhood.push_back(Neighbour(heap_[iNode], squaredDistance));
        std::push_heap(oNeighbourhood.begin(), oNeighbourhood.end());
        if (oNeighbourhood.size() > iMaxCount)
        {
            std::pop_heap(oNeighbourhood.begin(), oNeighbourhood.end());
            ioSquaredRadius = oNeighbourhood.front().squaredDistance();
        }
    }
}




#ifdef LASS_SPAT_KD_TREE_DIAGNOSTICS
template <typename O, template <class> class OT>
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
			using lass::prim::operator<<;
			xml_ << iAabb;

			if (iNode >= heap_.size() || heap_[iNode] == end_)
			{
				return;
			}

			const typename TObjectTraits::TPoint p = TObjectTraits::position(*heap_[iNode]);
			xml_ << p;

			const TAxis split = splits_[iNode];
			if (split == dummyAxis_)
			{
				return;
			}
			
			TAabb less = iAabb;
			typename TAabb::TPoint max = less.max();
			max[split] = p[split];
			less.setMax(max);
			visit(2 * iNode + 1, less);

			TAabb greater = iAabb;
			typename TAabb::TPoint min = greater.min();
			min[split] = p[split];
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
		aabb += TObjectTraits::position(*i);
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
