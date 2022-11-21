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
 *	Copyright (C) 2004-2022 the Initial Developer.
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

#include "../lass/prim/line_segment_2d.h"
#include "../lass/prim/line_segment_3d.h"
#include "../lass/num/random.h"
#include "../lass/num/distribution.h"

#include <random>

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

	const T epsilon = static_cast<T>(1e-4);

	TPoint a(53, 16);
	TPoint b(46, 37);
	TPoint c(61, 97);
	TPoint d(40, 10);

	TLineSegBounded bounded(a, b);
	TLineSegUnbounded unbounded(c, d);

	LASS_TEST_CHECK_EQUAL(bounded.tail(), a);
	LASS_TEST_CHECK_EQUAL(bounded.head(), b);
	LASS_TEST_CHECK_EQUAL(unbounded.tail(), c);
	LASS_TEST_CHECK_EQUAL(unbounded.head(), d);
	LASS_TEST_CHECK_EQUAL(bounded.vector(), b - a);
	LASS_TEST_CHECK_EQUAL(unbounded.vector(), d - c);
	LASS_TEST_CHECK_EQUAL(bounded.length(), prim::distance(a, b));
	LASS_TEST_CHECK_EQUAL(unbounded.length(), prim::distance(c, d));

	LASS_TEST_CHECK_THROW(bounded.point(T(1.1)), std::exception);
	LASS_TEST_CHECK_THROW(bounded.point(T(-0.1)), std::exception);
	LASS_TEST_CHECK_NO_THROW(bounded.point(T(0)));
	LASS_TEST_CHECK_NO_THROW(bounded.point(T(1)));
	LASS_TEST_CHECK_NO_THROW(bounded.point(T(.5)));

	LASS_TEST_CHECK_NO_THROW(unbounded.point(T(1.1)));
	LASS_TEST_CHECK_NO_THROW(unbounded.point(T(-0.1)));
	LASS_TEST_CHECK_NO_THROW(unbounded.point(T(0)));
	LASS_TEST_CHECK_NO_THROW(unbounded.point(T(1)));
	LASS_TEST_CHECK_NO_THROW(unbounded.point(T(.5)));

	LASS_TEST_CHECK_EQUAL(bounded.point(T(0)), bounded.tail());
	LASS_TEST_CHECK_EQUAL(bounded.point(T(1)), bounded.head());
	LASS_TEST_CHECK_EQUAL(unbounded.point(T(0)), unbounded.tail());
	LASS_TEST_CHECK_EQUAL(unbounded.point(T(1)), unbounded.head());

	T t, u;
	LASS_TEST_CHECK_EQUAL(prim::intersect(bounded, unbounded, t, u), prim::rOne);
	LASS_TEST_CHECK(t >= T(0) && t <= T(1));
	LASS_TEST_CHECK(u >= T(0) && u <= T(1));
	LASS_TEST_CHECK_CLOSE_ARRAY(bounded.point(t), unbounded.point(u), epsilon, 2);

	TPoint p;
	LASS_TEST_CHECK_EQUAL(prim::intersect(bounded, unbounded, p), prim::rOne);
	LASS_TEST_CHECK_CLOSE_ARRAY(bounded.point(t), p, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(unbounded.point(u), p, epsilon, 2);

	std::mt19937_64 random;
	std::uniform_real_distribution<T> uniform;
	for (size_t i = 0; i < 100; ++i)
	{
		const T t = uniform(random);
		LASS_TEST_CHECK_CLOSE(bounded.t(bounded.point(t)), t, epsilon);
		LASS_TEST_CHECK_CLOSE(unbounded.t(unbounded.point(t)), t, epsilon);
	}
}



template <typename T>
void testPrimLineSegment3D()
{
	typedef prim::LineSegment3D<T, prim::Bounded> TLineSegBounded;
	typedef prim::LineSegment3D<T, prim::Unbounded> TLineSegUnbounded;
	typedef prim::Point3D<T> TPoint;

	const T epsilon = static_cast<T>(1e-4);

	TPoint a(53, 16, 85);
	TPoint b(46, 37, 30);
	TPoint c(61, 97, 05);
	TPoint d(40, 10, 34);

	TLineSegBounded bounded(a, b);
	TLineSegUnbounded unbounded(c, d);

	LASS_TEST_CHECK_EQUAL(bounded.tail(), a);
	LASS_TEST_CHECK_EQUAL(bounded.head(), b);
	LASS_TEST_CHECK_EQUAL(unbounded.tail(), c);
	LASS_TEST_CHECK_EQUAL(unbounded.head(), d);
	LASS_TEST_CHECK_EQUAL(bounded.vector(), b - a);
	LASS_TEST_CHECK_EQUAL(unbounded.vector(), d - c);
	LASS_TEST_CHECK_EQUAL(bounded.length(), prim::distance(a, b));
	LASS_TEST_CHECK_EQUAL(unbounded.length(), prim::distance(c, d));

	LASS_TEST_CHECK_THROW(bounded.point(T(1.1)), std::exception);
	LASS_TEST_CHECK_THROW(bounded.point(T(-0.1)), std::exception);
	LASS_TEST_CHECK_NO_THROW(bounded.point(T(0)));
	LASS_TEST_CHECK_NO_THROW(bounded.point(T(1)));
	LASS_TEST_CHECK_NO_THROW(bounded.point(T(.5)));

	LASS_TEST_CHECK_NO_THROW(unbounded.point(T(1.1)));
	LASS_TEST_CHECK_NO_THROW(unbounded.point(T(-0.1)));
	LASS_TEST_CHECK_NO_THROW(unbounded.point(T(0)));
	LASS_TEST_CHECK_NO_THROW(unbounded.point(T(1)));
	LASS_TEST_CHECK_NO_THROW(unbounded.point(T(.5)));

	LASS_TEST_CHECK_EQUAL(bounded.point(T(0)), bounded.tail());
	LASS_TEST_CHECK_EQUAL(bounded.point(T(1)), bounded.head());
	LASS_TEST_CHECK_EQUAL(unbounded.point(T(0)), unbounded.tail());
	LASS_TEST_CHECK_EQUAL(unbounded.point(T(1)), unbounded.head());

	std::mt19937_64 random;
	std::uniform_real_distribution<T> uniform;
	for (size_t i = 0; i < 100; ++i)
	{
		const T t = uniform(random);
		LASS_TEST_CHECK_CLOSE(bounded.t(bounded.point(t)), t, epsilon);
		LASS_TEST_CHECK_CLOSE(unbounded.t(unbounded.point(t)), t, epsilon);
	}
}



TUnitTest test_prim_line_segments()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testPrimLineSegment2D<float>));
	result.push_back(LASS_TEST_CASE(testPrimLineSegment2D<double>));
	result.push_back(LASS_TEST_CASE(testPrimLineSegment3D<float>));
	result.push_back(LASS_TEST_CASE(testPrimLineSegment3D<double>));
	return result;
}


}

}

// EOF
