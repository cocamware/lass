/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 */

/** @class lass::spat::KdTree
 *  @brief a KD tree for point-like objects
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_KD_TREE_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_KD_TREE_H

#define LASS_SPAT_KD_TREE_DIAGNOSTICS

#include "spat_common.h"

namespace lass
{
namespace spat
{

template <typename PointType>
struct KdTreeDefaultTraits
{
    typedef PointType TPoint;
    typedef typename PointType::TValue TValue;
    enum { dimension = PointType::dimension };

    static const PointType& position(const PointType& iPoint) { return iPoint; }
};



template 
<
    class ObjectType, 
    template <class> class ObjectTraits = KdTreeDefaultTraits
>
class KdTree
{
public:

    typedef ObjectType TObjectType;
    typedef ObjectTraits<ObjectType> TObjectTraits;

    typedef typename TObjectTraits::TPoint TPoint;
	typedef typename TObjectTraits::TValue TValue;

    enum { dimension = TObjectTraits::dimension };

    typedef std::vector<ObjectType> TObjects;

	KdTree(const TObjects& iObjects);
    template <typename InputIterator> KdTree(InputIterator iObjectBegin, InputIterator iObjectEnd);

private:

	class LessDim
	{
	public:
		LessDim(const TObjects& iObjects, unsigned iIndex): objects_(iObjects), index_(iIndex) {}
		bool operator()(unsigned iA, unsigned iB)
		{
			return TObjectTraits::position(objects_[iA])[index_] < TObjectTraits::position(objects_[iB])[index_];
		}
	private:
		const TObjects& objects_;
		unsigned index_;
	};

	typedef std::vector<unsigned> TIndices;

    enum { dummy_ = unsigned(-1)} ;

	void buildHeap();
    void balance(unsigned iNode, TIndices& ioIndices, unsigned iBegin, unsigned iEnd);
    unsigned findSplitDimension(const TIndices& iIndices, unsigned iBegin, unsigned iEnd);
    void assignNode(unsigned iNode, unsigned iIndex, unsigned iSplitDimension);

    TObjects objects_;
	TIndices heap_;
    TIndices splits_;

#ifdef LASS_SPAT_KD_TREE_DIAGNOSTICS
	void diagnostics();
#endif
};


}

}

#include "kd_tree.inl"

#endif

// EOF
