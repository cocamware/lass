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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_DB_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_DB_INL

#include "test_common.h"

#include "../num/db.h"

namespace lass
{
namespace test
{

template <typename T>
void testNumDb()
{
	T epsilon = T(1e-5);

	BOOST_CHECK_CLOSE(num::p2dB(T(20.e-8)), T(-40), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::p2dB(T(20.e-7)), T(-20), 100 * epsilon);
	BOOST_CHECK(num::abs(num::p2dB(T(20.e-6))) < epsilon);
	BOOST_CHECK_CLOSE(num::p2dB(T(20.e-5)), T(20), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::p2dB(T(20.e-4)), T(40), 100 * epsilon);
	BOOST_CHECK_EQUAL(num::p2dB(T(0)), -num::NumTraits<T>::infinity);
	BOOST_CHECK_THROW(num::p2dB(T(-1)), util::Exception);
	LASS_COUT << "v----\n";
	BOOST_CHECK_NO_THROW(num::p2dB(num::NumTraits<T>::qNaN));
	LASS_COUT << "^----\n";

	BOOST_CHECK_CLOSE(num::I2dB(T(1.e-14)), T(-20), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::I2dB(T(1.e-13)), T(-10), 100 * epsilon);
	BOOST_CHECK(num::abs(num::I2dB(T(1.e-12))) < epsilon);
	BOOST_CHECK_CLOSE(num::I2dB(T(1.e-11)), T(10), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::I2dB(T(1.e-10)), T(20), 100 * epsilon);
	BOOST_CHECK_EQUAL(num::I2dB(T(0)), -num::NumTraits<T>::infinity);
	BOOST_CHECK_THROW(num::I2dB(T(-1)), util::Exception);
	LASS_COUT << "v----\n";
	BOOST_CHECK_NO_THROW(num::I2dB(num::NumTraits<T>::qNaN));
	LASS_COUT << "^----\n";

	BOOST_CHECK_CLOSE(num::W2dB(T(1.e-14)), T(-20), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::W2dB(T(1.e-13)), T(-10), 100 * epsilon);
	BOOST_CHECK(num::abs(num::W2dB(T(1.e-12))) < epsilon);
	BOOST_CHECK_CLOSE(num::W2dB(T(1.e-11)), T(10), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::W2dB(T(1.e-10)), T(20), 100 * epsilon);
	BOOST_CHECK_EQUAL(num::W2dB(T(0)), -num::NumTraits<T>::infinity);
	BOOST_CHECK_THROW(num::W2dB(T(-1)), util::Exception);
	LASS_COUT << "v----\n";
	BOOST_CHECK_NO_THROW(num::W2dB(num::NumTraits<T>::qNaN));
	LASS_COUT << "^----\n";

	BOOST_CHECK_CLOSE(num::dB2p(T(-40)), T(20.e-8), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2p(T(-20)), T(20.e-7), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2p(T(0)), T(20.e-6), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2p(T(20)), T(20.e-5), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2p(T(40)), T(20.e-4), 100 * epsilon);
	BOOST_CHECK_EQUAL(num::dB2p(-num::NumTraits<T>::infinity), T(0));
	BOOST_CHECK_NO_THROW(num::dB2p(num::NumTraits<T>::qNaN));

	BOOST_CHECK_CLOSE(num::dB2I(T(-20)), T(1.e-14), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2I(T(-10)), T(1.e-13), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2I(T(0)), T(1.e-12), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2I(T(10)), T(1.e-11), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2I(T(20)), T(1.e-10), 100 * epsilon);
	BOOST_CHECK_EQUAL(num::dB2I(-num::NumTraits<T>::infinity), T(0));
	BOOST_CHECK_NO_THROW(num::dB2I(num::NumTraits<T>::qNaN));

	BOOST_CHECK_CLOSE(num::dB2W(T(-20)), T(1.e-14), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2W(T(-10)), T(1.e-13), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2W(T(0)), T(1.e-12), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2W(T(10)), T(1.e-11), 100 * epsilon);
	BOOST_CHECK_CLOSE(num::dB2W(T(20)), T(1.e-10), 100 * epsilon);
	BOOST_CHECK_EQUAL(num::dB2W(-num::NumTraits<T>::infinity), T(0));
	BOOST_CHECK_NO_THROW(num::dB2W(num::NumTraits<T>::qNaN));

}



}

}

#endif

// EOF
