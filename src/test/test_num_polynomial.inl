/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_POLYNOMIAL_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_POLYNOMIAL_INL

#include "test_common.h"

#include "../num/polynomial.h"

namespace lass
{
namespace test
{

template <typename T>
void testNumPolynomial()
{
	typedef num::Polynomial<T> TPolynomial;

	TPolynomial zero;
	LASS_TEST_CHECK_LEXICAL(zero, "0");

	TPolynomial one(1);
	LASS_TEST_CHECK_LEXICAL(one, "1");

	T a_coeff[2] = { 1, 1 };
	TPolynomial a(a_coeff, a_coeff + 2);
	LASS_TEST_CHECK_LEXICAL(a, "1 + 1x^1");

	TPolynomial b = a * a;
	LASS_TEST_CHECK_LEXICAL(b, "1 + 2x^1 + 1x^2");

	TPolynomial c = b + a;
	LASS_TEST_CHECK_LEXICAL(c, "2 + 3x^1 + 1x^2");
}


}
}

#endif

// EOF
