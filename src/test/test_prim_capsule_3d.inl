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
#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_CAPSULE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_CAPSULE_3D_INL

#include "test_common.h"

#include "../prim/capsule_3d.h"
#include "../prim/ray_3d.h"
#include "../num/random.h"

namespace lass
{
namespace test
{

template<typename T>
void testPrimCapsule3D()
{
	typedef prim::Capsule3D<T> TCapsule;
	typedef prim::Point3D<T> TPoint;
	typedef prim::LineSegment3D<T> TLineSegment;
	typedef prim::Vector3D<T> TVector;
	typedef prim::Ray3D<T> TRay;
	typedef num::NumTraits<T> TNumTraits;
	typedef prim::Aabb3D<T> TAabb;

	const T tolerance = 1e-4f;
	const unsigned n = 100000;

	const TPoint origin;

	TCapsule capsule;
	LASS_TEST_CHECK(capsule.isValid());
	LASS_TEST_CHECK_EQUAL(capsule.axis().head(), origin);
	LASS_TEST_CHECK_EQUAL(capsule.axis().tail(), origin);
	LASS_TEST_CHECK_EQUAL(capsule.radius(), 0);
	LASS_TEST_CHECK_EQUAL(capsule.area(), 0);
	LASS_TEST_CHECK_EQUAL(capsule.volume(), 0);

	const TPoint tail(-4, 0, 6);
	const TPoint head(10, 2, 1);
	const TLineSegment axis(tail, head);
	const T radius = 2;

	capsule = TCapsule(axis, radius);
	LASS_TEST_CHECK(capsule.isValid());
	LASS_TEST_CHECK_EQUAL(capsule.axis().head(), head);
	LASS_TEST_CHECK_EQUAL(capsule.axis().tail(), tail);
	LASS_TEST_CHECK_EQUAL(capsule.radius(), radius);
	LASS_TEST_CHECK_EQUAL(capsule.area(), T(2) * TNumTraits::pi * radius * axis.length() + T(4) * TNumTraits::pi * num::sqr(radius));

	num::RandomMT19937 generator;

	TAabb a = aabb(capsule);
	a.grow(radius);
	TRay testRay;
	T t;
	for (unsigned i = 0; i < n; ++i)
	{
		testRay = TRay(a.random(generator), TVector::random(generator));
		if(intersect(capsule, testRay, t) == prim::rOne)
		{
            LASS_TEST_CHECK_CLOSE( capsule.axis().squaredDistance(testRay.point(t)), num::sqr(capsule.radius()), tolerance); 
			LASS_TEST_CHECK(squaredDistance(capsule.axis(), testRay) <= num::sqr(capsule.radius())); 
		}
		else
		{
			LASS_TEST_CHECK(squaredDistance(capsule.axis(), testRay) > num::sqr(capsule.radius())); 
		}
	}
}
}
}


#endif


