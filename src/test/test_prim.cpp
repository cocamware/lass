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
	result.push_back(LASS_UNIT_TEST((testPrimSphere3D<Type__>)));\
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
