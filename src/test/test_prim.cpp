/**	@file
 *  @internal
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



#include "test_common.h"
#include "test_prim.h"

#include "test_prim_aabb.inl"
#include "test_prim_line_2d.inl"
#include "test_prim_line_3d.inl"
#include "test_prim_line_segments.inl"
#include "test_prim_plane_3d.inl"
#include "test_prim_points.inl"
#include "test_prim_ray.inl"
#include "test_prim_sphere_3d.inl"
//#include "test_prim_simple_polygon_2d.inl"
#include "test_prim_transformation_3d.inl"
#include "test_prim_vectors.inl"
#include "test_prim_xml.inl"

#define LASS_TEST_PRIM_ADD_TEST_CASES(Type__)\
	result->add(BOOST_TEST_CASE(testPrimAabb2D<Type__>));\
	result->add(BOOST_TEST_CASE(testPrimAabb3D<Type__>));\
    result->add(BOOST_TEST_CASE((testPrimLine2D<Type__, prim::Unnormalized>)));\
    result->add(BOOST_TEST_CASE((testPrimLine2D<Type__,prim::Normalized>)));\
    result->add(BOOST_TEST_CASE((testPrimLine3D<Type__, prim::Unnormalized>)));\
    result->add(BOOST_TEST_CASE((testPrimLine3D<Type__,prim::Normalized>)));\
    result->add(BOOST_TEST_CASE(testPrimLineSegment2D<Type__>));\
    result->add(BOOST_TEST_CASE(testPrimLineSegment3D<Type__>));\
    result->add(BOOST_TEST_CASE((testPrimPlane3D<Type__,prim::Normalized>)));\
    result->add(BOOST_TEST_CASE((testPrimPlane3D<Type__,prim::Unnormalized>)));\
	result->add(BOOST_TEST_CASE(testPrimPoint2D<Type__>));\
	result->add(BOOST_TEST_CASE(testPrimPoint3D<Type__>));\
	result->add(BOOST_TEST_CASE(testPrimPoint3DH<Type__>));\
    result->add(BOOST_TEST_CASE((testPrimRay2D<Type__, prim::Unnormalized, prim::Unbounded>)));\
    result->add(BOOST_TEST_CASE((testPrimRay2D<Type__, prim::Unnormalized, prim::Bounded>)));\
    result->add(BOOST_TEST_CASE((testPrimRay2D<Type__, prim::Normalized, prim::Unbounded>)));\
    result->add(BOOST_TEST_CASE((testPrimRay2D<Type__, prim::Normalized, prim::Bounded>)));\
    result->add(BOOST_TEST_CASE((testPrimRay3D<Type__, prim::Unnormalized, prim::Unbounded>)));\
    result->add(BOOST_TEST_CASE((testPrimRay3D<Type__, prim::Unnormalized, prim::Bounded>)));\
    result->add(BOOST_TEST_CASE((testPrimRay3D<Type__, prim::Normalized, prim::Unbounded>)));\
    result->add(BOOST_TEST_CASE((testPrimRay3D<Type__, prim::Normalized, prim::Bounded>)));\
    result->add(BOOST_TEST_CASE(testPrimSphere3D<Type__>));\
    /*result->add(BOOST_TEST_CASE((testPrimSimplePolygon2D<Type__, prim::NoDegenerate>)));\
    result->add(BOOST_TEST_CASE((testPrimSimplePolygon2D<Type__, prim::StrictNoDegenerate>)));\
    result->add(BOOST_TEST_CASE((testPrimSimplePolygon2D<Type__, prim::AllowDegenerate>)));*/\
    result->add(BOOST_TEST_CASE(testPrimTransformation3D<Type__>));\
    result->add(BOOST_TEST_CASE(testPrimVector2D<Type__>));\
	result->add(BOOST_TEST_CASE(testPrimVector3D<Type__>));\
	result->add(BOOST_TEST_CASE(testPrimVector4D<Type__>));\
    /**/

namespace lass
{
namespace test
{

boost::unit_test_framework::test_suite* testPrim()
{
    boost::unit_test_framework::test_suite* result = BOOST_TEST_SUITE("lass::prim test suite");

    LASS_TEST_PRIM_ADD_TEST_CASES(float);
    LASS_TEST_PRIM_ADD_TEST_CASES(double);
	result->add(BOOST_TEST_CASE(testPrimXml));

    return result;
}

}

}

// EOF
