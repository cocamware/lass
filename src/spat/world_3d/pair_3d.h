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



/** @class lass::spat::world_3d::Pair3D
 *  @brief a pair connects faces and edges, and gives access to the neighbourhood
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PAIR_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PAIR_3D_H

#include "../spat_common.h"
#include "polarity.h"
#include "pair_3d_iterators.h"
#include "pair_3d_const_iterators.h"
#include "../../prim/line_segment_3d.h"
#include "../../prim/point_3d.h"
#include "../../prim/vector_3d.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

template <typename T> class Cell3D;
template <typename T> class Edge3D;
template <typename T> class Face3D;
template <typename T> class TVertex;

template <typename T>
class Pair3D
{
public:

	typedef Cell3D<T> TCell;
	typedef Edge3D<T> TEdge;
	typedef Face3D<T> TFace;
	typedef Pair3D<T> TPair;
	typedef TVertex<T> TVertex;
	typedef prim::LineSegment3D<T> TLineSegment;
	typedef prim::Point3D<T> TPoint;
	typedef prim::Vector3D<T> TVector;
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;

	typedef Pair3DSpinIterator<T> TSpinIterator;
	typedef Pair3DCounterSpinIterator<T> TCounterSpinIterator;
	typedef Pair3DClockIterator<T> TClockIterator;
	typedef Pair3DCounterClockIterator<T> TCounterClockIterator;

	typedef Pair3DConstSpinIterator<T> TConstSpinIterator;
	typedef Pair3DConstCounterSpinIterator<T> TConstCounterSpinIterator;
	typedef Pair3DConstClockIterator<T> TConstClockIterator;
	typedef Pair3DConstCounterClockIterator<T> TConstCounterClockIterator;

    // STRUCTORS

    Pair3D(TFace* a_face, TEdge* a_edge, Polarity a_edgePolarity);


    // METHODS

    TEdge* edge() const;
    TFace* face() const;
    TPair* clock(Polarity a_facePolarity = positive) const;
    TPair* clock(const TCell* a_cell) const;
    TPair* clock(const TPoint& a_point) const;
    TPair* counterClock(Polarity a_facePolarity = positive) const;
    TPair* counterClock(const TCell* a_cell) const;
    TPair* counterClock(const TPoint& a_point) const;
    TPair* spin(Polarity a_edgePolarity = positive) const;
    TPair* spin(const TCell* a_cell) const;
    TPair* spin(const TPoint& a_point) const;
    TPair* counterSpin(Polarity a_edgePolarity = positive) const;
    TPair* counterSpin(const TCell* a_cell) const;
    TPair* counterSpin(const TPoint& a_point) const;
    Polarity pairPolarity() const;
    Polarity facePolarity(const TCell* a_cell) const;
    Polarity facePolarity(const TPoint& a_point) const;
    Polarity edgePolarity(Polarity a_facePolarity) const;
    Polarity edgePolarity(const TCell* a_cell) const;
    Polarity edgePolarity(const TPoint& a_point) const;

    void setFace(TFace* a_face);
    void setClock(TPair* a_clock);
    void setCounterClock(TPair* a_counterClock);
    void setSpin(TPair* a_spin);
    void setSpin(TCell* a_cell, TPair* a_spin);
    void setCounterSpin(TPair* a_counterSpin);

    TVertex* tail(Polarity a_facePolarity = positive) const;
    TVertex* tail(const TCell* a_cell) const;
    TVertex* tail(const TPoint& a_point) const;
    TVertex* head(Polarity a_facePolarity = positive) const;
    TVertex* head(const TCell* a_cell) const;
    TVertex* head(const TPoint& a_point) const;
    TVector vector(Polarity a_facePolarity = positive) const;
    TVector vector(const TCell* a_cell) const;
    TVector vector(const TPoint& a_point) const;
    TLineSegment lineSeg(Polarity a_facePolarity = positive) const;
    TLineSegment lineSeg(const TCell* a_cell) const;
    TLineSegment lineSeg(const TPoint& a_point) const;
    TPoint point(TParam a_t, Polarity a_facePolarity = positive) const;
    TPoint point(TParam a_t, const TCell* a_cell) const;
    TPoint point(TParam a_t, const TPoint& a_point) const;

    void flip();

private:

    // DATA

    TFace* m_face;
    TEdge* m_edge;

    TPair* m_clock;
    TPair* m_counterClock;
    TPair* m_spin;
    TPair* m_counterSpin;

    Polarity m_pairPolarity;
};



}

}

}

#include "pair_3d.inl"

#endif

// EOF
