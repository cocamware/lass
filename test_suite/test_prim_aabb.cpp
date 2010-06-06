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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

#include "../lass/prim/aabb_2d.h"
#include "../lass/prim/aabb_3d.h"

namespace lass
{
namespace test
{

template <typename T>
void testPrimAabb2D()
{
	typedef prim::Aabb2D<T, prim::StrictMinMax> TAabbStrict;
	typedef prim::Aabb2D<T, prim::AutoMinMax> TAabbAuto;
	typedef prim::Aabb2D<T, prim::UncheckedMinMax> TAabbUnchecked;
	typedef prim::Point2D<T> TPoint;

	// create empty aabbs.

	TAabbStrict aabbStrict;
	TAabbAuto aabbAuto;
	TAabbUnchecked aabbUnchecked;

	LASS_TEST_CHECK(aabbStrict.isValid());
	LASS_TEST_CHECK(aabbAuto.isValid());
	LASS_TEST_CHECK(aabbUnchecked.isValid());
	LASS_TEST_CHECK(aabbStrict.isEmpty());
	LASS_TEST_CHECK(aabbAuto.isEmpty());
	LASS_TEST_CHECK(aabbUnchecked.isEmpty());

	// construct with corner points

	TPoint m(10, 100);
	TPoint M(1000, 10000);

	LASS_TEST_CHECK_NO_THROW(aabbStrict = TAabbStrict(m, M));
	LASS_TEST_CHECK_NO_THROW(aabbAuto = TAabbAuto(m, M));
	LASS_TEST_CHECK_NO_THROW(aabbUnchecked = TAabbUnchecked(m, M));

	LASS_TEST_CHECK(aabbStrict.isValid());
	LASS_TEST_CHECK(aabbAuto.isValid());
	LASS_TEST_CHECK(aabbUnchecked.isValid());
	LASS_TEST_CHECK(!aabbStrict.isEmpty());
	LASS_TEST_CHECK(!aabbAuto.isEmpty());
	LASS_TEST_CHECK(!aabbUnchecked.isEmpty());
	LASS_TEST_CHECK_EQUAL(aabbStrict.min(), aabbAuto.min());
	LASS_TEST_CHECK_EQUAL(aabbStrict.max(), aabbAuto.max());
	LASS_TEST_CHECK_EQUAL(aabbStrict.center(), aabbAuto.center());
	LASS_TEST_CHECK_EQUAL(aabbStrict.size(), aabbAuto.size());
	LASS_TEST_CHECK_EQUAL(aabbStrict.area(), aabbAuto.area());

	// reversed corner points

	LASS_TEST_CHECK_THROW(aabbStrict = TAabbStrict(M, m), std::exception);
	LASS_TEST_CHECK_NO_THROW(aabbAuto = TAabbAuto(M, m));
	LASS_TEST_CHECK_NO_THROW(aabbUnchecked = TAabbUnchecked(M, m));

	LASS_TEST_CHECK(aabbStrict.isValid());
	LASS_TEST_CHECK(aabbAuto.isValid());
	LASS_TEST_CHECK(aabbUnchecked.isValid()); // max_.x < min_.x
	LASS_TEST_CHECK(!aabbStrict.isEmpty());
	LASS_TEST_CHECK(!aabbAuto.isEmpty());
	LASS_TEST_CHECK(aabbUnchecked.isEmpty()); // max_.x < min_.x
	LASS_TEST_CHECK_EQUAL(aabbStrict.min(), aabbAuto.min());
	LASS_TEST_CHECK_EQUAL(aabbStrict.max(), aabbAuto.max());
	LASS_TEST_CHECK_EQUAL(aabbStrict.center(), aabbAuto.center());
	LASS_TEST_CHECK_EQUAL(aabbStrict.size(), aabbAuto.size());
	LASS_TEST_CHECK_EQUAL(aabbStrict.area(), aabbAuto.area());

	// classificiation

	LASS_TEST_CHECK(aabbStrict.classify(m) == prim::sSurface);
	LASS_TEST_CHECK(aabbStrict.classify(M) == prim::sSurface);
	LASS_TEST_CHECK(aabbStrict.classify(aabbStrict.center().affine()) == prim::sInside);
	LASS_TEST_CHECK(aabbStrict.classify(TPoint(10, 10)) == prim::sOutside);
	LASS_TEST_CHECK(aabbStrict.contains(m));
	LASS_TEST_CHECK(aabbStrict.contains(M));
	LASS_TEST_CHECK(aabbStrict.contains(aabbStrict.center().affine()));
	LASS_TEST_CHECK(!aabbStrict.contains(TPoint(10, 10)));
	LASS_TEST_CHECK(aabbStrict.contains(aabbAuto));
	LASS_TEST_CHECK(aabbAuto.contains(aabbStrict));

	// addition

	aabbStrict += TPoint(50, 10);  // (10, 10)-(1000, 10000)
	LASS_TEST_CHECK(aabbStrict.contains(m));
	LASS_TEST_CHECK(aabbStrict.contains(M));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(50, 10)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(10, 10)));
	LASS_TEST_CHECK(aabbStrict.contains(aabbAuto));
	LASS_TEST_CHECK(!aabbAuto.contains(aabbStrict));

	aabbStrict += TAabbAuto(TPoint(50, 50), TPoint(10000, 0)); // (10, 0)-(10000, 10000)
	LASS_TEST_CHECK(aabbStrict.contains(m));
	LASS_TEST_CHECK(aabbStrict.contains(M));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(50, 10)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(10, 10)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(50, 50)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(10000, 0)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(10, 0)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(10000, 10000)));
	LASS_TEST_CHECK(aabbStrict.contains(aabbAuto));
	LASS_TEST_CHECK(!aabbAuto.contains(aabbStrict));

	// distance and intersection

	TAabbStrict a(TPoint(0, 0), TPoint(10, 10));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, TPoint(-10, 5)), 10);
	LASS_TEST_CHECK_EQUAL(prim::distance(a, TPoint(20, 5)), 10);
	LASS_TEST_CHECK_EQUAL(prim::distance(a, TPoint(-10, -10)), num::sqrt(T(200)));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, TPoint(20, 20)), num::sqrt(T(200)));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, TPoint(5, 5)), 0);

	TAabbStrict b(TPoint(100, 100), TPoint(1000, 1000));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, b), prim::distance(TPoint(10, 10), TPoint(100, 100)));

	TAabbStrict c(TPoint(6, 66), TPoint(666, 6666));
	LASS_TEST_CHECK(!c.isEmpty());
	LASS_TEST_CHECK(prim::intersect(a, b, c) == prim::rNone);
	LASS_TEST_CHECK(c.isEmpty());

	b = TAabbStrict(TPoint(100, 5), TPoint(1000, 1000));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, b), 90);
	c = TAabbStrict(TPoint(6, 66), TPoint(666, 6666));
	LASS_TEST_CHECK(!c.isEmpty());
	LASS_TEST_CHECK(prim::intersect(a, b, c) == prim::rNone);
	LASS_TEST_CHECK(c.isEmpty());

	b = TAabbStrict(TPoint(5, 5), TPoint(1000, 1000));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, b), 0);
	LASS_TEST_CHECK(prim::intersect(a, b, c) == prim::rOne);
	LASS_TEST_CHECK(!c.isEmpty());
	LASS_TEST_CHECK_EQUAL(c.min(), TPoint(5, 5));
	LASS_TEST_CHECK_EQUAL(c.max(), TPoint(10, 10));

	// growing and scaling
	TAabbStrict d(TPoint(0, 0), TPoint(10, 10));
	d.grow(10);
	LASS_TEST_CHECK(d.isValid());
	LASS_TEST_CHECK(!d.isEmpty());
	LASS_TEST_CHECK_EQUAL(d.max(),TPoint(20,20));
	d.scale(0.5);
	LASS_TEST_CHECK_EQUAL(d.max(),TPoint(12.5,12.5));

}



template <typename T>
void testPrimAabb3D()
{
	typedef prim::Aabb3D<T, prim::StrictMinMax> TAabbStrict;
	typedef prim::Aabb3D<T, prim::AutoMinMax> TAabbAuto;
	typedef prim::Point3D<T> TPoint;

	T epsilon = 1e-6f;

	// create two empty aabbs.

	TAabbStrict aabbStrict;
	TAabbAuto aabbAuto;

	LASS_TEST_CHECK(aabbStrict.isValid());
	LASS_TEST_CHECK(aabbAuto.isValid());
	LASS_TEST_CHECK(aabbStrict.isEmpty());
	LASS_TEST_CHECK(aabbAuto.isEmpty());

	// construct with corner points

	TPoint m(10, 100, 200);
	TPoint M(1000, 10000, 20000);

	LASS_TEST_CHECK_NO_THROW(aabbStrict = TAabbStrict(m, M));
	LASS_TEST_CHECK_NO_THROW(aabbAuto = TAabbAuto(m, M));

	LASS_TEST_CHECK(aabbStrict.isValid());
	LASS_TEST_CHECK(aabbAuto.isValid());
	LASS_TEST_CHECK(!aabbStrict.isEmpty());
	LASS_TEST_CHECK(!aabbAuto.isEmpty());
	LASS_TEST_CHECK_EQUAL(aabbStrict.min(), aabbAuto.min());
	LASS_TEST_CHECK_EQUAL(aabbStrict.max(), aabbAuto.max());
	LASS_TEST_CHECK_EQUAL(aabbStrict.center(), aabbAuto.center());
	LASS_TEST_CHECK_EQUAL(aabbStrict.size(), aabbAuto.size());
	LASS_TEST_CHECK_EQUAL(aabbStrict.area(), aabbAuto.area());

	// reversed corner points

	LASS_TEST_CHECK_NO_THROW(aabbAuto = TAabbAuto(M, m));
	LASS_TEST_CHECK_THROW(aabbStrict = TAabbStrict(M, m), std::exception);
	LASS_TEST_CHECK_NO_THROW(aabbStrict = TAabbStrict(m, M));

	LASS_TEST_CHECK(aabbStrict.isValid());
	LASS_TEST_CHECK(aabbAuto.isValid());
	LASS_TEST_CHECK(!aabbStrict.isEmpty());
	LASS_TEST_CHECK(!aabbAuto.isEmpty());
	LASS_TEST_CHECK_EQUAL(aabbStrict.min(), aabbAuto.min());
	LASS_TEST_CHECK_EQUAL(aabbStrict.max(), aabbAuto.max());
	LASS_TEST_CHECK_EQUAL(aabbStrict.center(), aabbAuto.center());
	LASS_TEST_CHECK_EQUAL(aabbStrict.size(), aabbAuto.size());
	LASS_TEST_CHECK_EQUAL(aabbStrict.area(), aabbAuto.area());

	// classificiation

	LASS_TEST_CHECK(aabbStrict.classify(m) == prim::sSurface);
	LASS_TEST_CHECK(aabbStrict.classify(M) == prim::sSurface);
	LASS_TEST_CHECK(aabbStrict.classify(aabbStrict.center().affine()) == prim::sInside);
	LASS_TEST_CHECK(aabbStrict.classify(TPoint(10, 10, 20)) == prim::sOutside);
	LASS_TEST_CHECK(aabbStrict.contains(m));
	LASS_TEST_CHECK(aabbStrict.contains(M));
	LASS_TEST_CHECK(aabbStrict.contains(aabbStrict.center().affine()));
	LASS_TEST_CHECK(!aabbStrict.contains(TPoint(10, 10, 20)));
	LASS_TEST_CHECK(aabbStrict.contains(aabbAuto));
	LASS_TEST_CHECK(aabbAuto.contains(aabbStrict));

	// addition

	aabbStrict += TPoint(50, 10, 20);  // (10, 10, 20)-(1000, 10000, 20000)
	LASS_TEST_CHECK(aabbStrict.contains(m));
	LASS_TEST_CHECK(aabbStrict.contains(M));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(50, 10, 20)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(10, 10, 20)));
	LASS_TEST_CHECK(aabbStrict.contains(aabbAuto));
	LASS_TEST_CHECK(!aabbAuto.contains(aabbStrict));

	aabbStrict += TAabbAuto(TPoint(50, 50, 100), TPoint(10000, 0, 0));
	// (10, 0, 0)-(10000, 10000, 20000)
	LASS_TEST_CHECK(aabbStrict.contains(m));
	LASS_TEST_CHECK(aabbStrict.contains(M));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(50, 10, 20)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(10, 10, 20)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(50, 50, 100)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(10000, 0, 0)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(10, 0, 0)));
	LASS_TEST_CHECK(aabbStrict.contains(TPoint(10000, 10000, 20000)));
	LASS_TEST_CHECK(aabbStrict.contains(aabbAuto));
	LASS_TEST_CHECK(!aabbAuto.contains(aabbStrict));

	// distance and intersection

	TAabbStrict a(TPoint(0, 0, 0), TPoint(10, 10, 10));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, TPoint(-10, 5, 5)), 10);
	LASS_TEST_CHECK_EQUAL(prim::distance(a, TPoint(20, 5, 5)), 10);
	LASS_TEST_CHECK_CLOSE(prim::distance(a, TPoint(-10, -10, -10)), num::sqrt(T(300)), epsilon);
	LASS_TEST_CHECK_CLOSE(prim::distance(a, TPoint(20, 20, 20)), num::sqrt(T(300)), epsilon);
	LASS_TEST_CHECK_EQUAL(prim::distance(a, TPoint(5, 5, 5)), 0);

	TAabbStrict b(TPoint(100, 100, 100), TPoint(1000, 1000, 1000));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, b), prim::distance(TPoint(10, 10, 10), TPoint(100, 100, 100)));

	TAabbStrict c(TPoint(6, 66, 666), TPoint(6666, 66666, 666666));
	LASS_TEST_CHECK(!c.isEmpty());
	LASS_TEST_CHECK(prim::intersect(a, b, c) == prim::rNone);
	LASS_TEST_CHECK(c.isEmpty());

	b = TAabbStrict(TPoint(100, 5, 5), TPoint(1000, 1000, 1000));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, b), 90);
	c = TAabbStrict(TPoint(6, 66, 666), TPoint(6666, 66666, 666666));
	LASS_TEST_CHECK(!c.isEmpty());
	LASS_TEST_CHECK(prim::intersect(a, b, c) == prim::rNone);
	LASS_TEST_CHECK(c.isEmpty());

	b = TAabbStrict(TPoint(5, 5, 5), TPoint(1000, 1000, 1000));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, b), 0);
	LASS_TEST_CHECK(prim::intersect(a, b, c) == prim::rOne);
	LASS_TEST_CHECK(!c.isEmpty());
	LASS_TEST_CHECK_EQUAL(c.min(), TPoint(5, 5, 5));
	LASS_TEST_CHECK_EQUAL(c.max(), TPoint(10, 10, 10));
}

TUnitTest test_prim_aabb()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testPrimAabb2D<float>));
	result.push_back(LASS_TEST_CASE(testPrimAabb2D<double>));
	result.push_back(LASS_TEST_CASE(testPrimAabb3D<float>));
	result.push_back(LASS_TEST_CASE(testPrimAabb3D<double>));
	return result;
}



}

}

// EOF
