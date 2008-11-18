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

#include "../prim/plane_3d.h"

#define LASS_TEST_PRIM_PLANE_3D_EPSILON 1e-4

namespace lass
{
namespace test
{

template <typename T, class NormalizingPolicy>
void testPrimPlane3D()
{
	using namespace prim;

	typedef Plane3D<T, Cartesian, NormalizingPolicy> TPlaneCartesian;
	typedef Plane3D<T, Parametric, NormalizingPolicy> TPlaneParametric;
	typedef Plane3D<T, Combined, NormalizingPolicy> TPlaneCombined;
	typedef Point3D<T> TPoint;
	typedef Vector3D<T> TVector;

	typedef typename TVector::TValue TValue;

	const TValue epsilon = TValue(LASS_TEST_PRIM_PLANE_3D_EPSILON);

	TPlaneCombined combined;
	TPlaneCartesian cartesian;
	TPlaneParametric parametric;

	LASS_TEST_CHECK(!combined.isValid());
	LASS_TEST_CHECK(!cartesian.isValid());
	LASS_TEST_CHECK(!parametric.isValid());

	// TEST PLANES IF FEEDED WITH PARAMETRIC EQUATION

	const Point3D<T> support(74, 25, 69);
	const Vector3D<T> directionU(29, 39, 53);
	const Vector3D<T> directionV(34, 98, 62);

	combined = TPlaneCombined(support, directionU, directionV);
	cartesian = TPlaneCartesian(support, directionU, directionV);
	parametric = TPlaneParametric(support, directionU, directionV);

	LASS_TEST_CHECK(combined.isValid());
	LASS_TEST_CHECK(cartesian.isValid());
	LASS_TEST_CHECK(parametric.isValid());

	// if the planes are feeded by a parametric equation, then only the parametric plane
	// and the combined plane must act exactly identical for operations that involve
	// a support point or any of the (reciprocal) direction vectors.  This is because
	// the cartesian plane looses info, and has to recreate support point and (reciprocal)
	// direction vectors on demand.  Those are not necessarely the same as the originals.
	//
	// Of course, for some reason, this is not entirely the case ...
	//
	Lass_TEST_CHECK_EQUAL(combined.support(), parametric.support());
	LASS_TEST_CHECK_EQUAL(combined.directionU(), parametric.directionU());
	LASS_TEST_CHECK_EQUAL(combined.directionV(), parametric.directionV());
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reciprocalU(), parametric.reciprocalU(), epsilon, 3);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reciprocalV(), parametric.reciprocalV(), epislon, 3);

	// however, on operations that involve the cartesian quantities (normal and d), they
	// all should act identical, becuase they all have to generate these out of the
	// parametric quantities.  However, there's one tricky thingy in case of normalized
	// vectors.  cartesian and combined have calculated the normal based on the original
	// direction vectors.  parametric has to calculate the normal on request, and will have
	// its direction vectors normalized by then.  Now, that doesn't hurt the precision too
	// much, but enough to make sure not all the bits will be the same at the end. i.e.
	// the results won't be truely identical.  There's not much we can do about it, except to use
	// the Unnormalized policy so that the parametric model keeps its original directions, or
	// to let the cartesian and combined model normalize the direction vectors (according to
	// the policy) before generating the normal.  The former is a decision the user has to
	// make.  The latter is a waste of time for the cartesian model but also be a loss of
	// precision, hence no option to take.  So what can be do about it?  Nada, we'll have
	// to live with error.
	//
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.normal(), parametric.normal(), epsilon, 3);
	LASS_TEST_CHECK_EQUAL(combined.normal(), cartesian.normal());
	LASS_TEST_CHECK_CLOSE(combined.d(), parametric.d(), epsilon);
	LASS_TEST_CHECK_EQUAL(combined.d(), cartesian.d());

	// we can also use three points to initialize parametric equation

	const TPoint pointU(92, 49, 84);
	const TPoint pointV(82, 38, 40);

	combined = TPlaneCombined(support, pointU, pointV);
	cartesian = TPlaneCartesian(support, pointU, pointV);
	parametric = TPlaneParametric(support, pointU, pointV);

	LASS_TEST_CHECK(combined.isValid());
	LASS_TEST_CHECK(cartesian.isValid());
	LASS_TEST_CHECK(parametric.isValid());

	// same conditions as above should apply.

	LASS_TEST_CHECK_EQUAL(combined.support(), parametric.support());
	LASS_TEST_CHECK_EQUAL(combined.directionU(), parametric.directionU());
	LASS_TEST_CHECK_EQUAL(combined.directionV(), parametric.directionV());
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reciprocalU(), parametric.reciprocalU(), epsilon, 3);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.reciprocalV(), parametric.reciprocalV(), epsilon, 3);
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.normal(), parametric.normal(), epsilon, 3);
	LASS_TEST_CHECK_EQUAL(combined.normal(), cartesian.normal());
	LASS_TEST_CHECK_CLOSE(combined.d(), parametric.d(), epsilon);
	LASS_TEST_CHECK_EQUAL(combined.d(), cartesian.d());


	// TEST PLANES IF FEEDED WITH CARTESIAN EQUATION

	const Vector3D<T> normal(73, 50, 13);
	const T d = 43;

	combined = TPlaneCombined(normal, d);
	cartesian = TPlaneCartesian(normal, d);
	parametric = TPlaneParametric(normal, d);

	LASS_TEST_CHECK(combined.isValid());
	LASS_TEST_CHECK(cartesian.isValid());
	LASS_TEST_CHECK(parametric.isValid());

	// first, let's see what they say on cartesian quantities.  For cartesian and combined planes,
	// this should be exactly equal.  parametric can a lot in case of unnormalized
	// policy.  it should still be parallel to the original normal though ...
	//
	LASS_TEST_CHECK_EQUAL(combined.normal(), cartesian.normal());
	LASS_TEST_CHECK_CLOSE_ARRAY(combined.normal().normal(), parametric.normal().normal(), epsilon, 3);
	LASS_TEST_CHECK_EQUAL(combined.d(), cartesian.d());
	LASS_TEST_CHECK_CLOSE(combined.d() / combined.normal().norm(),
					  parametric.d() / parametric.normal().norm(), epsilon);
}



TUnitTest test_prim_plane_3d()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE((testPrimPlane3D<float,prim::Normalized>)));
	result.push_back(LASS_TEST_CASE((testPrimPlane3D<float,prim::Unnormalized>)));
	result.push_back(LASS_TEST_CASE((testPrimPlane3D<double,prim::Normalized>)));
	result.push_back(LASS_TEST_CASE((testPrimPlane3D<double,prim::Unnormalized>)));
	return result;
}




}

}

// EOF
