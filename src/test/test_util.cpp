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
#include "test_util_small_object.inl"
#include "test_util_string_cast.inl"
#include "test_util_thread_fun.inl"
#include "test_util_visitor.inl"

namespace lass
{
namespace test
{

TUnitTests testUtil()
{
	TUnitTests result;

	result.push_back(LASS_UNIT_TEST(testUtilCallback));

	result.push_back(LASS_UNIT_TEST(testUtilClock));

	result.push_back(LASS_UNIT_TEST(testUtilCloneFactory));

	result.push_back(LASS_UNIT_TEST(testUtilDictionary));

	result.push_back(LASS_UNIT_TEST(testUtilException));

	result.push_back(LASS_UNIT_TEST(testUtilFixedArray<int>));
	result.push_back(LASS_UNIT_TEST(testUtilFixedArray<float>));

	result.push_back(LASS_UNIT_TEST(testUtilIdGenerator<int>));
	result.push_back(LASS_UNIT_TEST(testUtilIdGenerator<char>));
	result.push_back(LASS_UNIT_TEST(testUtilIdGenerator<unsigned long>));

	result.push_back(LASS_UNIT_TEST(testUtilSmallObject));

	result.push_back(LASS_UNIT_TEST(testUtilStringCast));

	result.push_back(LASS_UNIT_TEST(testUtilThreadFun));

	result.push_back(LASS_UNIT_TEST(testUtilVisitor));

	result.push_back(LASS_UNIT_TEST(testUtilPython));

	return result;
}



}

}
