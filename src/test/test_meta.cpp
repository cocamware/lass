/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/
/** @file test_meta.cpp
 *  @brief implementation of lass::test::testMeta
 */

#include "test_common.h"
#include "test_meta.h"
#include "../meta/meta_assert.h"
#include "test_meta_type_list.inl"
#include "test_meta_type_traits.inl"
//#include "test_meta_various.inl"

namespace lass
{
namespace test
{

boost::unit_test_framework::test_suite* testMeta()
{
    boost::unit_test_framework::test_suite* result = BOOST_TEST_SUITE("lass::meta test suite");

	// test meta assert (if you're curious, try change the expression 1 + 1 == 2 into something
	// false like 1 + 1 == 3, and see what happens =) BdG]
	//
	LASS_META_ASSERT(1 + 1 == 2, it_s_the_end_of_the_world);

	result->add(BOOST_TEST_CASE(testMetaTypeList));
	result->add(BOOST_TEST_CASE(testMetaTypeTraits<int>));
	result->add(BOOST_TEST_CASE(testMetaTypeTraits<float>));
	result->add(BOOST_TEST_CASE(testMetaTypeTraits<std::string>));
//	result->add(BOOST_TEST_CASE(testMetaIsConvertibleType));

    return result;
}



}

}