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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_PLANE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_PLANE_3D_INL

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

    BOOST_CHECK(!combined.isValid());
    BOOST_CHECK(!cartesian.isValid());
    BOOST_CHECK(!parametric.isValid());

    // TEST PLANES IF FEEDED WITH PARAMETRIC EQUATION

    const Point3D<T> support(74, 25, 69);
    const Vector3D<T> directionU(29, 39, 53);
    const Vector3D<T> directionV(34, 98, 62);
 
    combined = TPlaneCombined(support, directionU, directionV);
    cartesian = TPlaneCartesian(support, directionU, directionV);
    parametric = TPlaneParametric(support, directionU, directionV);

    BOOST_CHECK(combined.isValid());
    BOOST_CHECK(cartesian.isValid());
    BOOST_CHECK(parametric.isValid());

    // if the planes are feeded by a parametric equation, then only the parametric plane
    // and the combined plane must act exactly identical for operations that involve
    // a support point or any of the (reciprocal) direction vectors.  This is because
    // the cartesian plane looses info, and has to recreate support point and (reciprocal)
    // direction vectors on demand.  Those are not necessarely the same as the originals.
    //
    BOOST_CHECK_EQUAL(combined.support(), parametric.support());
    BOOST_CHECK_EQUAL(combined.directionU(), parametric.directionU());
    BOOST_CHECK_EQUAL(combined.directionV(), parametric.directionV());
    BOOST_CHECK_EQUAL(combined.reciprocalU(), parametric.reciprocalU());
    BOOST_CHECK_EQUAL(combined.reciprocalV(), parametric.reciprocalV());

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
    LASS_CLOSE_VECTOR(combined.normal(), parametric.normal(), 100 * epsilon);
    BOOST_CHECK_EQUAL(combined.normal(), cartesian.normal());
    BOOST_CHECK_CLOSE(combined.d(), parametric.d(), 100 * epsilon);
    BOOST_CHECK_EQUAL(combined.d(), cartesian.d());

    // we can also use three points to initialize parametric equation

    const TPoint pointU(92, 49, 84);
    const TPoint pointV(82, 38, 40);

    combined = TPlaneCombined(support, pointU, pointV);
    cartesian = TPlaneCartesian(support, pointU, pointV);
    parametric = TPlaneParametric(support, pointU, pointV);

    BOOST_CHECK(combined.isValid());
    BOOST_CHECK(cartesian.isValid());
    BOOST_CHECK(parametric.isValid());

    // same conditions as above should apply.

    BOOST_CHECK_EQUAL(combined.support(), parametric.support());
    BOOST_CHECK_EQUAL(combined.directionU(), parametric.directionU());
    BOOST_CHECK_EQUAL(combined.directionV(), parametric.directionV());
    BOOST_CHECK_EQUAL(combined.reciprocalU(), parametric.reciprocalU());
    BOOST_CHECK_EQUAL(combined.reciprocalV(), parametric.reciprocalV());
    LASS_CLOSE_VECTOR(combined.normal(), parametric.normal(), 100 * epsilon);
    BOOST_CHECK_EQUAL(combined.normal(), cartesian.normal());
    BOOST_CHECK_CLOSE(combined.d(), parametric.d(), 100 * epsilon);
    BOOST_CHECK_EQUAL(combined.d(), cartesian.d());

   
    // TEST PLANES IF FEEDED WITH CARTESIAN EQUATION

    const Vector3D<T> normal(73, 50, 13);
    const T d = 43;

    combined = TPlaneCombined(normal, d);
    cartesian = TPlaneCartesian(normal, d);
    parametric = TPlaneParametric(normal, d);

    BOOST_CHECK(combined.isValid());
    BOOST_CHECK(cartesian.isValid());
    BOOST_CHECK(parametric.isValid());

	// first, let's see what they say on cartesian quantities.  For cartesian and combined planes,
	// this should be exactly equal.  parametric can a lot in case of unnormalized 
    // policy.  it should still be parallel to the original normal though ...
    //
    BOOST_CHECK_EQUAL(combined.normal(), cartesian.normal());
    LASS_CLOSE_VECTOR(combined.normal().normal(), parametric.normal().normal(), 100 * epsilon);
    BOOST_CHECK_EQUAL(combined.d(), cartesian.d());
    BOOST_CHECK_CLOSE(combined.d() / combined.normal().norm(), 
					  parametric.d() / parametric.normal().norm(), 100 * epsilon);
}




}

}

#endif

// EOF
