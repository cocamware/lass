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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_SIMPLE_POLYGON_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_SIMPLE_POLYGON_2D_INL

#include "test_common.h"

#include "../prim/simple_polygon_2d.h"
#include "../prim/aabb_2d.h"
#include "../prim/ray_3d.h"
#include "../meta/is_same_type.h"

#define LASS_TEST_PRIM_RAY_EPSILON 1e-5

namespace lass
{
namespace test
{

template
<
	typename T,
	class DegeneratePolicy
>
void testPrimSimplePolygon2D()
{
	typedef prim::SimplePolygon2D<T, DegeneratePolicy> TPolygon;
	typedef prim::Point2D<T> TPoint;
	typedef prim::Vector2D<T> TVector;
	typedef typename TVector::TNumTraits TNumTraits;

	const bool isNoDegenerate = meta::IsSameType<DegeneratePolicy, prim::NoDegenerate>::value;
	const bool isStrictNoDegenerate = meta::IsSameType<DegeneratePolicy, prim::StrictNoDegenerate>::value;

	// empty polygon
	//
	TPolygon polygon;
	BOOST_CHECK_THROW(polygon.at(37), util::Exception);
	BOOST_CHECK_THROW(polygon.edge(37), util::Exception);
	BOOST_CHECK_THROW(polygon.vector(37), util::Exception);
	BOOST_CHECK_THROW(polygon.insert(37, TPoint(51, 95)), util::Exception);
	BOOST_CHECK_THROW(polygon.erase(37), util::Exception);
	BOOST_CHECK(polygon.isEmpty());
	BOOST_CHECK_EQUAL(polygon.size(), 0);
	BOOST_CHECK_NO_THROW(polygon.signedArea());
	BOOST_CHECK_NO_THROW(polygon.area());
	BOOST_CHECK_EQUAL(polygon.signedArea(), TNumTraits::zero);
	BOOST_CHECK_EQUAL(polygon.area(), TNumTraits::zero);
	if (isStrictNoDegenerate || isNoDegenerate)
	{
	   BOOST_CHECK_THROW(polygon.orientation(), util::Exception);
	}
	else
	{
		BOOST_CHECK_NO_THROW(polygon.orientation());
		BOOST_CHECK_EQUAL(polygon.orientation(), prim::oInvalid);
	}
	BOOST_CHECK_EQUAL(polygon.perimeter(), TNumTraits::zero);
	BOOST_CHECK(!polygon.vertexCentroid().isValid());
	BOOST_CHECK(!polygon.surfaceCentroid().isValid());
	BOOST_CHECK(polygon.isSimple());
	BOOST_CHECK(polygon.isConvex());
	BOOST_CHECK_THROW(polygon.isReflex(37), util::Exception);
	BOOST_CHECK(!polygon.isValid());

	// one point
	//
	const TPoint p0(35, 64);
	BOOST_CHECK_NO_THROW(polygon.add(p0));
	BOOST_CHECK_EQUAL(polygon[0], p0);
	BOOST_CHECK_EQUAL(polygon.at(37), p0); // 37 => 0
	if (isNoDegenerate || isStrictNoDegenerate)
	{
		BOOST_CHECK_THROW(polygon.edge(37), util::Exception);
		BOOST_CHECK_THROW(polygon.vector(37), util::Exception);
	}
	else
	{
		BOOST_CHECK_NO_THROW(polygon.edge(37)); // 37 => 0
		BOOST_CHECK_NO_THROW(polygon.vector(37)); // 37 => 0
		BOOST_CHECK_EQUAL(polygon.edge(37).length(), TNumTraits::zero);
		BOOST_CHECK_EQUAL(polygon.vector(37).squaredNorm(), TNumTraits::zero);
	}
	BOOST_CHECK(!polygon.isEmpty());
	BOOST_CHECK_EQUAL(polygon.size(), 1);
	BOOST_CHECK_NO_THROW(polygon.signedArea());
	BOOST_CHECK_NO_THROW(polygon.area());
	BOOST_CHECK_EQUAL(polygon.signedArea(), TNumTraits::zero);
	BOOST_CHECK_EQUAL(polygon.area(), TNumTraits::zero);
	if (isStrictNoDegenerate || isNoDegenerate)
	{
	   BOOST_CHECK_THROW(polygon.orientation(), util::Exception);
	}
	else
	{
		BOOST_CHECK_NO_THROW(polygon.orientation());
		BOOST_CHECK_EQUAL(polygon.orientation(), prim::oInvalid);
	}
	BOOST_CHECK_EQUAL(polygon.perimeter(), TNumTraits::zero);
	BOOST_CHECK_EQUAL(polygon.vertexCentroid().affine(), p0);
	BOOST_CHECK_EQUAL(polygon.surfaceCentroid().affine(), p0);
	BOOST_CHECK(polygon.isSimple());
	BOOST_CHECK(polygon.isConvex());
	if (isStrictNoDegenerate || isNoDegenerate)
	{
		BOOST_CHECK_THROW(polygon.isReflex(37), util::Exception);
	}
	else
	{
		BOOST_CHECK_NO_THROW(polygon.isReflex(37));
		BOOST_CHECK_EQUAL(polygon.isReflex(37), false);
	}
	BOOST_CHECK(!polygon.isValid());

	BOOST_CHECK_NO_THROW(polygon.erase(37)); // 37 => 0
	BOOST_CHECK_NO_THROW(polygon.add(p0));
	BOOST_CHECK_NO_THROW(polygon.insert(37, TPoint(51, 95))); // 37 => 0
	BOOST_CHECK_EQUAL(polygon.size(), 2);
	BOOST_CHECK_NO_THROW(polygon.erase(37)); // 37 => 1

	// a vector of points
	//
	std::vector<TPoint> points;
	points.push_back(TPoint(2, 0));
	points.push_back(TPoint(8, 1));
	points.push_back(TPoint(10, 5));
	points.push_back(TPoint(5, 10));
	points.push_back(TPoint(0, 0));
	polygon = TPolygon(points.begin(), points.end());
	BOOST_CHECK_EQUAL(polygon.size(), 5);
	for (int i = 0; i < 5; ++i)
	{
		BOOST_CHECK_EQUAL(polygon[i], points[i]);
	}
	prim::Aabb2D<T> box = prim::aabb(polygon);
    BOOST_CHECK_EQUAL(box.min().x, 0); 
    BOOST_CHECK_EQUAL(box.min().y, 0); 
    BOOST_CHECK_EQUAL(box.max().x, 10); 
    BOOST_CHECK_EQUAL(box.max().y, 10); 
}

}

}

#endif

// EOF
