/**	@file
 *  @internal
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_RAY_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_RAY_INL

#include "test_common.h"

#include "../prim/ray_2d.h"
#include "../prim/ray_3d.h"
#include "../meta/is_same_type.h"

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

    const bool isNormalized = meta::IsSameType<NormalizingPolicy, prim::Normalized>::value;
    const bool isBounded = meta::IsSameType<ParameterPolicy, prim::Bounded>::value;

    TRay ray;
    BOOST_CHECK(!ray.isValid());

    TPoint support(64, 37);
    TVector direction(15, 59);
    ray = TRay(support, direction);
    BOOST_CHECK(ray.isValid());
    BOOST_CHECK_EQUAL(ray.support(), support);
    BOOST_CHECK_EQUAL(ray.direction(), isNormalized ? direction.normal() : direction);

    support = TPoint(68, 46);
    ray.support() = support;
    BOOST_CHECK_EQUAL(ray.support(), support);
    BOOST_CHECK_EQUAL(ray.direction(), isNormalized ? direction.normal() : direction);

    direction = TVector(38, 73);
    ray.setDirection(direction);
    BOOST_CHECK_EQUAL(ray.support(), support);
    BOOST_CHECK_EQUAL(ray.direction(), isNormalized ? direction.normal() : direction);

    TPoint lookAt(86, 15);
    ray = TRay(support, lookAt);
    BOOST_CHECK_EQUAL(ray.support(), support);
    BOOST_CHECK_EQUAL(ray.direction(), isNormalized ? (lookAt - support).normal() : (lookAt - support));

    lookAt = TPoint(24, 93);
    ray.lookAt(lookAt);
    BOOST_CHECK_EQUAL(ray.support(), support);
    BOOST_CHECK_EQUAL(ray.direction(), isNormalized ? (lookAt - support).normal() : (lookAt - support));

    BOOST_CHECK_EQUAL(ray.point(T(0)), ray.support());
    BOOST_CHECK_EQUAL(ray.point(T(1)), ray.support() + ray.direction()); 
    if (isBounded)
    {
        BOOST_CHECK_THROW(ray.point(T(-1)), util::Exception);
    }
    else
    {
        BOOST_CHECK_NO_THROW(ray.point(T(-1)));
        BOOST_CHECK_EQUAL(ray.point(T(-1)), ray.support() - ray.direction()); 
    }
    BOOST_CHECK_EQUAL(ray.t(ray.support()), T(0));
    BOOST_CHECK_CLOSE(ray.t(ray.support() + ray.direction()), T(1), 100 * epsilon);
    BOOST_CHECK_CLOSE(ray.t(ray.support() - ray.direction()), T(-1), 100 * epsilon);
    BOOST_CHECK_CLOSE(ray.t(ray.support() - ray.direction()), T(-1), 100 * epsilon);
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

    const bool isNormalized = meta::IsSameType<NormalizingPolicy, prim::Normalized>::value;
    const bool isBounded = meta::IsSameType<ParameterPolicy, prim::Bounded>::value;

    TRay ray;
    BOOST_CHECK(!ray.isValid());

    TPoint support(64, 37, 18);
    TVector direction(15, 59, 67);
    ray = TRay(support, direction);
    BOOST_CHECK(ray.isValid());
    BOOST_CHECK_EQUAL(ray.support(), support);
    BOOST_CHECK_EQUAL(ray.direction(), isNormalized ? direction.normal() : direction);

    support = TPoint(68, 46, 30);
    ray.support() = support;
    BOOST_CHECK_EQUAL(ray.support(), support);
    BOOST_CHECK_EQUAL(ray.direction(), isNormalized ? direction.normal() : direction);

    direction = TVector(38, 73, 68);
    ray.setDirection(direction);
    BOOST_CHECK_EQUAL(ray.support(), support);
    BOOST_CHECK_EQUAL(ray.direction(), isNormalized ? direction.normal() : direction);

    TPoint lookAt(86, 15, 69);
    ray = TRay(support, lookAt);
    BOOST_CHECK_EQUAL(ray.support(), support);
    BOOST_CHECK_EQUAL(ray.direction(), isNormalized ? (lookAt - support).normal() : (lookAt - support));

    lookAt = TPoint(24, 93, 78);
    ray.lookAt(lookAt);
    BOOST_CHECK_EQUAL(ray.support(), support);
    BOOST_CHECK_EQUAL(ray.direction(), isNormalized ? (lookAt - support).normal() : (lookAt - support));

    BOOST_CHECK_EQUAL(ray.point(T(0)), ray.support());
    BOOST_CHECK_EQUAL(ray.point(T(1)), ray.support() + ray.direction()); 
    if (isBounded)
    {
        BOOST_CHECK_THROW(ray.point(T(-1)), util::Exception);
    }
    else
    {
        BOOST_CHECK_NO_THROW(ray.point(T(-1)));
        BOOST_CHECK_EQUAL(ray.point(T(-1)), ray.support() - ray.direction()); 
    }
    BOOST_CHECK_EQUAL(ray.t(ray.support()), T(0));
    BOOST_CHECK_CLOSE(ray.t(ray.support() + ray.direction()), T(1), 100 * epsilon);
    BOOST_CHECK_CLOSE(ray.t(ray.support() - ray.direction()), T(-1), 100 * epsilon);
    BOOST_CHECK_CLOSE(ray.t(ray.support() - ray.direction()), T(-1), 100 * epsilon);
}

}

}

#endif