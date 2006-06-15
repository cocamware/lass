/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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
#include "../num/floating_point_consistency.h"


namespace lass
{
namespace test
{

	typedef num::Consistent<double> TestType;

int countCalls = 0;

bool CountCalls( ::lass::spat::PlanarMesh<TestType,int,int,int>::TEdge* /*e*/)
{
	++countCalls;
	return true;
}

class ColorEdges
{
	typedef ::lass::spat::PlanarMesh<TestType,int,int,int> TPlanarMesh;
public:
	bool leftEdges;
	bool rightEdges;
	lass::io::MatlabOStream stream;

	ColorEdges() : leftEdges(true), rightEdges(true) {}
	virtual ~ColorEdges() {}

	bool toMatlabOStream(TPlanarMesh::TEdge* e)
	{
		bool isLeftEdge = TPlanarMesh::org(e).y < TPlanarMesh::dest(e).y;
		bool isRightEdge = !isLeftEdge;

		if (!leftEdges && isLeftEdge)
			return true;
		if (!rightEdges && isRightEdge)
			return true;
			
		int edgeH= TPlanarMesh::edgeHandle(e);
		if (edgeH)
		{
			switch (edgeH)
			{
			case 0:
				stream.setColor(lass::io::mcBlack);
				break;
			case 1:
				stream.setColor(lass::io::mcBlue);
				break;
			case 2:
				stream.setColor(lass::io::mcGreen);
				break;
			case 3:
				stream.setColor(lass::io::mcRed);
				break;
			case 4:
				stream.setColor(lass::io::mcCyan);
				break;
			case 5:
				stream.setColor(lass::io::mcMagenta);
				break;
			default:
				stream.setColor(lass::io::mcBlue);
			}	
			stream << TPlanarMesh::TLineSegment2D(TPlanarMesh::org(e), TPlanarMesh::dest(e) );
		}
		else
		{
			stream.setColor(lass::io::mcYellow);
		}
		return true;
	}
};


class ColorTriangles
{
	typedef ::lass::spat::PlanarMesh<TestType,int,int,int> TPlanarMesh;
public:
	lass::io::MatlabOStream stream;

	ColorTriangles() {}
	virtual ~ColorTriangles() {}

	bool toMatlabOStream(TPlanarMesh::TEdge* e)
	{
		int edgeH= TPlanarMesh::faceHandle(e->invRot());
		if (edgeH)
		{
			switch (edgeH)
			{
			case 0:
				stream.setColor(lass::io::mcBlack);
				break;
			case 1:
				stream.setColor(lass::io::mcBlue);
				break;
			case 2:
				stream.setColor(lass::io::mcGreen);
				break;
			case 3:
				stream.setColor(lass::io::mcRed);
				break;
			case 4:
				stream.setColor(lass::io::mcCyan);
				break;
			case 5:
				stream.setColor(lass::io::mcMagenta);
				break;
			default:
				stream.setColor(lass::io::mcYellow);
			}	
		}
		else
		{
			stream.setColor(lass::io::mcYellow);
		}
		// e is in the dual mesh, switch back and extract triangle
		
		TPlanarMesh::TTriangle2D triangle = TPlanarMesh::triangle(e->invRot());
		// skip the inside-out outer triangles
		if (triangle.orientation() == lass::prim::oCounterClockWise)
			stream << triangle;
		//stream << TPlanarMesh::TLineSegment2D(TPlanarMesh::org(e), TPlanarMesh::dest(e) );
		return true;
	}
};


bool ComputeVoronoiArea( ::lass::spat::PlanarMesh<TestType,int,int,int>::TEdge* e)
{
	typedef ::lass::spat::PlanarMesh<TestType,int,int,int> TPlanarMesh;
	int n = TPlanarMesh::vertexOrder(e);
	TestType area = 0.0;
	for (int i=0;i<n;++i)
	{
		lass::prim::Triangle2D<TestType> tri( TPlanarMesh::org(e), TPlanarMesh::dest(e), TPlanarMesh::dest(e->lNext()) );
		area += lass::prim::partialVoronoiArea( tri, 0 );
		e = e->oNext();
	}
	LASS_COUT << TPlanarMesh::org(e) << "\t" << n << "\t" << area << std::endl;

	if (TPlanarMesh::org(e)==lass::prim::Point2D<TestType>(40,40))
	{
		lass::io::MatlabOStream testPoint;
		testPoint.open("testPoint.m");
		testPoint << TPlanarMesh::org(e);
		for (int i=0;i<n;++i)
		{
			e = e->oNext();
			lass::prim::Triangle2D<TestType> tri( TPlanarMesh::org(e), TPlanarMesh::dest(e), TPlanarMesh::dest(e->lNext()) );
			testPoint << TPlanarMesh::TLineSegment2D( 
					TPlanarMesh::org(e), TPlanarMesh::dest(e) );
			//testPoint << *e;
		}
		testPoint.close();
	}

	return true;
}

bool CountCallsLong( ::lass::spat::PlanarMesh<long,int,int,int>::TEdge* /*e*/)
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


//#pragma LASS_FIXME("PlanarMesh: no test for faceHandles" )
		int  tempHandle=5;
		TPlanarMesh::setFaceHandle( e, tempHandle );

		TPlanarMesh::TEdge* currentEdge = e;
		do
		{
			LASS_TEST_CHECK_EQUAL( TPlanarMesh::faceHandle( e ) , 5 );
			currentEdge = currentEdge->lNext();
		} while ( currentEdge != e );
		TPlanarMesh::setFaceHandle( e, 0 );
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
	typedef Triangle2D<TestType>	TTriangle;
	typedef Point2D<TestType>	TPoint;
	typedef SimplePolygon2D<TestType> TSimplePolygon;

	TTriangle testTriangle(TPoint(0,0),TPoint(1,0),TPoint(0,1));
	LASS_TEST_CHECK_EQUAL( partialVoronoiArea(testTriangle,0)
							+ partialVoronoiArea(testTriangle,1)
							+ partialVoronoiArea(testTriangle,2), 0.5 );


#pragma LASS_FIXME("PlanarMesh: Only the double typename is tested, due to broken compiler support!")
	using namespace spat;

	typedef PlanarMesh<TestType, int, int, int >    TPlanarMesh;
	//typedef PlanarMesh<long , int, int, int >   TPlanarMeshLong;
	typedef TPlanarMesh::TPoint2D   TPoint2D;
	typedef TPlanarMesh::TEdge      TEdge;
	//typedef TPlanarMeshLong::TPoint2D   TPoint2DLong;

	//TPlanarMesh		voronoiTest(TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));
	//for (int x=10;x<90;x+=10)
	//{
	//	for (int y=10;y<90;y+=10)
	//	{
	//		voronoiTest.insertSite(TPoint2D(x,y));
	//	}
	//}
	//lass::io::MatlabOStream testVoronoiIo;
	//voronoiTest.forAllVertices( TPlanarMesh::TEdgeCallback( ComputeVoronoiArea )  );
	//testVoronoiIo.open("voronoiTest.m");
	//testVoronoiIo << voronoiTest;
	//testVoronoiIo.close();
	//

	//TPlanarMesh     testMesh( TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));
	//TPlanarMesh     testMesh2( TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));
	//TPlanarMeshLong testMesh3( TPoint2DLong(0,0), TPoint2DLong(100,0), TPoint2DLong(100,100), TPoint2DLong(0,100));

	//TEdge* e = testMesh.startEdge();

	//if ( TPlanarMesh::inDualMesh( e ) )
	//	e = e->rot();

	//LASS_TEST_CHECK_EQUAL( e, e->lNext()->lNext()->lNext() );

	//testMesh.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	//std::vector< TPoint2D > randomPoints;
	//std::vector< TPoint2DLong > randomPointsLong;
	//for (int i=0;i<20;++i)
	//{
	//	double rx = (double)rand()/(double)RAND_MAX;
	//	double ry = (double)rand()/(double)RAND_MAX;

	//	randomPoints.push_back( TPoint2D( 10.0+rx*80.0, 10.0+ry*80.0 ) );
	//}

	lass::io::MatlabOStream testIo;
	lass::io::MatlabOStream testIo2;

	//for (int i=0;i<int(randomPoints.size());++i)
	//	testMesh.insertSite( randomPoints[i], false );

	//testIo.open( "testPlanarMeshIO_nonDelaunay.m" );
	//testIo << testMesh;
	//testIo.close();

	//testMesh.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	//for (int i=0;i<int(randomPoints.size());++i)
	//	testMesh2.insertSite( randomPoints[i], true );

	//testIo2.open( "testPlanarMeshIO_Delaunay.m" );
	//testIo2 << testMesh2;
	//testIo2.close();

	//testMesh2.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	//testMesh2.makeMaximalConvexPolygon();
	//testMesh2.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	//testIo2.open( "testPlanarMeshIO_convex.m" );
	//testIo2 << testMesh2;
	//testIo2.close();

	//randomPointsLong.clear();
	//for (long x=10;x<91;x+=10)
	//	for (long y=10;y<91;y+=10)
	//	{
	//		randomPointsLong.push_back( TPoint2DLong(x,y ) );
	//	}

	////std::random_shuffle( randomPointsLong.begin(), randomPointsLong.end() );
	//for (int i=0;i<int(randomPointsLong.size());++i)
	//{
	//	testMesh3.insertSite( randomPointsLong[i], true );
	//}


	//testIo2.open( "testPlanarMeshIO_sq_Delaunay.m" );
	//testIo2 << testMesh3;
	//testIo2.close();
	//testMesh2.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	//testMesh3.makeMaximalConvexPolygon();
	//testIo2.open( "testPlanarMeshIO_sq_convex.m" );
	//testIo2 << testMesh3;
	//testIo2.close();

	//int countEdges = 0;
	//int countVertices = 0;
	//int countFaces = 0;

	//countCalls = 0;
	//testMesh3.forAllPrimaryUndirectedEdges( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	//LASS_COUT << "Number of edges = " << countCalls << "\n";
	//countEdges = countCalls;

	//countCalls = 0;
	//testMesh3.forAllVertices( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	//LASS_COUT << "Number of vertices = " << countCalls << "\n";
	//countVertices = countCalls;

	//countCalls = 0;
	//testMesh3.forAllFaces( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	//LASS_COUT << "Number of faces = " << countCalls << "\n";
	//countFaces = countCalls;

	//testIo.open( "testPlanarMeshIO_nonDelaunay.m" );
	//testIo << testMesh;
	//testIo.close();

	//testMesh.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	//for (int i=0;i<int(randomPoints.size());++i)
	//	testMesh2.insertSite( randomPoints[i], true );

	//testIo2.open( "testPlanarMeshIO_Delaunay.m" );
	//testIo2 << testMesh2;
	//testIo2.close();

	//testMesh2.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	//testMesh2.makeMaximalConvexPolygon();
	//testMesh2.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	//testIo2.open( "testPlanarMeshIO_convex.m" );
	//testIo2 << testMesh2;
	//testIo2.close();

	//randomPointsLong.clear();
	//for (long x=10;x<91;x+=10)
	//	for (long y=10;y<91;y+=10)
	//	{
	//		randomPointsLong.push_back( TPoint2DLong(x,y ) );
	//	}

	////std::random_shuffle( randomPointsLong.begin(), randomPointsLong.end() );
	//for (int i=0;i<int(randomPointsLong.size());++i)
	//{
	//	testMesh3.insertSite( randomPointsLong[i], true );
	//}


	//testIo2.open( "testPlanarMeshIO_sq_Delaunay.m" );
	//testIo2 << testMesh3;
	//testIo2.close();
	//testMesh2.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	//testMesh3.makeMaximalConvexPolygon();
	//testIo2.open( "testPlanarMeshIO_sq_convex.m" );
	//testIo2 << testMesh3;
	//testIo2.close();

	//int countEdges = 0;
	//int countVertices = 0;
	//int countFaces = 0;

	//countCalls = 0;
	//testMesh3.forAllPrimaryUndirectedEdges( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	//LASS_COUT << "Number of edges = " << countCalls << "\n";
	//countEdges = countCalls;

	//countCalls = 0;
	//testMesh3.forAllVertices( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	//LASS_COUT << "Number of vertices = " << countCalls << "\n";
	//countVertices = countCalls;

	//countCalls = 0;
	//testMesh3.forAllFaces( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	//LASS_COUT << "Number of faces = " << countCalls << "\n";
	//countFaces = countCalls;

	///* check euler relation */
	//LASS_TEST_CHECK_EQUAL( countFaces - countEdges + countVertices, 2 );
	
	///* check euler relation */
	//LASS_TEST_CHECK_EQUAL( countFaces - countEdges + countVertices, 2 );
	


	TPlanarMesh     testMesh4( TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));
	TPlanarMesh     testMesh5( TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));

	/*
	for (int i=0;i<int(randomPoints.size());++i)
		testMesh4.insertSite( randomPoints[i], true);
	*/
	/* test insertion of constrained edges */
	/*
	testIo.open( "testPlanarMeshIO_constrained_b4.m" );
	testIo << testMesh4;
	testIo.close();

	int intHandles[10];
	for (int i=0;i<10;++i)
	{
		intHandles[i] = i+1;
	}

	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,10.0), TPoint2D(60.0,10.0) ),&intHandles[0],&intHandles[0] );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(60.0,10.0), TPoint2D(60.0,60.0) ),&intHandles[0],&intHandles[0] );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(60.0,60.0), TPoint2D(10.0,60.0) ),&intHandles[0],&intHandles[0] );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,60.0), TPoint2D(10.0,10.0) ),&intHandles[0],&intHandles[0] );

	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(5.0,20.0), TPoint2D(65.0,20.0) ),&intHandles[1],&intHandles[1] );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,30.0), TPoint2D(60.0,30.0) ),&intHandles[2],&intHandles[2] );

	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,20.0), TPoint2D(15.0,25.0) ),&intHandles[3],&intHandles[3] );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(8.0,18.0), TPoint2D(35.0,40.0) ),&intHandles[3],&intHandles[3] );
	testMesh4.insertSite( TPoint2D(10.0,20.0) );

	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(60.0,20.0) ),&intHandles[4],&intHandles[4] );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0,60.0) ),&intHandles[5],&intHandles[5] );
	for (int i=0;i<10;++i)
		testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0+num::cos(float(i))*25.0,40.0+num::sin(float(i))*25.0) ),&intHandles[i],&intHandles[i] );

	TSimplePolygon testPoly;
	testPoly.add( TPoint2D(75,75) );
	testPoly.add( TPoint2D(85,75) );
	testPoly.add( TPoint2D(85,85) );
	testPoly.add( TPoint2D(75,85) );
	testMesh4.insertPolygon( testPoly, &intHandles[0],&intHandles[0],&intHandles[1] );

	*/
	//testMesh4.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );
	int intHandles[10];
	for (int i=0;i<10;++i)
	{
		intHandles[i] = i+1;
	}

	testIo.open( "test1.m" ); testIo << testMesh4; testIo.close();


	TSimplePolygon testPoly1;
	testPoly1.add( TPoint2D(10,10) );
	testPoly1.add( TPoint2D(60,10) );
	testPoly1.add( TPoint2D(60,60) );
	testPoly1.add( TPoint2D(10,60) );
	ColorEdges colorEdges;


	//testMesh4.insertPolygon( testPoly1, intHandles[0], intHandles[0],false );
	/*testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,10.0), TPoint2D(60.0,10.0) ),&intHandles[0],&intHandles[0] );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(60.0,10.0), TPoint2D(60.0,60.0) ),&intHandles[0],&intHandles[0] );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(60.0,60.0), TPoint2D(10.0,60.0) ),&intHandles[0],&intHandles[0] );
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,60.0), TPoint2D(10.0,10.0) ),&intHandles[0],&intHandles[0] );
	*/
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(5.0,20.0), TPoint2D(65.0,20.0) ),intHandles[1],intHandles[1] );
	colorEdges.stream.open("test2.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,30.0), TPoint2D(60.0,30.0) ),intHandles[2],intHandles[2] );
	colorEdges.stream.open("test3.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();

	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,20.0), TPoint2D(15.0,25.0) ),intHandles[3],intHandles[3] );
	colorEdges.stream.open("test4.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(8.0,18.0), TPoint2D(35.0,40.0) ),intHandles[3],intHandles[3] );
	colorEdges.stream.open("test5.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	testMesh4.insertSite( TPoint2D(10.0,20.0) );
	colorEdges.stream.open("test6.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();

	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(60.0,20.0) ),intHandles[4],intHandles[4] );
	colorEdges.stream.open("test7.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0,60.0) ),intHandles[0],intHandles[0] );
	colorEdges.stream.open("test8.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();

	int i=0;
	i = 0; 
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0+num::cos(float(i))*25.0,40.0+num::sin(float(i))*25.0) ),intHandles[0],intHandles[0] );
	colorEdges.stream.open("test9.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	i = 1; 
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0+num::cos(float(i))*25.0,40.0+num::sin(float(i))*25.0) ),intHandles[0],intHandles[0] );
	colorEdges.stream.open("test10.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	i = 2; 
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0+num::cos(float(i))*25.0,40.0+num::sin(float(i))*25.0) ),intHandles[0],intHandles[0] );
	colorEdges.stream.open("test11.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	

	for (int i=0;i<10;++i)
	{
		testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0+num::cos(float(i))*25.0,40.0+num::sin(float(i))*25.0) ),intHandles[0],intHandles[0] );
		std::string filen = std::string("test")+lass::util::stringCast<std::string>(12+i)+".m";
		colorEdges.stream.open(filen.c_str());	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	}

	testMesh4.setTolerance(1e-8);
	{
		for (int j=0;j<15;++j)
		{
			for (int i=0;i<15;++i)
			{
				double d = pow(10.0,-(j+1));
				std::cout << "Level of detail = " << d << "\n";
				//if (i==7)
				{
					if (j==0 && i==3)
					{
						int a = 0;
					}

					TSimplePolygon testPoly;
					testPoly.add( TPoint2D(75+d*i,75+d*i) );
					testPoly.add( TPoint2D(85+d*i,75-d*i) );
					testPoly.add( TPoint2D(85-d*i,85+d*i) );
					testPoly.add( TPoint2D(75+d*i,85-d*i) );
					std::cout << "Inserting " << i << " polygons, edgecount = " << testMesh4.edgeCount() << "\n";
					testMesh4.insertPolygon( testPoly, intHandles[0],intHandles[0],intHandles[1] );
					//testMesh4.insertPolygon( testPoly, intHandles[0],intHandles[0],intHandles[1] );
					std::cout << "Marking " << i << " polygons, edgecount = " << testMesh4.edgeCount() << "\n";
					//testMesh4.markPolygon( testMesh4.locate(testPoly1[0]), testPoly1, intHandles[3] );
					//testMesh4.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

				}
				/*
				else
				{
					//testMesh4.insertSite(TPoint2D(75+d*i,75+d*i));
					//testMesh4.insertSite(TPoint2D(85+d*i,75-d*i));
					testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(75+d*i,75+d*i), TPoint2D(85+d*i,75-d*i) ),intHandles[1],intHandles[2] );

				}
				*/
				std::string filen = std::string("testPlanarMeshIO_constrained_")+lass::util::stringCast<std::string>(j)+"_"+lass::util::stringCast<std::string>(i)+"_.m";
				colorEdges.stream.open(filen.c_str());	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();

				/*
				lass::io::MatlabOStream testIo;
				testIo.open(  filen.c_str());
				testIo << testMesh4;
				testIo.close();
				*/

			}
		}
	}


	//testMesh4.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );

	testIo.open( "testPlanarMeshIO_constrained.m" );
	testIo << testMesh4;
	testIo.close();

	colorEdges.stream.open("testPlanarMesh_colored.m");
	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );
	colorEdges.stream.close();

	/*
	ColorEdges colorEdges;
	colorEdges.stream.open("testPlanarMesh_colored.m");
	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );
	colorEdges.stream.close();
	*/
    ColorTriangles colorTriangles;
	colorTriangles.stream.open("testPlanarMesh_coloredFaces.m");
	testMesh4.forAllDualEdges( lass::util::makeCallback( &colorTriangles, &ColorTriangles::toMatlabOStream )  );
	colorTriangles.stream.close();

	{
		ColorEdges colorEdges;
		colorEdges.stream.open("testPlanarMesh_coloredWalk.m");
		std::vector< TPlanarMesh::TEdge* > crossedEdges;
		//TPlanarMesh::TLineSegment2D lineSegment(TPlanarMesh::TPoint2D(10,10), TPlanarMesh::TPoint2D(88,70) );
		TPlanarMesh::TLineSegment2D lineSegment(TPlanarMesh::TPoint2D(30,40), TPlanarMesh::TPoint2D(88,40) );
		testMesh4.walk( lineSegment, std::back_inserter(crossedEdges) );
		for (int i=0;i<crossedEdges.size();++i)
		{
			colorEdges.toMatlabOStream( crossedEdges[i] );
		}
		colorEdges.stream << lineSegment;
		colorEdges.stream.close();
	}

	testMesh5.insertPolygon( testPoly1, intHandles[1], intHandles[2],false );
	testMesh5.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(5.0,20.0), TPoint2D(65.0,20.0) ),intHandles[1],intHandles[2] );
	testMesh5.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,30.0), TPoint2D(60.0,30.0) ),intHandles[1],intHandles[2] );
	testMesh5.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(10.0,20.0), TPoint2D(15.0,25.0) ),intHandles[1],intHandles[2] );
	testMesh5.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(8.0,18.0), TPoint2D(35.0,40.0) ),intHandles[1],intHandles[2] );
	testMesh5.insertSite( TPoint2D(10.0,20.0) );
	testMesh5.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(60.0,20.0) ),intHandles[1],intHandles[2] );
	testMesh5.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0,60.0) ),intHandles[1],intHandles[2] );
	for (int i=0;i<10;++i)
		testMesh5.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0+num::cos(float(i))*25.0,40.0+num::sin(float(i))*25.0) ),intHandles[1],intHandles[2] );

	{
		ColorEdges colorEdges;
		colorEdges.stream.open("testPlanarMesh_colored_left.m");
		colorEdges.rightEdges = false;
		testMesh5.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );
		colorEdges.stream.close();
	}
	{
		ColorEdges colorEdges;
		colorEdges.stream.open("testPlanarMesh_colored_right.m");
		colorEdges.leftEdges = false;
		testMesh5.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );
		colorEdges.stream.close();
	}

}

}
}
