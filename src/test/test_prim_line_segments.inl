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


 *  @date 2004
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_LINE_SEGMENTS_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_LINE_SEGMENTS_INL

#include "test_common.h"

#include "../prim/line_segment_2d.h"
#include "../prim/line_segment_3d.h"

namespace lass
{
namespace test
{

template <typename T> 
void testPrimLineSegment2D()
{
	typedef prim::LineSegment2D<T, prim::Bounded> TLineSegBounded;
	typedef prim::LineSegment2D<T, prim::Unbounded> TLineSegUnbounded;
	typedef prim::Point2D<T> TPoint;

    const T epsilon = static_cast<T>(1e-5);

	TPoint a(53, 16);
	TPoint b(46, 37);
	TPoint c(61, 97);
	TPoint d(40, 10);

	TLineSegBounded bounded(a, b);
	TLineSegUnbounded unbounded(c, d);
	
	BOOST_CHECK_EQUAL(bounded.tail(), a);
	BOOST_CHECK_EQUAL(bounded.head(), b);
	BOOST_CHECK_EQUAL(unbounded.tail(), c);
	BOOST_CHECK_EQUAL(unbounded.head(), d);
	BOOST_CHECK_EQUAL(bounded.vector(), b - a);
	BOOST_CHECK_EQUAL(unbounded.vector(), d - c);
	BOOST_CHECK_EQUAL(bounded.length(), prim::distance(a, b));
	BOOST_CHECK_EQUAL(unbounded.length(), prim::distance(c, d));

	BOOST_CHECK_THROW(bounded.point(T(1.1)), std::exception);
	BOOST_CHECK_THROW(bounded.point(T(-0.1)), std::exception);
	BOOST_CHECK_NO_THROW(bounded.point(T(0)));
	BOOST_CHECK_NO_THROW(bounded.point(T(1)));
	BOOST_CHECK_NO_THROW(bounded.point(T(.5)));

	BOOST_CHECK_NO_THROW(unbounded.point(T(1.1)));
	BOOST_CHECK_NO_THROW(unbounded.point(T(-0.1)));
	BOOST_CHECK_NO_THROW(unbounded.point(T(0)));
	BOOST_CHECK_NO_THROW(unbounded.point(T(1)));
	BOOST_CHECK_NO_THROW(unbounded.point(T(.5)));

	BOOST_CHECK_EQUAL(bounded.point(T(0)), bounded.tail());
	BOOST_CHECK_EQUAL(bounded.point(T(1)), bounded.head());
	BOOST_CHECK_EQUAL(unbounded.point(T(0)), unbounded.tail());
	BOOST_CHECK_EQUAL(unbounded.point(T(1)), unbounded.head());

	T t, u;
	BOOST_CHECK_EQUAL(prim::intersect(bounded, unbounded, t, u), prim::rOne);
	BOOST_CHECK(t >= T(0) && t <= T(1));
	BOOST_CHECK(u >= T(0) && u <= T(1));
	LASS_CLOSE_VECTOR(bounded.point(t), unbounded.point(u), 100 * epsilon);

	TPoint p;
	BOOST_CHECK_EQUAL(prim::intersect(bounded, unbounded, p), prim::rOne);
	LASS_CLOSE_VECTOR(bounded.point(t), p, 100 * epsilon);
	LASS_CLOSE_VECTOR(unbounded.point(u), p, 100 * epsilon);
}



template <typename T> 
void testPrimLineSegment3D()
{
	typedef prim::LineSegment3D<T, prim::Bounded> TLineSegBounded;
	typedef prim::LineSegment3D<T, prim::Unbounded> TLineSegUnbounded;
	typedef prim::Point3D<T> TPoint;

	TPoint a(53, 16, 85);
	TPoint b(46, 37, 30);
	TPoint c(61, 97, 05);
	TPoint d(40, 10, 34);

	TLineSegBounded bounded(a, b);
	TLineSegUnbounded unbounded(c, d);
	
	BOOST_CHECK_EQUAL(bounded.tail(), a);
	BOOST_CHECK_EQUAL(bounded.head(), b);
	BOOST_CHECK_EQUAL(unbounded.tail(), c);
	BOOST_CHECK_EQUAL(unbounded.head(), d);
	BOOST_CHECK_EQUAL(bounded.vector(), b - a);
	BOOST_CHECK_EQUAL(unbounded.vector(), d - c);
	BOOST_CHECK_EQUAL(bounded.length(), prim::distance(a, b));
	BOOST_CHECK_EQUAL(unbounded.length(), prim::distance(c, d));

	BOOST_CHECK_THROW(bounded.point(T(1.1)), std::exception);
	BOOST_CHECK_THROW(bounded.point(T(-0.1)), std::exception);
	BOOST_CHECK_NO_THROW(bounded.point(T(0)));
	BOOST_CHECK_NO_THROW(bounded.point(T(1)));
	BOOST_CHECK_NO_THROW(bounded.point(T(.5)));

	BOOST_CHECK_NO_THROW(unbounded.point(T(1.1)));
	BOOST_CHECK_NO_THROW(unbounded.point(T(-0.1)));
	BOOST_CHECK_NO_THROW(unbounded.point(T(0)));
	BOOST_CHECK_NO_THROW(unbounded.point(T(1)));
	BOOST_CHECK_NO_THROW(unbounded.point(T(.5)));

	BOOST_CHECK_EQUAL(bounded.point(T(0)), bounded.tail());
	BOOST_CHECK_EQUAL(bounded.point(T(1)), bounded.head());
	BOOST_CHECK_EQUAL(unbounded.point(T(0)), unbounded.tail());
	BOOST_CHECK_EQUAL(unbounded.point(T(1)), unbounded.head());
}



}

}

#endif

// EOF
