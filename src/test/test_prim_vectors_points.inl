/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/
/** @file test/test_prim_vectors_points.inl
 *  @brief test beds for lass::prim vectors, affine points and homogenous points.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_VECTORS_POINTS_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_VECTORS_POINTS_INL

#include "test_prim_vectors_points.inl"

#include "../prim/vector_2d.h"
#include "../prim/vector_3d.h"
#include "../prim/vector_4d.h"
#include "../prim/point_2d.h"
#include "../prim/point_3d.h"
#include "../prim/point_2dh.h"
#include "../prim/point_3dh.h"

#define LASS_TEST_PRIM_VECTORS_POINTS_EPSILON 1e-6

namespace lass
{
namespace test
{




template <typename T> void testPrimPoint2D()
{
	BOOST_MESSAGE("testing lass::prim::Point2D<" << num::NumTraits<T>::name() << "> ...");
	using namespace prim;

	Point2D<T> p;
	BOOST_CHECK(p.isZero());

	Vector2D<T> a(1, 2);
	Vector2D<T> b(4, 5);
	
	p = Point2D<T>(a.x, a.y);
	BOOST_CHECK_EQUAL(p.position, a);
	BOOST_CHECK_EQUAL(p.x, a.x);
	BOOST_CHECK_EQUAL(p.y, a.y);

	Point2D<T> q(b);
	BOOST_CHECK_EQUAL(q.position, b);

	BOOST_CHECK_EQUAL(distance(p, q), (a - b).norm());
	BOOST_CHECK_EQUAL((p + b).position, a + b);
	BOOST_CHECK((q - b).isZero());
}



template <typename T> void testPrimPoint3D()
{
	BOOST_MESSAGE("testing lass::prim::Point3D<" << num::NumTraits<T>::name() << "> ...");
	using namespace prim;

	Point3D<T> p;
	BOOST_CHECK(p.isZero());

	Vector3D<T> a(1, 2, 3);
	Vector3D<T> b(4, 5, 6);
	
	p = Point3D<T>(a.x, a.y, a.z);
	BOOST_CHECK_EQUAL(p.position, a);
	BOOST_CHECK_EQUAL(p.x, a.x);
	BOOST_CHECK_EQUAL(p.y, a.y);
	BOOST_CHECK_EQUAL(p.z, a.z);

	Point3D<T> q(b);
	BOOST_CHECK_EQUAL(q.position, b);

	BOOST_CHECK_EQUAL(distance(p, q), (a - b).norm());
	BOOST_CHECK_EQUAL((p + b).position, a + b);
	BOOST_CHECK((q - b).isZero());
}



template<typename T> void testPrimPoint3DH()
{
	BOOST_MESSAGE("testing lass::prim::Point3DH<" << num::NumTraits<T>::name() << "> ...");
	using namespace prim;
}



}

}

#endif

// EOF
