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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_AABB_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_AABB_INL

#include "test_common.h"

#include "../prim/aabb_2d.h"
#include "../prim/aabb_3d.h"

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
	LASS_TEST_CHECK_EQUAL(prim::distance(a, TPoint(-10, -10, -10)), num::sqrt(T(300)));
	LASS_TEST_CHECK_EQUAL(prim::distance(a, TPoint(20, 20, 20)), num::sqrt(T(300)));
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



}

}

#endif

// EOF
