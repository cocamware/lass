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



#include "../spat/quad_tree.h"
#include "../prim/line_2d.h"
#include "../prim/simple_polygon_2d.h"
#include "../prim/simple_polygon_3d.h"
#include "../prim/side.h"
#include "../prim/plane_3d.h"

namespace lass
{
namespace test
{

typedef lass::prim::SimplePolygon2D<double> TSimplePolygon2D;
typedef lass::prim::SimplePolygon3D<double> TSimplePolygon3D;

template<class T>
struct QuadTreeTraits
{};

template<>
struct QuadTreeTraits< TSimplePolygon2D >
{
	typedef double Real;
	typedef prim::Point2D<Real> TPoint;
	typedef prim::Aabb2D<Real>  TAabb;
	typedef prim::Line2D<Real>  TSeparator;

	static TAabb aabb(const TSimplePolygon2D& iP)
	{
		return prim::aabb(iP);
	}
	static bool contains( const TSimplePolygon2D& iP, const TPoint& iPoint)
	{
		return iP.classify(iPoint) == prim::sInside;
	}
};

template<>
struct QuadTreeTraits< TSimplePolygon3D >
{
	typedef double Real;
	typedef prim::Point3D<Real> TPoint;
	typedef prim::Aabb3D<Real>  TAabb;
	typedef prim::Plane3D<Real> TSeparator;

	static TAabb aabb(const TSimplePolygon3D& iP)
	{
		return prim::aabb(iP);
	}
	static bool contains( const TSimplePolygon3D& iP, const TPoint& iPoint)
	{
		return iP.classify(iPoint) == prim::sInside;
	}
};



void doTestQuadTree()
{
	typedef TSimplePolygon2D::TPoint TPoint;
	typedef TPoint::TVector TVector;
	typedef lass::spat::QuadTree< TSimplePolygon2D, QuadTreeTraits >    TQuadTree;

	TQuadTree   tree( TPoint(50,50), TVector(50,50));
	LASS_TEST_CHECK_EQUAL( tree.objectCount(), 0 );

	const int objectsToAdd = 100;
	const int maxList = 2;
	const int maxDepth = 5;
	for (int i=0;i<objectsToAdd;++i)
	{
		TSimplePolygon2D tempPoly;
		tempPoly.add( TPoint(10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX));
		tempPoly.add( TPoint(10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX));
		tempPoly.add( TPoint(10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX));
		tree.add(tempPoly, maxList, maxDepth);
	}

	LASS_TEST_CHECK_EQUAL( tree.objectCount(), objectsToAdd );
	LASS_TEST_CHECK( tree.maxDepth() <= maxDepth );

	std::vector< TSimplePolygon2D* >    resultQuery;
	for (int i=0;i<objectsToAdd;++i)
	{
		resultQuery.clear();
		int hitcount = tree.contains( TPoint(10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX), resultQuery);
		LASS_TEST_CHECK_EQUAL( hitcount, resultQuery.size() );
	}


	LASS_COUT << "Quad tree max depth : " << tree.maxDepth() << std::endl;
}

void doTestOctTree()
{
	typedef TSimplePolygon3D::TPoint TPoint;
	typedef TPoint::TVector TVector;
	typedef lass::spat::QuadTree< TSimplePolygon3D, QuadTreeTraits > TOctTree;

	TOctTree    tree( TPoint(50,50,50), TVector(50,50,50));
	LASS_TEST_CHECK_EQUAL( tree.objectCount(), 0 );

	const int objectsToAdd = 100;
	const int maxList = 2;
	const int maxDepth = 5;
	for (int i=0;i<objectsToAdd;++i)
	{
		TSimplePolygon3D tempPoly(
			( TPoint(10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX)),
			( TPoint(10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX)),
			( TPoint(10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX))
			);
		tree.add(tempPoly, maxList, maxDepth);
	}

	LASS_TEST_CHECK_EQUAL( tree.objectCount(), objectsToAdd );
	LASS_TEST_CHECK( tree.maxDepth() <= maxDepth );

	std::vector< TSimplePolygon3D* >    resultQuery;
	for (int i=0;i<objectsToAdd;++i)
	{
		resultQuery.clear();
		int hitcount = tree.contains( TPoint(10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX,10.0+80.0*(double)rand()/(double)RAND_MAX), resultQuery);
		LASS_TEST_CHECK_EQUAL( hitcount, resultQuery.size() );
	}

	LASS_COUT << "Oct tree max depth : " << tree.maxDepth() << std::endl;
}
}
}
