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
#include "test_spat.h"

#include "test_spat_object_trees.inl"
#include "test_spat_kd_tree.inl"
#include "test_spat_quad_tree.inl"
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

	//LASS_WARNING( "Tests for planar mesh and mesh interpolator are not active." );
	result.push_back(LASS_UNIT_TEST(doTestPlanarMesh));
	result.push_back(LASS_UNIT_TEST(doTestMeshInterpolator));

	TTestCase objectTreesFloat2 = testSpatObjectTrees<float, 2>;
	TTestCase objectTreesFloat3 = testSpatObjectTrees<float, 3>;
	TTestCase objectTreesDouble2 = testSpatObjectTrees<double, 2>;
	TTestCase objectTreesDouble3 = testSpatObjectTrees<double, 3>;
	result.push_back(LASS_UNIT_TEST(objectTreesFloat2));
	result.push_back(LASS_UNIT_TEST(objectTreesFloat3));
	result.push_back(LASS_UNIT_TEST(objectTreesDouble2));
	result.push_back(LASS_UNIT_TEST(objectTreesDouble3));

	result.push_back(LASS_UNIT_TEST(testSpatKdTree<prim::Point2D<float> >));
	result.push_back(LASS_UNIT_TEST(testSpatKdTree<prim::Point3D<float> >));
	result.push_back(LASS_UNIT_TEST(testSpatKdTree<prim::Point2D<double> >));
	result.push_back(LASS_UNIT_TEST(testSpatKdTree<prim::Point3D<double> >));

	result.push_back(LASS_UNIT_TEST(doTestQuadTree));
	result.push_back(LASS_UNIT_TEST(doTestOctTree));

	return result;
}



}

}
