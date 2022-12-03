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
 *	Copyright (C) 2004-2022 the Initial Developer.
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

#include "../lass/prim/simple_polygon_2d.h"
#include "../lass/prim/aabb_2d.h"
#include "../lass/prim/ray_3d.h"
#include "../lass/meta/is_same.h"

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

	const bool isNoDegenerate = meta::IsSame<DegeneratePolicy, prim::NoDegenerate>::value;
	const bool isStrictNoDegenerate = meta::IsSame<DegeneratePolicy, prim::StrictNoDegenerate>::value;

	// empty polygon
	//
	TPolygon polygon;
	LASS_TEST_CHECK_THROW(polygon.at(37), util::Exception);
	LASS_TEST_CHECK_THROW(polygon.edge(37), util::Exception);
	LASS_TEST_CHECK_THROW(polygon.vector(37), util::Exception);
	LASS_TEST_CHECK_THROW(polygon.insert(37, TPoint(51, 95)), util::Exception);
	LASS_TEST_CHECK_THROW(polygon.erase(37), util::Exception);
	LASS_TEST_CHECK(polygon.isEmpty());
	LASS_TEST_CHECK_EQUAL(polygon.size(), static_cast<size_t>(0));
	LASS_TEST_CHECK_NO_THROW(polygon.signedArea());
	LASS_TEST_CHECK_NO_THROW(polygon.area());
	LASS_TEST_CHECK_EQUAL(polygon.signedArea(), TNumTraits::zero);
	LASS_TEST_CHECK_EQUAL(polygon.area(), TNumTraits::zero);
	if (isStrictNoDegenerate || isNoDegenerate)
	{
		LASS_TEST_CHECK_THROW(polygon.orientation(), util::Exception);
	}
	else
	{
		LASS_TEST_CHECK_NO_THROW(polygon.orientation());
		LASS_TEST_CHECK_EQUAL(polygon.orientation(), prim::oInvalid);
	}
	LASS_TEST_CHECK_EQUAL(polygon.perimeter(), TNumTraits::zero);
	LASS_TEST_CHECK(!polygon.vertexCentroid().isValid());
	LASS_TEST_CHECK(!polygon.surfaceCentroid().isValid());
	LASS_TEST_CHECK(polygon.isSimple());
	LASS_TEST_CHECK(polygon.isConvex());
	LASS_TEST_CHECK_THROW(polygon.isReflex(37), util::Exception);
	LASS_TEST_CHECK(!polygon.isValid());

	// one point
	//
	const TPoint p0(35, 64);
	LASS_TEST_CHECK_NO_THROW(polygon.add(p0));
	LASS_TEST_CHECK_EQUAL(polygon[0], p0);
	LASS_TEST_CHECK_EQUAL(polygon.at(37), p0); // 37 => 0
	if (isNoDegenerate || isStrictNoDegenerate)
	{
		LASS_TEST_CHECK_THROW(polygon.edge(37), util::Exception);
		LASS_TEST_CHECK_THROW(polygon.vector(37), util::Exception);
	}
	else
	{
		LASS_TEST_CHECK_NO_THROW(polygon.edge(37)); // 37 => 0
		LASS_TEST_CHECK_NO_THROW(polygon.vector(37)); // 37 => 0
		LASS_TEST_CHECK_EQUAL(polygon.edge(37).length(), TNumTraits::zero);
		LASS_TEST_CHECK_EQUAL(polygon.vector(37).squaredNorm(), TNumTraits::zero);
	}
	LASS_TEST_CHECK(!polygon.isEmpty());
	LASS_TEST_CHECK_EQUAL(polygon.size(), static_cast<size_t>(1));
	LASS_TEST_CHECK_NO_THROW(polygon.signedArea());
	LASS_TEST_CHECK_NO_THROW(polygon.area());
	LASS_TEST_CHECK_EQUAL(polygon.signedArea(), TNumTraits::zero);
	LASS_TEST_CHECK_EQUAL(polygon.area(), TNumTraits::zero);
	if (isStrictNoDegenerate || isNoDegenerate)
	{
		LASS_TEST_CHECK_THROW(polygon.orientation(), util::Exception);
	}
	else
	{
		LASS_TEST_CHECK_NO_THROW(polygon.orientation());
		LASS_TEST_CHECK_EQUAL(polygon.orientation(), prim::oInvalid);
	}
	LASS_TEST_CHECK_EQUAL(polygon.perimeter(), TNumTraits::zero);
	LASS_TEST_CHECK_EQUAL(polygon.vertexCentroid().affine(), p0);
	LASS_TEST_CHECK_EQUAL(polygon.surfaceCentroid().affine(), p0);
	LASS_TEST_CHECK(polygon.isSimple());
	LASS_TEST_CHECK(polygon.isConvex());
	if (isStrictNoDegenerate || isNoDegenerate)
	{
		LASS_TEST_CHECK_THROW(polygon.isReflex(37), util::Exception);
	}
	else
	{
		LASS_TEST_CHECK_NO_THROW(polygon.isReflex(37));
		LASS_TEST_CHECK_EQUAL(polygon.isReflex(37), false);
	}
	LASS_TEST_CHECK(!polygon.isValid());

	LASS_TEST_CHECK_NO_THROW(polygon.erase(37)); // 37 => 0
	LASS_TEST_CHECK_NO_THROW(polygon.add(p0));
	LASS_TEST_CHECK_NO_THROW(polygon.insert(37, TPoint(51, 95))); // 37 => 0
	LASS_TEST_CHECK_EQUAL(polygon.size(), static_cast<size_t>(2));
	LASS_TEST_CHECK_NO_THROW(polygon.erase(37)); // 37 => 1

	// a vector of points
	//
	std::vector<TPoint> points;
	points.push_back(TPoint(2, 0));
	points.push_back(TPoint(8, 1));
	points.push_back(TPoint(10, 5));
	points.push_back(TPoint(5, 10));
	points.push_back(TPoint(0, 0));
	polygon = TPolygon(points.begin(), points.end());
	LASS_TEST_CHECK_EQUAL(polygon.size(), static_cast<size_t>(5));
	for (size_t i = 0; i < 5; ++i)
	{
		LASS_TEST_CHECK_EQUAL(polygon[i], points[i]);
	}
	LASS_TEST_CHECK(polygon.isSimple());
	prim::Aabb2D<T> box = prim::aabb(polygon);
    LASS_TEST_CHECK_EQUAL(box.min().x, static_cast<T>(0));
    LASS_TEST_CHECK_EQUAL(box.min().y, static_cast<T>(0));
    LASS_TEST_CHECK_EQUAL(box.max().x, static_cast<T>(10));
    LASS_TEST_CHECK_EQUAL(box.max().y, static_cast<T>(10));

	// non-simple polygons
	{
		polygon = TPolygon({ TPoint(0,0), TPoint(10, 10), TPoint(10, 0), TPoint(0, 10) });
		LASS_TEST_CHECK_EQUAL(polygon.size(), static_cast<size_t>(4));
		LASS_TEST_CHECK(!polygon.isSimple());
	}
	{
		polygon = TPolygon({ TPoint(-6,-8), TPoint(0, 10), TPoint(6, -8), TPoint(-10, 3), TPoint(10, 3)});
		LASS_TEST_CHECK_EQUAL(polygon.size(), static_cast<size_t>(5));
		LASS_TEST_CHECK(!polygon.isSimple());
	}
}



TUnitTest test_prim_simple_polygon_2d()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE((testPrimSimplePolygon2D<float, prim::NoDegenerate>)));
	result.push_back(LASS_TEST_CASE((testPrimSimplePolygon2D<float, prim::StrictNoDegenerate>)));
	result.push_back(LASS_TEST_CASE((testPrimSimplePolygon2D<float, prim::AllowDegenerate>)));
	result.push_back(LASS_TEST_CASE((testPrimSimplePolygon2D<double, prim::NoDegenerate>)));
	result.push_back(LASS_TEST_CASE((testPrimSimplePolygon2D<double, prim::StrictNoDegenerate>)));
	result.push_back(LASS_TEST_CASE((testPrimSimplePolygon2D<double, prim::AllowDegenerate>)));
	return result;
}

}

}

// EOF
