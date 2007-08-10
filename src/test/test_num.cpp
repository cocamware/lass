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




#include "test_common.h"
#include "test_num.h"

#include "test_num_basic_ops.inl"
#include "test_num_db.inl"
#include "test_num_spline.inl"
#include "test_num_matrix_vector.inl"
#include "test_num_num_traits.inl"
#include "test_num_polynomial.inl"
#include "test_num_filters.inl"
#include "test_num_random.inl"
#include "test_num_tri_bool.inl"
#include "test_num_filtered_float.inl"

namespace lass
{
namespace test
{

TUnitTests testNum()
{
	TUnitTests result;

	result.push_back(LASS_UNIT_TEST(testNumBasicOps<float>));
	result.push_back(LASS_UNIT_TEST(testNumBasicOps<double>));
	result.push_back(LASS_UNIT_TEST(testNumBasicOps<long double>));

	result.push_back(LASS_UNIT_TEST(testNumDb<float>));
	result.push_back(LASS_UNIT_TEST(testNumDb<double>));
	result.push_back(LASS_UNIT_TEST(testNumDb<long double>));

	result.push_back(LASS_UNIT_TEST(testNumSpline));

	result.push_back(LASS_UNIT_TEST(testNumNumTraits));

	result.push_back(LASS_UNIT_TEST(testNumMatrix<double>));
	result.push_back(LASS_UNIT_TEST(testNumMatrix< std::complex<double> >));
	result.push_back(LASS_UNIT_TEST(testNumVector<double>));
	result.push_back(LASS_UNIT_TEST(testNumVector< std::complex<double> >));
	result.push_back(LASS_UNIT_TEST(testNumSolve<double>));
	result.push_back(LASS_UNIT_TEST(testNumSolve< std::complex<double> >));

	result.push_back(LASS_UNIT_TEST(testNumPolynomial<double>));
	result.push_back(LASS_UNIT_TEST(testNumFilters<double>));

	result.push_back(LASS_UNIT_TEST(testNumRandomMersenne));
	result.push_back(LASS_UNIT_TEST(testNumDistributions));

	result.push_back(LASS_UNIT_TEST(testNumTriBool));

	result.push_back(LASS_UNIT_TEST(testNumFilteredFloat));

	return result;
}




}

}
