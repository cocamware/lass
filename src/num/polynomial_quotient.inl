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
PolynomialQuotient<T>::PolynomialQuotient(TParam iScalar):
	numerator_(iScalar),
	denominator_(TNumTraits::one)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TCoefficients& iNominator):
	numerator_(iNominator),
	denominator_(TNumTraits::one)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TPolynomial& iNominator):
	numerator_(iNominator),
	denominator_(TNumTraits::one)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(TParam iScalar, const TCoefficients& iDenomirator):
	numerator_(iScalar),
	denominator_(iDenomirator)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(TParam iScalar, const TPolynomial& iDenomirator):
	numerator_(iScalar),
	denominator_(iDenomirator)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TCoefficients& iNominator, const TCoefficients& iDenominator):
	numerator_(iNominator),
	denominator_(iDenominator)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TPolynomial& iNominator, const TPolynomial& iDenomirator):
	numerator_(iNominator),
	denominator_(iDenomirator)
{
}


template <typename T>
template <typename InputIterator> 
PolynomialQuotient<T>::PolynomialQuotient(InputIterator iNomBegin, InputIterator iNomEnd):
	numerator_(iNomBegin, iNomEnd),
	denominator_(TNumTraits::one)
{
}


template <typename T>
template <typename InputIterator> 
PolynomialQuotient<T>::PolynomialQuotient(InputIterator iNomBegin, InputIterator iNomEnd,
		InputIterator iDenBegin, InputIterator iDenEnd):
	numerator_(iNomBegin, iNomEnd),
	denominator_(iDenBegin, iDenEnd)
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
PolynomialQuotient<T>::operator()(TParam iX) const
{
	return numerator_(iX) / denominator_(iX);
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
PolynomialQuotient<T>& PolynomialQuotient<T>::operator+=(const TSelf& iOther)
{
	numerator_ = numerator_ * iOther.denominator_ + iOther.numerator_ * iOther.denominator_;
	denominator_ *= iOther.denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator-=(const TSelf& iOther)
{
	numerator_ = numerator_ * iOther.denominator_ - iOther.numerator_ * iOther.denominator_;
	denominator_ *= iOther.denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator*=(const TSelf& iOther)
{
	numerator_ *= iOther.numerator_;
	denominator_ *= iOther.denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator/=(const TSelf& iOther)
{
	numerator_ *= iOther.denominator_;
	denominator_ *= iOther.numerator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator+=(const Polynomial<T>& iOther)
{
	numerator_ += iOther * denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator-=(const Polynomial<T>& iOther)
{
	numerator_ -= iOther * denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator*=(const Polynomial<T>& iOther)
{
	numerator_ *= iOther;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator/=(const Polynomial<T>& iOther)
{
	denominator_ *= iOther;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator+=(TParam iScalar)
{
	numerator_ += iScalar * denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator-=(TParam iScalar)
{
	numerator_ -= iScalar * denominator_;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator*=(TParam iScalar)
{
	numerator_ *= iScalar;
	return *this;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator/=(TParam iScalar)
{
	numerator_ /= iScalar;
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
PolynomialQuotient<T> PolynomialQuotient<T>::pow(unsigned iPower) const
{
	return PolynomialQuotient<T>(numerator_.pow(iPower), denominator_.pow(iPower)); 
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
bool operator==(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB)
{
	return iA.numerator() == iB.numerator() && iA.denominator() == iB.denominator();
}



template <typename T> inline 
bool operator!=(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB)
{
	return !(iA == iB);
}



template <typename T> inline
PolynomialQuotient<T> operator+(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result += iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator-(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result -= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result *= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result /= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator+(const PolynomialQuotient<T>& iA, const Polynomial<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result += iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator-(const PolynomialQuotient<T>& iA, const Polynomial<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result -= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& iA, const Polynomial<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result *= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const PolynomialQuotient<T>& iA, const Polynomial<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result /= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator+(const Polynomial<T>& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iB);
	result += iA;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator-(const Polynomial<T>& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result -= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator*(const Polynomial<T>& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iB);
	result *= iA;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const Polynomial<T>& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result /= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator+(const T& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iB);
	result += iA;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator-(const T& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(-iB);
	result += iA;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator*(const T& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iB);
	result *= iA;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const T& iA, const PolynomialQuotient<T>& iB)
{
	PolynomialQuotient<T> result(iA);
	result /= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator+(const PolynomialQuotient<T>& iA, const T& iB)
{
	PolynomialQuotient<T> result(iA);
	result += iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator-(const PolynomialQuotient<T>& iA, const T& iB)
{
	PolynomialQuotient<T> result(iA);
	result -= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& iA, const T& iB)
{
	PolynomialQuotient<T> result(iA);
	result *= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const PolynomialQuotient<T>& iA, const T& iB)
{
	PolynomialQuotient<T> result(iA);
	result /= iB;
	return result;
}



template <typename T> inline
PolynomialQuotient<T> operator/(const Polynomial<T>& iA, const Polynomial<T>& iB)
{
	return PolynomialQuotient<T>(iA, iB);
}



template <typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& iS, const PolynomialQuotient<T>& iA)
{
	iS << "(" << iA.numerator() << ")/(" << iA.denominator() << ")";
	return iS;
}



}

}

#endif

// EOF
