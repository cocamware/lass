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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_SPHERE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_SPHERE_3D_INL

#include "test_common.h"

#include "../prim/sphere_3d.h"

namespace lass
{
namespace test
{

template
<
	typename T
>
void testPrimSphere3D()
{
	typedef prim::Sphere3D<T> TSphere;
	typedef prim::Point3D<T> TPoint;
	typedef prim::Vector3D<T> TVector;
	typedef num::NumTraits<T> TNumTraits;

	const TPoint origin;

	TSphere sphere;
	LASS_TEST_CHECK(sphere.isValid());
	LASS_TEST_CHECK_EQUAL(sphere.center(), origin);
	LASS_TEST_CHECK_EQUAL(sphere.radius(), 0);
	LASS_TEST_CHECK_EQUAL(sphere.area(), 0);
	LASS_TEST_CHECK_EQUAL(sphere.volume(), 0);
	LASS_TEST_CHECK_EQUAL(sphere.classify(origin), prim::sSurface);
	LASS_TEST_CHECK_EQUAL(sphere.equation(origin), 0);
	LASS_TEST_CHECK_EQUAL(sphere.signedDistance(origin), 0);

	const TPoint center(1, 2, 3);
	const T radius = 4;

	sphere = TSphere(center, radius);
	LASS_TEST_CHECK(sphere.isValid());
	LASS_TEST_CHECK_EQUAL(sphere.center(), center);
	LASS_TEST_CHECK_EQUAL(sphere.radius(), radius);
	LASS_TEST_CHECK_EQUAL(sphere.area(), T(4) * TNumTraits::pi * num::sqr(radius));
	LASS_TEST_CHECK_EQUAL(sphere.volume(), T(4) / T(3) * TNumTraits::pi * num::sqr(radius) * radius);
	LASS_TEST_CHECK_EQUAL(sphere.classify(center), prim::sInside);
	LASS_TEST_CHECK_EQUAL(sphere.equation(center), -num::sqr(radius));
	LASS_TEST_CHECK_EQUAL(sphere.signedDistance(center), -radius);
}

}

}

#endif
