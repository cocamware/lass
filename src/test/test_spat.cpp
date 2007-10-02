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
#include "test_spat.h"

// due to a unfortunate design of num/basic_ops, we have a problem using classes that overload them
// the two-phase name lookup doesn't work because we always use qualified names when calling basic_ops
// therefore, we need to make sure all overloads are known before any template that uses them is defined
// So, include this as early as possible ...
//
#include "../num/floating_point_consistency.h"

#include "test_spat_object_trees.inl"
#include "test_spat_kd_tree.inl"
//#include "test_spat_quad_tree.inl"
#include "test_spat_planar_mesh.inl"
#include "test_spat_mesh_interpolator.inl"

namespace lass
{
namespace test
{

TUnitTests testSpat()
{
	TUnitTests result;

	typedef void(*TTestCase)();
	TTestCase objectTreesFloat2 = testSpatObjectTrees<float, 2>;
	TTestCase objectTreesFloat3 = testSpatObjectTrees<float, 3>;
	TTestCase objectTreesDouble2 = testSpatObjectTrees<double, 2>;
	TTestCase objectTreesDouble3 = testSpatObjectTrees<double, 3>;
	result.push_back(LASS_UNIT_TEST(objectTreesDouble2));
	result.push_back(LASS_UNIT_TEST(objectTreesDouble3));
	result.push_back(LASS_UNIT_TEST(objectTreesFloat2));
	result.push_back(LASS_UNIT_TEST(objectTreesFloat3));

	result.push_back(LASS_UNIT_TEST(testSpatKdTree<prim::Point2D<float> >));
	result.push_back(LASS_UNIT_TEST(testSpatKdTree<prim::Point3D<float> >));
	result.push_back(LASS_UNIT_TEST(testSpatKdTree<prim::Point2D<double> >));
	result.push_back(LASS_UNIT_TEST(testSpatKdTree<prim::Point3D<double> >));

	result.push_back(LASS_UNIT_TEST(doTestPlanarMesh));
	result.push_back(LASS_UNIT_TEST(doTestMeshInterpolator));

	//result.push_back(LASS_UNIT_TEST(doTestQuadTree));
	//result.push_back(LASS_UNIT_TEST(doTestOctTree));

	return result;
}



}

}
