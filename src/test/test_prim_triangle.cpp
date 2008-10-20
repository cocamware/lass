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

#include "../prim/aabb_2d.h"
#include "../prim/simple_polygon_2d.h"
#include "../prim/triangle_2d.h"
#include "../prim/line_segment_2d.h"
#include "../num/random.h"

namespace lass
{
namespace test
{

template
<
	typename T
>
void testPrimTriangle2D()
{
	typedef prim::Aabb2D<T> TAabb;
	typedef prim::Triangle2D<T> TTriangle;
	typedef prim::LineSegment2D<T> TLineSegment;
	typedef prim::SimplePolygon2D<T> TPolygon;
	typedef prim::Point2D<T> TPoint;
	typedef prim::Vector2D<T> TVector;
	typedef typename TVector::TNumTraits TNumTraits;

	const T tolerance = 1e-3f;

	TAabb bounds(TPoint(-100, -100), TPoint(100, 100));
	num::RandomMT19937 random;

	for (size_t i = 0; i < 1000; ++i)
	{
		const TPoint points[3] = 
		{
			bounds.random(random),
			bounds.random(random),
			bounds.random(random)
		};

		TTriangle triangle(points[0], points[1], points[2]);
		TPolygon polygon(points, points + 3);

		LASS_TEST_CHECK_CLOSE(triangle.area(), polygon.area(), tolerance);

		// distance thing
		const TPoint target = bounds.random(random);
		T bruteSquareDistance = TNumTraits::infinity;
		for (size_t k1 = 0, k0 = 2; k1 < 3; k0 = k1++)
		{
			TLineSegment seg(points[k0], points[k1]);
			bruteSquareDistance = std::min(bruteSquareDistance, squaredDistance(seg, target));
		}
		LASS_TEST_CHECK_CLOSE(squaredDistance(triangle, target), bruteSquareDistance, tolerance);
	}
}



TUnitTest test_prim_triangle()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testPrimTriangle2D<float>));\
	result.push_back(LASS_TEST_CASE(testPrimTriangle2D<double>));\
	return result;
}

}
}

// EOF
