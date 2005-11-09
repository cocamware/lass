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
#include "test_prim.h"

#include "test_prim_aabb.inl"
#include "test_prim_line_2d.inl"
#include "test_prim_line_3d.inl"
#include "test_prim_line_segments.inl"
#include "test_prim_plane_3d.inl"
#include "test_prim_points.inl"
#include "test_prim_ray.inl"
#include "test_prim_sphere_3d.inl"
#include "test_prim_simple_polygon_2d.inl"
#include "test_prim_transformation_3d.inl"
#include "test_prim_vectors.inl"
#include "test_prim_xml.inl"

#define LASS_TEST_PRIM_ADD_TEST_CASES(Type__)\
	result.push_back(LASS_UNIT_TEST(testPrimAabb2D<Type__>));\
	result.push_back(LASS_UNIT_TEST(testPrimAabb3D<Type__>));\
	result.push_back(LASS_UNIT_TEST((testPrimLine2D<Type__, prim::Unnormalized>)));\
	result.push_back(LASS_UNIT_TEST((testPrimLine2D<Type__,prim::Normalized>)));\
	result.push_back(LASS_UNIT_TEST((testPrimLine3D<Type__, prim::Unnormalized>)));\
	result.push_back(LASS_UNIT_TEST((testPrimLine3D<Type__,prim::Normalized>)));\
	result.push_back(LASS_UNIT_TEST(testPrimLineSegment2D<Type__>));\
	result.push_back(LASS_UNIT_TEST(testPrimLineSegment3D<Type__>));\
	result.push_back(LASS_UNIT_TEST((testPrimPlane3D<Type__,prim::Normalized>)));\
	result.push_back(LASS_UNIT_TEST((testPrimPlane3D<Type__,prim::Unnormalized>)));\
	result.push_back(LASS_UNIT_TEST(testPrimPoint2D<Type__>));\
	result.push_back(LASS_UNIT_TEST(testPrimPoint3D<Type__>));\
	result.push_back(LASS_UNIT_TEST(testPrimPoint3DH<Type__>));\
	result.push_back(LASS_UNIT_TEST((testPrimRay2D<Type__, prim::Unnormalized, prim::Unbounded>)));\
	result.push_back(LASS_UNIT_TEST((testPrimRay2D<Type__, prim::Unnormalized, prim::Bounded>)));\
	result.push_back(LASS_UNIT_TEST((testPrimRay2D<Type__, prim::Normalized, prim::Unbounded>)));\
	result.push_back(LASS_UNIT_TEST((testPrimRay2D<Type__, prim::Normalized, prim::Bounded>)));\
	result.push_back(LASS_UNIT_TEST((testPrimRay3D<Type__, prim::Unnormalized, prim::Unbounded>)));\
	result.push_back(LASS_UNIT_TEST((testPrimRay3D<Type__, prim::Unnormalized, prim::Bounded>)));\
	result.push_back(LASS_UNIT_TEST((testPrimRay3D<Type__, prim::Normalized, prim::Unbounded>)));\
	result.push_back(LASS_UNIT_TEST((testPrimRay3D<Type__, prim::Normalized, prim::Bounded>)));\
	result.push_back(LASS_UNIT_TEST(testPrimSphere3D<Type__>));\
	result.push_back(LASS_UNIT_TEST((testPrimSimplePolygon2D<Type__, prim::NoDegenerate>)));\
	result.push_back(LASS_UNIT_TEST((testPrimSimplePolygon2D<Type__, prim::StrictNoDegenerate>)));\
	result.push_back(LASS_UNIT_TEST((testPrimSimplePolygon2D<Type__, prim::AllowDegenerate>)));\
	result.push_back(LASS_UNIT_TEST(testPrimTransformation3D<Type__>));\
	result.push_back(LASS_UNIT_TEST(testPrimVector2D<Type__>));\
	result.push_back(LASS_UNIT_TEST(testPrimVector3D<Type__>));\
	result.push_back(LASS_UNIT_TEST(testPrimVector4D<Type__>));\
	/**/

namespace lass
{
namespace test
{

TUnitTests testPrim()
{
	TUnitTests result;

	LASS_TEST_PRIM_ADD_TEST_CASES(float);
	LASS_TEST_PRIM_ADD_TEST_CASES(double);
	result.push_back(LASS_UNIT_TEST(testPrimXml));

	return result;
}

}

}

// EOF
