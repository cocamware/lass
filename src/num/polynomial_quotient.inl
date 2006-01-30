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
PolynomialQuotient<T>::PolynomialQuotient(TParam iScalar, const TCoefficients& iDenumerator):
	numerator_(iScalar),
	denumerator_(iDenumerator)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(TParam iScalar, const TPolynomial& iDenumerator):
	numerator_(iScalar),
	denumerator_(iDenumerator)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TCoefficients& iNominator, const TCoefficients& iDenumerator):
	numerator_(iNominator),
	denumerator_(iDenumerator)
{
}



template <typename T>
PolynomialQuotient<T>::PolynomialQuotient(const TPolynomial& iNominator, const TPolynomial& iDenumerator):
	numerator_(iNominator),
	denumerator_(iDenumerator)
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
	numerator_(iNomBegin, iNomEnd)
	denumerator_(iDenBegin, iDenEnd)
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
PolynomialQuotient<T>::denumerator() const
{
	return denumerator_;
}



template <typename T>
const typename PolynomialQuotient<T>::TValue 
PolynomialQuotient<T>::operator()(TParam iX) const
{
	return numerator_(iX) / denumerator_(iX);
}



template <typename T>
const PolynomialQuotient<T>& PolynomialQuotient<T>::operator+() const
{
	return *this;
}



template <typename T>
const PolynomialQuotient<T> PolynomialQuotient<T>::operator-() const
{
	return TSelf(-numerator_, denumerator_);
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator+=(const TSelf& iOther)
{
	numerator_ = numerator_ * iOther.denumerator_ + iOther.numerator_ * iOther.denumerator_
	denumerator_ *= iOther.denumerator_;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator-=(const TSelf& iOther)
{
	numerator_ = numerator_ * iOther.denumerator_ - iOther.numerator_ * iOther.denumerator_
	denumerator_ *= iOther.denumerator_;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator*=(const TSelf& iOther)
{
	numerator_ *= iOther.numerator_;
	denumerator_ *= iOther.denumerator_;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator/=(const TSelf& iOther)
{
	numerator_ *= iOther.denumerator_;
	denumerator_ *= iOther.numerator_;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator+=(const Polynomial<T>& iOther)
{
	numerator_ += iOther * denumerator_;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator-=(const Polynomial<T>& iOther)
{
	numerator_ -= iOther * denumerator_;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator*=(const Polynomial<T>& iOther)
{
	numerator_ *= iOther;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator/=(const Polynomial<T>& iOther)
{
	denumerator_ *= iOther;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator+=(TParam iScalar)
{
	numerator_ += iScalar * denumerator_;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator-=(TParam iScalar)
{
	numerator_ -= iScalar * denumerator_;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator*=(TParam iScalar)
{
	numerator_ *= iScalar;
}



template <typename T>
PolynomialQuotient<T>& PolynomialQuotient<T>::operator/=(TParam iScalar)
{
	numerator_ /= iScalar;
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
	return PolynomialQuotient<T>(numerator_.pow(iPower), denumerator_.pow(iPower)); 
}



template <typename T>
PolynomialQuotient<T> PolynomialQuotient<T>::x() const
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
PolynomialQuotient<T> operator*(const PolynomialQuotient<T>& iA, const PolynomialQuotient<T>& iB)
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
