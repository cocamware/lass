/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_NUM_TRAITS_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_NUM_TRAITS_INL

#include "test_common.h"
#include "../num/num_traits.h"
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
		volatile char minusOne = zero - one;

		const bool isSigned = minusOne < one;
#ifdef LASS_CHAR_IS_SIGNED
		LASS_TEST_CHECK(isSigned);
#else
		LASS_TEST_CHECK(!isSigned);
#endif

		const char min = isSigned ? -128 : 0;
		const char max = isSigned ? 127 : 255;

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
		LASS_TEST_CHECK_EQUAL(TNumTraits::isSigned == 1, std::numeric_limits<T>::is_signed);
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

void testNumNumTraits()
{
	using namespace num_traits;

	testChar();
	
	testInteger<signed char>();
	testInteger<unsigned char>();
	testInteger<signed short>();
	testInteger<unsigned short>();
	testInteger<signed int>();
	testInteger<unsigned int>();
	testInteger<signed long>();
	testInteger<unsigned long>();
	
	testFloat<float>();
	testFloat<double>();

	testComplex< std::complex<float> >();
	testComplex< std::complex<double> >();
}



}

}

#endif

// EOF
