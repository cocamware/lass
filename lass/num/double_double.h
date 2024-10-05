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

#pragma once

#include "num_common.h"
#include <utility>

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#else
#   pragma GCC push_options
#   pragma GCC optimize ("-fno-unsafe-math-optimizations")
#endif

namespace lass
{
namespace num
{

template <typename T>
constexpr std::pair<T, T> sum2Fast(T a, T b)
{
	const T s = a + b;
	const T z = s - a;
	const T t = b - z;
	return std::make_pair(s, t);
}

template <typename T>
constexpr std::pair<T, T> sum2(T a, T b)
{
	const T s = a + b;
	const T a1 = s - b;
	const T b1 = s - a1;
	const T t = (a - a1) + (b - b1);
	return std::make_pair(s, t);
}

template <typename T>
constexpr std::pair<T, T> prod2(T a, T b)
{
	const T p = a * b;
	const T q = std::fma(a, b, -p);
	return std::make_pair(p, q);
}


/**	@class lass::num::DoubleWordFloat
 *	@author Bram de Greve [BdG]
 *
 *	@note S. Boldo, C.-P. Jeannerod, G. Melquiond, J.-M. Muller. Floating-point arithmetic.
 *		Acta Numerica, 2023, 32, pp.203-290. 10.1017/S0962492922000101. hal-04095151
 *		https://doi.org/10.1017/S0962492922000101
 *		https://hal.science/hal-04095151
 *
 *	@note M.M. Joldes, J.-M. Muller, V. Popescu. Tight and rigorous error bounds for
 *		basic building blocks of double-word arithmetic. ACM Transactions of Mathematical
 *		Software, 2017, 44 (2), pp.1-27. 10.1145/3121432. hal-01351529v3
 *		https://doi.org/10.1145/3121432
 *		https://hal.science/hal-01351529v3
 *
 *	@note J.-M. Muller, L. Rideau. Formalization of double-word arithmetic, and comments on
 *		"Tight and rigorous error bounds for basic building blocks of double-word arithmetic".
 *		ACM Transactions of Mathematical Software, 2022, 48 (1), pp.1-24. 10.1145/3484514. hal-02972245v2
 *		https://doi.org/10.1145/3484514
 *		https://hal.science/hal-02972245v2
 */
template <typename T>
class DoubleWordFloat
{
public:
	using TValue = T;

	constexpr DoubleWordFloat(): DoubleWordFloat(0, 0) {}
	constexpr DoubleWordFloat(TValue value): DoubleWordFloat(value, 0) {}
	constexpr DoubleWordFloat(TValue high, TValue low): high_(high), low_(low) {}

	explicit operator TValue() const { return high_; }
	TValue high() const { return high_; }
	TValue low() const { return low_; }

	const DoubleWordFloat& operator+() const { return *this; }
	DoubleWordFloat operator-() const { return DoubleWordFloat(-high_, -low_); }

	/** DW + FP, DWPlusFP, bound = 2u² */
	DoubleWordFloat& operator+=(TValue other)
	{
		const auto s = sum2(high_, other);
		const auto v = low_ + s.low_;
		*this = sum2Fast(s.high_, v);
		return *this;
	}
	/** DW - FP, DWPlusFP, bound = 2u² */
	DoubleWordFloat& operator-=(TValue other)
	{
		return *this += -other;
	}

	/** DW + DW, AccurateDWPlusDW, bound = 3u²+13u³ */
	DoubleWordFloat& operator+=(const DoubleWordFloat& other)
	{
		const auto s = sum2(high_, other.high_);
		const auto t = sum2(low_, other.low_);
		const auto c = s.low_ + t.high_;
		const auto v = sum2Fast(s.high_, c);
		const auto w = t.low_ + v.low_;
		*this = sum2Fast(v.high_, w);
		return *this;
	}
	/** DW - DW, AccurateDWPlusDW, bound = 3u²+13u³ */
	DoubleWordFloat& operator-=(const DoubleWordFloat& other)
	{
		return *this += -other;
	}

	/** DW * FP, DWTimesFP3, bound = 2u² */
	DoubleWordFloat& operator*=(TValue other)
	{
		const auto p = prod2(high_, other);
		const auto c3 = std::fma(low_, other, p.low_);
		*this = sum2Fast(p.high_, c3);
		return *this;
	}

	/** DW * DW, DWTimesDW3, bound = 4u² */
	DoubleWordFloat& operator*=(const DoubleWordFloat& other)
	{
		const auto c = prod2(high_, other.high_);
		const auto t0 = prod2(low_, other.low_);
		const auto t1 = std::fma(high_, other.low_, t0.low_);
		const auto c2 = std::fma(low_, other.high_, t1);
		const auto c3 = c.low_ + c2;
		*this = sum2Fast(c.high_, c3);
		return *this;
	}

	/** DW / FP, DWDivFP3, bound = 3u² */
	DoubleWordFloat& operator/=(TValue other)
	{
		const auto th = high_ / other;
		const auto p = prod2(th, other);
		const auto dh = high_ - p.high_;
		const auto dt = dh - p.low_;
		const auto d = dt + low_;
		const auto tl = d / other;
		*this = sum2Fast(th, tl);
		return *this;
	}

	/** DW / DW, DWDivDW3, bound = 9.8u² */
	DoubleWordFloat& operator/=(const DoubleWordFloat& other)
	{
		constexpr TValue one = 1;
		const auto th = one / other.high_;
		const auto rh = std::fma(-other.high_, th, one);
		const auto rl = -other.low_ * th;
		const auto e = sum2Fast(rh, rl);
		const auto d = e * th;
		const auto m = d + th;
		*this *= m;
		return *this;
	}

	bool operator==(TValue other) const
	{
		return high_ == other && low_ == 0;
	}
	bool operator!=(TValue other) const
	{
		return high_ != other || low_ != 0;
	}
	bool operator<(TValue other) const
	{
		return high_ < other || (high_ == other && low_ < 0);
	}

	bool operator==(const DoubleWordFloat& other) const
	{
		return high_ == other.high_ && low_ == other.low_;
	}
	bool operator!=(const DoubleWordFloat& other) const
	{
		return high_ != other.high_ || low_ != other.low_;
	}

	auto operator<=>(TValue other) const
	{
		if (high_ < other)
		{
			return std::weak_ordering::less;
		}
		if (high_ > other)
		{
			return std::weak_ordering::greater;
		}
		if (low_ < 0)
		{
			return std::weak_ordering::less;
		}
		if (low_ > 0)
		{
			return std::weak_ordering::greater;
		}
		return std::weak_ordering::equivalent;
	}

	auto operator<=>(const DoubleWordFloat& other) const
	{
		if (high_ < other.high_)
		{
			return std::weak_ordering::less;
		}
		if (high_ > other.high_)
		{
			return std::weak_ordering::greater;
		}
		if (low_ < other.low_)
		{
			return std::weak_ordering::less;
		}
		if (low_ > other.low_)
		{
			return std::weak_ordering::greater;
		}
		return std::weak_ordering::equivalent;
	}

	static constexpr DoubleWordFloat sum2(TValue a, TValue b)
	{
		const auto [s, t] = lass::num::sum2(a, b);
		return DoubleWordFloat(s, t);
	}
	static constexpr DoubleWordFloat sum2Fast(TValue a, TValue b)
	{
		const auto [s, t] = lass::num::sum2Fast(a, b);
		return DoubleWordFloat(s, t);
	}
	static constexpr DoubleWordFloat prod2(TValue a, TValue b)
	{
		const auto [p, q] = lass::num::prod2(a, b);
		return DoubleWordFloat(p, q);
	}

	TValue high_;
	TValue low_;
};

using DoubleDouble = DoubleWordFloat<double>;

template <typename T> inline DoubleWordFloat<T> operator+(DoubleWordFloat<T> a, T b) { return a += b; }	/**< @relates DoubleWordFloat */
template <typename T> inline DoubleWordFloat<T> operator-(DoubleWordFloat<T> a, T b) { return a -= b; }	/**< @relates DoubleWordFloat */
template <typename T> inline DoubleWordFloat<T> operator*(DoubleWordFloat<T> a, T b) { return a *= b; }	/**< @relates DoubleWordFloat */
template <typename T> inline DoubleWordFloat<T> operator/(DoubleWordFloat<T> a, T b) { return a /= b; }	/**< @relates DoubleWordFloat */

template <typename T> inline DoubleWordFloat<T> operator+(T a, DoubleWordFloat<T> b) { return b += a; }	/**< @relates DoubleWordFloat */
template <typename T> inline DoubleWordFloat<T> operator-(T a, DoubleWordFloat<T> b) { return -b += a; }	/**< @relates DoubleWordFloat */
template <typename T> inline DoubleWordFloat<T> operator*(T a, DoubleWordFloat<T> b) { return b *= a; }	/**< @relates DoubleWordFloat */
template <typename T> inline DoubleWordFloat<T> operator/(T a, DoubleWordFloat<T> b) { return DoubleWordFloat<T>(a) /= b; }	/**< @relates DoubleWordFloat */

template <typename T> inline DoubleWordFloat<T> operator+(DoubleWordFloat<T> a, const DoubleWordFloat<T>& b) { return a += b; }	/**< @relates DoubleWordFloat */
template <typename T> inline DoubleWordFloat<T> operator-(DoubleWordFloat<T> a, const DoubleWordFloat<T>& b) { return a -= b; }	/**< @relates DoubleWordFloat */
template <typename T> inline DoubleWordFloat<T> operator*(DoubleWordFloat<T> a, const DoubleWordFloat<T>& b) { return a *= b; }	/**< @relates DoubleWordFloat */
template <typename T> inline DoubleWordFloat<T> operator/(DoubleWordFloat<T> a, const DoubleWordFloat<T>& b) { return a /= b; }	/**< @relates DoubleWordFloat */

/**	@relates DoubleWordFloat */
template <typename T>
DoubleWordFloat<T> abs(const DoubleWordFloat<T>& x)
{
	constexpr T zero = 0;
	return x < zero ? -x : x;
}

/**	sqrt(DW), SQRTDWtoDW, bound = (25/8)u² 
 * 
 *	@relates DoubleWordFloat
*/
template <typename T>
DoubleWordFloat<T> sqrt(const DoubleWordFloat<T>& x)
{
	const auto xh = x.high();
	if (xh == 0)
	{
		return DoubleWordFloat<T>();
	}
	const auto sh = num::sqrt(xh);
	const auto r1 = std::fma(-sh, sh, xh);
	const auto r2 = x.low() + r1;
	const auto sl = r2 / (2 * sh);
	return DoubleWordFloat<T>::sum2Fast(sh, sl);
}

/**	@relates DoubleWordFloat */
template <typename T>
bool isInf(const DoubleWordFloat<T>& x)
{
	LASS_ASSERT(!(!num::isInf(x.high()) && num::isInf(x.low()))); // low can never be inf if high is not.
	return num::isInf(x.high());
}

/**	@relates DoubleWordFloat */
template <typename T>
bool isNan(const DoubleWordFloat<T>& x)
{
	LASS_ASSERT(!(!num::isNan(x.high()) && num::isNan(x.low()))); // low can never be nan if high is not.
	return num::isNan(x.high());
}



using DoubleDouble = DoubleWordFloat<double>;

template <>
struct NumTraits<DoubleDouble>
{
private:
    using TDoubleTraits = NumTraits<double>;

    // how big should DoubleDouble epsilon be?
    // Let's have x = (DoubleDouble(1.) + TDoubleTraits::epislon) / 2, which cannot be represented by a double.
    // Then let ddEps_ be the smallest possible value so that (x + ddEps_) > x.
    static constexpr double ddEps_ = TDoubleTraits::epsilon * TDoubleTraits::epsilon / 2;
public:
	using selfType = DoubleDouble;
	using baseType = DoubleDouble;
	using intervalType = DoubleDouble;
	enum
	{
		isDistribution = false,
		isIntegral = false,
		isNative = false,
		isSigned = true,
		hasInfinity = true,
		hasNaN = true,
		isFloatingPoint = true
	};
	static constexpr size_t memorySize = sizeof(DoubleDouble);
	static constexpr size_t mantisseSize = 31;
	static const std::string name() { return "DoubleDouble" ; }
	static constexpr DoubleDouble zero { 0 };
	static constexpr DoubleDouble one { 1 };
	static constexpr DoubleDouble infinity { TDoubleTraits::infinity, TDoubleTraits::infinity };
	static constexpr DoubleDouble qNaN { TDoubleTraits::qNaN, TDoubleTraits::qNaN };
	static constexpr DoubleDouble sNaN { TDoubleTraits::sNaN, TDoubleTraits::sNaN };
	static constexpr DoubleDouble epsilon { ddEps_ };
	static constexpr DoubleDouble min { TDoubleTraits::min, TDoubleTraits::min * TDoubleTraits::epsilon / 4 };
	static constexpr DoubleDouble max { TDoubleTraits::max, TDoubleTraits::max * TDoubleTraits::epsilon / 4 };
	//static constexpr DoubleDouble minStrictPositive { TDoubleTraits::minStrictPositive };
	static constexpr DoubleDouble pi { 0x1.921fb54442d18p+1, 0x1.1a62633145c07p-53 };
	static constexpr DoubleDouble e { 0x1.5bf0a8b145769p+1, 0x1.4d57ee2b1013ap-53 };
	static constexpr DoubleDouble sqrt2 { 0x1.6a09e667f3bcdp+0, -0x1.bdd3413b26456p-54 };
	static constexpr DoubleDouble sqrtPi { 0x1.c5bf891b4ef6bp+0, -0x1.618f13eb7ca89p-54 };
	static constexpr DoubleDouble log2 { 0x1.62e42fefa39efp-1, 0x1.abc9e3b39803fp-56 };

	static constexpr DoubleDouble gamma(unsigned n)
	{ 
		// gamma(n) = n*u / (1 - n*u), where u = epsilon / 2
		//          = n*u * (1 + n*u) / (1 - (n*u)^2)
		//          ≈ n*u + (n*u)^2, because RN(1 - (n*u)^2) == 1
		const double nu = n * ddEps_ / 2;
		return DoubleDouble(nu, nu * nu);
	}
};

}
}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#else
#   pragma GCC push_options
#endif
