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



/** @class lass::spat::world_3d::Preprocessor2D5
 *  @brief A preprocessor that creates a lass::spat::world_3d::World3D from 2.5D information.
 *  @author Bram de Greve --- BDG ---
 *  @date 2002-2004
 *
 *  This preprocessor can build a world from a heightmap and 2.5D obstacles.
 *  
 *  2.5D obstacles have a 2D floorplan and a height.  These obstacles are 
 *  provided by a 2D polygon and some handles containing information about the 
 *  obstacles: handles for the outside walls, inside walls, outside roof and
 *  inside roof.  The inside floor handle is obtained by using the map, as 
 *  for the floor altitude.
 *
 *  To create a world using this preprocessor, you must perform next steps:
 *  - you (re)initialize the preprocessor: init()
 *  - add obstacles to the world: addObstacle()
 *  - process the data and get back a World3_p to the built world: process()
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PREPROCESSOR_2D5_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PREPROCESSOR_2D5_H



// --- OLD INTERFACES ----------------------------------------------------------

#include "../spat_common.h"
#include "world_3d.h"
#include "../planar_mesh.h"
#include "../../prim/point_2d.h"
#include "../../prim/polygon_2d.h"
#include "../../prim/aabb_3d.h"
#include "../../util/callback_r_1.h>
#include "../../util/shared_ptr.h>

namespace lass
{
namespace spat
{
namespace world_3d
{

template <typename T, typename Handle>
class Preprocessor2D5
{
public:

	typedef Preprocessor2D5<T> TSelf;

	typedef typename util::CallTraits<T>::TValue TValue;
	typename Handle THandle;

	typedef World3D<T> TWorld;
	typedef util::SharedPtr<TWorld> TWorldPtr;

	typedef prim::Aabb3D<T> TAabb3D;
	typedef prim::Point2D<T> TPoint2D;
	typedef prim::Polygon2D<T> TPolygon2D;

	typedef util::CallbackR1<TValue, const TPoint2D&> CBGetMapHeight;
    typedef util::CallbackR1<THandle, const TPoint2D&> CBGetMapHandle;
    typedef util::CallbackR1<TValue, THandle> CBGetHandleHeight;

    void init(const CBGetMapHeight& a_getMapHeight,
              const CBGetMapHandle& a_getMapHandle,
              const CBGetHandleHeight& a_getHandleHeight,
              const TAabb3D& a_worldBoundary);

    void addObstacle(const TPolygon2D& a_obstacle, ComposedCellHandle* a_cellHandles);
	void addGroundSite( const TPoint2D& iPoint );

    TWorldPtr process();

private:

    // PRIVATE TYPEDEFS

	typedef PlanarMesh<

    typedef std::vector<Edge3*>             Edge3_pvector;
    typedef std::vector<Object3*>           Object3_pvector;
    typedef std::vector<Pair3*>             Pair3_pvector;
    typedef std::multimap<Real, Vertex3*>   Vertex3_pmap;
    typedef std::multimap<Real, Edge3*>     Edge3_pmap;


    // PRIVATE METHODS

    // the big ones
    void buildWorld(const Triangle2_pvector& a_triangles, 
                    const Edge2_pvector& a_edges);
    void buildCellsAndHorizontalFaces(const Triangle2_pvector& a_triangles);
    void buildVerticalFaces(const Edge2_pvector& a_edges);

    // adding stuff
    Face3* addFace(const Polygon3& a_poly, 
                   Cell3* a_frontCell, 
                   Cell3* a_backCell,
                   void* a_frontHandle,
                   void* a_backHandle);
    Pair3_pvector addPair(Face3* a_face, 
                          const Point3& a_tail, const Point3& a_head);
    Edge3_pvector addEdge(const Point3& a_tail, const Point3& a_head);
    Edge3_pvector addVerticalEdge(Vertex3*  a_tail, Vertex3*  a_head);
    Edge3* addHorizontalEdge(Vertex3*  a_tail, Vertex3*  a_head);
    Vertex3* addVertex(const Point3& a_position);

    // finding stuff
    Vertex3* findVertex(const Point3& a_position) const;


    // PRIVATE DATA:

    Triangulator* m_triangulator;
    CBGetMapHeight m_getMapHeight;
    CBGetMapHandle m_getMapHandle;
    CBGetHandleHeight m_getHandleHeight;
    AABB3 m_aabb;

    World3_p m_tempWorld;
    Vertex3_pmap m_tempVertices;
    Edge3_pmap m_tempEdges;
    Object3_pvector m_tempReceivers;
};



} // namespace bass3

#endif // guardian of inclusion

// --- END OF FILE preprocessor2d5.h ----------------------------------------
