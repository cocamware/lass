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

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_POLYNOMIAL_QUOTIENT_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_POLYNOMIAL_QUOTIENT_INL

#include "num_common.h"
#include "polynomial.h"

namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
PolynomialQuotient<T>::PolynomialQuotient():
	numerator_(),
	denominator_(TNumTraits::one)
{
}


template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(TParam scalar):
	numerator_(scalar),
	denominator_(TNumTraits::one)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TCoefficients& numerator):
	numerator_(numerator),
	denominator_(TNumTraits::one)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TPolynomial& numerator):
	numerator_(numerator),
	denominator_(TNumTraits::one)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TCoefficientsPair& numeratorDenominator):
	numerator_(numeratorDenominator.first),
	denominator_(numeratorDenominator.second)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(TParam scalar, const TCoefficients& denominator):
	numerator_(scalar),
	denominator_(denominator)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(TParam scalar, const TPolynomial& denominator):
	numerator_(scalar),
	denominator_(denominator)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TCoefficients& numerator, const TCoefficients& denominator):
	numerator_(numerator),
	denominator_(denominator)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TPolynomial& numerator, const TPolynomial& denominator):
	numerator_(numerator),
	denominator_(denominator)
{
}


template <typename T>
template <typename InputIterator> 
PolynomialQuotient<T>::PolynomialQuotient(InputIterator numFirst, InputIterator numLast):
	numerator_(numFirst, numLast),
	denominator_(TNumTraits::one)
{
}


template <typename T>
template <typename InputIterator> 
PolynomialQuotient<T>::PolynomialQuotient(
		InputIterator numFirst, InputIterator numLast, InputIterator denFirst, InputIterator denLast):
	numerator_(numFirst, numLast),
	denominator_(denFirst, denLast)
{
}



template <typename T> inline
const typename PolynomialQuotient<T>::TPolynomial& 
PolynomialQuotient<T>::numerator() const
{
	return numerator_;
}



template <typename T> inline
const typename PolynomialQuotient<T>::TPolynomial& 
PolynomialQuotient<T>::denominator() const
{
	return denominator_;
}



template <typename T>
const typename PolynomialQuotient<T>::TValue 
PolynomialQuotient<T>::operator()(TParam x) const
{
	return numerator_(x) / denominator_(x);
}



template <typename T>
const PolynomialQuotient<T>& PolynomialQuotient<T>::operator+() const
{
	return *this;
}



template <typename T>
const PolynomialQuotient<T> PolynomialQuotient<T>::operator-() const
{
	return TSelf(-numerator_, denominator_);
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator+=(const TSelf& other)
{
	numerator_ = numerator_ * other.denominator_ + other.numerator_ * other.denominator_;
	denominator_ *= other.denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator-=(const TSelf& other)
{
	numerator_ = numerator_ * other.denominator_ - other.numerator_ * other.denominator_;
	denominator_ *= other.denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator*=(const TSelf& other)
{
	numerator_ *= other.numerator_;
	denominator_ *= other.denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator/=(const TSelf& other)
{
	numerator_ *= other.denominator_;
	denominator_ *= other.numerator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator+=(const Polynomial<T>& other)
{
	numerator_ += other * denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator-=(const Polynomial<T>& other)
{
	numerator_ -= other * denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator*=(const Polynomial<T>& other)
{
	numerator_ *= other;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator/=(const Polynomial<T>& other)
{
	denominator_ *= other;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator+=(TParam scalar)
{
	numerator_ += scalar * denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator-=(TParam scalar)
{
	numerator_ -= scalar * denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator*=(TParam scalar)
{
	numerator_ *= scalar;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator/=(TParam scalar)
{
	numerator_ /= scalar;
	return *this;
}



template <typename T>
PolynomialQuotient<T> PolynomialQuotient<T>::derivative() const
{
	const TPolynomial num = denominator_ * numerator_.derivative() - denominator_.derivative() * numerator_;
	const TPolynomial den = denominator_ * denominator_;
	return TSelf(num, den);
}



template <typename T>
PolynomialQuotient<T> PolynomialQuotient<T>::pow(unsigned power) const
{
	return PolynomialQuotient<T>(numerator_.pow(power), denominator_.pow(power)); 
}



template <typename T>
PolynomialQuotient<T> PolynomialQuotient<T>::one()
{
	static PolynomialQuotient result(1);
	return result;
}



template <typename T>
PolynomialQuotient<T> PolynomialQuotient<T>::x()
{
	static TValue coefficients[2] = { 0, 1 };
	static PolynomialQuotient result(coefficients, coefficients + 2);
	return result;
}



// --- free ----------------------------------------------------------------------------------------

template <typename T> 
bool operator==(const PolynomialQuotient<T>& a, const PolynomialQuotient<T>& b)
{
	return a.numerator() == b.numerator() && a.denominator() == b.denominator();
}



template <typename T> inline 
bool operator!=(const PolynomialQuotient<T>& a, const PolynomialQuotient<T>& b)
{
	return !(a == b);
}



template <typename T> inline
PolynomialQuotient<T> operator+(const PolynomialQuotient<T>& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(a);
	result += b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator-(const PolynomialQuotient<T>& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(a);
	result -= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(a);
	result *= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const PolynomialQuotient<T>& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(a);
	result /= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator+(const PolynomialQuotient<T>& a, const Polynomial<T>& b)
{
	PolynomialQuotient<T> result(a);
	result += b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator-(const PolynomialQuotient<T>& a, const Polynomial<T>& b)
{
	PolynomialQuotient<T> result(a);
	result -= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& a, const Polynomial<T>& b)
{
	PolynomialQuotient<T> result(a);
	result *= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const PolynomialQuotient<T>& a, const Polynomial<T>& b)
{
	PolynomialQuotient<T> result(a);
	result /= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator+(const Polynomial<T>& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(b);
	result += a;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator-(const Polynomial<T>& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(a);
	result -= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator*(const Polynomial<T>& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(b);
	result *= a;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const Polynomial<T>& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(a);
	result /= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator+(const T& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(b);
	result += a;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator-(const T& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(-b);
	result += a;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator*(const T& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(b);
	result *= a;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const T& a, const PolynomialQuotient<T>& b)
{
	PolynomialQuotient<T> result(a);
	result /= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator+(const PolynomialQuotient<T>& a, const T& b)
{
	PolynomialQuotient<T> result(a);
	result += b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator-(const PolynomialQuotient<T>& a, const T& b)
{
	PolynomialQuotient<T> result(a);
	result -= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& a, const T& b)
{
	PolynomialQuotient<T> result(a);
	result *= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const PolynomialQuotient<T>& a, const T& b)
{
	PolynomialQuotient<T> result(a);
	result /= b;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const Polynomial<T>& a, const Polynomial<T>& b)
{
	return PolynomialQuotient<T>(a, b);
}



template <typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& s, const PolynomialQuotient<T>& a)
{
	s << "(" << a.numerator() << ")/(" << a.denominator() << ")";
	return s;
}



}

}

#endif

// EOF
