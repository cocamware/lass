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
#include "test_spat.h"

#include "test_spat_kd_tree.inl"
#include "test_spat_quad_tree.inl"
#include "test_spat_planar_mesh.inl"
#include "test_spat_mesh_interpolator.inl"

namespace lass
{
namespace test
{

boost::unit_test_framework::test_suite* testSpat()
{
    boost::unit_test_framework::test_suite* result = BOOST_TEST_SUITE("lass::spat test suite");

    result->add(BOOST_TEST_CASE(testSpatKdTree<prim::Point3D<float> >));

	//LASS_WARNING( "Tests for planar mesh and mesh interpolator are not active." );
    result->add(BOOST_TEST_CASE(doTestPlanarMesh));
    result->add(BOOST_TEST_CASE(doTestMeshInterpolator));
    result->add(BOOST_TEST_CASE(doTestQuadTree));
    result->add(BOOST_TEST_CASE(doTestOctTree));

    return result;
}



}

}
