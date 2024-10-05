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
 *	Copyright (C) 2024 the Initial Developer.
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

#include "../lass/num/double_double.h"

#include <random>

#if TEST_HAVE_QUADMATH

#include <quadmath.h>

namespace lass
{
namespace num
{

inline __float128 sqrt(const __float128& x) { return sqrtq(x); }
inline __float128 abs(const __float128& x) { return __builtin_fabsq(x); }

inline __float128 toFloat128(const DoubleDouble& x)
{
    return static_cast<__float128>(x.high()) + static_cast<__float128>(x.low());
}

}
}

#endif

namespace
{

using namespace lass;

template <typename T, typename RNG>
T randomFloat(RNG&& rng)
{
	// generate a random SingleType in the entire range of normal floats
	// by generating a random significand and exponent and combining them into a SingleType
	using TSingleTraits = num::NumTraits<T>;
	constexpr T zero = TSingleTraits::zero;
	constexpr T one = TSingleTraits::one;

	// we limit the exponent to half the range, so that sum of exponents fits in the range
	// when multiplying two numbers.
	// we further offset minExp with size of significand (in bits), to avoid subnormal numbers in low part.
	constexpr int minExp = std::numeric_limits<T>::min_exponent / 2 + std::numeric_limits<T>::digits;
	constexpr int maxExp = std::numeric_limits<T>::max_exponent / 2 - std::numeric_limits<T>::digits;
	static_assert(minExp < 0 && maxExp > 0);

	const T sign = std::uniform_real_distribution<T>(zero, one)(rng) < (one / 2) ? one : -one;
	const T significand = std::uniform_real_distribution<T>(zero, one)(rng);
	const int exponent = std::uniform_int_distribution<int>(minExp, maxExp)(rng);

	return sign * num::ldexp(significand, exponent);
}

template <typename T, typename RNG>
num::DoubleWordFloat<T> randomDoubleFloat(RNG&& rng)
{
	// we limit the exponent to half the range, so that sum of exponents fits in the range
	// when multiplying two numbers.
	// we further offset minExp with size of significand (in bits), to avoid subnormal numbers in low part.
	constexpr int minExp = std::numeric_limits<T>::min_exponent / 2 + std::numeric_limits<T>::digits;
	constexpr int maxExp = std::numeric_limits<T>::max_exponent / 2 - std::numeric_limits<T>::digits;
	static_assert(minExp < 0 && maxExp > 0);

	using TSingleTraits = num::NumTraits<T>;
	constexpr T zero = TSingleTraits::zero;
	constexpr T one = TSingleTraits::one;

	const T sign1 = std::uniform_real_distribution<T>(zero, one)(rng) < (one / 2) ? one : -one;
	const T significand1 = std::uniform_real_distribution<T>(zero, one)(rng);
	const int exponent1 = std::uniform_int_distribution<int>(minExp, maxExp)(rng);
	const T high = sign1 * num::ldexp(significand1, exponent1);

	const T sign2 = std::uniform_real_distribution<T>(zero, one)(rng) < (one / 2) ? one : -one;
	const T significand2 = std::uniform_real_distribution<T>(zero, one)(rng);
	const int exponent2 = exponent1 - std::numeric_limits<T>::digits;
	const T low = sign2 * num::ldexp(significand2, exponent2);

	return num::DoubleWordFloat<T>::sum2Fast(high, low);
}


template <typename T>
T relError(T a, T b)
{
	return num::abs((a - b) / b);
}

}

namespace lass
{
namespace test
{

// we're actually going to test double-float, because we can compare it to a single double.

template <typename SingleType, typename DoubleType>
void testNumDoubleFloat()
{
	using TSingleTraits = num::NumTraits<SingleType>;
	using TDoubleSingle = num::DoubleWordFloat<SingleType>;

	std::mt19937_64 rng;

	auto toDouble = [](const TDoubleSingle& a) { return static_cast<DoubleType>(a.high()) + static_cast<DoubleType>(a.low()); };

	const DoubleType u = TSingleTraits::epsilon / 2; // unit roundoff
	const DoubleType u2 = u * u;
	const DoubleType u3 = u2 * u;

	{
		constexpr size_t n = 100000;
		for (size_t i = 0; i < n; ++i)
		{
			const SingleType a = randomFloat<SingleType>(rng);
			const SingleType b = randomFloat<SingleType>(rng);

			// DW + FP, DWPlusFP, bound = 2u²
			{
				const DoubleType d = static_cast<DoubleType>(a) + static_cast<DoubleType>(b);
				const TDoubleSingle df = TDoubleSingle(a) + b;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(df), static_cast<SingleType>(d));
				LASS_TEST_CHECK(relError(toDouble(df), d) < 2 * u2);
			}

			// DW - FP, DWPlusFP, bound = 2u²
			{
				const DoubleType d = static_cast<DoubleType>(a) - static_cast<DoubleType>(b);
				const TDoubleSingle df = TDoubleSingle(a) - b;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(df), static_cast<SingleType>(d));
				LASS_TEST_CHECK(relError(toDouble(df), d) < 2 * u2);
			}

			// DW + DW, AccurateDWPlusDW, bound = 3u²+13u³
			{
				const DoubleType d = static_cast<DoubleType>(a) + static_cast<DoubleType>(b);
				const TDoubleSingle df = TDoubleSingle(a) + TDoubleSingle(b);
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(df), static_cast<SingleType>(d));
				LASS_TEST_CHECK(relError(toDouble(df), d) < 3 * u2 + 13 * u3);
			}

			// DW - DW, AccurateDWPlusDW, bound = 3u²+13u³
			{
				const DoubleType d = static_cast<DoubleType>(a) - static_cast<DoubleType>(b);
				const TDoubleSingle df = TDoubleSingle(a) - TDoubleSingle(b);
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(df), static_cast<SingleType>(d));
				LASS_TEST_CHECK(relError(toDouble(df), d) < 3 * u2 + 13 * u3);
			}

			// DW * FP, DWTimesFP3, bound = 2u²
			{
				const DoubleType d = static_cast<DoubleType>(a) * static_cast<DoubleType>(b);
				const TDoubleSingle df = TDoubleSingle(a) * b;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(df), static_cast<SingleType>(d));
				LASS_TEST_CHECK(relError(toDouble(df), d) <= 2 * u2);
			}

			// DW * DW, DWTimesDW3, bound = 4u²
			{
				const DoubleType d = static_cast<DoubleType>(a) * static_cast<DoubleType>(b);
				const TDoubleSingle df = TDoubleSingle(a) * TDoubleSingle(b);
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(df), static_cast<SingleType>(d));
				LASS_TEST_CHECK(relError(toDouble(df), d) < 4 * u2);
			}

			// DW / FP, DWDivFP3, bound = 3u²
			{
				const DoubleType d = static_cast<DoubleType>(a) / static_cast<DoubleType>(b);
				const TDoubleSingle df = TDoubleSingle(a) / b;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(df), static_cast<SingleType>(d));
				LASS_TEST_CHECK(relError(toDouble(df), d) < 3 * u2);
			}

			// DW / DW, DWDivDW3, bound = 9.8u²
			{
				const DoubleType d = static_cast<DoubleType>(a) / static_cast<DoubleType>(b);
				const TDoubleSingle df = TDoubleSingle(a) / TDoubleSingle(b);
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(df), static_cast<SingleType>(d));
				LASS_TEST_CHECK(relError(toDouble(df), d) < 9.8 * u2);
			}

			// sqrt(DW), SQRTDWtoDW, bound = 25/8*u²
			{
				const DoubleType d = num::sqrt(static_cast<DoubleType>(num::abs(a)));
				const TDoubleSingle df = num::sqrt(TDoubleSingle(num::abs(a)));
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(df), static_cast<SingleType>(d));
				LASS_TEST_CHECK(relError(toDouble(df), d) < 3.125 * u2);
			}
		}
	}

	{
		constexpr size_t n = 100000;
		for (size_t i = 0; i < n; ++i)
		{
			const TDoubleSingle aa = randomDoubleFloat<SingleType>(rng);
			const TDoubleSingle bb = randomDoubleFloat<SingleType>(rng);
			const DoubleType a = static_cast<DoubleType>(aa.high()) + static_cast<DoubleType>(aa.low());
			const DoubleType b = static_cast<DoubleType>(bb.high()) + static_cast<DoubleType>(bb.low());
			const SingleType bh = static_cast<SingleType>(bb.high());

			// DW + FP, DWPlusFP, bound = 2u²
			{
				const DoubleType c = a + static_cast<DoubleType>(bh);
				const TDoubleSingle cc = aa + bh;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(cc), static_cast<SingleType>(c));
				LASS_TEST_CHECK(relError(toDouble(cc), c) < 2 * u2);
			}

			// DW - FP, DWPlusFP, bound = 2u²
			{
				const DoubleType c = a - bh;
				const TDoubleSingle cc = aa - bh;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(cc), static_cast<SingleType>(c));
				LASS_TEST_CHECK(relError(toDouble(cc), c) < 2 * u2);
			}

			// DW + DW, AccurateDWPlusDW, bound = 3u²+13u³
			{
				const DoubleType c = a + b;
				const TDoubleSingle cc = aa + bb;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(cc), static_cast<SingleType>(c));
				LASS_TEST_CHECK(relError(toDouble(cc), c) < 3 * u2 + 13 * u3);
			}

			// DW - DW, AccurateDWPlusDW, bound = 3u²+13u³
			{
				const DoubleType c = a - b;
				const TDoubleSingle cc = aa - bb;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(cc), static_cast<SingleType>(c));
				LASS_TEST_CHECK(relError(toDouble(cc), c) < 3 * u2 + 13 * u3);
			}

			// DW * FP, DWTimesFP3, bound = 2u²
			{
				const DoubleType c = a * bh;
				const TDoubleSingle cc = aa * bh;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(cc), static_cast<SingleType>(c));
				LASS_TEST_CHECK(relError(toDouble(cc), c) <= 2 * u2);
			}

			// DW * DW, DWTimesDW3, bound = 4u²
			{
				const DoubleType c = a * b;
				const TDoubleSingle cc = aa * bb;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(cc), static_cast<SingleType>(c));
				LASS_TEST_CHECK(relError(toDouble(cc), c) < 4 * u2);
			}

			// DW / FP, DWDivFP3, bound = 3u²
			{
				const DoubleType c = a / bh;
				const TDoubleSingle cc = aa / bh;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(cc), static_cast<SingleType>(c));
				LASS_TEST_CHECK(relError(toDouble(cc), c) < 3 * u2);
			}

			// DW / DW, bound = 9.8u²
			{
				const DoubleType c = a / b;
				const TDoubleSingle cc = aa / bb;
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(cc), static_cast<SingleType>(c));
				LASS_TEST_CHECK(relError(toDouble(cc), c) < 9.8 * u2);
			}

			// sqrt(DW), SQRTDWtoDW, algorithm 17, bound = 25/8*u²
			{
				const DoubleType c = num::sqrt(num::abs(a));
				const TDoubleSingle cc = num::sqrt(num::abs(aa));
				LASS_TEST_CHECK_EQUAL(static_cast<SingleType>(cc), static_cast<SingleType>(c));
				LASS_TEST_CHECK(relError(toDouble(cc), c) < 3.125 * u2);
			}
		}
	}
}


void testDoubleDoubleNumTraits()
{
	using TNumTraits = num::NumTraits<num::DoubleDouble>;
	using TDoubleTraits = num::NumTraits<double>;

	LASS_TEST_CHECK_EQUAL(TNumTraits::zero.high(), 0.0);
	LASS_TEST_CHECK_EQUAL(TNumTraits::zero.low(), 0.0);
	LASS_TEST_CHECK_EQUAL(TNumTraits::one.high(), 1.0);
	LASS_TEST_CHECK_EQUAL(TNumTraits::one.low(), 0.0);
	LASS_TEST_CHECK(num::isInf(TNumTraits::infinity.high()));
	LASS_TEST_CHECK(num::isInf(TNumTraits::infinity.low()));
	LASS_TEST_CHECK(num::isNaN(TNumTraits::qNaN.high()));
	LASS_TEST_CHECK(num::isNaN(TNumTraits::qNaN.low()));
	LASS_TEST_CHECK(num::isNaN(TNumTraits::sNaN.high()));
	LASS_TEST_CHECK(num::isNaN(TNumTraits::sNaN.low()));

	{
		const auto x = (TNumTraits::one + (TNumTraits::one + TDoubleTraits::epsilon)) / 2.0;
		LASS_TEST_CHECK(static_cast<double>(x) == 1.0 || static_cast<double>(x) == (1.0 + TDoubleTraits::epsilon));
		LASS_TEST_CHECK_EQUAL(x.low(), TDoubleTraits::epsilon / 2);
		const auto eps = std::nextafter(x.low(), 1.0) - x.low();
		LASS_TEST_CHECK_EQUAL(static_cast<double>(TNumTraits::epsilon), eps);
#if TEST_HAVE_QUADMATH
		LASS_TEST_CHECK(toFloat128(TNumTraits::epsilon) == static_cast<__float128>(eps));
#endif
	}

	{
		// min and max are still problematic because incrementing them by epsilon doesn't make them infinite
		
		LASS_TEST_CHECK(!num::isInf(TNumTraits::max));
		const auto moreThanMax = TNumTraits::max + TNumTraits::max * TNumTraits::epsilon;
		LASS_COUT << "moreThanMax=" << moreThanMax.high() << " + " << moreThanMax.low() << std::endl; // prints -nan +  -nan :-(
		// LASS_TEST_CHECK(num::isInf(moreThanMax)); // fails

		LASS_TEST_CHECK(!num::isInf(TNumTraits::min));
		const auto lessThanMin = TNumTraits::min - TNumTraits::min * TNumTraits::epsilon;
		LASS_COUT << "lessThanMin=" << lessThanMin.high() << " + " << lessThanMin.low() << std::endl; // prints -1.79769e+308 + -9.9792e+291 :-(
		// LASS_TEST_CHECK(num::isInf(lessThanMin)); // fails
	}

	LASS_TEST_CHECK_EQUAL(static_cast<double>(TNumTraits::pi), TDoubleTraits::pi);
	LASS_TEST_CHECK_EQUAL(static_cast<double>(TNumTraits::e), TDoubleTraits::e);
	LASS_TEST_CHECK_EQUAL(static_cast<double>(TNumTraits::sqrt2), TDoubleTraits::sqrt2);
	LASS_TEST_CHECK_EQUAL(static_cast<double>(TNumTraits::sqrtPi), TDoubleTraits::sqrtPi);
	LASS_TEST_CHECK_EQUAL(static_cast<double>(TNumTraits::log2), num::log(2.0));

#if TEST_HAVE_QUADMATH
	{
		const __float128 u = toFloat128(TNumTraits::epsilon) / 2; // unit roundoff
		LASS_TEST_CHECK(relError(toFloat128(TNumTraits::pi), M_PIq) < u);
		LASS_TEST_CHECK(relError(toFloat128(TNumTraits::e), M_Eq) < u);
		LASS_TEST_CHECK(relError(toFloat128(TNumTraits::sqrt2), M_SQRT2q) < u);
		LASS_TEST_CHECK(relError(toFloat128(TNumTraits::sqrtPi), sqrtq(M_PIq)) < u);
		LASS_TEST_CHECK(relError(toFloat128(TNumTraits::log2), M_LN2q) < u);
	}
#endif

	{
		// gamma(n) = (n * u) / (1 - n * u)
		const double u = static_cast<double>(TNumTraits::epsilon) / 2;
		for (unsigned n = 1; n < 10; ++n)
		{
			const auto gamma = TNumTraits::gamma(n);

			const auto nu = n * u;
			const auto oneMinNu = TNumTraits::one - nu;
			LASS_TEST_CHECK_EQUAL(oneMinNu.high(), 1.0);
			LASS_TEST_CHECK_EQUAL(oneMinNu.low(), -nu);
			const auto gamma2 = nu / oneMinNu;
			LASS_TEST_CHECK_EQUAL(gamma.high(), gamma2.high());
			LASS_TEST_CHECK_EQUAL(gamma.low(), gamma2.low());

#if TEST_HAVE_QUADMATH
			const auto gamma3 = (n * static_cast<__float128>(u)) / (1 - n * static_cast<__float128>(u));
			LASS_TEST_CHECK(toFloat128(gamma) == gamma3);
#endif
		}
	}
}


TUnitTest test_num_double_double()
{
	return TUnitTest{
		LASS_TEST_CASE((testNumDoubleFloat<float, double>)),
#if TEST_HAVE_QUADMATH
		LASS_TEST_CASE((testNumDoubleFloat<double, __float128>)),
#endif
		LASS_TEST_CASE(testDoubleDoubleNumTraits),
};
}


}

}
