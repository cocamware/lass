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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_FACE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_FACE_3D_INL

#include "world_3d_common.h"
#include "face_3d.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
Face3D<T>::Face3D(const TPlane& iPlane, 
                  TCell* iFrontCell, 
                  TCell* iBackCell,
                  THandle iFrontHandle,
                  THandle iBackHandle):
    plane_(iPlane),
    frontHandle_(iFrontHandle),
    backHandle_(iBackHandle),
    frontCell_(iFrontCell),
    backCell_(iBackCell),
    pair_(0),
    midAir_(false)
{
    LASS_ASSERT(iFrontCell);
    iFrontCell->addFace(this);

    if (iBackCell)
    {
        iBackCell->addFace(this);
    }
}



/** Delete a face and its pairs.  Does not delete any edges nor cells.
 */
template <typename T>
Face3D<T>::~Face3D()
{
    clearPairs();
}



/** set the backcell of the face.
 */
template <typename T>
void Face3D<T>::setBackCell(TCell* iCell)
{
    LASS_ASSERT(!backCell_ || !iCell); // don't assign twice, unless to zero
    backCell_ = iCell;
}



/** replace pointer to old cell by pointer to new cell.  See if old cell is
 *  front or back cell, and then assign new cell to that one.
 *  @warning a face at least need a front cell.  this has as consequence that if
 *           you replace the frontcell by a null pointer, the face will 
 *           automatically be flipped so that the backcell becomes the front and
 *           the null pointer assigned as back.
 *  @warning this has as consequence that you may not set a front cell to the
 *           null pointer if it doesn't have a back cell!
 */
template <typename T>
void Face3D<T>::replaceCell(const TCell* iOldCell, TCell* iNewCell)
{
    LASS_ASSERT(link(iOldCell));
    if (frontCell_ == iOldCell)
    {        
        if (iNewCell == 0)
        {
            // we want to set the front cell to null, but we always need a 
            // frontcell.  So, if we want to get rid of the front cell, we must
            // have a backcell we can use as future frontCell by flipping the 
            // face.  After that, we can safely set the backcell (our previous 
            // frontcell, confusing huh? :) safely to null as requested.
            LASS_ASSERT(backCell_);
            flip();
            backCell_ = 0;
        }
        else
        {
            frontCell_ = iNewCell;
        }
    }
    else
    {
        backCell_ = iNewCell;
    }


}



/** return the front cell of the face.  
 *  @warning the face is supposed the have its normal to the frontcell.
 */
template <typename T>
Face3D<T>::TCell* Face3D<T>::frontCell() const
{
    return frontCell_;
}



/** return the back cell of the face.  
 *  @warning the face is supposed the have its normal to the frontcell.
 */
template <typename T>
Face3D<T>::TCell* Face3D<T>::backCell() const
{
    return backCell_;
}



/** return that face's cell that is at the same side of the face's plane as 
 *  iPoint.
 *  if iPoint lays exactly on the face's plane, return the front cell.
 */
template <typename T>
Face3D<T>::TCell* Face3D<T>::cell(const TPoint& iPoint) const
{
    return facePolarity(iPoint) == positive ? frontCell_ : backCell_;
}



/** return the twin cell at the other side of the face.  if you pass the
 *  frontcell, you'll get the backcell.  And if you pass the backcell, you'll
 *  get back the front cell.  Wonderfull huh?
 */
template <typename T>
Face3D<T>::TCell* Face3D<T>::twinCell(const TCell* iCell) const
{
    return iCell == frontCell_ ? backCell_ : frontCell_;
}



/** check if face has a link to the given cell.
 *  - return true if iCell is front or back cell of this face
 *  - return false if not.
 */
template <typename T>
bool Face3D<T>::link(const TCell* iCell) const
{
    return iCell == frontCell_ || iCell == backCell_;
}



/** check if one of the face's pairs links to the given edge.
 *  - return true if so
 *  - return false if not
 */
template <typename T>
bool Face3D<T>::link(const TEdge* iEdge) const
{
	LASS_ASSERT(iEdge);
	return iEdge->link(this);
}



/** return the front handle.
 */
template <typename T>
Face3D<T>::THandle Face3D<T>::frontHandle() const
{
    return frontHandle_;
}



/** return the back handle.
 */
template <typename T>
Face3D<T>::THandle Face3D<T>::backHandle() const
{
    return backHandle_;
}



/** return the handle of the face on the side of iCell.
 */
template <typename T>
Face3D<T>::THandle Face3D<T>::handle(const TCell* iCell) const
{
    LASS_ASSERT(link(iCell));
    return iCell == backCell_ ? backHandle_ : frontHandle_;
}



/** set the first pair of the face.  All other pairs can be visited by using the
 *  methods clock() and counterClock() of TPair.  These methods will give you
 *  the next pair of the face in clockwise or counterclockwise order.
 */
template <typename T>
void Face3D<T>::setPair(TPair* iPair)
{
    pair_ = iPair;
}



/** return the first pair of the face.  All other pairs can be visited by using
 *  the methods clock() and counterClock() of TPair.  These methods will give
 *  you the next pair of the face in clockwise or counterclockwise order.
 */
template <typename T>
TPair* Face3D<T>::pair() const
{
    return pair_;
}



/** Return counter clock iterator reseted to the first pair, and ready to
 *  go around the face.
 */
template <typename T>
Face3D<T>::TPair::TCounterClockIterator Face3D<T>::beginPair(Polarity iFacePolarity) const
{
    return TPair::TCounterClockIterator(pair_, iFacePolarity);
}



/** Return counter clock iterator reseted to the first pair, and ready to
 *  go around the face.
 */
template <typename T>
Face3D<T>::TPair::TCounterClockIterator Face3D<T>::beginPair(const TCell* iCell) const
{
    return beginPair(facePolarity(iCell));
}



/** Return counter clock iterator reseted to the first pair, and ready to
 *  go around the face.
 */
template <typename T>
Face3D<T>::TPair::TCounterClockIterator Face3D<T>::beginPair(const TPoint& iPoint) const
{
    return beginPair(facePolarity(iPoint));
}



/** Delete the pairs of the face
 */
template <typename T>
void Face3D<T>::clearPairs()
{
    if (pair_)
    {
        // although pair_ gets deleted immediately, you can still use its
        // original address to deal with the iterator.
        TPair::TCounterClockIterator pit;
        for (pit.reset(pair_); !pit.end(); )
        {
            TPair* pair = pit.get();
            ++pit;
            pair->edge()->erasePair(pair);
        }
        pair_ = 0;
    }
}



/** return the face's barycenter.
 *  @warning This is a pseudo barycenter, calculated on base of it's vertices.
 *           Hence, this is not the barycenter of the surface area of the face.
 */
template <typename T>
Face3D<T>::TPointH Face3D<T>::barycenter() const
{
    TPointH result;

    LASS_ASSERT(pair_);
    TPair::TCounterClockIterator pit;
    for (pit.reset(pair_); !pit.end(); ++pit)
    {
        const TPoint& head = pit->head()->position();
        result += head;
    }

    return result;
}



/** Return polygon of contour of face.
 */
template <typename T>
Face3D<T>::TPolygon Face3D<T>::polygon(Polarity iFacePolarity) const
{
    TPolygon result;

    TPair::TCounterClockIterator pit;
    for (pit.reset(pair_, iFacePolarity); !pit.end(); ++pit)
    {
        TPoint tail = pit->tail()->position();
        result.addVertex(tail);
    }
    result.setPlane(plane_);

    return result;
}



/** Return polygon of contour of face.
 */
template <typename T>
Face3D<T>::TPolygon Face3D<T>::polygon(const TCell* iCell) const
{
    return polygon(facePolarity(iCell));
}



/** Return polygon of contour of face.
 */
template <typename T>
Face3D<T>::TPolygon Face3D<T>::polygon(const TPoint& iPoint) const
{
    return polygon(facePolarity(iPoint));
}



template <typename T>
Face3D<T>::TPlane Face3D<T>::plane(Polarity iFacePolarity) const
{
    if (iFacePolarity)
    {
        return plane_;
    }
    else
    {
        TPlane result = plane_;
        result.flip();
        return result;
    }
}



/** return the support plane of the face.
 */
template <typename T>
Face3D<T>::TPlane Face3D<T>::plane(const TCell* iCell) const
{
    LASS_ASSERT(link(iCell));
    return plane(facePolarity(iCell));
}




/** return the support plane of the face.
 */
template <typename T>
Face3D<T>::TPlane Face3D<T>::plane(const TPoint& iPoint) const
{
    return plane(facePolarity(iPoint));
}



/** return the face's normal.
 */
template <typename T>
Face3D<T>::TVector Face3D<T>::normal(Polarity iFacePolarity) const
{
    return iFacePolarity ? plane_.normal() : -plane_.normal();
}



/** return the face's normal to the given cell.
 *  @warning this function will also return something if the given cell is nor
 *           the front cell nor the back cell.  In that case, it will return
 *           the positive normal.  Use link() to check if iCell belongs to face
 */
template <typename T>
Face3D<T>::TVector Face3D<T>::normal(const TCell* iCell) const
{
    LASS_ASSERT(link(iCell));
    return normal(facePolarity(iCell));
}



/** return the face's normal to the give point.
 */
template <typename T>
Face3D<T>::TVector Face3D<T>::normal(const TPoint& iPoint) const
{
    return normal(facePolarity(iPoint));
}



/** Determine polarity of face against cell.
 *  - return negative if iCell is back cell of face
 *  - return positive if iCell is front cell.
 *  @warning this function will also return positive if iCell is nor the back 
 *           cell nor the front cell.  i.e. if it is not a cell that is linked 
 *           to the face at all.  Be carefull about that!  Use link() to check 
 *           that.
 */
template <typename T>
Face3D<T>::Polarity Face3D<T>::facePolarity(const TCell* iCell) const
{
    LASS_ASSERT(link(iCell));
    return iCell == frontCell_ ? positive : negative;
}



/** Determine polarity of face against point.
 *  - return Postive if point is in front of face with respect to the faces's
 *    normal (or exactly on the plane)
 *  - return negative if point is at back.
 */
template <typename T>
Polarity Face3D<T>::facePolarity(const TPoint& iPoint) const
{
    return plane_.side(iPoint) != psInBack ? positive : negative;
}



/** return true if BOTH SIDES OF THE FACE are isBlack.  Being isBlack means it 
 *  absorbs everything: no reflection and no transmission.
 */
template <typename T>
bool Face3D<T>::isBlack() const
{
    return !backCell_ || (frontHandle_ == blackHandle() && backHandle_ == blackHandle());
}



/** return true if THIS SIDE OF THE FACE is isBlack.  Being isBlack means it 
 *  absorbs everything.  That means there is no reflection and no transmission.
 *  Only the side for the cell is checked!  It doesn't matter what the other 
 *  side tells.
 *  @warning we use a special handle for this: the blackHandle!  If you want
 *           a face to return true on this thest, you must set the (right)
 *           handle to blackHandle()
 */
template <typename T>
bool Face3D<T>::isBlack(const TCell* iCell) const
{
    LASS_ASSERT(link(iCell));
    return !backCell_ || handle(iCell) == blackHandle();
}



/** return true if face seems as it doesn't exits.  A face is isImmaterial if
 *  - it doesn't reflect (from this side)
 *  - it transmits everything (from this side)
 *  - the cell (air) media at both sides are equal.
 */
template <typename T>
bool Face3D<T>::isImmaterial() const
{
    return backCell_ && !frontHandle_ && !backHandle_ && 
        (frontCell_->medium() == backCell_->medium());
}



/** return true if face is isMidAir (i.e. al it's edges are middAir)
 */
template <typename T>
bool Face3D<T>::isMidAir() const
{
    return midAir_;
}



/** set face mid air status.
 */
template <typename T>
void Face3D<T>::setMidAirBit(bool iMidAir)
{
    midAir_ = iMidAir;
}



/** flip the face.  Plane(normal) is flipped, front and back cell are swapped,
 *  all polarities of the pairs are flipped, and the sequence of pairs is
 *  flipped.
 */
template <typename T>
void Face3D<T>::flip()
{
    LASS_ASSERT(backCell_);
    std::swap(frontCell_, backCell_);
    std::swap(frontHandle_, backHandle_);
    plane_.flip();

    TPair::TCounterClockIterator pit;
    for (pit.reset(pair_); pit.get(); ++pit)
    {
        pit->flip();
    }
}



/** flip the face so that the given cell is the front cell.
 */
template <typename T>
void Face3D<T>::flip(const TCell* iCell)
{
    LASS_ASSERT(link(iCell));
    if (iCell == backCell_)
    {
        flip();
    }
}



/** intersect a linesegment and face.  Intersectionpoint is returned by its
 *  parameter on linesegment via a_t.  The returnvalue of the function indicates
 *  the number of intersections:
 *  - 0: no intersection between linesegment and face.  a_t is invalid.
 *  - 1: exactly 1 intersection.  a_t is parameter of that intersection on the 
 *       linesegment.
 *  - -1: infinitely many intersections.  a_t is invalid.
 *
 *  @todo -1 part?
 */
template <typename T>
int Face3D<T>::intersect(const LineSeg3& a_lineSeg, Real& a_t) const
{
    // first of all, line segment must intersect support plane.  
    // if it doesn't, it can never intersect the polygon
    if (plane_.intersect(a_lineSeg, a_t) != 1)
    {
        // -1 part?
        return 0;
    }

    // secondly, candidate must be part of polygon.  First I wanted to check
    // directly if line goes through polygon instead of checking the candidate
    // intersection point.  But I need to check out plucker coordinates for that
    // first.  Instead I'm checking if the candidate is at the left of each 
    // edge.  This is possible since the face is convex.
    const TPoint candidate = a_lineSeg.point(a_t);

    TPair::TCounterClockIterator pit;
    for (pit.reset(pair_); !pit.end(); ++pit)
    {
        const TPoint edgeTail = pit->tail()->position();
        const TPoint edgeHead = pit->head()->position();

        if (triple(normal(), edgeTail - candidate, edgeHead - candidate) < 0.0)
        {
            return 0;
        }
    }

    // it's ok, we have a winner.
    return 1;
}



/** Return pointer of THE isBlack handle.
 */
template <typename T>
THandle Face3D<T>::blackHandle()
{
    static DummyHandle blackHandle;
    return &blackHandle;
}


// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



}

}

}

#endif

// EOF
