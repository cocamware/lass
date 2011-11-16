/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
 *
 *	*** BEGIN LICENSE INFORMATION ***
 *	
 *	The contents of this file are subject to the Common Public Attribution License 
 *	Version 1.0 (the "License"); you may not use this file except in compliance with 
 *	the License. You may obtain a copy of the License at 
 *	http://lass.sourceforge.net/cpal-license. The License is based on the 
 *	Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *	use of software over a computer network and provide for limited attribution for 
 *	the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *	with Exhibit B.
 *	
 *	Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *	WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *	language governing rights and limitations under the License.
 *	
 *	The Original Code is LASS - Library of Assembled Shared Sources.
 *	
 *	The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *	The Original Developer is the Initial Developer.
 *	
 *	All portions of the code written by the Initial Developer are:
 *	Copyright (C) 2004-2011 the Initial Developer.
 *	All Rights Reserved.
 *	
 *	Contributor(s):
 *
 *	Alternatively, the contents of this file may be used under the terms of the 
 *	GNU General Public License Version 2 or later (the GPL), in which case the 
 *	provisions of GPL are applicable instead of those above.  If you wish to allow use
 *	of your version of this file only under the terms of the GPL and not to allow 
 *	others to use your version of this file under the CPAL, indicate your decision by 
 *	deleting the provisions above and replace them with the notice and other 
 *	provisions required by the GPL License. If you do not delete the provisions above,
 *	a recipient may use your version of this file under either the CPAL or the GPL.
 *	
 *	*** END LICENSE INFORMATION ***
 */


#include "test_common.h"
#include "../lass/num/floating_point_consistency.h"
#include "../lass/spat/planar_mesh.h"
#include "../lass/io/matlab_o_stream.h"
#include "../lass/io/file_attribute.h"
#include "../lass/prim/triangle_2d.h"
#include "../lass/util/clock.h"
#include "../lass/util/stop_watch.h"
#include "../lass/num/filtered_float.h"
#include "../lass/num/interval.h"

namespace lass
{
namespace test
{

typedef num::Consistent<double> TestType;
//typedef num::FilteredFloat<double> TestType;
//typedef double TestType;
//typedef num::interval<double> TestType;

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
	TestType area(0.0);
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
		/*
		LASS_TEST_CHECK_THROW( TPlanarMesh::setPointHandle( e, 0 ), std::runtime_error );
		LASS_TEST_CHECK_THROW( TPlanarMesh::setEdgeHandle( e, 0 ), std::runtime_error );
		LASS_TEST_CHECK_THROW( TPlanarMesh::setFaceHandle( e, 0 ), std::runtime_error );
		*/
	}

	return true;
}

struct SplitConnectTest
{
	typedef ::lass::spat::PlanarMesh<TestType,int,int,int> TPlanarMesh;
	TPlanarMesh* mesh;

	bool testSplitConnect(TPlanarMesh::TEdge* iEdge)
	{
		if (TPlanarMesh::hasRightFace(iEdge))
		{
			TPlanarMesh::TEdge* oppEdge = iEdge->lNext()->lNext();
			TPlanarMesh::TEdge* e = mesh->split(iEdge,0.5);
			LASS_TEST_CHECK(TPlanarMesh::dest(iEdge)==TPlanarMesh::org(e));
			LASS_TEST_CHECK(TPlanarMesh::org(iEdge->sym())==TPlanarMesh::dest(e->sym()));
			LASS_TEST_CHECK(iEdge->lNext()==e);
			LASS_TEST_CHECK(e->lPrev()==iEdge);
			TPlanarMesh::TPoint2D conPoint = TPlanarMesh::org(oppEdge);
			TPlanarMesh::TPoint2D splPoint = TPlanarMesh::dest(iEdge);
			mesh->connect(iEdge,oppEdge);
			return false;
		}
		return true;
	};

};



void testSpatPlanarMesh()
{
	using namespace prim;
	// this in preparation for a Voronoi surface test
	typedef Triangle2D<TestType>	TTriangle;
	typedef Point2D<TestType>	TPoint;
	typedef SimplePolygon2D<TestType> TSimplePolygon;

	TTriangle testTriangle(TPoint(0,0),TPoint(1,0),TPoint(0,1));
	LASS_TEST_CHECK_EQUAL( partialVoronoiArea(testTriangle,0)
							+ partialVoronoiArea(testTriangle,1)
							+ partialVoronoiArea(testTriangle,2), TestType(0.5) );


#pragma LASS_FIXME("PlanarMesh: Only the double typename is tested, due to broken compiler support!")
	using namespace spat;

	typedef PlanarMesh<TestType, int, int, int >    TPlanarMesh;
	typedef PlanarMesh<long , int, int, int >   TPlanarMeshLong;
	typedef TPlanarMesh::TPoint2D   TPoint2D;
	typedef TPlanarMesh::TEdge      TEdge;
	typedef TPlanarMeshLong::TPoint2D   TPoint2DLong;

	LASS_EVAL(sizeof(TPlanarMesh::TQuadEdge));
	LASS_EVAL(sizeof(TPlanarMesh::TQuadEdge::Edge));
	LASS_EVAL(sizeof(TPlanarMesh::TQuadEdge::TEdgeHandle));
	LASS_EVAL(sizeof(TPlanarMesh::TQuadEdge::TEdgeHandle::THandles));
	LASS_EVAL(sizeof(TPlanarMesh::TBitField));
	LASS_EVAL(sizeof(TPoint2D));

	TPlanarMesh		voronoiTest(TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));
	for (int x=10;x<90;x+=10)
	{
		for (int y=10;y<90;y+=10)
		{
			voronoiTest.insertSite(TPoint2D(x,y));
		}
	}
	lass::io::MatlabOStream testVoronoiIo;
	voronoiTest.forAllVertices( TPlanarMesh::TEdgeCallback( ComputeVoronoiArea )  );
	testVoronoiIo.open("voronoiTest.m");
	testVoronoiIo << voronoiTest;
	testVoronoiIo.close();
	

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
			randomPointsLong.push_back( TPoint2DLong(x,y ) );

#pragma LASS_TODO("Make the test for integral types such as long working again... problem is sqrt")
	/*
	for (int i=0;i<int(randomPointsLong.size());++i)
		testMesh3.insertSite( randomPointsLong[i], true );
	*/

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
	LASS_COUT << "Number of edges = " << countCalls << "\n";
	countEdges = countCalls;

	countCalls = 0;
	testMesh3.forAllVertices( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	LASS_COUT << "Number of vertices = " << countCalls << "\n";
	countVertices = countCalls;

	countCalls = 0;
	testMesh3.forAllFaces( TPlanarMeshLong::TEdgeCallback( CountCallsLong )  );
	LASS_COUT << "Number of faces = " << countCalls << "\n";
	countFaces = countCalls;

	testIo.open( "testPlanarMeshIO_nonDelaunay.m" );
	testIo << testMesh;
	testIo.close();

	testMesh.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );


	///* check euler relation */
	LASS_TEST_CHECK_EQUAL( countFaces - countEdges + countVertices, 2 );
	
	TPlanarMesh     testMesh4( TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));
	TPlanarMesh     testMesh5( TPoint2D(0,0), TPoint2D(100,0), TPoint2D(100,100), TPoint2D(0,100));
	double offsetX = 708000;
	double offsetY = 5251350;
	TPlanarMesh     testMeshContours( TPoint2D(708000-offsetX,5251350-offsetY), TPoint2D(708000+10000-offsetX,5251350-offsetY),
									  TPoint2D(708000+10000-offsetX,5251350+10000-offsetY), TPoint2D(708000-offsetX,5251350+10000-offsetY));

	std::ifstream contoursTest;
	
	if (io::fileDoesExist("contours.txt"))
	{
		contoursTest.open("contours.txt");
		std::string line;
		int segmentCount = 0;
		while (std::getline(contoursTest, line, '\n'))
		{
			std::istringstream stream(line);
			double x1,y1,x2,y2;
			stream >> x1 >> y1 >> x2 >> y2;
			x1 -= offsetX;
			x2 -= offsetX;
			y1 -= offsetY;
			y2 -= offsetY;

			if (	x1>0 && x2>0 && y1<12000 && y2<12000 
				 && y1>0 && y2>0 &&	x1<12500 && x2<12500)
			{
				std::cout << x1 << "\t" << y1 << "\t" << x2 << "\t" << y2 << "\n";
				testMeshContours.insertEdge( 
					TPlanarMesh::TLineSegment2D( TPoint2D(x1,y1), TPoint2D(x2,y2) ),3,3 );
				++segmentCount;
				if (segmentCount%25==-1)
				{
					ColorEdges colorEdges;
					std::ostringstream buf;
					buf << "countour" << segmentCount << ".m";
					colorEdges.stream.open(buf.str().c_str());	
					testMeshContours.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	
					colorEdges.stream.close();
				}
			}
		}
		contoursTest.close();
	}
	else
	{
		LASS_COUT << "*** Could not find contours.txt for testing ***\n";
	}

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

	for (int i=0;i<3;++i)
        testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(5.0+i*10,60.0), TPoint2D(15.0+i*10,60.0) ),intHandles[1],intHandles[1] );
	for (int i=0;i<3;++i)
        testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(5.0+i*10,65.0), TPoint2D(15.0+i*10,65.0) ),intHandles[1],intHandles[1] );
	for (int i=0;i<3;++i)
        testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(5.0+i*10,62.0), TPoint2D(15.0+i*10,62.0) ),intHandles[1],intHandles[1] );


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

	int ni=0;
	ni = 0; 
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0+num::cos(float(ni))*25.0,40.0+num::sin(float(ni))*25.0) ),intHandles[0],intHandles[0] );
	colorEdges.stream.open("test9.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	ni = 1; 
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0+num::cos(float(ni))*25.0,40.0+num::sin(float(ni))*25.0) ),intHandles[0],intHandles[0] );
	colorEdges.stream.open("test10.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	ni = 2; 
	testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0+num::cos(float(ni))*25.0,40.0+num::sin(float(ni))*25.0) ),intHandles[0],intHandles[0] );
	colorEdges.stream.open("test11.m");	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	

	for (int i=0;i<10;++i)
	{
		testMesh4.insertEdge( TPlanarMesh::TLineSegment2D( TPoint2D(30.0,40.0), TPoint2D(30.0+num::cos(float(i))*25.0,40.0+num::sin(float(i))*25.0) ),intHandles[0],intHandles[0] );
		std::string filen = std::string("test")+lass::util::stringCast<std::string>(12+i)+".m";
		colorEdges.stream.open(filen.c_str());	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );	colorEdges.stream.close();
	}

	testMesh4.setTolerance(TestType(1e-12));
	testMesh4.setPointDistanceTolerance(TestType(1e-5));

	util::Clock aClock;
	util::StopWatch stopWatch(aClock);
	stopWatch.start();
	{
		for (int j=0;j<1;++j)
		{
			double d = pow(10.0,-(j+1));
			std::cout << "Level of detail = " << d << "\n";
			for (int i=0;i<15;++i)
			{
				{
					TSimplePolygon testPoly;
					testPoly.add( TPoint2D(75+d*i,75+d*i) );
					testPoly.add( TPoint2D(85+d*i,75-d*i) );
					testPoly.add( TPoint2D(85-d*i,85+d*i) );
					testPoly.add( TPoint2D(75+d*i,85-d*i) );
					testMesh4.insertPolygon( testPoly, intHandles[0],intHandles[1] );
					//testMesh4.insertPolygon( testPoly, intHandles[0],intHandles[0],intHandles[1] );
					//std::cout << "Marking " << i << " polygons, edgecount = " << testMesh4.edgeCount() << "\n";
					//testMesh4.markPolygon( testMesh4.locate(testPoly1[0]), testPoly1, intHandles[3] );
					//testMesh4.forAllEdges( TPlanarMesh::TEdgeCallback( TestPropertiesDouble )  );
				}
			}
		}
	}
	stopWatch.stop();
	LASS_COUT << "Mesh stability test took " << stopWatch.time() << std::endl; 

	/*
	 * SOMETHING GOES WRONG IN HERE !!!!!!
	 * I've disabled this code because it breaks the nightly build and leaves ugly boxes open on my computer 
	 */

	testIo.open( "testPlanarMeshIO_constrained.m" );
	testIo << testMesh4;
	testIo.close();

	colorEdges.stream.open("testPlanarMesh_colored.m");
	testMesh4.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );
	colorEdges.stream.close();

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
		for (size_t i=0;i<crossedEdges.size();++i)
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
	{
		SplitConnectTest splitConnect;
		splitConnect.mesh = &testMesh5;
		for (int i=0;i<50;++i)
			testMesh5.forAllPrimaryEdges( lass::util::makeCallback( &splitConnect, &SplitConnectTest::testSplitConnect)  );
		ColorEdges colorEdges;
		colorEdges.stream.open("testPlanarMesh_split_connect.m");
		colorEdges.leftEdges = false;
		testMesh5.forAllPrimaryEdges( lass::util::makeCallback( &colorEdges, &ColorEdges::toMatlabOStream )  );
		colorEdges.stream.close();
	}

	/**/

}


TUnitTest test_spat_planar_mesh()
{
	return TUnitTest(1, LASS_TEST_CASE(testSpatPlanarMesh));
}

}
}
