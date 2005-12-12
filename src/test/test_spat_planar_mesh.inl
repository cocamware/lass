/** @file
 *  @internal
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



#include "test_common.h"
#include "test_spat.h"

#include "../spat/planar_mesh.h"
#include "../io/matlab_o_stream.h"
#include "../prim/triangle_2d.h"

namespace lass
{
namespace test
{

typedef double TestType;

int countCalls = 0;

bool CountCalls( ::lass::spat::PlanarMesh<TestType,int,int,int>::TEdge* e)
{
	++countCalls;
	return true;
}

bool CountCallsLong( ::lass::spat::PlanarMesh<long,int,int,int>::TEdge* e)
{
	++countCalls;
	return true;
}

bool TestPropertiesDouble( ::lass::spat::PlanarMesh<TestType,int,int,int>::TEdge* e)
{
	typedef ::lass::spat::PlanarMesh<TestType,int,int,int> TPlanarMesh;

	LASS_TEST_CHECK_EQUAL( (e->rot()->rot()->rot()->rot()), e );
	LASS_TEST_CHECK_EQUAL( e->rot()->oNext()->rot()->oNext(), e );
	LASS_TEST_CHECK( e->rot()->rot() != e );
	if ( TPlanarMesh::inPrimaryMesh( e ) )
	{
		LASS_TEST_CHECK( TPlanarMesh::inDualMesh( e->rot() ) );
	}
	else
	{
		LASS_TEST_CHECK( TPlanarMesh::inPrimaryMesh( e->rot() ) );
	}
	if ( TPlanarMesh::inPrimaryMesh( e ) )
	{
		LASS_TEST_CHECK( TPlanarMesh::inPrimaryMesh( e->oNext() ) );
	}
	LASS_TEST_CHECK_EQUAL( e->sym(), e->rot()->rot() );
	LASS_TEST_CHECK_EQUAL( e->invRot(), e->rot()->rot()->rot() );

	LASS_TEST_CHECK_EQUAL( e->lNext(), e->invRot()->oNext()->rot() );
	LASS_TEST_CHECK_EQUAL( e->rNext(), e->rot()->oNext()->invRot() );
	LASS_TEST_CHECK_EQUAL( e->dNext(), e->sym()->oNext()->sym() );

	LASS_TEST_CHECK_EQUAL( e->oPrev(), e->rot()->oNext()->rot() );
	LASS_TEST_CHECK_EQUAL( e->lPrev(), e->oNext()->sym() );
	LASS_TEST_CHECK_EQUAL( e->rPrev(), e->sym()->oNext() );
	LASS_TEST_CHECK_EQUAL( e->dPrev(), e->invRot()->oNext()->invRot() );
	LASS_TEST_CHECK( TPlanarMesh::inPrimaryMesh( e ) != TPlanarMesh::inDualMesh( e ) );

	LASS_TEST_CHECK_NO_THROW( TPlanarMesh::edgeHandle( e ) );
	LASS_TEST_CHECK_NO_THROW( TPlanarMesh::faceHandle( e ) );
	LASS_TEST_CHECK_NO_THROW( TPlanarMesh::pointHandle( e ) );

	if ( TPlanarMesh::inPrimaryMesh( e ) )
	{
		LASS_TEST_CHECK_NO_THROW( TPlanarMesh::setPointHandle( e, 0 ) );
		LASS_TEST_CHECK_NO_THROW( TPlanarMesh::setEdgeHandle( e, 0 ) );
		LASS_TEST_CHECK_NO_THROW( TPlanarMesh::setFaceHandle( e, 0 ) );


#pragma LASS_FIXME("PlanarMesh: no test for faceHandles" )
		/*
		std::auto_ptr<int>  tempHandle(new int(5));
		TPlanarMesh::setFaceHandle( e, tempHandle.get() );

		TPlanarMesh::TEdge* currentEdge = e;
		do
		{
			LASS_TEST_CHECK_EQUAL( *TPlanarMesh::faceHandle( e ) , 5 );
			currentEdge = currentEdge->lNext();
		} while ( currentEdge != e );
		*/

	}
	else
	{
		LASS_TEST_CHECK_THROW( TPlanarMesh::setPointHandle( e, 0 ), std::runtime_error );
		LASS_TEST_CHECK_THROW( TPlanarMesh::setEdgeHandle( e, 0 ), std::runtime_error );
		LASS_TEST_CHECK_THROW( TPlanarMesh::setFaceHandle( e, 0 ), std::runtime_error );
	}

	return true;
}

void doTestPlanarMesh()
{
	using namespace prim;
	// this in preparation for a Voronoi surface test
	typedef Triangle2D<double>	TTriangle;
	typedef Point2D<double>	TPoint;

	TTriangle testTriangle(TPoint(0,0),TPoint(1,0),TPoint(0,1));
	LASS_TEST_CHECK_EQUAL( partialVoronoiSurface(testTriangle,0)
							+ partialVoronoiSurface(testTriangle,1)
							+ partialVoronoiSurface(testTriangle,2), 0.5 );


#pragma LASS_FIXME("PlanarMesh: Only the double typename is tested, due to broken compiler support!")
	using namespace spat;

	typedef PlanarMesh<TestType, int, int, int >    TPlanarMesh;
	typedef PlanarMesh<long , int, int, int >   TPlanarMeshLong;
	typedef TPlanarMesh::TPoint2D   TPoint2D;
	typedef TPlanarMesh::TEdge      TEdge;
	typedef TPlanarMeshLong::TPoint2D   TPoint2DLong;

	TPlanarMesh     testMesh( TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));
	TPlanarMesh     testMesh2( TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));
	TPlanarMeshLong testMesh3( TPoint2DLong(0,0), TPoint2DLong(100,0), TPoint2DLong(100,100), TPoint2DLong(0,100));

	TEdge* e = testMesh.startEdge();

	if ( TPlanarMesh::inDualMesh( e ) )
		e = e->rot();

	LASS_TEST_CHECK_EQUAL( e, e->lNext()->lNext()->lNext() );

	testMesh.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	std::vector< TPoint2D > randomPoints;
	std::vector< TPoint2DLong > randomPointsLong;
	for (int i=0;i<20;++i)
	{
		double rx = (double)rand()/(double)RAND_MAX;
		double ry = (double)rand()/(double)RAND_MAX;

		randomPoints.push_back( TPoint2D( 10.0+rx*80.0, 10.0+ry*80.0 ) );
	}

	lass::io::MatlabOStream testIo;
	lass::io::MatlabOStream testIo2;

	for (int i=0;i<int(randomPoints.size());++i)
		testMesh.insertSite( randomPoints[i], false );

	testIo.open( "testPlanarMeshIO_nonDelaunay.m" );
	testIo << testMesh;
	testIo.close();

	testMesh.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	for (int i=0;i<int(randomPoints.size());++i)
		testMesh2.insertSite( randomPoints[i], true );

	testIo2.open( "testPlanarMeshIO_Delaunay.m" );
	testIo2 << testMesh2;
	testIo2.close();

	testMesh2.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	testMesh2.makeMaximalConvexPolygon();
	testMesh2.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	testIo2.open( "testPlanarMeshIO_convex.m" );
	testIo2 << testMesh2;
	testIo2.close();

	randomPointsLong.clear();
	for (long x=10;x<91;x+=10)
		for (long y=10;y<91;y+=10)
		{
			randomPointsLong.push_back( TPoint2DLong(x,y ) );
		}

	//std::random_shuffle( randomPointsLong.begin(), randomPointsLong.end() );
	for (int i=0;i<int(randomPointsLong.size());++i)
	{
		testMesh3.insertSite( randomPointsLong[i], true );
	}


	testIo2.open( "testPlanarMeshIO_sq_Delaunay.m" );
	testIo2 << testMesh3;
	testIo2.close();
	testMesh2.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	testMesh3.makeMaximalConvexPolygon();
	testIo2.open( "testPlanarMeshIO_sq_convex.m" );
	testIo2 << testMesh3;
	testIo2.close();

	int countEdges = 0;
	int countVertices = 0;
	int countFaces = 0;

	countCalls = 0;
	testMesh3.forAllPrimaryUndirectedEdges( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	std::cout << "Number of edges = " << countCalls << "\n";
	countEdges = countCalls;

	countCalls = 0;
	testMesh3.forAllVertices( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	std::cout << "Number of vertices = " << countCalls << "\n";
	countVertices = countCalls;

	countCalls = 0;
	testMesh3.forAllFaces( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	std::cout << "Number of faces = " << countCalls << "\n";
	countFaces = countCalls;

	/* check euler relation */
	LASS_TEST_CHECK_EQUAL( countFaces - countEdges + countVertices, 2 );

	TPlanarMesh     testMesh4( TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));

	for (int i=0;i<int(randomPoints.size());++i)
		testMesh4.insertSite( randomPoints[i], true);
	/* test insertion of constrained edges */
	testIo.open( "testPlanarMeshIO_constrained_b4.m" );
	testIo << testMesh4;
	testIo.close();

	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,10.0), TPoint2D(90.0,10.0) ) );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(90.0,10.0), TPoint2D(90.0,90.0) ) );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(90.0,90.0), TPoint2D(10.0,90.0) ) );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,90.0), TPoint2D(10.0,10.0) ) );
	testMesh4.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	testIo.open( "testPlanarMeshIO_constrained.m" );
	testIo << testMesh4;
	testIo.close();
}

}
}
