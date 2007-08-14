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

	static TSelf one();
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
