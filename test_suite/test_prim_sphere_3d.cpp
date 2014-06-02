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

#include "../lass/prim/sphere_3d.h"

namespace lass
{
namespace test
{

template
<
	typename T
>
void testPrimSphere3D()
{
	typedef prim::Sphere3D<T> TSphere;
	typedef prim::Point3D<T> TPoint;
	typedef num::NumTraits<T> TNumTraits;

	const TPoint origin;

	TSphere sphere;
	LASS_TEST_CHECK(sphere.isValid());
	LASS_TEST_CHECK_EQUAL(sphere.center(), origin);
	LASS_TEST_CHECK_EQUAL(sphere.radius(), 0);
	LASS_TEST_CHECK_EQUAL(sphere.area(), 0);
	LASS_TEST_CHECK_EQUAL(sphere.volume(), 0);
	LASS_TEST_CHECK_EQUAL(sphere.classify(origin), prim::sSurface);
	LASS_TEST_CHECK_EQUAL(sphere.equation(origin), 0);
	LASS_TEST_CHECK_EQUAL(sphere.signedDistance(origin), 0);

	const TPoint center(1, 2, 3);
	const T radius = 4;

	sphere = TSphere(center, radius);
	LASS_TEST_CHECK(sphere.isValid());
	LASS_TEST_CHECK_EQUAL(sphere.center(), center);
	LASS_TEST_CHECK_EQUAL(sphere.radius(), radius);
	LASS_TEST_CHECK_EQUAL(sphere.area(), T(4) * TNumTraits::pi * num::sqr(radius));
	LASS_TEST_CHECK_EQUAL(sphere.volume(), T(4) / T(3) * TNumTraits::pi * num::sqr(radius) * radius);
	LASS_TEST_CHECK_EQUAL(sphere.classify(center), prim::sInside);
	LASS_TEST_CHECK_EQUAL(sphere.equation(center), -num::sqr(radius));
	LASS_TEST_CHECK_EQUAL(sphere.signedDistance(center), -radius);
}



TUnitTest test_prim_sphere_3d()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE((testPrimSphere3D<float>)));
	result.push_back(LASS_TEST_CASE((testPrimSphere3D<double>)));
	return result;
}

}

}
