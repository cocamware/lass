/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#include "bass3_common.h"
#include "preprocessor2d5.h"

#include BASS3_NO_MEMORY_DEBUGGER
#include <stdlib.h>
#include BASS3_MEMORY_DEBUGGER

#include "point2h.h"
#include "edge2.h"
#include "triangle2.h"



namespace bass3
{

// --- METHODS -----------------------------------------------------------------

/** (Re)Initialize the preprocessor for building a world.
 *  @warning you should reinit the preprocessor each time you build a world.
 *           Always follow these steps: init > add obstacles > process.
 *  @param a_triangulator An instance of triangulation strategy so that the
 *                        preprocessor can triangulate :)  THE PREPROCESSOR
 *                        DOES @b NOT TAKE OWNERSHIP OF THE TRIANGULATOR.
 *                        The triangulator is @b not destructed after a
 *                        process(), but it is reset.
 *  @param a_getMapHeight A callback with what the preprocessor can determine
 *                        the street/floor-level of the world.  It must always
 *                        return the correct value for each (x,y) point within
 *                        the boundaries of the world, and the result (z) must
 *                        be within the boundaries of the world.
 *  @param a_getMapHandle A callback with what the preprocessor can determine
 *                        the street/floor-handle.  Must always return a handle
 *                        for each (x,yà point within the boundaries of the
 *                        world.  NO NULL HANDLE AS RESULT IS ALLOWED.
 *  @param a_getHandleHeight A callback with what the preprocessor can determine the
 *                    altitude of the roof of an obstacle.  Therefore, the
 *                    preprocessor will provide the cellHandle of the obstacle.
 *  @param a_worldBoundary The bounds of the world by an 3D AABB.  Horizontally
 *                         it must be larger then the convex hull of all
 *                         obstacles.  Vertically, its minimum must be lower
 *                         than lowest street/floor level, and its maximum must
 *                         be bigger than highest street/floor level or obstacle
 */
void Preprocessor2D5::init(const CBGetMapHeight& a_getMapHeight,
						   const CBGetMapHandle& a_getMapHandle,
						   const CBGetHandleHeight& a_getHandleHeight,
						   const TAabb3D& a_worldBoundary)
{
	m_getMapHeight = a_getMapHeight;
	m_getMapHandle = a_getMapHandle;
	m_getHandleHeight = a_getHandleHeight;
	m_aabb = a_worldBoundary;

	mmesh_.reset(new PlanarMesh(TAabb2D(TPoint2D(m_aabb.min().x, m_aabb.min().y),
										TPoint2D(m_aabb.max().x, m_aabb.max().y))));
}



/** add an obstacle defined by a 2D polygon and its handles
 *  @param a_obstacle 2D projection of obstacle
 *  @param a_cellHandles properties of obstacle.  The Preprocessor does NOT take ownership of
 *                       these cellHandles, but you must keep the alive as long as the preprocessor.
 */
void Preprocessor2D5::addObstacle(const Polygon2& a_obstacle,
								  ComposedCellHandle* a_cellHandles)
{
	LASS_ASSERT(mesh_);
	mesh_->insertPolygon(a_obstacle, a_cellHandles);
}


void Preprocessor2D5::addGroundSite( const Point2& iPoint )
{
	m_triangulator->insertVertex( iPoint );
}

World3_p Preprocessor2D5::process()
{
	BASS3_HEADER("bass3::Preprocessor2D5::process ...");

	LASS_ASSERT(m_triangulator);
	LASS_ASSERT(!m_getMapHeight.isEmpty());
	LASS_ASSERT(!m_getMapHandle.isEmpty());
	LASS_ASSERT(!m_getHandleHeight.isEmpty());

	// step 1: build delaunayMesh
	m_triangulator->build();
	m_triangulator->exportToMatlab("triangulator.m");

	// step 2: build World from delaunayMesh
	buildWorld(m_triangulator->triangles(), m_triangulator->edges());
	m_tempWorld->sortPairs();

	// step 3: clear the temporary maps (without deleting the pointed meat!)
	//         and shut down triangulator
	m_tempVertices.clear();
	m_tempEdges.clear();
	m_triangulator->reset();

	return m_tempWorld;
}



// --- PRIVATE METHODS ---------------------------------------------------------

/** build 3D world from planar structure.
 *  The big one :)
 */
void Preprocessor2D5::buildWorld(const Triangle2_pvector& a_triangles,
								 const Edge2_pvector& a_edges)
{
	BASS3_HEADER("bass3::Preprocessor2D5::buildWorld ...");

	m_tempWorld.reset(new World3);
	buildCellsAndHorizontalFaces(a_triangles);
	buildVerticalFaces(a_edges);
}



/** Create all cells and create their bottom and top faces (floors and ceilings)
 */
void Preprocessor2D5::buildCellsAndHorizontalFaces(const Triangle2_pvector&
												   a_triangles)
{
	BASS3_HEADER("bass3::Preprocessor2D5::buildCellsAndHorizontalFaces");

	const Vector3 k(0.0, 0.0, 1.0);
	const Real zBottom = m_aabb.min().z;
	const Real zTop = m_aabb.max().z;
	void* blackHandle = Face3::blackHandle();

	Triangle2_pvector::const_iterator tit;
	for (tit = a_triangles.begin(); tit != a_triangles.end(); ++tit)
	{
		Triangle2* triangle = *tit;

		Cell3* cell = m_tempWorld->newCell();
		triangle->setTopCell(cell);
		Point2 a2 = triangle->vertex(0);
		Point2 b2 = triangle->vertex(1);
		Point2 c2 = triangle->vertex(2);

		// create ceiling of topcell, normal downwards
		Point3 a3 = Point3(a2, zTop);
		Point3 b3 = Point3(b2, zTop);
		Point3 c3 = Point3(c2, zTop);
		Polygon3 poly(a3, b3, c3);
		poly.turnAs(-k);
		poly.setPlane(Plane3(-k, zTop)); // explicit
		addFace(poly, cell, 0, blackHandle, blackHandle);

		// if there is a middlecell, create roof, normal upwards
		if (triangle->cellHandles())
		{
			ComposedCellHandle* cellHandles = triangle->cellHandles();
			void* hCellMedium = cellHandles->cellMedium();
			void* hWallOutside = cellHandles->wallOutside();
			void* hRoofOutside = cellHandles->roofOutside();
			void* hRoofInside = cellHandles->roofInside();

			Cell3* middleCell = m_tempWorld->newCell(hCellMedium);
			triangle->setMiddleCell(middleCell);

			const Real zRoof = m_getHandleHeight(hWallOutside);
			a3.z = zRoof;
			b3.z = zRoof;
			c3.z = zRoof;
			poly.resetVertices(a3, b3, c3);
			poly.turnAs(k);
			poly.setPlane(Plane3(k, -zRoof)); // explicit
			addFace(poly, cell, middleCell, hRoofOutside, hRoofInside);

			// next operation will be done on middle cell and bottom cell
			// instead of top and bottom cell.
			cell = middleCell;
		}

		// next face is on street level.  between top/middle cell and botomcell,
		// normal upwards.
		Cell3* bottomCell = m_tempWorld->newCell();
		triangle->setBottomCell(bottomCell);
		a3.z = m_getMapHeight(a2);
		b3.z = m_getMapHeight(b2);
		c3.z = m_getMapHeight(c2);
		poly.resetVertices(a3, b3, c3);
		if (a3.z == b3.z && b3.z == c3.z)
		{
			poly.setPlane(Plane3(k, -a3.z));
		}
		poly.turnAs(k);
		Point2h bary = a2 + b2 + c2;
		void* streetHandle = m_getMapHandle(bary.affine());
		addFace(poly, cell, bottomCell, streetHandle, blackHandle);

		// bottom face, normal upwards
		a3.z = zBottom;
		b3.z = zBottom;
		c3.z = zBottom;
		poly.resetVertices(a3, b3, c3);
		poly.turnAs(k);
		poly.setPlane(Plane3(k, -zBottom)); // explcit
		addFace(poly, bottomCell, 0, blackHandle, blackHandle);
	}
}



/** Step2: Create all vertical faces.
 */
void Preprocessor2D5::buildVerticalFaces(const Edge2_pvector& a_edges)
{
	BASS3_HEADER("bass3::Preprocessor2D5::buidVerticalFaces");

	// now, we add all vertical faces.  Each face can consist of 1 to 4 parts.
	// this depends on the fact if the adjacent cells have middlecells.  There
	// are six possible situations as shown below.  We fill in information
	// of these parts in an array, and then we'll use that array to insert the
	// necessary faces in the world.
	//
	// SITUATIONS:
	//  front | back
	//
	//  1.        2.        3.        4.        5.        6.
	//  ---+---   ---+---   ---+---   ---+---   ---+---   ---+---
	//     |         |[3]      |[3]      |         |         |
	//     |[2]   ---+         +---      |[2]      |[2]      |
	//  ---+---      |[2]      |[2]   ---+         +---      |
	//     |         +---   ---+         |         |         |
	//     |[1]      |[1]      |[1]      |[1]      |[1]      |[1]
	//  ---+---   ---+---   ---+---   ---+---   ---+---   ---+---
	//     |[0]      |[0]      |[0]      |[0]      |[0]      |[0]
	//  ---+---   ---+---   ---+---   ---+---   ---+---   ---+---
	// numParts:
	//     3         4         4         3         3         2

	const Real zBottom = m_aabb.min().z;
	const Real zTop = m_aabb.max().z;
	void* blackHandle = Face3::blackHandle();

	Edge2_pvector::const_iterator eit;
	for (eit = a_edges.begin(); eit != a_edges.end(); ++ eit)
	{
		const unsigned maxParts = 4;
		Real partTailZ[maxParts + 1];       // bottom z level at tailXY of part
		Real partHeadZ[maxParts + 1];       // bottom z level at headXY of part
		void* partFrontHandle[maxParts];
		void* partBackHandle[maxParts];
		Cell3* partFrontCell[maxParts];
		Cell3* partBackCell[maxParts];

		Edge2* edge = *eit;

		// get information of face.
		const Point2 tailXY = edge->tail();
		const Point2 headXY = edge->head();
		const Real tailZ = m_getMapHeight(tailXY);
		const Real headZ = m_getMapHeight(headXY);
		LASS_ASSERT(tailZ > zBottom && tailZ < zTop);
		LASS_ASSERT(headZ > zBottom && headZ < zTop);

		// get information of front cells
		Triangle2* triangle = edge->frontTriangle();
		LASS_ASSERT(triangle);
		ComposedCellHandle* frontHandles = triangle->cellHandles();
		void* frontOutside = frontHandles ? frontHandles->wallOutside() : 0;
		void* frontInside = frontHandles ? frontHandles->wallInside() : 0;
		const Real frontZ = frontHandles ? m_getHandleHeight(frontOutside) :0.0;
		Cell3* const frontTop = triangle->topCell();
		Cell3* const frontMiddle = triangle->middleCell();
		Cell3* const frontBottom = triangle->bottomCell();

		// back cells
		triangle = edge->backTriangle();
		ComposedCellHandle* backHandles = triangle ? triangle->cellHandles() :0;
		void* backOutside = backHandles ? backHandles->wallOutside() : 0;
		void* backInside = backHandles ? backHandles->wallInside() : 0;
		const Real backZ = backHandles ? m_getHandleHeight(backOutside) : 0.0;
		Cell3* const backTop = triangle ? triangle->topCell() : 0;
		Cell3* const backMiddle = triangle ? triangle->middleCell() : 0;
		Cell3* const backBottom = triangle ? triangle->bottomCell() : 0;

		// there's a correction to be made.  If both sides have same cell-
		// handles, then face should be immaterial.  no handles here
		if (frontHandles == backHandles)
		{
			frontOutside = 0;
			frontInside = 0;
			backOutside = 0;
			backInside = 0;
		}

		void* airHandle = triangle ? 0 : blackHandle;
		if (!triangle)
		{
			frontOutside = blackHandle;
			frontInside = blackHandle;
			backOutside = blackHandle;
			backInside = blackHandle;
		}

		// fill the arrays with information about each part of the face.
		// there are six possible situations as described above.
		unsigned numParts;

		// - part between bottomcells
		partTailZ[0] = zBottom;
		partHeadZ[0] = zBottom;
		partFrontHandle[0] = airHandle;
		partBackHandle[0] = airHandle;
		partFrontCell[0] = frontBottom;
		partBackCell[0] = backBottom;

		partTailZ[1] = tailZ;
		partHeadZ[1] = headZ;

		// - parts between top/middlecells
		if (frontMiddle && backMiddle)
		{
			// situation 1, 2 or 3
			partFrontHandle[1] = frontInside;
			partBackHandle[1] = backInside;
			partFrontCell[1] = frontMiddle;
			partBackCell[1] = backMiddle;

			if (frontZ == backZ)
			{
				// situation 1
				numParts = 3;
				partTailZ[2] = frontZ;
				partHeadZ[2] = frontZ;
			}
			else
			{
				// situation 2 or 3
				numParts = 4;

				if (frontZ > backZ)
				{
					// situation 2
					partTailZ[2] = backZ;
					partHeadZ[2] = backZ;
					partFrontHandle[2] = frontInside;
					partBackHandle[2] = frontOutside;
					partFrontCell[2] = frontMiddle;
					partBackCell[2] = backTop;

					partTailZ[3] = frontZ;
					partHeadZ[3] = frontZ;
				}
				else
				{
					// situation 3
					partTailZ[2] = frontZ;
					partHeadZ[2] = frontZ;
					partFrontHandle[2] = backOutside;
					partBackHandle[2] = backInside;
					partFrontCell[2] = frontTop;
					partBackCell[2] = backMiddle;

					partTailZ[3] = backZ;
					partHeadZ[3] = backZ;
				}
			}
		}
		else if (frontMiddle)
		{
			// situation 4
			numParts = 3;
			partFrontHandle[1] = frontInside;
			partBackHandle[1] = frontOutside;
			partFrontCell[1] = frontMiddle;
			partBackCell[1] = backTop;

			partTailZ[2] = frontZ;
			partHeadZ[2] = frontZ;
	   }
		else if (backMiddle)
		{
			// situation 5
			numParts = 3;
			partFrontHandle[1] = backOutside;
			partBackHandle[1] = backInside;
			partFrontCell[1] = frontTop;
			partBackCell[1] = backMiddle;

			partTailZ[2] = backZ;
			partHeadZ[2] = backZ;
	   }
		else
		{
			// situation 6
			numParts = 2;
		}

		partFrontHandle[numParts - 1] = airHandle;
		partBackHandle[numParts - 1] = airHandle;
		partFrontCell[numParts - 1] = frontTop;
		partBackCell[numParts - 1] = backTop;

		partTailZ[numParts] = zTop;
		partHeadZ[numParts] = zTop;

		// all parts are coplanar and vertical.  use this info to set plane
		// explicit.
		const Vector2 directionXY = headXY - tailXY;
		const Vector3 normal(-directionXY.y, directionXY.x, 0.0);
		const Plane3 plane(normal, Point3(tailXY, 0.0));

		// insert the parts into the world.
		for (unsigned i = 0; i < numParts; ++i)
		{
			Polygon3 poly(Point3(tailXY, partTailZ[i + 1]),
						  Point3(headXY, partHeadZ[i + 1]),
						  Point3(headXY, partHeadZ[i]),
						  Point3(tailXY, partTailZ[i]));
			poly.setPlane(plane);
			addFace(poly, partFrontCell[i], partBackCell[i],
					partFrontHandle[i], partBackHandle[i]);
		}
	}
}



/** Add to the world, a face between two cells.  A face must
 *  at least have a front cell, but a backCell may be ommited.
 *  @warning the normal of the face is supposed to stick into the frontcell.
 *           So make sure the polygon is correct ordered.
 */
Face3* Preprocessor2D5::addFace(const Polygon3& a_poly,
								Cell3* a_frontCell,
								Cell3* a_backCell,
								void* a_frontHandle,
								void* a_backHandle)
{
	LASS_ASSERT(a_frontCell);

	Face3* face = m_tempWorld->newFace(a_poly.plane(), a_frontCell, a_backCell,
									   a_frontHandle, a_backHandle,
									   a_poly.barycenter());

	// create for each edge of the polygon, a Pair3 with an Edge3.
	LASS_ASSERT(!face->pair());
	LASS_ASSERT(!a_poly.empty());
	Pair3* previous = 0;
	for (unsigned i = 0; i < a_poly.size(); ++i)
	{
		const Point3 tail = a_poly.vertex(i);
		const Point3 head = a_poly.vertex(i + 1);
		Pair3_pvector pairs = addPair(face, tail, head);

		// link pairs around face
		Pair3_pvector::iterator pit;
		for (pit = pairs.begin(); pit != pairs.end(); ++pit)
		{
			Pair3* pair = *pit;

			if (!face->pair())
			{
				face->setPair(pair);
			}
			else
			{
				previous->setCounterClock(pair);
				pair->setClock(previous);
			}
			previous = pair;
		}
	}

	// the last one (now available in previous) is then again linked to
	// the first one.
	previous->setCounterClock(face->pair());
	face->pair()->setClock(previous);

	return face;
}



/** Add to the world, a pair of a face and edge.  Sometimes (in fact, for
 *  vertical edges) an edge can be splitted up when it is created.  This is,
 *  when there are vertices found that lay on the edge.  In that case we get
 *  a set of edges and we must create a set of pairs.
 *  See addEdge for more details
 *  @warning addEdge assumes all vertices are already in the world,
 *           i.e. no more vertices will be added.
 */
Preprocessor2D5::Pair3_pvector Preprocessor2D5::addPair(Face3* a_face,
														const Point3& a_tail,
														const Point3& a_head)
{
	LASS_ASSERT(a_face->frontCell());

	const Vector3 dirPair = a_head - a_tail;

	Edge3_pvector edges = addEdge(a_tail, a_head);
	Pair3_pvector pairs;

	Edge3_pvector::iterator eit;
	for (eit = edges.begin(); eit != edges.end(); ++eit)
	{
		Edge3* edge = *eit;
		const Vector3 dirEdge = edge->vector();
		const Polarity edgePolarity = dot(dirPair, dirEdge) > 0.0 ?
									  Positive : Negative;
		Pair3* pair = m_tempWorld->newPair(a_face, edge, edgePolarity);
		pairs.push_back(pair);
	}

	return pairs;
}



/** Add to the world, an edge between tail and head.  Search for existing edges.
 *  if no exists, create a new ones.  For vertical edges, if vertices are found
 *  that lay on the edge, then the edge is there splitted, and we get a set of
 *  edges.
 *  @warning it is assumed that
 *           - not-vertical edges can never be splitted
 *           - all vertices are already present in the world when vertical edges
 *             are added, i.e. no vertices must still be added.
 */
Preprocessor2D5::Edge3_pvector Preprocessor2D5::addEdge(const Point3& a_tail,
														const Point3& a_head)
{
	Vertex3* tail = addVertex(a_tail);
	Vertex3* head = addVertex(a_head);
	Edge3_pvector edges;

	if (a_tail.x == a_head.x && a_tail.y == a_head.y)
	{
		return addVerticalEdge(tail, head);
	}
	else
	{
		Edge3_pvector edge;
		edge.push_back(addHorizontalEdge(tail, head));
		return edge;
	}
}



/** Add a vertical edge(s).  Add edges from tail to head, i.e. start adding
 *  edges at tail (in the direction of the head) until you reach the head.
 */
Preprocessor2D5::Edge3_pvector Preprocessor2D5::addVerticalEdge(Vertex3* a_tail,
																Vertex3* a_head)
{
	typedef Edge3_pmap::iterator Eit;
	typedef Vertex3_pmap::iterator Vit;

	// notice that all relevant edges will have the same keyvalue x, since
	// they are all vertical
	const Real xTail = a_tail->position().x;
	const Real xHead = a_head->position().x;
	const Real xKey = xTail < xHead ? xTail : xHead;
	std::pair<Eit, Eit> eRange = m_tempEdges.equal_range(xKey);
	std::pair<Vit, Vit> vRange = m_tempVertices.equal_range(xKey);

	Edge3_pvector edges;
	Vertex3* start = a_tail;
	while (start != a_head)
	{
		// search for a good existing edge
		const LineSeg3 lineSeg(start->position(), a_head->position());
		bool found = false;
		for (Eit eit = eRange.first; eit != eRange.second && !found; ++eit)
		{
			Edge3* candidate = eit->second;
			if (candidate->tail() == start &&
				lineSeg.contains(candidate->head()->position()))
			{
				edges.push_back(candidate);
				start = candidate->head();
				found = true;
			}
			else if (candidate->head() == start &&
					 lineSeg.contains(candidate->tail()->position()))
			{
				edges.push_back(candidate);
				start = candidate->tail();
				found = true;
			}
		}

		if (!found)
		{
			// create new edge to closest vertex
			Real minDistance = a_head->position().z - start->position().z;
			bool negative = false;
			if (minDistance < 0.0)
			{
				negative = true;
				minDistance = -minDistance;
			}

			Vertex3* closestVertex = a_head;
			for (Vit vit = vRange.first; vit != vRange.second; ++vit)
			{
				const Point3& point = vit->second->position();

				// point must be on same vertical line, x is equal already.
				if (point.y == start->position().y)
				{
					// and it must lay on the linesegment, as close
					// as possible to the start.
					Real distance = point.z - start->position().z;
					if (negative)
					{
						distance = -distance;
					}

					if (distance > 0.0 && distance < minDistance)
					{
						minDistance = distance;
						closestVertex = vit->second;
					}
				}
			}

			Edge3* edge = m_tempWorld->newEdge(start, closestVertex);
			m_tempEdges.insert(std::make_pair(xKey, edge));
			edges.push_back(edge);

			start = closestVertex;
		}
	}

	return edges;
}



/** Add a horizontal edge.  A horizontal edge can never exists as the "sum"
 *  of different horizontal edges.  No special stuff here.
 */
Edge3* Preprocessor2D5::addHorizontalEdge(Vertex3* a_tail, Vertex3* a_head)
{
	typedef Edge3_pmap::iterator Eit;

	const Real xTail = a_tail->position().x;
	const Real xHead = a_head->position().x;
	const Real xKey = xTail < xHead ? xTail : xHead;
	std::pair<Eit, Eit> eRange = m_tempEdges.equal_range(xKey);

	// search in the map for an equal edge
	for (Eit eit = eRange.first; eit != eRange.second; ++eit)
	{
		Edge3* candidate = eit->second;
		if ((candidate->tail() == a_tail && candidate->head() == a_head) ||
			(candidate->tail() == a_head && candidate->head() == a_tail))
		{
			return candidate;
		}
	}

	// if you can't find it, add it
	Edge3* edge = m_tempWorld->newEdge(a_tail, a_head);
	m_tempEdges.insert(std::make_pair(xKey, edge));
	return edge;
}



/** Return a (new) vertex with given position.  If there already exists one with
 *  the same position, return that one.  Otherwise create it.
 */
Vertex3* Preprocessor2D5::addVertex(const Point3& a_position)
{
	Vertex3* vertex = findVertex(a_position);
	if (vertex == 0)
	{
		vertex = m_tempWorld->newVertex(a_position);
		m_tempVertices.insert(std::make_pair(a_position.x, vertex));

	}
	return vertex;
}



/** Return pointer of vertex with given position.
 *  @return null pointer if no such vertex exists.
 */
Vertex3* Preprocessor2D5::findVertex(const Point3& a_position) const
{
	typedef Vertex3_pmap::const_iterator Vit;
	std::pair<Vit, Vit> range = m_tempVertices.equal_range(a_position.x);

	for (Vit vit = range.first; vit != range.second; ++vit)
	{
		if (vit->second->position() == a_position)
		{
			return vit->second;
		}
	}

	return 0;
}



} // namespace bass3

// --- END OF FILE preprocessor2d5.cpp ----------------------------------------
