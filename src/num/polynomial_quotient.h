/** @file
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



/** @class lass::num::Polynomial
 *  @brief an univariate polynomial.
 *  @author Bram de Greve [BdG]
 *
 *  <i>A polynomial is a mathematical expression involving a sum of powers in one or more variables
 *  multiplied by coefficients.</i>,
 *  Eric W. Weisstein. "Polynomial." From MathWorld--A Wolfram Web Resource. 
 *  http://mathworld.wolfram.com/Polynomial.html
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_POLYNOMIAL_QUOTIENT_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_POLYNOMIAL_QUOTIENT_H

#include "num_common.h"
#include "polynomial.h"

namespace lass
{
namespace num
{

template <typename T>
class PolynomialQuotient
{
public:

	typedef PolynomialQuotient<T> TSelf;
	typedef Polynomial<T> TPolynomial;
	typedef NumTraits<T> TNumTraits;
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef std::vector<TValue> TCoefficients;

	PolynomialQuotient();
	explicit PolynomialQuotient(TParam iScalar);
	explicit PolynomialQuotient(const TCoefficients& iNominator);
	explicit PolynomialQuotient(const TPolynomial& iNominator);
	PolynomialQuotient(TParam iScalar, const TCoefficients& iDenominator);
	PolynomialQuotient(TParam iScalar, const TPolynomial& iDenominator);
	PolynomialQuotient(const TCoefficients& iNominator, const TCoefficients& iDenominator);
	PolynomialQuotient(const TPolynomial& iNominator, const TPolynomial& iDenominator);
	template <typename InputIterator> PolynomialQuotient(InputIterator iNomBegin, InputIterator iNomEnd);
	template <typename InputIterator> PolynomialQuotient(InputIterator iNomBegin, InputIterator iNomEnd,
		InputIterator iDenBegin, InputIterator iDenEnd);

	const TPolynomial& numerator() const;
	const TPolynomial& denominator() const;

	const TValue operator()(TParam iX) const;

	const TSelf& operator+() const;
	const TSelf operator-() const;

	TSelf& operator+=(const TSelf& iOther);
	TSelf& operator-=(const TSelf& iOther);
	TSelf& operator*=(const TSelf& iOther);
	TSelf& operator/=(const TSelf& iOther);

	TSelf& operator+=(const TPolynomial& iOther);
	TSelf& operator-=(const TPolynomial& iOther);
	TSelf& operator*=(const TPolynomial& iOther);
	TSelf& operator/=(const TPolynomial& iOther);

	TSelf& operator+=(TParam iScalar);
	TSelf& operator-=(TParam iScalar);
	TSelf& operator*=(TParam iScalar);
	TSelf& operator/=(TParam iScalar);

	TSelf derivative() const;
	TSelf pow(unsigned iPower) const;

	static TSelf x();

private:

	TPolynomial numerator_;
	TPolynomial denominator_;
};

template <typename T> bool operator==(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB);
template <typename T> bool operator!=(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB);

template <typename T> PolynomialQuotient<T> operator+(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB);
template <typename T> PolynomialQuotient<T> operator-(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB);
template <typename T> PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB);
template <typename T> PolynomialQuotient<T> operator/(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB);

template <typename T> PolynomialQuotient<T> operator+(const Polynomial<T>& iA, const PolynomialQuotient<T>& iB);
template <typename T> PolynomialQuotient<T> operator-(const Polynomial<T>& iA, const PolynomialQuotient<T>& iB);
template <typename T> PolynomialQuotient<T> operator*(const Polynomial<T>& iA, const PolynomialQuotient<T>& iB);
template <typename T> PolynomialQuotient<T> operator*(const Polynomial<T>& iA, const PolynomialQuotient<T>& iB);

template <typename T> PolynomialQuotient<T> operator+(const PolynomialQuotient<T>& iA, const Polynomial<T>& iB);
template <typename T> PolynomialQuotient<T> operator-(const PolynomialQuotient<T>& iA, const Polynomial<T>& iB);
template <typename T> PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& iA, const Polynomial<T>& iB);
template <typename T> PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& iA, const Polynomial<T>& iB);

template <typename T> PolynomialQuotient<T> operator+(const T& iA, const PolynomialQuotient<T>& iB);
template <typename T> PolynomialQuotient<T> operator-(const T& iA, const PolynomialQuotient<T>& iB);
template <typename T> PolynomialQuotient<T> operator*(const T& iA, const PolynomialQuotient<T>& iB);
template <typename T> PolynomialQuotient<T> operator/(const T& iA, const PolynomialQuotient<T>& iB);

template <typename T> PolynomialQuotient<T> operator+(const PolynomialQuotient<T>& iA, const T& iB);
template <typename T> PolynomialQuotient<T> operator-(const PolynomialQuotient<T>& iA, const T& iB);
template <typename T> PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& iA, const T& iB);
template <typename T> PolynomialQuotient<T> operator/(const PolynomialQuotient<T>& iA, const T& iB);

template <typename T> PolynomialQuotient<T> operator/(const Polynomial<T>& iA, const Polynomial<T>& iB);

template <typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& iS, const PolynomialQuotient<T>& iA);


}
}

#include "polynomial_quotient.inl"

#endif

// EOF
