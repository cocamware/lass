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

#include "../num/db.h"

namespace lass
{
namespace test
{

template <typename T>
void testNumDb()
{
	T epsilon = T(1e-5);

	LASS_TEST_CHECK_CLOSE(num::pow2dB(T(0.01)), T(-20), epsilon);
	LASS_TEST_CHECK_CLOSE(num::pow2dB(T(0.1)), T(-10), epsilon);
	LASS_TEST_CHECK(num::abs(num::pow2dB(T(1))) < epsilon);
	LASS_TEST_CHECK_CLOSE(num::pow2dB(T(10)), T(10), epsilon);
	LASS_TEST_CHECK_CLOSE(num::pow2dB(T(100)), T(20), epsilon);
	LASS_TEST_CHECK_EQUAL(num::pow2dB(T(0)), -num::NumTraits<T>::infinity);

	LASS_TEST_CHECK_CLOSE(num::amp2dB(T(0.01)), T(-40), epsilon);
	LASS_TEST_CHECK_CLOSE(num::amp2dB(T(0.1)), T(-20), epsilon);
	LASS_TEST_CHECK(num::abs(num::amp2dB(T(1))) < epsilon);
	LASS_TEST_CHECK_CLOSE(num::amp2dB(T(10)), T(20), epsilon);
	LASS_TEST_CHECK_CLOSE(num::amp2dB(T(100)), T(40), epsilon);
	LASS_TEST_CHECK_EQUAL(num::amp2dB(T(0)), -num::NumTraits<T>::infinity);

	LASS_TEST_CHECK_CLOSE(num::dB2pow(T(-20)), T(0.01), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2pow(T(-10)), T(0.1), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2pow(T(0)), T(1), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2pow(T(10)), T(10), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2pow(T(20)), T(100), epsilon);

	LASS_TEST_CHECK_CLOSE(num::dB2amp(T(-40)), T(0.01), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2amp(T(-20)), T(0.1), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2amp(T(0)), T(1), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2amp(T(20)), T(10), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2amp(T(40)), T(100), epsilon);



	LASS_TEST_CHECK_CLOSE(num::p2dB(T(20.e-8)), T(-40), epsilon);
	LASS_TEST_CHECK_CLOSE(num::p2dB(T(20.e-7)), T(-20), epsilon);
	LASS_TEST_CHECK(num::abs(num::p2dB(T(20.e-6))) < epsilon);
	LASS_TEST_CHECK_CLOSE(num::p2dB(T(20.e-5)), T(20), epsilon);
	LASS_TEST_CHECK_CLOSE(num::p2dB(T(20.e-4)), T(40), epsilon);
	LASS_TEST_CHECK_EQUAL(num::p2dB(T(0)), -num::NumTraits<T>::infinity);
	LASS_TEST_CHECK_THROW(num::p2dB(T(-1)), util::Exception);
	LASS_TEST_CHECK_NO_THROW(num::p2dB(num::NumTraits<T>::qNaN));

	LASS_TEST_CHECK_CLOSE(num::I2dB(T(1.e-14)), T(-20), epsilon);
	LASS_TEST_CHECK_CLOSE(num::I2dB(T(1.e-13)), T(-10), epsilon);
	LASS_TEST_CHECK(num::abs(num::I2dB(T(1.e-12))) < epsilon);
	LASS_TEST_CHECK_CLOSE(num::I2dB(T(1.e-11)), T(10), epsilon);
	LASS_TEST_CHECK_CLOSE(num::I2dB(T(1.e-10)), T(20), epsilon);
	LASS_TEST_CHECK_EQUAL(num::I2dB(T(0)), -num::NumTraits<T>::infinity);
	LASS_TEST_CHECK_THROW(num::I2dB(T(-1)), util::Exception);
	LASS_TEST_CHECK_NO_THROW(num::I2dB(num::NumTraits<T>::qNaN));

	LASS_TEST_CHECK_CLOSE(num::W2dB(T(1.e-14)), T(-20), epsilon);
	LASS_TEST_CHECK_CLOSE(num::W2dB(T(1.e-13)), T(-10), epsilon);
	LASS_TEST_CHECK(num::abs(num::W2dB(T(1.e-12))) < epsilon);
	LASS_TEST_CHECK_CLOSE(num::W2dB(T(1.e-11)), T(10), epsilon);
	LASS_TEST_CHECK_CLOSE(num::W2dB(T(1.e-10)), T(20), epsilon);
	LASS_TEST_CHECK_EQUAL(num::W2dB(T(0)), -num::NumTraits<T>::infinity);
	LASS_TEST_CHECK_THROW(num::W2dB(T(-1)), util::Exception);
	LASS_TEST_CHECK_NO_THROW(num::W2dB(num::NumTraits<T>::qNaN));

	LASS_TEST_CHECK_CLOSE(num::dB2p(T(-40)), T(20.e-8), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2p(T(-20)), T(20.e-7), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2p(T(0)), T(20.e-6), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2p(T(20)), T(20.e-5), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2p(T(40)), T(20.e-4), epsilon);
	LASS_TEST_CHECK_EQUAL(num::dB2p(-num::NumTraits<T>::infinity), T(0));
	LASS_TEST_CHECK_NO_THROW(num::dB2p(num::NumTraits<T>::qNaN));

	LASS_TEST_CHECK_CLOSE(num::dB2I(T(-20)), T(1.e-14), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2I(T(-10)), T(1.e-13), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2I(T(0)), T(1.e-12), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2I(T(10)), T(1.e-11), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2I(T(20)), T(1.e-10), epsilon);
	LASS_TEST_CHECK_EQUAL(num::dB2I(-num::NumTraits<T>::infinity), T(0));
	LASS_TEST_CHECK_NO_THROW(num::dB2I(num::NumTraits<T>::qNaN));

	LASS_TEST_CHECK_CLOSE(num::dB2W(T(-20)), T(1.e-14), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2W(T(-10)), T(1.e-13), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2W(T(0)), T(1.e-12), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2W(T(10)), T(1.e-11), epsilon);
	LASS_TEST_CHECK_CLOSE(num::dB2W(T(20)), T(1.e-10), epsilon);
	LASS_TEST_CHECK_EQUAL(num::dB2W(-num::NumTraits<T>::infinity), T(0));
	LASS_TEST_CHECK_NO_THROW(num::dB2W(num::NumTraits<T>::qNaN));
}

TUnitTest test_num_db()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testNumDb<float>));
	result.push_back(LASS_TEST_CASE(testNumDb<double>));
	result.push_back(LASS_TEST_CASE(testNumDb<long double>));
	return result;
}



}

}

// EOF
