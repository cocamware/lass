/**	@file
 *  @internal
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_ID_GENERATOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_ID_GENERATOR_INL

#include "test_common.h"

#include "../util/id_generator.h"

namespace lass
{
namespace test
{

template <typename T>
void testUtilIdGenerator()
{
	using namespace util;

	IdGenerator<T> generator;
	BOOST_CHECK_EQUAL(generator(), 0);
	BOOST_CHECK_EQUAL(generator(), 1);
	BOOST_CHECK_EQUAL(generator(), 2);
	BOOST_CHECK_EQUAL(generator(), 3);

	generator.setNext(7);
	BOOST_CHECK_EQUAL(generator(), 7);
	BOOST_CHECK_EQUAL(generator(), 8);
	BOOST_CHECK_EQUAL(generator(), 9);
	BOOST_CHECK_EQUAL(generator(), 10);

	generator.setNext(num::NumTraits<T>::max - 1);
	BOOST_CHECK_EQUAL(generator(), num::NumTraits<T>::max - 1);
	BOOST_CHECK_THROW(generator(), std::exception);

	IdGenerator<T> generator2(10);
	BOOST_CHECK_EQUAL(generator2(), 10);
	BOOST_CHECK_EQUAL(generator2(), 11);
	BOOST_CHECK_EQUAL(generator2(), 12);
	BOOST_CHECK_EQUAL(generator2(), 13);
}

}

}

#endif

// EOF
