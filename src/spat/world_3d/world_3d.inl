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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_WORLD_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_WORLD_3D_INL

#include "world_3d.h"



namespace lass
{
namespace spat
{
namespace world_3d
{

// --- public --------------------------------------------------------------------------------------

/** Destruct the world and all it's content (vertices, edges, faces, cells, ...)
 */
template <typename T>
World3D<T>::~World3D()
{
	LASS_LOG("global destruction ...");

    // free all dynamicly allocated objects of the world
    //
    // The destructors of the cells will destroy all faces and pairs.
    // Delete cells before the edges, since the destructors of cells need the
    // edges to get rid of their faces (well, actually the pairs of their faces)
    //
    // Delete edges before the vertices, although the destructors don't need
    // the vertices to get rid of their stuff (the edges don't have a 
    // destructor anyway :), but I feel a lot safer this way.  You can never
    // know what the future brings, can you? :)

    typename TCells::iterator cit;
    for (cit = m_cells.begin(); cit != m_cells.end(); ++cit)
    {
        delete *cit;
    }

    typename TEdges::iterator eit;
    for (eit = m_edges.begin(); eit != m_edges.end(); ++eit)
    {
        delete *eit;
    }
 
    typename TVertices::iterator vit;
    for (vit = m_vertices.begin(); vit != m_vertices.end(); ++vit)
    {
        delete *vit;
    }

    typename TObjects::iterator oit;
    for (oit = m_objects.begin(); oit != m_objects.end(); ++oit)
    {
        delete *oit;
    }
}



// --- METHODS -----------------------------------------------------------------

/** Return iterator to first edge of world.
 */
template <typename T>
typename World3D<T>::TEdges::iterator World3D<T>::beginEdge()
{
    return m_edges.begin();
}



/** Return iterator to one past last edge of world.
 */
typename World3D<T>::TEdges::iterator World3D<T>::endEdge()
{
    return m_edges.end();
}



/** Return iterator to first cell of world.
 */
typename World3D<T>::TCells::iterator World3D<T>::beginCell()
{
    return m_cells.begin();
}



/** Return iterator to one past last cell of world.
 */
typename World3D<T>::TCells::iterator World3D<T>::endCell()
{
    return m_cells.end();
}



/** Return const_iterator to first edge of world.
 */
typename World3D<T>::TEdges::const_iterator World3D<T>::beginEdge() const
{
    return m_edges.begin();
}



/** Return const_iterator to one past last edge of world.
 */
typename World3D<T>::TEdges::const_iterator World3D<T>::endEdge() const
{
    return m_edges.end();
}



/** Return const_iterator to first cell of world.
 */
typename World3D<T>::TCells::const_iterator World3D<T>::beginCell() const
{
    return m_cells.begin();
}



/** Return const_iterator to one past last cell of world.
 */
typename World3D<T>::TCells::const_iterator World3D<T>::endCell() const
{
    return m_cells.end();
}



/** Allocate a new vertex, add to world and return it
 */
World3D<T>::TVertex* World3D<T>::newVertex(const TPoint& a_position)
{
	std::auto_ptr<TVertex> vertex(new TVertex(a_position));
    m_vertices.push_back(vertex.get());
    return vertex.release();
}



/** Allocate a new edge, add to world and return it
 */
World3D<T>::TEdge* World3D<T>::newEdge(TVertex* a_tail, TVertex* a_head)
{
	std::auto_ptr<TEdge> edge(new TEdge(a_tail, a_head));
    m_edges.push_back(edge.get());
    return edge.release();
}



/** Allocate a new pair, add to world and return it
 */
World3D<T>::TPair* World3D<T>::newPair(TFace* a_face, TEdge* a_edge, Polarity a_edgePolarity)
{
#pragma LASS_TODO("how owns this?")
	return new TPair(a_face, a_edge, a_edgePolarity);
}



/** Allocate a new face, add to world and return it
 */
World3D<T>::TFace* World3D<T>::newFace(const TPlane& a_plane, 
                                       TCell* a_frontCell, 
                                       TCell* a_backCell, 
                                       THandle a_frontHandle, 
                                       THandle a_backHandle,
                                       const TPoint& a_barycenter)
{
#pragma LASS_TODO("how owns this?")
    return new TFace(a_plane, a_frontCell, a_backCell, a_frontHandle, a_backHandle);
}



/** Allocate a new cell, add to world and return it
 */
World3D<T>::TCell* World3D<T>::newCell(THandle a_cellMedium)
{
	std::auto_ptr<TCell> cell(new TCell(this, a_cellMedium));
    m_cells.push_back(cell.get());
    return cell.release();
}



/** Allocate a new receiver object, add to world and return it.
 */
World3D<T>::TObject* World3D<T>::newObject(const TPoint& a_position, 
                                           THandle a_handle,
                                           TParam a_spacing)
{
    static Cell3Finder cellFinder(this);
    cellFinder.setSpacing(a_spacing);
    TCell* cell = cellFinder.findCell(a_position);

	std::auto_ptr<TObject> object(new TObject(a_position, a_handle));
    m_objects.push_back(object);
    cell->addObject(object.release());

    return m_objects.back();
}



/** Erase and deallocate object.
 */
void World3D<T>::deleteObject(TObject* a_object)
{
    static Cell3Finder cellFinder(this);

	// try to find the cell in which the object is
	//
	TCell* cell = 0;
	bool planAsucceeded = false;
	try
	{
		cell = cellFinder.findCell(a_object->position());
		if (cell)
		{
			// if you could find the cell, then hopefully the iterator is in there.  try to 
			// remove the object.  If you can't, you'll have to change to plan B.
			planAsucceeded = cell->removeObject(a_object);
		}
	}
	catch(...)
	{
	}


	bool planBsucceeded = false;
	if (!planAsucceeded)
	{
		// plan B.  try all cells until you find it and erase it.
		LASS_LOG("Plan B: Cell3Finder failed, so trying all cells now ... slooooooow :(");
		TCells::iterator cit;
		for (cit = m_cells.begin(); cit != m_cells.end(); ++cit)
		{
			cell = *cit;
			planBsucceeded = cell->removeObject(a_object);
			if (planBsucceeded)
			{
				break;
			}
		}
	}

	if (!planBsucceeded)
	{
		// OK, now something is REALLY screwed up!   well ... hihi, forget it.  we had to 
		// remove it, but it's not there at all! ...  so, that's exactly what we wanted!
		// Alright!  Anyway, just tell the folks we've spent a hell of a lot time to find out
		// the object wasn't in any cell at all.
		LASS_LOG("We've tried both plan A and plan B to find the cell in which the object is, "
			"but none succeeded.  So now forget about it.  You wanted to get rid of it?  Well, "
			"be glad, because it's nver been there.  Just carry on with what you're doing, dude!");
	}
		
	m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), a_object), m_objects.end());
	delete a_object;
}



/** move object to a new position and to a new cell if necessary.
 *  @warning the address of the object will not be the same!  that's why you have to give a non
 *           constant reference to the pointer of the object, so this method can change it.
 */
void World3D<T>::moveObject(TObject*& a_object, const TPoint& a_newPosition, Real a_spacing)
{
	void* handle = a_object->handle();
	deleteObject(a_object);
	a_object = newObject(a_newPosition, handle, a_spacing);
}



/** Sort pairs around their edge so that they can be spinned 
 *  @warning never spin pairs before you did this.
 */
void World3D<T>::sortPairs()
{
    BASS3_HEADER("lass::spat::world_3d::World3D<T>::sortPairs");

    std::vector<TEdge*>::iterator eit;
    for (eit = m_edges.begin(); eit != m_edges.end(); ++eit)
    {
        (*eit)->sortPairs();
    }
}



/** Clean up obsolete information in cell.
 */
void World3D<T>::eraseObsolete()
{  
    // obsolete edges
    TEdges tempEdges;
    TEdges::iterator eit;
    for (eit = m_edges.begin(); eit != m_edges.end(); ++eit)
    {
        TEdge* edge = *eit;
        if (edge->sizePairs() == 0)
        {
            delete edge;
        }
        else
        {
            tempEdges.push_back(edge);
        }
    }
    std::swap(m_edges, tempEdges);

    // obsolete cells
    TCells tempCells;
    TCells::iterator cit;
    for (cit = m_cells.begin(); cit != m_cells.end(); ++cit)
    {
        TCell* cell = *cit;
        if (cell->sizeFaces() == 0)
        {
            delete cell;
        }
        else
        {
            cell->eraseObsolete();
            tempCells.push_back(cell);
        }
    }
    std::swap(m_cells, tempCells);
}

           


/** return number of vertices
 */
unsigned World3D<T>::sizeVertices() const
{
    return m_vertices.size();
}



/** return number of edges
 */
unsigned World3D<T>::sizeEdges() const
{
    return m_edges.size();
}



/** return number of vertices
 */
unsigned World3D<T>::sizePairs() const
{
    unsigned result = 0;

    std::vector<TEdge*>::const_iterator eit;
    for (eit = m_edges.begin(); eit != m_edges.end(); ++eit)
    {
        TEdge* edge = *eit;
        result += edge->sizePairs();
    }

    return result;
}



/** return number of vertices
 */
unsigned World3D<T>::sizeFaces() const
{
    unsigned result = 0;

    std::vector<TCell*>::const_iterator cit;
    for (cit = m_cells.begin(); cit != m_cells.end(); ++cit)
    {
        TCell* cell = *cit;
        result += cell->sizeFaces();
    }

    return result;
}



/** return number of cells
 */
unsigned World3D<T>::sizeCells() const
{
    return m_cells.size();
}



/** return number of objects in world
 */
unsigned World3D<T>::sizeObjects() const
{
    unsigned result = 0;

    TCells::const_iterator cit;
    for (cit = m_cells.begin(); cit != m_cells.end(); ++cit)
    {
        TCell* cell = *cit;
        result += cell->sizeObjects();
    }

    return result;
}



/** return a string with statistics of world.
 *  - #V: number of vertices.
 *  - #E: number of edges.
 *  - #F: number of faces.
 *  - #P: number of pairs.
 *  - #C: number of cells.
 *  - #O: number of objects.
 */
std::string World3D<T>::size() const
{
    std::ostringstream s;
    s << "#V=" << sizeVertices();
    s << ", #E=" << sizeEdges();
    s << ", #P=" << sizePairs();
    s << ", #F=" << sizeFaces();
    s << ", #C=" << sizeCells();
    s << ", #O=" << sizeObjects();
    return s.str();
}


/** export world to m file to be viewed in matlab
 */
void World3D<T>::exportToMatlab(const std::string& a_filename, 
                            bool a_wireframe,
                            bool a_filled,
                            bool a_noblack)
{
    BASS3_HEADER("lass::spat::world_3d::World3D<T>::exportToMatlab: " + a_filename);

    // open file for output and empty it
    MatlabOStream m(a_filename, std::ios_base::trunc | std::ios_base::out);
    m << MatlabOStream::Flag("world3_wireframe", a_wireframe);
    m << MatlabOStream::Flag("world3_filled", a_filled);
    m << MatlabOStream::Flag("world3_noblack", a_noblack);
    m << this;
}



// --- FREE OPERATORS ----------------------------------------------------------

MatlabOStream& operator<<(MatlabOStream& a_os, World3_p a_world)
{
    a_os << a_world.get();
    return a_os;
}



MatlabOStream& operator<<(MatlabOStream& a_os, World3* a_world)
{
    const bool noBlack = a_os.flag("world3_noblack", true);

    if (a_os.flag("world3_filled", true))
    {
        a_os << "% --- faces ---\n";
        a_os << MatlabOStream::csRed;
        
        World3D<T>::TCells::const_iterator cit;
        for (cit = a_world->beginCell(); cit != a_world->endCell(); ++cit)
        {
            TCell* cell = *cit;

            TCell::Face3_pvector::const_iterator fit;
            for (fit = cell->beginFace(); fit != cell->endFace(); ++fit)
            {
                TFace* face = *fit;
                if (face->frontCell() != cell)
                {
                    // only draw the faces of which this cell is the front cell
                    // that way, we'll draw each face only once.  Don't worry,
                    // each face has at least a front cell.                    
                    continue;
                }

                if (!face->frontHandle() && !face->backHandle())
                {
                    // don't draw faces that don't have handles
                    continue;
                }
                else
                {
                    if (face->black())
                    {
                        if (noBlack)
                        {
                            // don't draw black faces if you don't want to.
                            continue;
                        }
                        else
                        {
                            a_os << MatlabOStream::csGreen;
                        }
                    }
                    else
                    {
                        a_os << MatlabOStream::csRed;
                    }
                }

                // draw face
                Polygon3 poly;
                Pair3CounterClockIterator pit;
                for (pit.reset(face->pair(), cell); !pit.end(); ++pit)
                {
                    TPoint p = pit->tail(cell)->position();
                    poly.addVertex(p);
                }

                a_os << poly;
            }
        }
    }

    if (a_os.flag("world3_wireframe", true))
    {
        a_os << "% --- edges ---\n";
        a_os << MatlabOStream::csBlue;

        World3D<T>::TEdges::const_iterator eit;
        for (eit = a_world->beginEdge(); eit != a_world->endEdge(); ++eit)
        {
            LineSeg3 seg((*eit)->head()->position(),
                         (*eit)->tail()->position());
            a_os << seg;
        }
    }

    return a_os;
}



} // namespace bass3

// --- END OF FILE world3.cpp --------------------------------------------------
