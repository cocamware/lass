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
	const Vector2D<T> project_plus_reject = a.project(b) + a.reject(b); // compiler fucks up
	LASS_TEST_CHECK_EQUAL(project_plus_reject, b);
	LASS_TEST_CHECK(dot(a.project(b), a.reject(b)) < epsilon);

	Vector2D<T> n(b);
	n.normalize();
	LASS_TEST_CHECK_EQUAL(n, b.normal());
	LASS_TEST_CHECK_EQUAL(n.norm(), one);

	a = Vector2D<T>(a0, a1);
	b = Vector2D<T>(b0, b1);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0), a);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 1), b);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).x, (a.x + b.x) / 2);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).y, (a.y + b.y) / 2);

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

	a = Vector3D<T>(a0, a1, a2);
	b = Vector3D<T>(b0, b1, b2);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0), a);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 1), b);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).x, (a.x + b.x) / 2);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).y, (a.y + b.y) / 2);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).z, (a.z + b.z) / 2);

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

	a = Vector4D<T>(a0, a1, a2, a3);
	b = Vector4D<T>(b0, b1, b2, b3);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0), a);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 1), b);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).x, (a.x + b.x) / 2);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).y, (a.y + b.y) / 2);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).z, (a.z + b.z) / 2);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).w, (a.w + b.w) / 2);

	std::stringstream ss;
	ss << a;
	ss >> b;
	LASS_TEST_CHECK_EQUAL(a, b);
}



TUnitTests test_prim_vectors()
{
	TUnitTests result;
	result.push_back(LASS_UNIT_TEST(testPrimVector2D<float>));
	result.push_back(LASS_UNIT_TEST(testPrimVector2D<double>));
	result.push_back(LASS_UNIT_TEST(testPrimVector3D<float>));
	result.push_back(LASS_UNIT_TEST(testPrimVector3D<double>));
	result.push_back(LASS_UNIT_TEST(testPrimVector4D<float>));
	result.push_back(LASS_UNIT_TEST(testPrimVector4D<double>));
	return result;
}



}

}

// EOF
