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

#include "../prim/line_3d.h"
#include "../meta/is_same.h"

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

	const bool isNormalized = meta::IsSame<NormalizingPolicy, prim::Normalized>::value;

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


TUnitTests test_prim_line_3d()
{
	TUnitTests result;
	result.push_back(LASS_UNIT_TEST((testPrimLine3D<float, prim::Unnormalized>)));
	result.push_back(LASS_UNIT_TEST((testPrimLine3D<float,prim::Normalized>)));
	result.push_back(LASS_UNIT_TEST((testPrimLine3D<double, prim::Unnormalized>)));
	result.push_back(LASS_UNIT_TEST((testPrimLine3D<double,prim::Normalized>)));
	return result;
}

}

}

