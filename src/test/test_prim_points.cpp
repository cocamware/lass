/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

#include "../prim/point_2d.h"
#include "../prim/point_3d.h"
#include "../prim/point_2dh.h"
#include "../prim/point_3dh.h"

namespace lass
{
namespace test
{

template <typename T> void testPrimPoint2D()
{
	using namespace prim;

	Point2D<T> p;
	LASS_TEST_CHECK(p.isZero());

	Vector2D<T> a(1, 2);
	Vector2D<T> b(4, 5);

	p = Point2D<T>(a.x, a.y);
	LASS_TEST_CHECK_EQUAL(p.position(), a);
	LASS_TEST_CHECK_EQUAL(p.x, a.x);
	LASS_TEST_CHECK_EQUAL(p.y, a.y);

	Point2D<T> q(b);
	LASS_TEST_CHECK_EQUAL(q.position(), b);

	LASS_TEST_CHECK_EQUAL(distance(p, q), (a - b).norm());
	LASS_TEST_CHECK_EQUAL((p + b).position(), a + b);
	LASS_TEST_CHECK((q - b).isZero());

	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0), a);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 1), b);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).x, (a.x + b.x) / 2);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).y, (a.y + b.y) / 2);
}



template <typename T> void testPrimPoint3D()
{
	using namespace prim;

	Point3D<T> p;
	LASS_TEST_CHECK(p.isZero());

	Vector3D<T> a(1, 2, 3);
	Vector3D<T> b(4, 5, 6);

	p = Point3D<T>(a.x, a.y, a.z);
	LASS_TEST_CHECK_EQUAL(p.position(), a);
	LASS_TEST_CHECK_EQUAL(p.x, a.x);
	LASS_TEST_CHECK_EQUAL(p.y, a.y);
	LASS_TEST_CHECK_EQUAL(p.z, a.z);

	Point3D<T> q(b);
	LASS_TEST_CHECK_EQUAL(q.position(), b);

	LASS_TEST_CHECK_EQUAL(distance(p, q), (a - b).norm());
	LASS_TEST_CHECK_EQUAL((p + b).position(), a + b);
	LASS_TEST_CHECK((q - b).isZero());

	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0), a);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 1), b);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).x, (a.x + b.x) / 2);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).y, (a.y + b.y) / 2);
	LASS_TEST_CHECK_EQUAL(lerp(a, b, 0.5f).z, (a.z + b.z) / 2);
}



template<typename T> void testPrimPoint3DH()
{
	using namespace prim;
}


TUnitTest test_prim_points()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testPrimPoint2D<float>));
	result.push_back(LASS_TEST_CASE(testPrimPoint2D<double>));
	result.push_back(LASS_TEST_CASE(testPrimPoint3D<float>));
	result.push_back(LASS_TEST_CASE(testPrimPoint3D<double>));
	result.push_back(LASS_TEST_CASE(testPrimPoint3DH<float>));
	result.push_back(LASS_TEST_CASE(testPrimPoint3DH<double>));
	return result;
}


}

}

// EOF
