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

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PAIR_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PAIR_3D_INL

#include "../spat_common.h"
#include "pair_3d.h"
#include "edge_3d.h"
#include "face_3d.h"
#include "vertex_3d.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
Pair3D<T>::Pair3D(TFace* a_face, TEdge* a_edge, Polarity a_pairPolarity):
    m_face(a_face),
    m_edge(a_edge),
    m_counterClock(0),
    m_clock(0),
    m_spin(0),
    m_counterSpin(0),
    m_pairPolarity(a_pairPolarity)
{
    m_edge->addPair(this);
}



// --- PUBLIC METHODS ----------------------------------------------------------

/** Return pointer to the edge.
 */
template <typename T>
Pair3D<T>::TEdge* Pair3D<T>::edge() const
{
    return m_edge;
}



/** Return pointer to the face.
 */
template <typename T>
Pair3D<T>::TFace* Pair3D<T>::face() const
{
    return m_face;
}



/** Return next pair around face in clock direction, with correct face
 *  polarity by a_polarity.
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::clock(Polarity a_facePolarity) const
{
    return a_facePolarity == positive ? m_clock : m_counterClock;
}



/** Return next pair around face in clock direction, with correct face
 *  polarity by a_cell.
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::clock(const TCell* a_cell) const
{
    LASS_ASSERT(m_face->link(a_cell));
    return clock(m_face->facePolarity(a_cell));
}



/** Return next pair around face in clock direction, with correcct face
 *  polarity by a_point.
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::clock(const TPoint& a_point) const
{
    return clock(m_face->facePolarity(a_point));
}



/** Return next pair around face in counter clock direction, with correct face
 *  polarity by a_polarity.
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::counterClock(Polarity a_facePolarity) const
{
    return a_facePolarity == positive ? m_counterClock : m_clock;
}



/** Return next pair around face in counterClock direction, with correct face
 *  polarity by a_cell.
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::counterClock(const TCell* a_cell) const
{
    LASS_ASSERT(m_face->link(a_cell));
    return counterClock(m_face->facePolarity(a_cell));
}



/** Return next pair around face in counterClock direction, with correct face
 *  polarity by a_point.
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::counterClock(const TPoint& a_point) const
{
    return counterClock(m_face->facePolarity(a_point));
}



/** Return next pair around edge in spin direction, with correct edge
 *  polarity by a_polarity
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::spin(Polarity a_edgePolarity) const
{
    return a_edgePolarity == positive ? m_spin : m_counterSpin;
}




/** Return next pair around edge in spin direction, with correct edge
 *  polarity by a_cell.
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::spin(const TCell* a_cell) const
{
    LASS_ASSERT(m_face->link(a_cell));
    return spin(edgePolarity(a_cell));
}




/** Return next pair around edge in spin direction, with correct edge
 *  polarity by a_point.
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::spin(const TPoint& a_point) const
{
    return spin(edgePolarity(a_point));
}



/** Return next pair around edge in counter spin direction, with correct edge
 *  polarity by a_polarity
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::counterSpin(Polarity a_edgePolarity) const
{
    return a_edgePolarity == positive ? m_counterSpin : m_spin;
}




/** Return next pair around edge in counter spin direction, with correct edge
 *  polarity by a_cell.
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::counterSpin(const TCell* a_cell) const
{
    LASS_ASSERT(m_face->link(a_cell));
    return counterSpin(edgePolarity(a_cell));
}



/** Return next pair around edge in counter spin direction, with correct edge
 *  polarity by a_point.
 */
template <typename T>
Pair3D<T>::TPair* Pair3D<T>::counterSpin(const TPoint& a_point) const
{
    return counterSpin(edgePolarity(a_point));
}



/** Return build in pair polarity.
 */
template <typename T>
Polarity Pair3D<T>::pairPolarity() const
{
    return m_pairPolarity;
}



/** Return polarity of edge against face, assuming facePolarity.
 */
template <typename T>
Polarity Pair3D<T>::edgePolarity(Polarity a_facePolarity) const
{
    return m_pairPolarity == a_facePolarity ? positive : Negative;
}



/** WHAT IS THIS???
 */
template <typename T>
Polarity Pair3D<T>::edgePolarity(const TCell* a_cell) const
{
    return edgePolarity(facePolarity(a_cell));
}



/** WHAT IS THIS???
 */
template <typename T>
Polarity Pair3D<T>::edgePolarity(const TPoint& a_point) const
{
    return edgePolarity(facePolarity(a_point));
}



/** Return polarity of face agains cell.
 */
template <typename T>
Polarity Pair3D<T>::facePolarity(const TCell* a_cell) const
{
    return m_face->facePolarity(a_cell);
}



/** Return polarity of face agains point.
 */
template <typename T>
Polarity Pair3D<T>::facePolarity(const TPoint& a_point) const
{
    return m_face->facePolarity(a_point);
}



/** Set the face of the pair.
 */
template <typename T>
void Pair3D<T>::setFace(TFace* a_face)
{
    m_face = a_face;
}



/** Set the next pair that is found on the face in COUNTERCLOCKWISE order.  
 */
template <typename T>
void Pair3D<T>::setCounterClock(TPair* a_counterClock)
{
    m_counterClock = a_counterClock;
}



/** Set the next pair that is found on the face in CLOCKWISE order.  
 */
template <typename T>
void Pair3D<T>::setClock(TPair* a_clock)
{
    m_clock = a_clock;
}



/** Set the counterClock pair that is found around the edge in clockwise order.  
 */
template <typename T>
void Pair3D<T>::setSpin(TPair* a_spin)
{
    m_spin = a_spin;
}



/** Set the counterClock pair that is found by spinning around the edge in this cell.
 *  This pair found is the one we would get by spinning around the edge under
 *  the condtion that the face's normal is pointed inside a_cell, and the edge
 *  is oriented counterclockwise accoring the face's normal.
 */
template <typename T>
void Pair3D<T>::setSpin(TCell* a_cell, TPair* a_spin)
{
    if (edgePolarity(a_cell) == positive)
    {
        m_spin = a_spin;
    }
    else
    {
        m_counterSpin = a_spin;
    }
}



/** Set the counterClock pair that is found around the edge in counterclockwise order.
 */
template <typename T>
void Pair3D<T>::setCounterSpin(TPair* a_pair)
{
    m_counterSpin = a_pair;
}



/** Return tail of edge, following face polarity.
 */
template <typename T>
Pair3D<T>::TVertex* Pair3D<T>::tail(Polarity a_facePolarity) const
{
    return m_edge->tail(edgePolarity(a_facePolarity));
}



/** Return tail of edge, following pair's and cell's orientation.
 */
template <typename T>
Pair3D<T>::TVertex* Pair3D<T>::tail(const TCell* a_cell) const
{
    return m_edge->tail(edgePolarity(a_cell));
}



/** Return tail of pair with correct polarity for point.
 */
template <typename T>
Pair3D<T>::TVertex* Pair3D<T>::tail(const TPoint& a_point) const
{
    return m_edge->tail(edgePolarity(a_point));
}



/** Return head of edge, following face's orientation.
 */
template <typename T>
Pair3D<T>::TVertex* Pair3D<T>::head(Polarity a_facePolarity) const
{
    return m_edge->head(edgePolarity(a_facePolarity));
}



/** Return head of edge, following pair's and cell's orientation.
 */
template <typename T>
Pair3D<T>::TVertex* Pair3D<T>::head(const TCell* a_cell) const
{
    return m_edge->head(edgePolarity(a_cell));
}



/** Return head of pair, with correct polarity for point.
 */
template <typename T>
Pair3D<T>::TVertex* Pair3D<T>::head(const TPoint& a_point) const
{
    return m_edge->head(edgePolarity(a_point));
}



/** Return vector from tail to head, with correct polarity for a_face.
 */
template <typename T>
Pair3D<T>::TVector Pair3D<T>::vector(Polarity a_facePolarity) const
{
    return m_edge->vector(edgePolarity(a_facePolarity));
}



/** Return a vector from tail to head, with correct polarity for a_cell.
 */
template <typename T>
Pair3D<T>::TVector Pair3D<T>::vector(const TCell* a_cell) const
{
    return m_edge->vector(edgePolarity(a_cell));
}



/** Return a vector from tail to head, with correct polarity for a_point.
 */
template <typename T>
Pair3D<T>::TVector Pair3D<T>::vector(const TPoint& a_point) const
{
    return m_edge->vector(edgePolarity(a_point));
}



/** Return line segment from tail to head, with correct polarity for a_face.
 */
template <typename T>
Pair3D<T>::TLineSegment Pair3D<T>::lineSeg(Polarity a_facePolarity) const
{
    return m_edge->lineSeg(edgePolarity(a_facePolarity));
}



/** Return a line segment from tail to head, with correct polarity for a_cell.
 */
template <typename T>
Pair3D<T>::TLineSegment Pair3D<T>::lineSeg(const TCell* a_cell) const
{
    return m_edge->lineSeg(edgePolarity(a_cell));
}



/** Return a line segment from tail to head, with correct polarity for a_point.
 */
template <typename T>
Pair3D<T>::TLineSegment Pair3D<T>::lineSeg(const TPoint& a_point) const
{
    return m_edge->lineSeg(edgePolarity(a_point));
}



/** Return point on pair by its parameter in the range of [0.0, 1.0].
 *  With correct polarity by a_facePolarity.
 *  @return - a_t <= 0.0: tail
 *          - 0.0 < a_t < 1.0: interpolation between tail and head.
 *          - 1.0 <= a_t: head
 */
template <typename T>
Pair3D<T>::TPoint Pair3D<T>::point(TParam a_t, Polarity a_facePolarity) const
{
	typedef prim::Point3DH<T> TPointH;

	a_t = num::clamp(a_t, 0.0, 1.0);
    const TPointH p = (1.0 - a_t) * tail(a_facePolarity)->position() + 
                      a_t * head(a_facePolarity)->position();
    return p.affine();
}



/** Return point on pair by its parameter in the range of [0.0, 1.0].
 *  With correct polarity for a_cell. 
 *  @return - a_t <= 0.0: tail
 *          - 0.0 < a_t < 1.0: interpolation between tail and head.
 *          - 1.0 <= a_t: head
 */
template <typename T>
Pair3D<T>::TPoint Pair3D<T>::point(TParam a_t, const TCell* a_cell) const
{
    return point(a_t, facePolarity(a_cell));
}



/** Return point on pair by its parameter in the range of [0.0, 1.0].
 *  With correct polarity for a_point. 
 *  @return - a_t <= 0.0: tail
 *          - 0.0 < a_t < 1.0: interpolation between tail and head.
 *          - 1.0 <= a_t: head
 */
template <typename T>
Pair3D<T>::TPoint Pair3D<T>::point(TParam a_t, const TPoint& a_point) const
{
    return point(a_t, facePolarity(a_point));
}



/** flip the edge's polarity and counterClock/clock pairs
 */
template <typename T>
void Pair3D<T>::flip()
{
    m_pairPolarity = !m_pairPolarity;
    std::swap(m_clock, m_counterClock);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



}

}

}

#endif

// EOF
