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
 *	Copyright (C) 2004-2010 the Initial Developer.
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
#include "../lass/num/num_traits.h"
#include <limits>

namespace lass
{
namespace test
{
namespace num_traits
{
	void testChar()
	{
		typedef num::NumTraits<char> TNumTraits;
		
		volatile char zero = char(0);
		volatile char one = char(1);
		volatile char minusOne = static_cast<char>(zero - one);

		const bool isSigned = minusOne < one;
#ifdef LASS_CHAR_IS_SIGNED
		LASS_TEST_CHECK(isSigned);
#else
		LASS_TEST_CHECK(!isSigned);
#endif

		const char min = static_cast<char>(isSigned ? -128 : 0);
		const char max = static_cast<char>(isSigned ? 127 : 255);

		LASS_TEST_CHECK(!TNumTraits::isDistribution);
		LASS_TEST_CHECK(TNumTraits::isIntegral);
		LASS_TEST_CHECK(TNumTraits::isNative);
		LASS_TEST_CHECK_EQUAL(TNumTraits::isSigned == 1, isSigned);
		LASS_TEST_CHECK(!TNumTraits::hasInfinity);
		LASS_TEST_CHECK(!TNumTraits::hasNaN);

		LASS_TEST_CHECK_EQUAL(TNumTraits::memorySize, sizeof(char));

		LASS_TEST_CHECK_EQUAL(TNumTraits::zero, char(0));
		LASS_TEST_CHECK_EQUAL(TNumTraits::one, char(1));
		LASS_TEST_CHECK_EQUAL(TNumTraits::epsilon, char(1));
		LASS_TEST_CHECK_EQUAL(TNumTraits::min, min);
		LASS_TEST_CHECK_EQUAL(TNumTraits::max, max);
		LASS_TEST_CHECK_EQUAL(TNumTraits::minStrictPositive, char(1));
	}
	
	template <typename T>
	void testInteger()
	{
		typedef num::NumTraits<T> TNumTraits;

		LASS_TEST_CHECK(!TNumTraits::isDistribution);
		LASS_TEST_CHECK(TNumTraits::isIntegral);
		LASS_TEST_CHECK(TNumTraits::isNative);
		LASS_TEST_CHECK_EQUAL(TNumTraits::isSigned == 1, bool(std::numeric_limits<T>::is_signed));
		LASS_TEST_CHECK(!TNumTraits::hasInfinity);
		LASS_TEST_CHECK(!TNumTraits::hasNaN);

		LASS_TEST_CHECK_EQUAL(TNumTraits::memorySize, sizeof(T));

		LASS_TEST_CHECK_EQUAL(TNumTraits::zero, T(0));
		LASS_TEST_CHECK_EQUAL(TNumTraits::one, T(1));
		LASS_TEST_CHECK_EQUAL(TNumTraits::epsilon, T(1));
		LASS_TEST_CHECK_EQUAL(TNumTraits::min, std::numeric_limits<T>::min());
		LASS_TEST_CHECK_EQUAL(TNumTraits::max, std::numeric_limits<T>::max());
		LASS_TEST_CHECK_EQUAL(TNumTraits::minStrictPositive, T(1));
	}

	template <typename T>
	void testFloat()
	{
		typedef num::NumTraits<T> TNumTraits;
		
		const T tolerance = 1e-7f;

		LASS_TEST_CHECK(!TNumTraits::isDistribution);
		LASS_TEST_CHECK(!TNumTraits::isIntegral);
		LASS_TEST_CHECK(TNumTraits::isNative);
		LASS_TEST_CHECK(TNumTraits::isSigned);
		LASS_TEST_CHECK(TNumTraits::hasInfinity);
		LASS_TEST_CHECK(TNumTraits::hasNaN);

		LASS_TEST_CHECK_EQUAL(TNumTraits::memorySize, sizeof(T));

		LASS_TEST_CHECK(num::isNaN(TNumTraits::qNaN));
		LASS_TEST_CHECK(num::isNaN(TNumTraits::sNaN));
		LASS_TEST_CHECK_EQUAL(TNumTraits::zero, T(0));
		LASS_TEST_CHECK_EQUAL(TNumTraits::one, T(1));
		LASS_TEST_CHECK_EQUAL(TNumTraits::infinity, std::numeric_limits<T>::infinity());
		LASS_TEST_CHECK_EQUAL(TNumTraits::epsilon, std::numeric_limits<T>::epsilon());
		LASS_TEST_CHECK_EQUAL(TNumTraits::min, -std::numeric_limits<T>::max());
		LASS_TEST_CHECK_EQUAL(TNumTraits::max, std::numeric_limits<T>::max());
		LASS_TEST_CHECK_EQUAL(TNumTraits::minStrictPositive, std::numeric_limits<T>::min());
		LASS_TEST_CHECK_CLOSE(TNumTraits::pi, T(4) * num::atan(T(1)), tolerance);
		LASS_TEST_CHECK_CLOSE(TNumTraits::e, num::exp(T(1)), tolerance);
		LASS_TEST_CHECK_CLOSE(TNumTraits::sqrt2, num::sqrt(T(2)), tolerance);
		LASS_TEST_CHECK_CLOSE(TNumTraits::sqrtPi, num::sqrt(TNumTraits::pi), tolerance);
	}


	template <typename T>
	void testComplex()
	{
		typedef num::NumTraits<T> TNumTraits;
		typedef typename TNumTraits::baseType TBase;
		
		const TBase tolerance = 1e-7f;

		LASS_TEST_CHECK(!TNumTraits::isDistribution);
		LASS_TEST_CHECK(!TNumTraits::isIntegral);
		LASS_TEST_CHECK(!TNumTraits::isNative);
		LASS_TEST_CHECK(TNumTraits::isSigned);
		LASS_TEST_CHECK(!TNumTraits::hasInfinity);
		LASS_TEST_CHECK(!TNumTraits::hasNaN);

		LASS_TEST_CHECK_EQUAL(TNumTraits::memorySize, sizeof(T));

		LASS_TEST_CHECK_EQUAL(TNumTraits::zero, T(0));
		LASS_TEST_CHECK_EQUAL(TNumTraits::one, T(1));
		LASS_TEST_CHECK_CLOSE(TNumTraits::pi.real(), TBase(4) * num::atan(TBase(1)), tolerance);
		LASS_TEST_CHECK_EQUAL(TNumTraits::pi.imag(), TBase(0));
		LASS_TEST_CHECK_CLOSE(TNumTraits::e.real(), num::exp(T(1)).real(), tolerance);
		LASS_TEST_CHECK_EQUAL(TNumTraits::e.imag(), num::exp(T(1)).imag());
		LASS_TEST_CHECK_CLOSE(TNumTraits::sqrt2.real(), num::sqrt(T(2)).real(), tolerance);
		LASS_TEST_CHECK_EQUAL(TNumTraits::sqrt2.imag(), num::sqrt(T(2)).imag());
		LASS_TEST_CHECK_CLOSE(TNumTraits::sqrtPi.real(), num::sqrt(TNumTraits::pi).real(), tolerance);
		LASS_TEST_CHECK_EQUAL(TNumTraits::sqrtPi.imag(), num::sqrt(TNumTraits::pi).imag());
	}

}

TUnitTest test_num_num_traits()
{
	using namespace num_traits;

	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testChar));
	
	result.push_back(LASS_TEST_CASE(testInteger<signed char>));
	result.push_back(LASS_TEST_CASE(testInteger<unsigned char>));
	result.push_back(LASS_TEST_CASE(testInteger<signed short>));
	result.push_back(LASS_TEST_CASE(testInteger<unsigned short>));
	result.push_back(LASS_TEST_CASE(testInteger<signed int>));
	result.push_back(LASS_TEST_CASE(testInteger<unsigned int>));
	result.push_back(LASS_TEST_CASE(testInteger<signed long>));
	result.push_back(LASS_TEST_CASE(testInteger<unsigned long>));
	
	result.push_back(LASS_TEST_CASE(testFloat<float>));
	result.push_back(LASS_TEST_CASE(testFloat<double>));

	result.push_back(LASS_TEST_CASE(testComplex< std::complex<float> >));
	result.push_back(LASS_TEST_CASE(testComplex< std::complex<double> >));

	return result;
}



}

}

// EOF
