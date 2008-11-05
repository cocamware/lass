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

#include "../num/basic_ops.h"

namespace lass
{
namespace test
{

template <typename T>
void testNumBasicOps()
{
	typedef num::NumTraits<T> TNumTraits;

	// sinc
	T x = TNumTraits::one;
	T y;
	do
	{
		x /= 10;
		y = num::sin(x) / x;
		LASS_TEST_CHECK(y <= TNumTraits::one);
		LASS_TEST_CHECK_EQUAL(num::sinc(x), y);
	}
	while (y < TNumTraits::one);
	LASS_COUT << "sinc(" << typeid(T).name() << ") == 1 for x <= " << x << std::endl;

	// fastSin
	const T x0 = -TNumTraits::pi;
	const T dx = 1e-3f;
	int i = 0;
	x = x0;
	while (x <= TNumTraits::pi)
	{
		LASS_TEST_CHECK(num::abs(num::sin(x) - num::fastSin(x)) < T(0.0012));
		x = x0 + i++ * dx;
	}
}

TUnitTest test_num_basic_ops()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testNumBasicOps<float>));
	result.push_back(LASS_TEST_CASE(testNumBasicOps<double>));
	result.push_back(LASS_TEST_CASE(testNumBasicOps<long double>));
	return result;
}



}

}

// EOF
