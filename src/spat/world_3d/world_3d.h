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



/** @namespace lass::spat::world_3d
 *  @brief the templated version of the original @c World3 model in @c bass3
 *  @author Bram de Greve --- BDG ---
 *  @date 2002-2004
 *
 *  This is the templated (and slightly improved) version of the original @c World3 model 
 *  data structure of @c bass3 [1].  It includes the classes to represent the world model
 *  plus some extra algorithm objects to use on the model.
 *
 *  References:
 *  [1] DE GREVE B., <i>3D Polygonale Bundeltrekker voor Akoestische Simulaties</i>, thesis.
 */



/** @class lass::spat::world_3d::World3D
 *  @brief A 3D World
 *  @author Bram de Greve --- BDG ---
 *  @date 2002-2004
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_WORLD_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_WORLD_3D_H



// --- OLD INTERFACES ----------------------------------------------------------

#include "../spat_common.h"

#include "cell_3d.h"
#include "edge_3d.h"
#include "face_3d.h"
#include "object_3d.h"
#include "pair_3d.h"
#include "polarity.h"
#include "vertex_3d.h"
#include "world_3d_p.h"

#include "../../util/non_copyable.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

// old interfaces

class Cell3Merger;
//class MatlabOStream;



// new interfaces

template <typename T>
class World3D: public util::NonCopyable
{
public:

	typedef World3D<T> TSelf;

	typedef Cell3D<T> TCell;					
	typedef Edge3D<T> TEdge;
	typedef Face3D<T> TFace;
	typedef Pair3D<T> TPair;
	typedef Object3D<T> TObject;
	typedef Vertex3D<T> TVertex;
	typedef World3D<T> TWorld;					
	typedef void* THandle;

	typedef prim::Plane3D<T> TPlane;
	typedef prim::Point3D<T> TPoint;
    typedef prim::Point3DH<T> TPointH;
	typedef prim::Vector3D<T> TVector;

	typedef util::CallTraits<T>::TValue TValue;
	typedef util::CallTraits<T>::TParam TParam;

    typedef std::vector<TEdge*>     TEdges;
    typedef std::vector<TCell*>     TCells;
    typedef std::vector<TVertex*>   TVertices;
    typedef std::vector<TObject*>   TObjects;

    friend Cell3Merger;


    // STRUCTORS

    ~World3D();


    // METHODS

    TEdges::iterator beginEdge();
    TEdges::iterator endEdge();
    TCells::iterator beginCell();
    TCells::iterator endCell();
    TEdges::const_iterator beginEdge() const;
    TEdges::const_iterator endEdge() const;
    TCells::const_iterator beginCell() const;
    TCells::const_iterator endCell() const;

    TVertex* newVertex(const TPoint& a_position);
    TEdge* newEdge(TVertex* a_tail, TVertex* a_head);
    TPair* newPair(TFace* a_face, TEdge* a_edge, Polarity a_edgePolarity);
    TFace* newFace(const TPlane& a_plane, TCell* a_frontCell, TCell* a_backCell, 
                   THandle a_frontHandle,THandle a_backHandle, const TPoint& a_barycentre);
    TCell* newCell(THandle a_cellMedium = 0);
    TObject* newObject(const TPoint& a_position, THandle a_handle, TParam a_spacing = 0);
    void deleteObject(TObject* a_object);
	void moveObject(TObject*& a_object, const TPoint& a_newPosition, TParam a_spacing = 0);

    void sortPairs();
    void eraseObsolete();

    unsigned sizeVertices() const;
    unsigned sizeEdges() const;
    unsigned sizePairs() const;
    unsigned sizeFaces() const;
    unsigned sizeCells() const;
    unsigned sizeObjects() const;
    std::string size() const;

    void exportToMatlab(const std::string& a_filename, 
                        bool a_wireframe = true,
                        bool a_filled = false,
                        bool a_noblack = true);

private:
  
    TVertices m_vertices;
    TEdges m_edges;
    TCells m_cells;
    TObjects m_objects;

    friend World3D_p;
    unsigned m_instances;               
};


// FREE OPERATORS

//MatlabOStream& operator<<(MatlabOStream& a_os, World3D_p a_world);
//MatlabOStream& operator<<(MatlabOStream& a_os, World3D* a_world);



} 

}

}

#endif // guardian of inclusion

// --- END OF FILE world_3d.h --------------------------------------------------
