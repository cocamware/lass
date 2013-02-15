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

#include "pylass_common.h"
#include "polynomial.h"

namespace pylass
{

PY_DECLARE_CLASS(Polynomial)
PY_CLASS_CONSTRUCTOR_0(Polynomial)
PY_CLASS_CONSTRUCTOR_1(Polynomial, TPolynomial::TParam)
PY_CLASS_CONSTRUCTOR_1(Polynomial, const TPolynomial::TCoefficients&)
PY_CLASS_MEMBER_R(Polynomial, coefficients)
PY_CLASS_METHOD(Polynomial, at)
PY_CLASS_METHOD_NAME(Polynomial, operator(), "__call__")
PY_CLASS_METHOD_NAME(Polynomial, operator+, "__pos__")
PY_CLASS_METHOD_NAME(Polynomial, operator-, "__neg__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(Polynomial, operator+=, TPolynomial&, const TPolynomial&, "__iadd__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(Polynomial, operator-=, TPolynomial&, const TPolynomial&, "__isub__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(Polynomial, operator*=, TPolynomial&, const TPolynomial&, "__imul__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(Polynomial, operator+=, TPolynomial&, TPolynomial::TParam, "__iadd__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(Polynomial, operator-=, TPolynomial&, TPolynomial::TParam, "__isub__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(Polynomial, operator*=, TPolynomial&, TPolynomial::TParam, "__imul__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(Polynomial, operator/=, TPolynomial&, TPolynomial::TParam, "__idiv__")
PY_CLASS_METHOD(Polynomial, derivative)
PY_CLASS_METHOD(Polynomial, integral)
PY_CLASS_METHOD(Polynomial, pow)
PY_CLASS_STATIC_METHOD(Polynomial, x)
PY_CLASS_FREE_METHOD_QUALIFIED_NAME_2(Polynomial, ::lass::num::operator==, bool, const TPolynomial&, const TPolynomial&, "__eq__")
PY_CLASS_FREE_METHOD_QUALIFIED_NAME_2(Polynomial, ::lass::num::operator!=, bool, const TPolynomial&, const TPolynomial&, "__ne__")

std::pair<TPolynomialQuotient::TCoefficients, TPolynomialQuotient::TCoefficients> 
quotientCoeffients(const TPolynomialQuotient& quotient)
{
	return std::make_pair(quotient.numerator().coefficients(), quotient.denominator().coefficients());
}

PY_DECLARE_CLASS(PolynomialQuotient)
PY_CLASS_CONSTRUCTOR_0(PolynomialQuotient)
PY_CLASS_CONSTRUCTOR_1(PolynomialQuotient, TPolynomialQuotient::TParam)
PY_CLASS_CONSTRUCTOR_1(PolynomialQuotient, const TPolynomialQuotient::TCoefficients&)
PY_CLASS_CONSTRUCTOR_1(PolynomialQuotient, const TPolynomialQuotient::TPolynomial&)
PY_CLASS_CONSTRUCTOR_2(PolynomialQuotient, TPolynomialQuotient::TParam, const TPolynomialQuotient::TCoefficients&)
PY_CLASS_CONSTRUCTOR_2(PolynomialQuotient, TPolynomialQuotient::TParam, const TPolynomialQuotient::TPolynomial&)
PY_CLASS_CONSTRUCTOR_2(PolynomialQuotient, const TPolynomialQuotient::TCoefficients&, const TPolynomialQuotient::TCoefficients&)
PY_CLASS_CONSTRUCTOR_2(PolynomialQuotient, const TPolynomialQuotient::TPolynomial&, const TPolynomialQuotient::TPolynomial&)
PY_CLASS_MEMBER_R(PolynomialQuotient, numerator)
PY_CLASS_MEMBER_R(PolynomialQuotient, denominator)
PY_CLASS_FREE_METHOD_NAME(PolynomialQuotient, quotientCoeffients, "coefficients")
PY_CLASS_METHOD_NAME(PolynomialQuotient, operator(), "__call__")
PY_CLASS_METHOD_NAME(PolynomialQuotient, operator+, "__pos__")
PY_CLASS_METHOD_NAME(PolynomialQuotient, operator-, "__neg__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator+=, TPolynomialQuotient&, const TPolynomialQuotient&, "__iadd__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator-=, TPolynomialQuotient&, const TPolynomialQuotient&, "__isub__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator*=, TPolynomialQuotient&, const TPolynomialQuotient&, "__imul__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator/=, TPolynomialQuotient&, const TPolynomialQuotient&, "__idiv__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator+=, TPolynomialQuotient&, const TPolynomialQuotient::TPolynomial&, "__iadd__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator-=, TPolynomialQuotient&, const TPolynomialQuotient::TPolynomial&, "__isub__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator*=, TPolynomialQuotient&, const TPolynomialQuotient::TPolynomial&, "__imul__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator/=, TPolynomialQuotient&, const TPolynomialQuotient::TPolynomial&, "__idiv__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator+=, TPolynomialQuotient&, TPolynomialQuotient::TParam, "__iadd__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator-=, TPolynomialQuotient&, TPolynomialQuotient::TParam, "__isub__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator*=, TPolynomialQuotient&, TPolynomialQuotient::TParam, "__imul__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(PolynomialQuotient, operator/=, TPolynomialQuotient&, TPolynomialQuotient::TParam, "__idiv__")
PY_CLASS_METHOD(PolynomialQuotient, derivative)
PY_CLASS_METHOD(PolynomialQuotient, pow)
PY_CLASS_STATIC_METHOD(PolynomialQuotient, x)



PY_DECLARE_CLASS(ComplexPolynomial)
PY_CLASS_CONSTRUCTOR_0(ComplexPolynomial)
PY_CLASS_CONSTRUCTOR_1(ComplexPolynomial, TComplexPolynomial::TParam)
PY_CLASS_CONSTRUCTOR_1(ComplexPolynomial, const TComplexPolynomial::TCoefficients&)
PY_CLASS_MEMBER_R(ComplexPolynomial, coefficients)
PY_CLASS_METHOD(ComplexPolynomial, at)
PY_CLASS_METHOD_NAME(ComplexPolynomial, operator(), "__call__")
PY_CLASS_METHOD_NAME(ComplexPolynomial, operator+, "__pos__")
PY_CLASS_METHOD_NAME(ComplexPolynomial, operator-, "__neg__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomial, operator+=, TComplexPolynomial&, const TComplexPolynomial&, "__iadd__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomial, operator-=, TComplexPolynomial&, const TComplexPolynomial&, "__isub__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomial, operator*=, TComplexPolynomial&, const TComplexPolynomial&, "__imul__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomial, operator+=, TComplexPolynomial&, TComplexPolynomial::TParam, "__iadd__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomial, operator-=, TComplexPolynomial&, TComplexPolynomial::TParam, "__isub__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomial, operator*=, TComplexPolynomial&, TComplexPolynomial::TParam, "__imul__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomial, operator/=, TComplexPolynomial&, TComplexPolynomial::TParam, "__idiv__")
PY_CLASS_METHOD(ComplexPolynomial, derivative)
PY_CLASS_METHOD(ComplexPolynomial, integral)
PY_CLASS_METHOD(ComplexPolynomial, pow)
PY_CLASS_STATIC_METHOD(ComplexPolynomial, x)
PY_CLASS_FREE_METHOD_QUALIFIED_NAME_2(ComplexPolynomial, ::lass::num::operator==, bool, const TComplexPolynomial&, const TComplexPolynomial&, "__eq__")
PY_CLASS_FREE_METHOD_QUALIFIED_NAME_2(ComplexPolynomial, ::lass::num::operator!=, bool, const TComplexPolynomial&, const TComplexPolynomial&, "__ne__")




std::pair<TComplexPolynomialQuotient::TCoefficients, TComplexPolynomialQuotient::TCoefficients> 
complexQuotientCoeffients(const TComplexPolynomialQuotient& quotient)
{
	return std::make_pair(quotient.numerator().coefficients(), quotient.denominator().coefficients());
}

PY_DECLARE_CLASS(ComplexPolynomialQuotient)
PY_CLASS_CONSTRUCTOR_0(ComplexPolynomialQuotient)
PY_CLASS_CONSTRUCTOR_1(ComplexPolynomialQuotient, TComplexPolynomialQuotient::TParam)
PY_CLASS_CONSTRUCTOR_1(ComplexPolynomialQuotient, const TComplexPolynomialQuotient::TCoefficients&)
PY_CLASS_CONSTRUCTOR_1(ComplexPolynomialQuotient, const TComplexPolynomialQuotient::TPolynomial&)
PY_CLASS_CONSTRUCTOR_2(ComplexPolynomialQuotient, TComplexPolynomialQuotient::TParam, const TComplexPolynomialQuotient::TCoefficients&)
PY_CLASS_CONSTRUCTOR_2(ComplexPolynomialQuotient, TComplexPolynomialQuotient::TParam, const TComplexPolynomialQuotient::TPolynomial&)
PY_CLASS_CONSTRUCTOR_2(ComplexPolynomialQuotient, const TComplexPolynomialQuotient::TCoefficients&, const TComplexPolynomialQuotient::TCoefficients&)
PY_CLASS_CONSTRUCTOR_2(ComplexPolynomialQuotient, const TComplexPolynomialQuotient::TPolynomial&, const TComplexPolynomialQuotient::TPolynomial&)
PY_CLASS_MEMBER_R(ComplexPolynomialQuotient, numerator)
PY_CLASS_MEMBER_R(ComplexPolynomialQuotient, denominator)
PY_CLASS_FREE_METHOD_NAME(ComplexPolynomialQuotient, complexQuotientCoeffients, "coefficients")
PY_CLASS_METHOD_NAME(ComplexPolynomialQuotient, operator(), "__call__")
PY_CLASS_METHOD_NAME(ComplexPolynomialQuotient, operator+, "__pos__")
PY_CLASS_METHOD_NAME(ComplexPolynomialQuotient, operator-, "__neg__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator+=, TComplexPolynomialQuotient&, const TComplexPolynomialQuotient&, "__iadd__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator-=, TComplexPolynomialQuotient&, const TComplexPolynomialQuotient&, "__isub__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator*=, TComplexPolynomialQuotient&, const TComplexPolynomialQuotient&, "__imul__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator/=, TComplexPolynomialQuotient&, const TComplexPolynomialQuotient&, "__idiv__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator+=, TComplexPolynomialQuotient&, const TComplexPolynomialQuotient::TPolynomial&, "__iadd__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator-=, TComplexPolynomialQuotient&, const TComplexPolynomialQuotient::TPolynomial&, "__isub__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator*=, TComplexPolynomialQuotient&, const TComplexPolynomialQuotient::TPolynomial&, "__imul__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator/=, TComplexPolynomialQuotient&, const TComplexPolynomialQuotient::TPolynomial&, "__idiv__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator+=, TComplexPolynomialQuotient&, TComplexPolynomialQuotient::TParam, "__iadd__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator-=, TComplexPolynomialQuotient&, TComplexPolynomialQuotient::TParam, "__isub__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator*=, TComplexPolynomialQuotient&, TComplexPolynomialQuotient::TParam, "__imul__")
PY_CLASS_METHOD_QUALIFIED_NAME_1(ComplexPolynomialQuotient, operator/=, TComplexPolynomialQuotient&, TComplexPolynomialQuotient::TParam, "__idiv__")
PY_CLASS_METHOD(ComplexPolynomialQuotient, derivative)
PY_CLASS_METHOD(ComplexPolynomialQuotient, pow)
PY_CLASS_STATIC_METHOD(ComplexPolynomialQuotient, x)


}
