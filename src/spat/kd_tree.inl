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
#	include "../meta/if.h"
#	include "../prim/aabb_2d.h"
#	include "../prim/aabb_3d.h"
#endif

namespace lass
{
namespace spat
{

// --- public --------------------------------------------------------------------------------------

template <typename O, template <class> class OT>
KdTree<O, OT>::KdTree(const TObjects& iObjects):
	objects_(iObjects)
{
	buildHeap();
}



template <typename O, template <class> class OT>
template <typename InputIterator>
KdTree<O, OT>::KdTree(InputIterator iObjectBegin, InputIterator iObjectEnd):
	objects_(iObjectBegin, iObjectEnd)
{
	buildHeap();
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename O, template <class> class OT>
void KdTree<O, OT>::buildHeap()
{
    const unsigned n = objects_.size();

    heap_.resize(n, dummy_);
    splits_.resize(n, dummy_);

    TIndices indices(n);
    for (unsigned i = 0; i < n; ++i)
    {
        indices[i] = i;
    }

    balance(0, indices, 0, n);

#ifdef LASS_SPAT_KD_TREE_DIAGNOSTICS
	diagnostics();
#endif
}



template <typename O, template <class> class OT>
void KdTree<O, OT>::balance(unsigned iNode,                  
                            TIndices& ioIndices, 
                            unsigned iBegin, 
                            unsigned iEnd)
{
    if (iEnd == iBegin)
    {
        return;
    }

    if (iEnd == iBegin + 1)
    {
        // exactly one element in content
        assignNode(iNode, ioIndices[iBegin], dummy_);
        return;
    }

    // more than one, do the balance
    //
    const unsigned d = findSplitDimension(ioIndices, iBegin, iEnd);
    const unsigned median = (iBegin + iEnd) / 2;
    std::nth_element(&ioIndices[iBegin], &ioIndices[median], &ioIndices[iEnd], LessDim(objects_, d));
    assignNode(iNode, ioIndices[median], d);

    balance(2 * iNode + 1, ioIndices, iBegin, median);
    balance(2 * iNode + 2, ioIndices, median + 1, iEnd);
}



template <typename O, template <class> class OT>
unsigned KdTree<O, OT>::findSplitDimension(const TIndices& iIndices,
                                           unsigned iBegin, 
                                           unsigned iEnd)
{
    TPoint min = TObjectTraits::position(objects_[iIndices[iBegin]]);
    TPoint max = min;

    for (unsigned i = iBegin + 1; i < iEnd; ++i)
    {
        const TPoint position = TObjectTraits::position(objects_[iIndices[i]]);
        for (unsigned k = 0; k < dimension; ++k)
        {
            min[k] = std::min(min[k], position[k]);
            max[k] = std::max(max[k], position[k]);
        }
    }

    unsigned split = 0;
    TValue maxDistance = max[0] - min[0];
    for (unsigned k = 1; k < dimension; ++k)
    {
        const TValue distance = max[k] - min[k];
        if (distance > maxDistance)
        {
            split = k;
            maxDistance = distance;
        }
    }

    return split;
}



template <typename O, template <class> class OT>
inline void KdTree<O, OT>::assignNode(unsigned iNode, unsigned iIndex, unsigned iSplitDimension)
{
    if (heap_.size() <= iNode)
    {
        heap_.resize(iNode + 1, dummy_);
        splits_.resize(iNode + 1, 0);
    }
    heap_[iNode] = iIndex;
    splits_[iNode] = iSplitDimension;
}



#ifdef LASS_SPAT_KD_TREE_DIAGNOSTICS
template <typename O, template <class> class OT>
void KdTree<O, OT>::diagnostics()
{
	typedef typename meta::If<dimension == 2, prim::Aabb2D<TValue>, prim::Aabb3D<TValue> >::Type TAabb;
	class Visitor
	{
	public:
		Visitor(const TObjects& iObjects, const TIndices& iHeap, const TIndices& iSplits):
			xml_("kdtree.xml", std::ios_base::trunc),
			objects_(iObjects),
			heap_(iHeap),
			splits_(iSplits)
		{
			using lass::prim::operator<<;
			xml_ << "<kdtree>" << std::endl;
		}

		~Visitor()
		{
			using lass::prim::operator<<;
			xml_ << "</kdtree>" << std::endl;
		}

		void visit(unsigned iIndex, const TAabb& iAabb)
		{
			using lass::prim::operator<<;
			xml_ << iAabb;

			if (iIndex >= heap_.size() || heap_[iIndex] == dummy_)
			{
				return;
			}

			const typename TObjectTraits::TPoint p = TObjectTraits::position(objects_[heap_[iIndex]]);
			xml_ << p;

			const unsigned d = splits_[iIndex];
			if (d == dummy_)
			{
				return;
			}
			
			//xml_ << "<less>" << std::endl;
			TAabb less = iAabb;
			typename TAabb::TPoint max = less.max();
			max[d] = p[d];
			less.setMax(max);
			visit(2 * iIndex + 1, less);
			//xml_ << "</less>" << std::endl;

			//xml_ << "<greater>" << std::endl;
			TAabb greater = iAabb;
			typename TAabb::TPoint min = greater.min();
			min[d] = p[d];
			greater.setMin(min);
			visit(2 * iIndex + 2, greater);
			//xml_ << "</greater>" << std::endl;
		}

	private:
	    TObjects objects_;
		TIndices heap_;
		TIndices splits_;
		io::XmlOFile xml_;
	};		

	TAabb aabb;
	const unsigned n = objects_.size();
	for (unsigned i = 0; i < n; ++i) 
	{
		aabb += TObjectTraits::position(objects_[i]);
	}

	Visitor visitor(objects_, heap_, splits_);
	visitor.visit(0, aabb);
}
#endif



// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
