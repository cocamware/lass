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

/** @class lass::spat::world_3d::Face3D
 *  @brief Faces border the cells in World3D.
 *  @author Bram de Greve [BdG]
 *
 *  @warning: a face is supposed to face it's first cell, i.e.
 *            it's normal is supposed to stick into frontCell_;
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_FACE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_FACE_3D_H

#include "../spat_common.h"
#include "world_3d.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

template <typename T>
class Face3: public util::NonCopyable
{
public:

	template Face3D<T> TSelf;

	template World3D<T> TWorld;
	template typename TWorld::TCell TCell;
	template typename TWorld::TEdge TEdge;
	template typename TWorld::THandle THandle;
	template typename TWorld::TPair TPair;
	template typename TWorld::TPlane TPlane;
	template typename TWorld::TPoint TPoint;
	template typename TWorld::TPolygon3 TPolygon3;
	template typename TWorld::TVector TVector;

	Face3(const TPlane& iPlane, TCell* iFrontCell, TCell* iBackCell,
          THandle iFrontHandle, THandle iBackHandle);
    ~Face3();


    // METHODS

    void setBackCell(TCell* iCell);
    void replaceCell(const TCell* iOldCell, TCell* iNewCell);
    TCell* frontCell() const;
    TCell* backCell() const;
    TCell* cell(const TPoint& iPoint) const;
    TCell* twinCell(const TCell* iCell) const;
    bool link(const TCell* iCell) const;
	bool link(const TEdge* iEdge) const;

    THandle frontHandle() const;
    THandle backHandle() const;
    THandle handle(const TCell* iCell) const;

    void setPair(TPair* iPair);
    TPair* pair() const;
    TPair::TCounterClockIterator beginPair(Polarity iFacePol = positive) const;
    TPair::TCounterClockIterator beginPair(const TCell* iCell) const;
    TPair::TCounterClockIterator beginPair(const TPoint& iPoint) const;
    void clearPairs();

    TPolygon3 polygon(Polarity iFacePolarity = positive) const;
    TPolygon3 polygon(const TCell* iCell) const;
    TPolygon3 polygon(const TPoint& iPoint) const;
    TPlane plane(Polarity iFacePolarity = positive) const;
    TPlane plane(const TCell* cell) const;
    TPlane plane(const TPoint& iPoint) const;
    TVector normal(Polarity iFacePolarity = positive) const;
    TVector normal(const TCell* iCell) const;
    TVector normal(const TPoint& iPoint) const;
    TPoint barycenter() const;

    Polarity facePolarity(const TCell* iCell) const;
    Polarity facePolarity(const TPoint& iPoint) const;

    bool isBlack() const;
    bool isBlack(const TCell* iCell) const;
    bool isImmaterial() const;
    bool isMidAir() const;
    void setMidAirBit(bool a_midAir);

    void flip();
    void flip(const TCell* iCell);
    int intersect(const TLineSegment& a_lineSeg, TReference a_t) const;

    // STATIC METHODS

    static THandle blackHandle();

private:

    // DATA

    Plane3 plane_;
    void* frontHandle_;
    void* backHandle_;
    Cell3* frontCell_;
    Cell3* backCell_;
    TPair* pair_;
    bool midAir_;


    // PRIVATE STRUCTURES

    struct DummyHandle
    {
        unsigned dummy;
        DummyHandle(): dummy(0x66666666) {}
    };
};

}

}

}

#include "face_3d.inl"

#endif

// EOF