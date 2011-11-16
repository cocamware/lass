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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#include "../lass/prim/ray_2d.h"
#include "../lass/prim/ray_3d.h"
#include "../lass/meta/is_same.h"

#define LASS_TEST_PRIM_RAY_EPSILON 1e-5

namespace lass
{
namespace test
{

template
<
	typename T,
	class NormalizingPolicy,
	class ParameterPolicy
>
void testPrimRay2D()
{
	typedef prim::Ray2D<T, NormalizingPolicy, ParameterPolicy> TRay;
	typedef prim::Point2D<T> TPoint;
	typedef prim::Vector2D<T> TVector;

	const T epsilon = T(LASS_TEST_PRIM_RAY_EPSILON);

	const bool isNormalized = meta::IsSame<NormalizingPolicy, prim::Normalized>::value;
	const bool isBounded = meta::IsSame<ParameterPolicy, prim::Bounded>::value;

	TRay ray;
	LASS_TEST_CHECK(!ray.isValid());

	TPoint support(64, 37);
	TVector direction(15, 59);
	ray = TRay(support, direction);
	LASS_TEST_CHECK(ray.isValid());
	LASS_TEST_CHECK_EQUAL(ray.support(), support);
	LASS_TEST_CHECK_CLOSE_ARRAY(ray.direction(), isNormalized ? direction.normal() : direction, epsilon, 2);

	support = TPoint(68, 46);
	ray.support() = support;
	LASS_TEST_CHECK_EQUAL(ray.support(), support);
	LASS_TEST_CHECK_CLOSE_ARRAY(ray.direction(), isNormalized ? direction.normal() : direction, epsilon, 2);

	direction = TVector(38, 73);
	ray.setDirection(direction);
	LASS_TEST_CHECK_EQUAL(ray.support(), support);
	LASS_TEST_CHECK_EQUAL(ray.direction(), isNormalized ? direction.normal() : direction);

	TPoint lookAt(86, 15);
	ray = TRay(support, lookAt);
	LASS_TEST_CHECK_EQUAL(ray.support(), support);
	LASS_TEST_CHECK_EQUAL(ray.direction(), isNormalized ? (lookAt - support).normal() : (lookAt - support));

	lookAt = TPoint(24, 93);
	ray.lookAt(lookAt);
	LASS_TEST_CHECK_EQUAL(ray.support(), support);
	LASS_TEST_CHECK_EQUAL(ray.direction(), isNormalized ? (lookAt - support).normal() : (lookAt - support));

	LASS_TEST_CHECK_EQUAL(ray.point(T(0)), ray.support());
	LASS_TEST_CHECK_EQUAL(ray.point(T(1)), ray.support() + ray.direction());
	if (isBounded)
	{
		LASS_TEST_CHECK_THROW(ray.point(T(-1)), util::Exception);
	}
	else
	{
		LASS_TEST_CHECK_NO_THROW(ray.point(T(-1)));
		LASS_TEST_CHECK_EQUAL(ray.point(T(-1)), ray.support() - ray.direction());
	}
	LASS_TEST_CHECK_EQUAL(ray.t(ray.support()), T(0));
	LASS_TEST_CHECK_CLOSE(ray.t(ray.support() + ray.direction()), T(1), epsilon);
	LASS_TEST_CHECK_CLOSE(ray.t(ray.support() - ray.direction()), T(-1), epsilon);
	LASS_TEST_CHECK_CLOSE(ray.t(ray.support() - ray.direction()), T(-1), epsilon);

	support = TPoint(0, 0);
	direction = TVector(1, 0);
	ray = TRay(support, direction);

	LASS_TEST_CHECK_EQUAL( ray.classify(TPoint(0,1)), lass::prim::sLeft );
	LASS_TEST_CHECK_EQUAL( ray.classify(TPoint(0,-1)), lass::prim::sRight );
	LASS_TEST_CHECK_EQUAL( ray.classify(TPoint(0,0)), lass::prim::sSurface );
}

template
<
	typename T,
	class NormalizingPolicy,
	class ParameterPolicy
>
void testPrimRay3D()
{
	typedef prim::Ray3D<T, NormalizingPolicy, ParameterPolicy> TRay;
	typedef prim::Point3D<T> TPoint;
	typedef prim::Vector3D<T> TVector;

	const T epsilon = T(LASS_TEST_PRIM_RAY_EPSILON);

	const bool isNormalized = meta::IsSame<NormalizingPolicy, prim::Normalized>::value;
	const bool isBounded = meta::IsSame<ParameterPolicy, prim::Bounded>::value;

	TRay ray;
	LASS_TEST_CHECK(!ray.isValid());

	TPoint support(64, 37, 18);
	TVector direction(15, 59, 67);
	ray = TRay(support, direction);
	LASS_TEST_CHECK(ray.isValid());
	LASS_TEST_CHECK_EQUAL(ray.support(), support);
	LASS_TEST_CHECK_CLOSE_ARRAY(ray.direction(), isNormalized ? direction.normal() : direction, epsilon, 3);

	support = TPoint(68, 46, 30);
	ray.support() = support;
	LASS_TEST_CHECK_EQUAL(ray.support(), support);
	LASS_TEST_CHECK_CLOSE_ARRAY(ray.direction(), isNormalized ? direction.normal() : direction, epsilon, 3);

	direction = TVector(38, 73, 68);
	ray.setDirection(direction);
	LASS_TEST_CHECK_EQUAL(ray.support(), support);
	LASS_TEST_CHECK_EQUAL(ray.direction(), isNormalized ? direction.normal() : direction);

	TPoint lookAt(86, 15, 69);
	ray = TRay(support, lookAt);
	LASS_TEST_CHECK_EQUAL(ray.support(), support);
	LASS_TEST_CHECK_EQUAL(ray.direction(), isNormalized ? (lookAt - support).normal() : (lookAt - support));

	lookAt = TPoint(24, 93, 78);
	ray.lookAt(lookAt);
	LASS_TEST_CHECK_EQUAL(ray.support(), support);
	LASS_TEST_CHECK_EQUAL(ray.direction(), isNormalized ? (lookAt - support).normal() : (lookAt - support));

	LASS_TEST_CHECK_EQUAL(ray.point(T(0)), ray.support());
	LASS_TEST_CHECK_EQUAL(ray.point(T(1)), ray.support() + ray.direction());
	if (isBounded)
	{
		LASS_TEST_CHECK_THROW(ray.point(T(-1)), util::Exception);
	}
	else
	{
		LASS_TEST_CHECK_NO_THROW(ray.point(T(-1)));
		LASS_TEST_CHECK_EQUAL(ray.point(T(-1)), ray.support() - ray.direction());
	}
	LASS_TEST_CHECK_EQUAL(ray.t(ray.support()), T(0));
	LASS_TEST_CHECK_CLOSE(ray.t(ray.support() + ray.direction()), T(1), epsilon);
	LASS_TEST_CHECK_CLOSE(ray.t(ray.support() - ray.direction()), T(-1), epsilon);
	LASS_TEST_CHECK_CLOSE(ray.t(ray.support() - ray.direction()), T(-1), epsilon);
}



TUnitTest test_prim_ray()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE((testPrimRay2D<float, prim::Unnormalized, prim::Unbounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay2D<float, prim::Unnormalized, prim::Bounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay2D<float, prim::Normalized, prim::Unbounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay2D<float, prim::Normalized, prim::Bounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay2D<double, prim::Unnormalized, prim::Unbounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay2D<double, prim::Unnormalized, prim::Bounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay2D<double, prim::Normalized, prim::Unbounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay2D<double, prim::Normalized, prim::Bounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay3D<float, prim::Unnormalized, prim::Unbounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay3D<float, prim::Unnormalized, prim::Bounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay3D<float, prim::Normalized, prim::Unbounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay3D<float, prim::Normalized, prim::Bounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay3D<double, prim::Unnormalized, prim::Unbounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay3D<double, prim::Unnormalized, prim::Bounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay3D<double, prim::Normalized, prim::Unbounded>)));
	result.push_back(LASS_TEST_CASE((testPrimRay3D<double, prim::Normalized, prim::Bounded>)));
	return result;
}

}
}
