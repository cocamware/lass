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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_EDGE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_EDGE_3D_INL

#include "../spat_common.h"
#include "edge_3d.h"
#include "face_3d.h"
#include "pair_3d.h"
#include "vertex_3d.h"



namespace lass
{
namespace spat
{
namespace world_3d
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
Edge3D<T>::Edge3D(TVector& iTail, TVector& iHead):
    pairs_(0),
    midAir_(false)
{
    vertices_[negative] = iTail;
    vertices_[positive] = iHead;
}



/** return pointer to tail vertex of edge
 */
template <typename T>
Edge3D<T>::TVector& Edge3D<T>::tail(Polarity iEdgePolarity) const
{
    return vertices_[!iEdgePolarity];
}



/** return pointer to head vertex of edge
 */
template <typename T>
Edge3D<T>::TVector& Edge3D<T>::head(Polarity iEdgePolarity) const
{
    return vertices_[iEdgePolarity];
}



/** return UNNORMALIZED vector from tail to head.
 */
template <typename T>
Edge3D<T>::TVector Edge3D<T>::vector(Polarity iEdgePolarity) const
{
    return head(iEdgePolarity).position() - tail(iEdgePolarity).position();
}



/** return line segment from tail to head.
 */
template <typename T>
Edge3D<T>::TLineSegment Edge3D<T>::lineSeg(Polarity iEdgePolarity) const
{
    return TLineSegment(tail(iEdgePolarity).position(), head(iEdgePolarity).position());
}



/** Add a pair to an edge.  
 *  @warning After adding it, the pairs are not linked correctly yet.  You still
 *           need to do a sortPairs before you can visit them with spin and 
 *           counterSpin.
 */
template <typename T>
void Edge3D<T>::addPair(TPair& iPair)
{
    LASS_ASSERT(iPair.face()->frontCell());

    if (!pairs_)
    {
        iPair.setSpin(0); // represents end of list
    }
    else
    {
        iPair.setSpin(pairs_);
    }
    pairs_ = &iPair;
}



/** return number of pairs around edge. also works when pairs aren't sorted yet.
 */
template <typename T>
unsigned Edge3D<T>::sizePairs() const
{
    unsigned result = 0;
    Pair3SpinIterator pit;
    for (pit.reset(pairs_); !pit.end(); ++pit)
    {
        ++result;
    }

    return result;
}



/** Return "first" pair around edge.  Well "first", there is not really a first
 *  one, since it's cyclic.  But the edge has one pointer to one of them.
 *  That one is considered to be the "first".
 */
template <typename T>
TPair* Edge3D<T>::pair() const
{
    return pairs_;
}



/** erase a pair from around and edge.  This is quite simple, to erase a pair
 *  we just reconnect the two pairs surrounding it.  Of course there's a special
 *  situation where this is the only pair left.
 *  @warning This method does DELETE THE PAIR!  If you erase a pair, make sure
 *           you erases its tracks from its face.  Otherwise, the face may run
 *           into a dead end sometime later.
 *  @warning This method does NOT DELETE THE EDGE!  Even not if this pair
 *           is the last one for this edge.
 */
template <typename T>
void Edge3D<T>::erasePair(TPair& iPair)
{
    if (iPair.spin(positive) == iPair)
    {
        pairs_ = 0;     // iPair was the last one
    }
    else
    {
        TPair* spinPair = iPair->spin(positive);
        TPair* counterSpinPair = iPair->counterSpin(positive);

        spinPair->setCounterSpin(counterSpinPair);
        counterSpinPair->setSpin(spinPair);

        if (pairs_ == iPair)
        {
            pairs_ = spinPair;
        }
    }

    delete iPair;
}



/** Erase all pairs around edge.
 *  @warning This method does DELETE THE PAIRS!  If you erase all pairs around
 *           an edge, make sure no faces need them anymore..  Otherwise, a 
 *           face may run into a dead end sometime later.
 *  @warning This method does NOT DELETE THE EDGE!  Even not if this pair
 *           is the last one for this edge.
 */
template <typename T>
void Edge3D<T>::clearPairs()
{
    Pair3SpinIterator pit;
    for (pit.reset(pairs_); !pit.end();)
    {
        TPair* pair = pit.get();
        ++pit;
        delete pair;
    }

    pairs_ = 0;
}



/** sort pairs around edge so that they can be visited.  We do this by
 *  examining the cells of the pair's face.  the faces of neighbour pairs share
 *  exactly 1 cell, this can also be the null cell.  No other pairs share that
 *  same cell, thus for each cell (well for each relevant cell :), there are
 *  exactly two pairs of which their face are connected to the cell.  Even for
 *  the null cell!
 *  @warning you need to sort the pairs before you try to visit them!
 */
template <typename T>
void Edge3D<T>::sortPairs()
{
    typedef std::list<TPair*> TTempList;

    // fill list of all pairs
    TTempList pairs;
    LASS_ASSERT(pairs_);
    TPair* pair = pairs_->spin(positive); // don't add pairs_ since you would 
                                           // get it out immediately anyway.
    while (pair != 0)
    {
        pairs.push_back(pair);
        pair = pair->spin(positive);
    }
    LASS_ASSERT(!pairs.empty()); // you have at least two pairs per edge.
                                 // thus there must be at least one in the list.

    // now start to arrange the pairs around the edge, we already have one
    // in position, and that's pairs_.
    pair = pairs_;
    TCell* cell = pair->face()->frontCell();
    do
    {
        // find the other one with that links to the same cell.
        TTempList::iterator pit;
        TPair* foundPair = 0;
        for (pit = pairs.begin(); pit != pairs.end() && !foundPair; ++pit)
        {
            if ((*pit)->face()->link(cell))
            {
                foundPair = *pit;
                pairs.erase(pit);
            }
        }
        LASS_ASSERT(foundPair);

        // now link up both pairs.
        pair->setSpin(cell, foundPair);
        foundPair->setSpin(cell, pair);

        pair = foundPair;
        cell = foundPair->face()->twinCell(cell);
    }
    while (!pairs.empty());
    
    // now we still need to link the last one back to pairs_
    pair->setSpin(cell, pairs_);
    pairs_->setSpin(cell, pair);
}



/** Return true if one of the edge's pairs is linked to iFace.
 */
template <typename T>
bool Edge3D<T>::link(const TFace* iFace) const
{
    Pair3SpinIterator pit;
    for (pit.reset(pairs_); !pit.end(); ++pit)
    {
        if (pit->face() == iFace)
        {
            return true;
        }
    }
    return false;
}



/** Return true if the face of one of the edge's pairs is linked to iCell
 */
template <typename T>
bool Edge3D<T>::link(const TCell* iCell) const
{
    Pair3SpinIterator pit;
    for (pit.reset(pairs_); !pit.end(); ++pit)
    {
        if (pit->face()->link(iCell))
        {
            return true;
        }
    }
    return false;
}



/** Return true if edge is in the middle of the air (or black sided)
 */
template <typename T>
bool Edge3D<T>::midAir() const
{
    return midAir_;
}



/** set edge mid air status.
 */
template <typename T>
void Edge3D<T>::setMidAirBit(bool iMidAir)
{
    midAir_ = iMidAir;
}


// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



}

}

}

#endif

// EOF
