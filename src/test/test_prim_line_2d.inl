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

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_LINE_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_LINE_2D_INL

#include "test_common.h"

#include "../prim/line_2d.h"

#define LASS_TEST_PRIM_LINE_2D_EPSILON 1e-5

namespace lass
{
namespace test
{

template <typename T, class NormalizingPolicy> 
void testPrimLine2D()
{
	using namespace prim;

    typedef Line2D<T, Combined, NormalizingPolicy> TLineCombined;
    typedef Line2D<T, Parametric, NormalizingPolicy> TLineParametric;
	typedef Line2D<T, Cartesian, NormalizingPolicy> TLineCartesian;

	typedef typename TLineCombined::TPoint TPoint;
	typedef typename TLineCombined::TVector TVector;
	typedef typename TLineCombined::TValue TValue;

	const TValue epsilon = TValue(LASS_TEST_PRIM_LINE_2D_EPSILON);

    TLineCombined combined;
    TLineParametric parametric;
    TLineCartesian cartesian;

    BOOST_CHECK(!cartesian.isValid());
    BOOST_CHECK(!parametric.isValid());
    BOOST_CHECK(!combined.isValid());

    // TEST LINES IF FEEDED WITH PARAMETRIC EQUATION

    const TPoint support(1, 2);
    const TVector dir(3, 4);
 
    combined = TLineCombined(support, dir);
    parametric = TLineParametric(support, dir);
    cartesian = TLineCartesian(support, dir);

    BOOST_CHECK(combined.isValid());
    BOOST_CHECK(parametric.isValid());
    BOOST_CHECK(cartesian.isValid());

    // if the lines are feeded by a parametric equation, then only the parametric plane
    // and the combined plane must act exactly identical for operations that involve
    // a support point or any of the direction vectors.  This is because
    // the cartesian line looses info, and has to recreate support point on demand.  
	// Those are not necessarely the same as the originals.
    //
    BOOST_CHECK_EQUAL(combined.support(), parametric.support());
    BOOST_CHECK_EQUAL(combined.direction(), parametric.direction());
	BOOST_CHECK_EQUAL(combined.direction(), cartesian.direction());

    // however, on operations that involve the cartesian quantities (normal and d), they 
    // all should act identical, becuase they all have to generate these out of the 
    // parametric quantities.  In contrary to the planes, this time they HAVE to be equal
	// since (a) normal vector and direction vector is just a rotation of 90 degrees which
	// is perfect and (b) they both calculate the value 'd' in the same way (in case of 
	// normalized vectors, d is twice calculated after normalisation.
    //
    BOOST_CHECK_EQUAL(combined.normal(), parametric.normal());
    BOOST_CHECK_EQUAL(combined.normal(), cartesian.normal());
    BOOST_CHECK_EQUAL(combined.d(), parametric.d());
    BOOST_CHECK_EQUAL(combined.d(), cartesian.d());

	// parametric equation can also be initialized with two points.
	//
    const TPoint lookAt(9, 15);
    combined = TLineCombined(support, lookAt);
    parametric = TLineParametric(support, lookAt);
    cartesian = TLineCartesian(support, lookAt);

	// same conditions should apply as above
	//
    BOOST_CHECK_EQUAL(combined.support(), parametric.support());
    BOOST_CHECK_EQUAL(combined.direction(), parametric.direction());
	BOOST_CHECK_EQUAL(combined.direction(), cartesian.direction());
    BOOST_CHECK_EQUAL(combined.normal(), parametric.normal());
    BOOST_CHECK_EQUAL(combined.normal(), cartesian.normal());
    BOOST_CHECK_EQUAL(combined.d(), parametric.d());
    BOOST_CHECK_EQUAL(combined.d(), cartesian.d());


    // TEST LINES IF FEEDED WITH CARTESIAN EQUATION

    const TVector normal(34, -16);
    const T d = 61;

    combined = TLineCombined(normal, d);
    parametric = TLineParametric(normal, d);
    cartesian = TLineCartesian(normal, d);

	// when created with the cartesian equation, all things should be equal since no information
	// is lost.  well, that's in theory.  for normal and direction vectors this isn't a problem
	// since it's only a 90 degrees rotation.  the value 'd' is another case though.
	// cartesian and combined store the value, so they should give identical values.  parametric
	// doesn't store d, but "converts" it to a support point.  When d is requested, it has to 
	// convert it back.  This will introduce some precision losses.
	//
    BOOST_CHECK_EQUAL(combined.normal(), parametric.normal());
    BOOST_CHECK_EQUAL(combined.normal(), cartesian.normal());
    BOOST_CHECK_CLOSE(combined.d(), parametric.d(), 100 * epsilon);
    BOOST_CHECK_EQUAL(combined.d(), cartesian.d());
	
	// as said, direction vectors should always be the same.  support points don't have to.  
	// cartesian has to calculate it later on request, and this can differ.
	//
    BOOST_CHECK_EQUAL(combined.support(), parametric.support());
    LASS_CLOSE_VECTOR(combined.support(), cartesian.support(), 100 * epsilon);
    BOOST_CHECK_EQUAL(combined.direction(), parametric.direction());
    BOOST_CHECK_EQUAL(combined.direction(), cartesian.direction());

	// you can also initialize cartesian equation with a support point and a normal vector
	//
    combined = TLineCombined(normal, support);
    parametric = TLineParametric(normal, support);
    cartesian = TLineCartesian(normal, support);

	// same conditions should apply as above except for support point of cartesian
	//
    BOOST_CHECK_EQUAL(combined.normal(), parametric.normal());
    BOOST_CHECK_EQUAL(combined.normal(), cartesian.normal());
    BOOST_CHECK_EQUAL(combined.d(), parametric.d());
    BOOST_CHECK_EQUAL(combined.d(), cartesian.d());
    BOOST_CHECK_EQUAL(combined.support(), parametric.support());
    BOOST_CHECK_EQUAL(combined.direction(), parametric.direction());
    BOOST_CHECK_EQUAL(combined.direction(), cartesian.direction());


	// NOW DO SOME TESTS ON FUNCTIONALITY OF LINES
	// normal == (5, 6), support == (1, 2)

	// classifications and distances

	const TPoint front(54, 81);
	const TPoint back(-354, -534);
	BOOST_CHECK_EQUAL(combined.classify(front), sFront);
	BOOST_CHECK_EQUAL(parametric.classify(front), sFront);
	BOOST_CHECK_EQUAL(cartesian.classify(front), sFront);
	BOOST_CHECK_EQUAL(combined.classify(back), sBack);
	BOOST_CHECK_EQUAL(parametric.classify(back), sBack);
	BOOST_CHECK_EQUAL(cartesian.classify(back), sBack);
	BOOST_CHECK_EQUAL(combined.classify(support), sSurface);
	BOOST_CHECK_EQUAL(parametric.classify(support), sSurface);
	BOOST_CHECK_EQUAL(cartesian.classify(support), sSurface);

	BOOST_CHECK_CLOSE(combined.equation(front), parametric.equation(front), 100 * epsilon);
	BOOST_CHECK_EQUAL(combined.equation(front), cartesian.equation(front));
	BOOST_CHECK_CLOSE(combined.equation(back), parametric.equation(back), 100 * epsilon);
	BOOST_CHECK_EQUAL(combined.equation(back), cartesian.equation(back));

	BOOST_CHECK_CLOSE(combined.signedDistance(front), parametric.signedDistance(front), 100 * epsilon);
	BOOST_CHECK_EQUAL(combined.signedDistance(front), cartesian.signedDistance(front));
	BOOST_CHECK_CLOSE(combined.signedDistance(back), parametric.signedDistance(back), 100 * epsilon);
	BOOST_CHECK_EQUAL(combined.signedDistance(back), cartesian.signedDistance(back));

	// projection, rejection and reflection

	const TPoint p(80, 18);
	const TVector v(23, 64);

    // here, combined should behave exactly the same as cartesian.  Both use the same
    // methods with reject as elementary operator, based on the normal vector.  
    // parametric has more advantage to use project as elementary operator, based on the
    // direction vector.  Hence the small difference in precision with the other two.
    //
	BOOST_CHECK_EQUAL(combined.reject(v), cartesian.reject(v));
	BOOST_CHECK_EQUAL(combined.reject(p), cartesian.reject(p));
	BOOST_CHECK_EQUAL(combined.project(v), cartesian.project(v));
	BOOST_CHECK_EQUAL(combined.project(p), cartesian.project(p));
	BOOST_CHECK_EQUAL(combined.reflect(v), cartesian.reflect(v));
	BOOST_CHECK_EQUAL(combined.reflect(p), cartesian.reflect(p));
	LASS_CLOSE_VECTOR(combined.reject(v), parametric.reject(v), 100 * epsilon);
	LASS_CLOSE_VECTOR(combined.reject(p), parametric.reject(p), 100 * epsilon);
	LASS_CLOSE_VECTOR(combined.project(v), parametric.project(v), 100 * epsilon);
	LASS_CLOSE_VECTOR(combined.project(p), parametric.project(p), 100 * epsilon);
	LASS_CLOSE_VECTOR(combined.reflect(v), parametric.reflect(v), 100 * epsilon);
	LASS_CLOSE_VECTOR(combined.reflect(p), parametric.reflect(p), 100 * epsilon);

    // in theory, the sum of the projected and the reject must give the original.
    // in theory ...
    //
	LASS_CLOSE_VECTOR(combined.project(v) + combined.reject(v), v, 100 * epsilon);
	LASS_CLOSE_VECTOR(combined.project(p) + combined.reject(p), p, 100 * epsilon);
	LASS_CLOSE_VECTOR(parametric.project(v) + parametric.reject(v), v, 100 * epsilon);
	LASS_CLOSE_VECTOR(parametric.project(p) + parametric.reject(p), p, 100 * epsilon);
	LASS_CLOSE_VECTOR(cartesian.project(v) + cartesian.reject(v), v, 100 * epsilon);
	LASS_CLOSE_VECTOR(cartesian.project(p) + cartesian.reject(p), p, 100 * epsilon);
	
    // in theory reflecting twice delivers the same point, but this is not so very
    // true (precision difference) since it needs to reject or project two totally
    // different points to get there.
    //
	LASS_CLOSE_VECTOR(combined.reflect(combined.reflect(v)), v, 100 * epsilon);
    LASS_CLOSE_VECTOR(combined.reflect(combined.reflect(p)), p, 100 * epsilon);
	LASS_CLOSE_VECTOR(parametric.reflect(parametric.reflect(v)), v, 100 * epsilon);
	LASS_CLOSE_VECTOR(parametric.reflect(parametric.reflect(p)), p, 100 * epsilon);
	LASS_CLOSE_VECTOR(cartesian.reflect(cartesian.reflect(v)), v, 100 * epsilon);
	LASS_CLOSE_VECTOR(cartesian.reflect(cartesian.reflect(p)), p, 100 * epsilon);


	// point parameters

    // since t is relative to the support point, cartesian will give different
    // results than combined and parametric, cause it can have a different support point.
    //
    // Yet, using t as a parameter in point() should deliver the project(p).
    //
    BOOST_CHECK_EQUAL(combined.point(T(0)), combined.support());
    BOOST_CHECK_EQUAL(combined.point(T(1)), combined.support() + combined.direction()); 
    BOOST_CHECK_EQUAL(combined.point(T(-1)), combined.support() - combined.direction()); 
    BOOST_CHECK_EQUAL(combined.t(combined.support()), T(0));
    BOOST_CHECK_CLOSE(combined.t(combined.support() + combined.direction()), T(1), 100 * epsilon);
    BOOST_CHECK_CLOSE(combined.t(combined.support() - combined.direction()), T(-1), 100 * epsilon);
    LASS_CLOSE_VECTOR(combined.project(p), combined.point(combined.t(p)), 100 * epsilon);

    BOOST_CHECK_EQUAL(parametric.point(T(0)), parametric.support());
    BOOST_CHECK_EQUAL(parametric.point(T(1)), parametric.support() + parametric.direction()); 
    BOOST_CHECK_EQUAL(parametric.point(T(-1)), parametric.support() - parametric.direction()); 
    BOOST_CHECK_EQUAL(parametric.t(parametric.support()), T(0));
    BOOST_CHECK_CLOSE(parametric.t(parametric.support() + parametric.direction()), T(1), 100 * epsilon);
    BOOST_CHECK_CLOSE(parametric.t(parametric.support() - parametric.direction()), T(-1), 100 * epsilon);
    LASS_CLOSE_VECTOR(parametric.project(p), parametric.point(parametric.t(p)), 100 * epsilon);

    BOOST_CHECK_EQUAL(cartesian.point(T(0)), cartesian.support());
    BOOST_CHECK_EQUAL(cartesian.point(T(1)), cartesian.support() + cartesian.direction()); 
    BOOST_CHECK_EQUAL(cartesian.point(T(-1)), cartesian.support() - cartesian.direction()); 
    BOOST_CHECK_EQUAL(cartesian.t(cartesian.support()), T(0));
    BOOST_CHECK_CLOSE(cartesian.t(cartesian.support() + cartesian.direction()), T(1), 100 * epsilon);
    BOOST_CHECK_CLOSE(cartesian.t(cartesian.support() - cartesian.direction()), T(-1), 100 * epsilon);
    LASS_CLOSE_VECTOR(cartesian.project(p), cartesian.point(cartesian.t(p)), 100 * epsilon);

    BOOST_CHECK_EQUAL(combined.t(p), parametric.t(p));
	LASS_CLOSE_VECTOR(parametric.point(parametric.t(p)), combined.point(combined.t(p)), 100 * epsilon);
	LASS_CLOSE_VECTOR(cartesian.point(cartesian.t(p)), combined.point(combined.t(p)), 100 * epsilon);

    // flip, if we flip planes, we need to find negative distances.

    // first, get a snapshot of this situation.
    //
    const TVector normalCo = combined.normal();
    const TVector normalPa = parametric.normal();
    const TVector normalCa = cartesian.normal();
    const TValue distCo = combined.signedDistance(p);
    const TValue distPa = parametric.signedDistance(p);
    const TValue distCa = cartesian.signedDistance(p);
    const TValue tCo = combined.t(p);
    const TValue tPa = parametric.t(p);
    const TValue tCa = cartesian.t(p);
    
    combined.flip();
    parametric.flip();
    cartesian.flip();

    BOOST_CHECK_EQUAL(combined.normal(), -normalCo);
    BOOST_CHECK_EQUAL(parametric.normal(), -normalPa);
    BOOST_CHECK_EQUAL(cartesian.normal(), -normalCa);
    BOOST_CHECK_EQUAL(combined.signedDistance(p), -distCo);
    BOOST_CHECK_EQUAL(parametric.signedDistance(p), -distPa);
    BOOST_CHECK_EQUAL(cartesian.signedDistance(p), -distCa);
    BOOST_CHECK_EQUAL(combined.t(p), -tCo);
    BOOST_CHECK_EQUAL(parametric.t(p), -tPa);
    BOOST_CHECK_EQUAL(cartesian.t(p), -tCa);

    // alright, that's it folks =)
}




}

}

#endif

// EOF
