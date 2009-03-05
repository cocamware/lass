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

	LASS_TEST_CHECK(!cartesian.isValid());
	LASS_TEST_CHECK(!parametric.isValid());
	LASS_TEST_CHECK(!combined.isValid());

	// TEST LINES IF FEEDED WITH PARAMETRIC EQUATION

	const TPoint support(1, 2);
	const TVector dir(3, 4);

	combined = TLineCombined(support, dir);
	parametric = TLineParametric(support, dir);
	cartesian = TLineCartesian(support, dir);

	LASS_TEST_CHECK(combined.isValid());
	LASS_TEST_CHECK(parametric.isValid());
	LASS_TEST_CHECK(cartesian.isValid());

	// if the lines are feeded by a parametric equation, then only the parametric plane
	// and the combined plane must act exactly identical for operations that involve
	// a support point or any of the direction vectors.  This is because
	// the cartesian line looses info, and has to recreate support point on demand.
	// Those are not necessarely the same as the originals.
	//
	LASS_TEST_CHECK_EQUAL(combined.support(), parametric.support());
	LASS_TEST_CHECK_EQUAL(combined.direction(), parametric.direction());
	LASS_TEST_CHECK_EQUAL(combined.direction(), cartesian.direction());

	// however, on operations that involve the cartesian quantities (normal and d), they
	// all should act identical, becuase they all have to generate these out of the
	// parametric quantities.  In contrary to the planes, this time they HAVE to be equal
	// since (a) normal vector and direction vector is just a rotation of 90 degrees which
	// is perfect and (b) they both calculate the value 'd' in the same way (in case of
	// normalized vectors, d is twice calculated after normalisation.
	//
	LASS_TEST_CHECK_EQUAL(combined.normal(), parametric.normal());
	LASS_TEST_CHECK_EQUAL(combined.normal(), cartesian.normal());
	LASS_TEST_CHECK_CLOSE(combined.d(), parametric.d(), epsilon);
	LASS_TEST_CHECK_CLOSE(combined.d(), cartesian.d(), epsilon);

	// parametric equation can also be initialized with two points.
	//
	const TPoint lookAt(9, 15);
	combined = TLineCombined(support, lookAt);
	parametric = TLineParametric(support, lookAt);
	cartesian = TLineCartesian(support, lookAt);

	// same conditions should apply as above
	//
	LASS_TEST_CHECK_EQUAL(combined.support(), parametric.support());
	LASS_TEST_CHECK_EQUAL(combined.direction(), parametric.direction());
	LASS_TEST_CHECK_EQUAL(combined.direction(), cartesian.direction());
	LASS_TEST_CHECK_EQUAL(combined.normal(), parametric.normal());
	LASS_TEST_CHECK_EQUAL(combined.normal(), cartesian.normal());
	LASS_TEST_CHECK_CLOSE(combined.d(), parametric.d(), epsilon);
	LASS_TEST_CHECK_CLOSE(combined.d(), cartesian.d(), epsilon); // should be equal ...


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
	LASS_TEST_CHECK_EQUAL(combined.normal(), parametric.normal());
	LASS_TEST_CHECK_EQUAL(combined.normal(), cartesian.normal());
	LASS_TEST_CHECK_CLOSE(combined.d(), parametric.d(), epsilon);
	LASS_TEST_CHECK_EQUAL(combined.d(), cartesian.d());

	// as said, direction vectors should always be the same.  support points don't have to.
	// cartesian has to calculate it later on request, and this can differ.
	//
	LASS_TEST_CHECK_EQUAL(combined.support(), parametric.support());
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.support(), cartesian.support(), epsilon, 2);
	LASS_TEST_CHECK_EQUAL(combined.direction(), parametric.direction());
	LASS_TEST_CHECK_EQUAL(combined.direction(), cartesian.direction());

	// you can also initialize cartesian equation with a support point and a normal vector
	//
	combined = TLineCombined(normal, support);
	parametric = TLineParametric(normal, support);
	cartesian = TLineCartesian(normal, support);

	// same conditions should apply as above except for support point of cartesian
	//
	LASS_TEST_CHECK_EQUAL(combined.normal(), parametric.normal());
	LASS_TEST_CHECK_EQUAL(combined.normal(), cartesian.normal());
	LASS_TEST_CHECK_CLOSE(combined.d(), parametric.d(), epsilon);
	LASS_TEST_CHECK_CLOSE(combined.d(), cartesian.d(), epsilon); // should be equal
	LASS_TEST_CHECK_EQUAL(combined.support(), parametric.support());
	LASS_TEST_CHECK_EQUAL(combined.direction(), parametric.direction());
	LASS_TEST_CHECK_EQUAL(combined.direction(), cartesian.direction());


	// NOW DO SOME TESTS ON FUNCTIONALITY OF LINES
	// normal == (5, 6), support == (1, 2)

	// classifications and distances

	const TPoint front(54, 81);
	const TPoint back(-354, -534);
	LASS_TEST_CHECK_EQUAL(combined.classify(front), sFront);
	LASS_TEST_CHECK_EQUAL(parametric.classify(front), sFront);
	LASS_TEST_CHECK_EQUAL(cartesian.classify(front), sFront);
	LASS_TEST_CHECK_EQUAL(combined.classify(back), sBack);
	LASS_TEST_CHECK_EQUAL(parametric.classify(back), sBack);
	LASS_TEST_CHECK_EQUAL(cartesian.classify(back), sBack);
	LASS_TEST_CHECK_EQUAL(combined.classify(support, epsilon), sSurface);
	LASS_TEST_CHECK_EQUAL(parametric.classify(support, epsilon), sSurface);
	LASS_TEST_CHECK_EQUAL(cartesian.classify(support, epsilon), sSurface);

	LASS_TEST_CHECK_CLOSE(combined.equation(front), parametric.equation(front), epsilon);
	LASS_TEST_CHECK_CLOSE(combined.equation(front), cartesian.equation(front), epsilon);
	LASS_TEST_CHECK_CLOSE(combined.equation(back), parametric.equation(back), epsilon);
	LASS_TEST_CHECK_CLOSE(combined.equation(back), cartesian.equation(back), epsilon);

	LASS_TEST_CHECK_CLOSE(combined.signedDistance(front), parametric.signedDistance(front), epsilon);
	LASS_TEST_CHECK_CLOSE(combined.signedDistance(front), cartesian.signedDistance(front), epsilon);
	LASS_TEST_CHECK_CLOSE(combined.signedDistance(back), parametric.signedDistance(back), epsilon);
	LASS_TEST_CHECK_CLOSE(combined.signedDistance(back), cartesian.signedDistance(back), epsilon);

	// projection, rejection and reflection

	const TPoint p(80, 18);
	const TVector v(23, 64);

	// here, combined should behave exactly the same as cartesian.  Both use the same
	// methods with reject as elementary operator, based on the normal vector.
	// parametric has more advantage to use project as elementary operator, based on the
	// direction vector.  Hence the small difference in precision with the other two.
	//
	LASS_TEST_CHECK_EQUAL(combined.reject(v), cartesian.reject(v));
	LASS_TEST_CHECK_EQUAL(combined.reject(p), cartesian.reject(p));
	LASS_TEST_CHECK_EQUAL(combined.project(v), cartesian.project(v));
	LASS_TEST_CHECK_EQUAL(combined.project(p), cartesian.project(p));
	LASS_TEST_CHECK_EQUAL(combined.reflect(v), cartesian.reflect(v));
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reflect(p), cartesian.reflect(p), epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reject(v), parametric.reject(v), epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reject(p), parametric.reject(p), epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.project(v), parametric.project(v), epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.project(p), parametric.project(p), epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reflect(v), parametric.reflect(v), epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reflect(p), parametric.reflect(p), epsilon, 2);

	// in theory, the sum of the projected and the reject must give the original.
	// in theory ...
	//
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.project(v) + combined.reject(v), v, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.project(p) + combined.reject(p), p, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(parametric.project(v) + parametric.reject(v), v, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(parametric.project(p) + parametric.reject(p), p, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(cartesian.project(v) + cartesian.reject(v), v, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(cartesian.project(p) + cartesian.reject(p), p, epsilon, 2);

	// in theory reflecting twice delivers the same point, but this is not so very
	// true (precision difference) since it needs to reject or project two totally
	// different points to get there.
	//
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reflect(combined.reflect(v)), v, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reflect(combined.reflect(p)), p, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(parametric.reflect(parametric.reflect(v)), v, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(parametric.reflect(parametric.reflect(p)), p, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(cartesian.reflect(cartesian.reflect(v)), v, epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(cartesian.reflect(cartesian.reflect(p)), p, epsilon, 2);


	// point parameters

	// since t is relative to the support point, cartesian will give different
	// results than combined and parametric, cause it can have a different support point.
	//
	// Yet, using t as a parameter in point() should deliver the project(p).
	//
	LASS_TEST_CHECK_EQUAL(combined.point(T(0)), combined.support());
	LASS_TEST_CHECK_EQUAL(combined.point(T(1)), combined.support() + combined.direction());
	LASS_TEST_CHECK_EQUAL(combined.point(T(-1)), combined.support() - combined.direction());
	LASS_TEST_CHECK_EQUAL(combined.t(combined.support()), T(0));
	LASS_TEST_CHECK_CLOSE(combined.t(combined.support() + combined.direction()), T(1), epsilon);
	LASS_TEST_CHECK_CLOSE(combined.t(combined.support() - combined.direction()), T(-1), epsilon);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.project(p), combined.point(combined.t(p)), epsilon, 2);

	LASS_TEST_CHECK_EQUAL(parametric.point(T(0)), parametric.support());
	LASS_TEST_CHECK_EQUAL(parametric.point(T(1)), parametric.support() + parametric.direction());
	LASS_TEST_CHECK_EQUAL(parametric.point(T(-1)), parametric.support() - parametric.direction());
	LASS_TEST_CHECK_EQUAL(parametric.t(parametric.support()), T(0));
	LASS_TEST_CHECK_CLOSE(parametric.t(parametric.support() + parametric.direction()), T(1), epsilon);
	LASS_TEST_CHECK_CLOSE(parametric.t(parametric.support() - parametric.direction()), T(-1), epsilon);
	LASS_TEST_CHECK_CLOSE_ARRAY(parametric.project(p), parametric.point(parametric.t(p)), epsilon, 2);

	LASS_TEST_CHECK_EQUAL(cartesian.point(T(0)), cartesian.support());
	LASS_TEST_CHECK_EQUAL(cartesian.point(T(1)), cartesian.support() + cartesian.direction());
	LASS_TEST_CHECK_EQUAL(cartesian.point(T(-1)), cartesian.support() - cartesian.direction());
	LASS_TEST_CHECK_EQUAL(cartesian.t(cartesian.support()), T(0));
	LASS_TEST_CHECK_CLOSE(cartesian.t(cartesian.support() + cartesian.direction()), T(1), epsilon);
	LASS_TEST_CHECK_CLOSE(cartesian.t(cartesian.support() - cartesian.direction()), T(-1), epsilon);
	LASS_TEST_CHECK_CLOSE_ARRAY(cartesian.project(p), cartesian.point(cartesian.t(p)), epsilon, 2);

	LASS_TEST_CHECK_EQUAL(combined.t(p), parametric.t(p));
	LASS_TEST_CHECK_CLOSE_ARRAY(parametric.point(parametric.t(p)), combined.point(combined.t(p)), epsilon, 2);
	LASS_TEST_CHECK_CLOSE_ARRAY(cartesian.point(cartesian.t(p)), combined.point(combined.t(p)), epsilon, 2);

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

	LASS_TEST_CHECK_EQUAL(combined.normal(), -normalCo);
	LASS_TEST_CHECK_EQUAL(parametric.normal(), -normalPa);
	LASS_TEST_CHECK_EQUAL(cartesian.normal(), -normalCa);
	LASS_TEST_CHECK_EQUAL(combined.signedDistance(p), -distCo);
	LASS_TEST_CHECK_CLOSE(parametric.signedDistance(p), -distPa, epsilon);
	LASS_TEST_CHECK_EQUAL(cartesian.signedDistance(p), -distCa);
	LASS_TEST_CHECK_CLOSE(combined.t(p), -tCo, epsilon);
	LASS_TEST_CHECK_EQUAL(parametric.t(p), -tPa);
	LASS_TEST_CHECK_CLOSE(cartesian.t(p), -tCa, epsilon);

	// alright, that's it folks =)
}


TUnitTest test_prim_line_2d()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE((testPrimLine2D<float, prim::Unnormalized>)));
	result.push_back(LASS_TEST_CASE((testPrimLine2D<float,prim::Normalized>)));
	result.push_back(LASS_TEST_CASE((testPrimLine2D<double, prim::Unnormalized>)));
	result.push_back(LASS_TEST_CASE((testPrimLine2D<double,prim::Normalized>)));
	return result;
}

}

}

// EOF
