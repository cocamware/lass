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



#include "test_common.h"
#include "test_util.h"

#include "test_util_callback.inl"
#include "test_util_clock.inl"
#include "test_util_clone_factory.inl"
#include "test_util_dictionary.inl"
#include "test_util_exception.inl"
#include "test_util_fixed_array.inl"
#include "test_util_id_generator.inl"
#include "test_util_python.inl"
#include "test_util_string_cast.inl"
#include "test_util_thread_fun.inl"
#include "test_util_visitor.inl"

namespace lass
{
namespace test
{

boost::unit_test_framework::test_suite* testUtil()
{
	boost::unit_test_framework::test_suite* result = BOOST_TEST_SUITE("lass::util test suite");

	result->add(BOOST_TEST_CASE(testUtilCallback));

	result->add(BOOST_TEST_CASE(testUtilClock));

	result->add(BOOST_TEST_CASE(testUtilCloneFactory));

	result->add(BOOST_TEST_CASE(testUtilDictionary));

	result->add(BOOST_TEST_CASE(testUtilException));

	result->add(BOOST_TEST_CASE(testUtilFixedArray<int>));
	result->add(BOOST_TEST_CASE(testUtilFixedArray<float>));

	result->add(BOOST_TEST_CASE(testUtilIdGenerator<int>));
	result->add(BOOST_TEST_CASE(testUtilIdGenerator<char>));
	result->add(BOOST_TEST_CASE(testUtilIdGenerator<unsigned long>));

	result->add(BOOST_TEST_CASE(testUtilStringCast));

	result->add(BOOST_TEST_CASE(testUtilThreadFun));

	result->add(BOOST_TEST_CASE(testUtilVisitor));

	result->add(BOOST_TEST_CASE(testUtilPython));

	return result;
}



}

}
