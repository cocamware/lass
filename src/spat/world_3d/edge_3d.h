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



/** @class lass::spat::world_3d::Edge3D
 *  @brief A 3D Edge
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_EDGE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_EDGE_3D_H

#include "../spat_common.h"
#include "world_3d.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

template <typename T>
class Edge3D: public util::NonCopyable
{
public:

	typedef Edge3D<T> TSelf;

	typedef World3D<T> TWorld;
    typedef typename TWorld::TFace TFace;
	typedef typename TWorld::TLineSegment TLineSegment;
	typedef typename TWorld::TPair TPair;
	typedef typename TWorld::TVertex TVertex;
	typedef typename TWorld::TVector TVector;

    // STRUCTORS

    Edge3D(TVertex& a_tail, TVertex& a_head);


    // METHODS

    TVertex& tail(Polarity iEdgePolarity = positive) const;
    TVertex& head(Polarity iEdgePolarity = positive) const;
    TVector vector(Polarity iEdgePolarity = positive) const;
    TLineSegment lineSeg(Polarity iEdgePolarity = positive) const;

    void addPair(TPair& iPair);
    unsigned sizePairs() const;
    TPair* pair() const;
    void erasePair(TPair& iPair);
    void clearPairs();
    void sortPairs();

    bool link(const TFace* iFace) const;
	bool link(const TCell* iCell) const;

    bool midAir() const;    
    void setMidAirBit(bool iMidAir);

private:

    // DATA

    TVertex& vertices_[2];
    TPair* pairs_;
    bool midAir_;
};



}

}

}

#include "edge_3d.inl"

#endif

// EOF
