/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_LINE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_LINE_3D_INL

#include "test_common.h"

#include "../prim/line_3d.h"
#include "../meta/is_same_type.h"

#define LASS_TEST_PRIM_LINE_3D_EPSILON 1e-5

namespace lass
{
namespace test
{

template <typename T, class NormalizingPolicy>
void testPrimLine3D()
{
	typedef prim::Line3D<T, NormalizingPolicy> TLine;
	typedef prim::Point3D<T> TPoint;
	typedef prim::Vector3D<T> TVector;

	const T epsilon =T(LASS_TEST_PRIM_LINE_3D_EPSILON);

	const bool isNormalized = meta::IsSameType<NormalizingPolicy, prim::Normalized>::value;

	TLine line;
	LASS_TEST_CHECK(!line.isValid());

	TPoint support(64, 37, 18);
	TVector direction(15, 59, 67);
	line = TLine(support, direction);
	LASS_TEST_CHECK(line.isValid());
	LASS_TEST_CHECK_EQUAL(line.support(), support);
	LASS_TEST_CHECK_EQUAL(line.direction(), isNormalized ? direction.normal() : direction);

	support = TPoint(68, 46, 30);
	line.support() = support;
	LASS_TEST_CHECK_EQUAL(line.support(), support);
	LASS_TEST_CHECK_EQUAL(line.direction(), isNormalized ? direction.normal() : direction);

	direction = TVector(38, 73, 68);
	line.setDirection(direction);
	LASS_TEST_CHECK_EQUAL(line.support(), support);
	LASS_TEST_CHECK_EQUAL(line.direction(), isNormalized ? direction.normal() : direction);

	TPoint lookAt(86, 15, 69);
	line = TLine(support, lookAt);
	LASS_TEST_CHECK_EQUAL(line.support(), support);
	LASS_TEST_CHECK_EQUAL(line.direction(), isNormalized ? (lookAt - support).normal() : (lookAt - support));

	lookAt = TPoint(24, 93, 78);
	line.lookAt(lookAt);
	LASS_TEST_CHECK_EQUAL(line.support(), support);
	LASS_TEST_CHECK_EQUAL(line.direction(), isNormalized ? (lookAt - support).normal() : (lookAt - support));

	TPoint p(83, 74, 38);
	TPoint v(36, 91, 23);
	LASS_TEST_CHECK_CLOSE_ARRAY(line.project(p) + line.reject(p), p, epsilon, 3);
	LASS_TEST_CHECK_CLOSE_ARRAY(line.project(v) + line.reject(v), v, epsilon, 3);
	LASS_TEST_CHECK_CLOSE_ARRAY(line.reflect(line.reflect(p)), p, epsilon, 3);
	LASS_TEST_CHECK_CLOSE_ARRAY(line.reflect(line.reflect(v)), v, epsilon, 3);

	LASS_TEST_CHECK_EQUAL(line.point(T(0)), line.support());
	LASS_TEST_CHECK_EQUAL(line.point(T(1)), line.support() + line.direction());
	LASS_TEST_CHECK_EQUAL(line.point(T(-1)), line.support() - line.direction());
	LASS_TEST_CHECK_EQUAL(line.t(line.support()), T(0));
	LASS_TEST_CHECK_CLOSE(line.t(line.support() + line.direction()), T(1), epsilon);
	LASS_TEST_CHECK_CLOSE(line.t(line.support() - line.direction()), T(-1), epsilon);
	LASS_TEST_CHECK_CLOSE_ARRAY(line.project(p), line.point(line.t(p)), epsilon, 3);
}

}

}

#endif
