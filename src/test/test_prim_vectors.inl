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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_VECTORS_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_VECTORS_INL

#include "test_common.h"

#include "../prim/vector_2d.h"
#include "../prim/vector_3d.h"
#include "../prim/vector_4d.h"

#define LASS_TEST_PRIM_VECTORS_EPSILON 1e-4

namespace lass
{
namespace test
{

template <typename T> void testPrimVector2D()
{
	using namespace prim;

	const T a0 = T(1), a1 = T(2);
	const T b0 = T(3), b1 = T(4);
	const T one = Vector2D<T>::TNumTraits::one;
	const T two = T(2), half = one / two;
	const T epsilon = T(LASS_TEST_PRIM_VECTORS_EPSILON);

	Vector2D<T> a;
	LASS_TEST_CHECK(a.isZero());

	a = Vector2D<T>(a0, a1);
	LASS_TEST_CHECK_EQUAL(a.x, a0);
	LASS_TEST_CHECK_EQUAL(a.y, a1);
	LASS_TEST_CHECK_EQUAL(a.squaredNorm(), (a0 * a0 + a1 * a1));
	LASS_TEST_CHECK_CLOSE(a.norm() * a.norm(), a.squaredNorm(), epsilon);

	Vector2D<T> b(b0, b1);
	LASS_TEST_CHECK_EQUAL(b[0], b.x);
	LASS_TEST_CHECK_EQUAL(b[1], b.y);
	LASS_TEST_CHECK_EQUAL(b.at(-3), b.y);
	LASS_TEST_CHECK_EQUAL(b.at(-2), b.x);
	LASS_TEST_CHECK_EQUAL(b.at(-1), b.y);

	LASS_TEST_CHECK_EQUAL(a + b, Vector2D<T>(a0 + b0, a1 + b1));
	LASS_TEST_CHECK_EQUAL(a - b, Vector2D<T>(a0 - b0, a1 - b1));
	LASS_TEST_CHECK_EQUAL(a * b, Vector2D<T>(a0 * b0, a1 * b1));
	LASS_TEST_CHECK_EQUAL(a / b, Vector2D<T>(a0 / b0, a1 / b1));

	LASS_TEST_CHECK_EQUAL(a + b, b + a);
	LASS_TEST_CHECK_EQUAL(a * b, b * a);
	LASS_TEST_CHECK_EQUAL(two * a, a * two);
	LASS_TEST_CHECK_EQUAL(two * a, a + a);
	LASS_TEST_CHECK_EQUAL(a / two, half * a);

	LASS_TEST_CHECK_EQUAL(dot(a, a), a.squaredNorm());
	LASS_TEST_CHECK_EQUAL(dot(a, b), dot(b, a));
	LASS_EVAL(a);
	LASS_EVAL(b);
	LASS_EVAL(a.project(b));
	LASS_EVAL(a.reject(b));
	LASS_EVAL(a.project(b) + a.reject(b));
	const Vector2D<T> project_plus_reject = a.project(b) + a.reject(b); // compile fucks up
	LASS_TEST_CHECK_EQUAL(project_plus_reject, b);
	LASS_TEST_CHECK(dot(a.project(b), a.reject(b)) < epsilon);

	Vector2D<T> n(b);
	n.normalize();
	LASS_TEST_CHECK_EQUAL(n, b.normal());
	LASS_TEST_CHECK_EQUAL(n.norm(), one);

	std::stringstream ss;
	ss << a;
	ss >> b;
	LASS_TEST_CHECK_EQUAL(a, b);
}



template <typename T> void testPrimVector3D()
{
	using namespace prim;

	const T a0 = T(1), a1 = T(2), a2 = T(3);
	const T b0 = T(4), b1 = T(5), b2 = T(6);
	const T zero = Vector3D<T>::TNumTraits::zero;
	const T one = Vector3D<T>::TNumTraits::one;
	const T two = T(2), half = one / two;
	const T epsilon = T(LASS_TEST_PRIM_VECTORS_EPSILON);

	Vector3D<T> a;
	LASS_TEST_CHECK(a.isZero());

	a = Vector3D<T>(a0, a1, a2);
	LASS_TEST_CHECK_EQUAL(a.x, a0);
	LASS_TEST_CHECK_EQUAL(a.y, a1);
	LASS_TEST_CHECK_EQUAL(a.z, a2);

	LASS_TEST_CHECK_EQUAL(a.squaredNorm(), (a0 * a0 + a1 * a1 + a2 * a2));
	LASS_TEST_CHECK_CLOSE(a.norm() * a.norm(), a.squaredNorm(), epsilon);

	Vector3D<T> b(b0, b1, b2);
	LASS_TEST_CHECK_EQUAL(b[0], b.x);
	LASS_TEST_CHECK_EQUAL(b[1], b.y);
	LASS_TEST_CHECK_EQUAL(b[2], b.z);
	LASS_TEST_CHECK_EQUAL(b.at(-3), b.x);
	LASS_TEST_CHECK_EQUAL(b.at(-2), b.y);
	LASS_TEST_CHECK_EQUAL(b.at(-1), b.z);

	LASS_TEST_CHECK_EQUAL(a + b, Vector3D<T>(a0 + b0, a1 + b1, a2 + b2));
	LASS_TEST_CHECK_EQUAL(a - b, Vector3D<T>(a0 - b0, a1 - b1, a2 - b2));
	LASS_TEST_CHECK_EQUAL(a * b, Vector3D<T>(a0 * b0, a1 * b1, a2 * b2));
	LASS_TEST_CHECK_EQUAL(a / b, Vector3D<T>(a0 / b0, a1 / b1, a2 / b2));

	LASS_TEST_CHECK_EQUAL(a + b, b + a);
	LASS_TEST_CHECK_EQUAL(a * b, b * a);
	LASS_TEST_CHECK_EQUAL(two * a, a * two);
	LASS_TEST_CHECK_EQUAL(two * a, a + a);
	LASS_TEST_CHECK_EQUAL(a / two, half * a);

	LASS_TEST_CHECK_EQUAL(dot(a, a), a.squaredNorm());
	LASS_TEST_CHECK_EQUAL(dot(a, b), dot(b, a));

	LASS_TEST_CHECK_EQUAL(dot(cross(a, b), a), zero);
	LASS_TEST_CHECK_EQUAL(dot(cross(a, b), b), zero);

	const Vector3D<T> project_plus_reject = a.project(b) + a.reject(b); // compile fucks up
	LASS_TEST_CHECK_EQUAL(project_plus_reject, b);
	LASS_TEST_CHECK(dot(a.project(b), a.reject(b)) < epsilon);

	Vector3D<T> n(b);
	n.normalize();
	LASS_TEST_CHECK_EQUAL(n, b.normal());
	LASS_TEST_CHECK_CLOSE(n.norm(), one, epsilon);

	std::stringstream ss;
	ss << a;
	ss >> b;
	LASS_TEST_CHECK_EQUAL(a, b);
}



template <typename T> void testPrimVector4D()
{
	using namespace prim;

	const T a0 = T(1), a1 = T(2), a2 = T(3), a3 = T(4);
	const T b0 = T(4), b1 = T(5), b2 = T(6), b3 = T(7);
	const T one = Vector3D<T>::TNumTraits::one;
	const T two = T(2), half = one / two;
	const T epsilon = T(LASS_TEST_PRIM_VECTORS_EPSILON);

	Vector4D<T> a;
	LASS_TEST_CHECK(a.isZero());

	a = Vector4D<T>(a0, a1, a2, a3);
	LASS_TEST_CHECK_EQUAL(a.x, a0);
	LASS_TEST_CHECK_EQUAL(a.y, a1);
	LASS_TEST_CHECK_EQUAL(a.z, a2);
	LASS_TEST_CHECK_EQUAL(a.w, a3);

	LASS_TEST_CHECK_EQUAL(a.squaredNorm(), (a0 * a0 + a1 * a1 + a2 * a2 + a3 * a3));
	LASS_TEST_CHECK_CLOSE(a.norm() * a.norm(), a.squaredNorm(), epsilon);

	Vector4D<T> b(b0, b1, b2, b3);
	LASS_TEST_CHECK_EQUAL(b[0], b.x);
	LASS_TEST_CHECK_EQUAL(b[1], b.y);
	LASS_TEST_CHECK_EQUAL(b[2], b.z);
	LASS_TEST_CHECK_EQUAL(b[3], b.w);
	LASS_TEST_CHECK_EQUAL(b.at(-3), b.y);
	LASS_TEST_CHECK_EQUAL(b.at(-2), b.z);
	LASS_TEST_CHECK_EQUAL(b.at(-1), b.w);

	LASS_TEST_CHECK_EQUAL(a + b, Vector4D<T>(a0 + b0, a1 + b1, a2 + b2, a3 + b3));
	LASS_TEST_CHECK_EQUAL(a - b, Vector4D<T>(a0 - b0, a1 - b1, a2 - b2, a3 - b3));
	LASS_TEST_CHECK_EQUAL(a * b, Vector4D<T>(a0 * b0, a1 * b1, a2 * b2, a3 * b3));
	LASS_TEST_CHECK_EQUAL(a / b, Vector4D<T>(a0 / b0, a1 / b1, a2 / b2, a3 / b3));

	LASS_TEST_CHECK_EQUAL(a + b, b + a);
	LASS_TEST_CHECK_EQUAL(a * b, b * a);
	LASS_TEST_CHECK_EQUAL(two * a, a * two);
	LASS_TEST_CHECK_EQUAL(two * a, a + a);
	LASS_TEST_CHECK_EQUAL(a / two, half * a);

	LASS_TEST_CHECK_EQUAL(dot(a, a), a.squaredNorm());
	LASS_TEST_CHECK_EQUAL(dot(a, b), dot(b, a));

	const Vector4D<T> project_plus_reject = a.project(b) + a.reject(b); // compile fucks up
	LASS_TEST_CHECK_EQUAL(project_plus_reject, b);
	LASS_TEST_CHECK(dot(a.project(b), a.reject(b)) < epsilon);

	Vector4D<T> n(b);
	n.normalize();
	LASS_TEST_CHECK_EQUAL(n, b.normal());
	LASS_TEST_CHECK_CLOSE(n.norm(), one, epsilon);

	std::stringstream ss;
	ss << a;
	ss >> b;
	LASS_TEST_CHECK_EQUAL(a, b);
}



}

}

#endif

// EOF
