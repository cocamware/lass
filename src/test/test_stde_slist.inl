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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_SLIST_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_SLIST_INL

#include "test_common.h"
#include "../stde/slist.h"
#include "../util/string_cast.h"

namespace lass
{
namespace test
{

void testStdeSlist()
{
    boost::test_toolbox::output_test_stream stream;

    typedef stde::slist<int> slist_type;

    slist_type empty_list;
    BOOST_CHECK(empty_list.empty());
    BOOST_CHECK_EQUAL(empty_list.size(), 0);
    BOOST_CHECK(empty_list.begin() == empty_list.end());

    slist_type some_list(empty_list);
    BOOST_CHECK_EQUAL(some_list, empty_list);
    BOOST_CHECK(some_list.empty());

    some_list.push_front(0);
    BOOST_CHECK(!some_list.empty());
    BOOST_CHECK_EQUAL(some_list.size(), 1);
    BOOST_CHECK(some_list.begin() != empty_list.end());

    some_list.push_front(1);
    some_list.insert_after(some_list.begin(), 2);
    BOOST_CHECK_EQUAL(some_list.size(), 3);
    stream << some_list;
    const std::string some_pattern = "[1, 2, 0]";
    BOOST_CHECK(stream.is_equal(some_pattern));

    slist_type other_list(some_list);
    BOOST_CHECK_EQUAL(other_list, some_list);

    slist_type::iterator back = other_list.prior(other_list.end());
    back = other_list.insert_after(back, 4);
    back = other_list.insert_after(back, 5);
    back = other_list.insert_after(back, 6);
    stream << other_list;
    const std::string other_pattern = "[1, 2, 0, 4, 5, 6]";
    BOOST_CHECK(stream.is_equal(other_pattern));

    slist_type read_list;
    BOOST_CHECK_NO_THROW(read_list = util::stringCast<slist_type>(other_pattern))
    BOOST_CHECK_EQUAL(read_list, other_list);
}

}

}

#endif

// EOF
