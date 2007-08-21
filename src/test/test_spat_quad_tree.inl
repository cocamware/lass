/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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
		LASS_TEST_CHECK_EQUAL( hitcount, static_cast<int>(resultQuery.size()) );
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
		LASS_TEST_CHECK_EQUAL( hitcount, static_cast<int>(resultQuery.size()) );
	}

	LASS_COUT << "Oct tree max depth : " << tree.maxDepth() << std::endl;
}
}
}
