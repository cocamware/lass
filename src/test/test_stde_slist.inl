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
#include "../stde/extended_iterator.h"
#include "../util/string_cast.h"

namespace lass
{
namespace test
{

void testStdeSlist()
{
    boost::test_toolbox::output_test_stream stream;

    typedef stde::slist<int> slist_type;

    // constructors 
    
    slist_type empty_list;
    BOOST_CHECK(empty_list.empty());
    BOOST_CHECK_EQUAL(empty_list.size(), 0);
    BOOST_CHECK(empty_list.begin() == empty_list.end());

    const slist_type::size_type n = 5;
    slist_type n_list(n, 37);
    BOOST_CHECK(!n_list.empty());
    BOOST_CHECK_EQUAL(n_list.size(), n);
    BOOST_CHECK(n_list.begin() != n_list.end());
    slist_type::const_iterator i = n_list.begin();
    for (slist_type::size_type k = 0; k < n; ++k)
    {
        BOOST_CHECK_NO_THROW(i != n_list.end());
        if (i != n_list.end()) // just a safety
        {
            BOOST_CHECK_EQUAL(*i, 37);
            BOOST_CHECK_NO_THROW(++i);
        }
    }

    const slist_type::size_type array_size = 7;
    int array[array_size] = {4, 3, 6, 1, 0, 2, 5};
    slist_type array_list(array, array + array_size);
    BOOST_CHECK_EQUAL(array_list.size(), array_size);
    i = array_list.begin();
    for (slist_type::size_type k = 0; k < array_size; ++k)
    {
        BOOST_CHECK_NO_THROW(i != array_list.end());
        if (i != array_list.end()) // just a safety
        {
            BOOST_CHECK_EQUAL(*i, array[k]);
            BOOST_CHECK_NO_THROW(++i);
        }
    }

    slist_type copy_list(array_list);
    BOOST_CHECK_EQUAL(copy_list.size(), array_size);
    BOOST_CHECK_EQUAL(array_list, copy_list);

    // assignment

    slist_type assigned_list;
    assigned_list = array_list;
    BOOST_CHECK_EQUAL(assigned_list, array_list);

    assigned_list.assign(n, 37);
    BOOST_CHECK_EQUAL(assigned_list, n_list);

    assigned_list.assign(array, array + array_size);
    BOOST_CHECK_EQUAL(assigned_list, array_list);

    // iterators

    const slist_type const_list = array_list;
    BOOST_CHECK_EQUAL(*array_list.begin(), array[0]);
    BOOST_CHECK_EQUAL(*const_list.begin(), array[0]);
    BOOST_CHECK_EQUAL(*array_list.prior(array_list.end()), array[array_size - 1]);
    BOOST_CHECK_EQUAL(*const_list.prior(const_list.end()), array[array_size - 1]);
    BOOST_CHECK(stde::next(array_list.before_begin()) == array_list.begin());
    BOOST_CHECK(stde::next(const_list.before_begin()) == const_list.begin());
    BOOST_CHECK_EQUAL(std::distance(array_list.begin(), array_list.end()), array_list.size());
    BOOST_CHECK_EQUAL(std::distance(const_list.begin(), const_list.end()), const_list.size());

    // resize

    slist_type::size_type m = 5;
    n_list.resize(n + m, 69);
    BOOST_CHECK_EQUAL(n_list.size(), n + m);
    i = n_list.begin();
    for (slist_type::size_type k = 0; k < n; ++k)
    {
        BOOST_CHECK_NO_THROW(i != n_list.end());
        if (i != n_list.end()) // just a safety
        {
            BOOST_CHECK_EQUAL(*i, 37);
            BOOST_CHECK_NO_THROW(++i);
        }
    }
    for (slist_type::size_type k = 0; k < m; ++k)
    {
        BOOST_CHECK_NO_THROW(i != n_list.end());
        if (i != n_list.end()) // just a safety
        {
            BOOST_CHECK_EQUAL(*i, 69);
            BOOST_CHECK_NO_THROW(++i);
        }
    }

    n_list.resize(n);
    BOOST_CHECK_EQUAL(n_list.size(), n + m);
    i = n_list.begin();
    for (slist_type::size_type k = 0; k < n; ++k)
    {
        BOOST_CHECK_NO_THROW(i != n_list.end());
        if (i != n_list.end()) // just a safety
        {
            BOOST_CHECK_EQUAL(*i, 37);
            BOOST_CHECK_NO_THROW(++i);
        }
    }

    // front operations

    slist_type some_list;
    some_list.push_front(0);
    BOOST_CHECK(!some_list.empty());
    BOOST_CHECK_EQUAL(some_list.size(), 1);
    BOOST_CHECK_EQUAL(some_list.front(), 0);

    some_list.push_front(1);
    BOOST_CHECK_EQUAL(some_list.size(), 2);
    BOOST_CHECK_EQUAL(some_list.front(), 1);

    some_list.pop_front();
    BOOST_CHECK_EQUAL(some_list.size(), 1);
    BOOST_CHECK_EQUAL(some_list.front(), 0);

    some_list.insert(some_list.begin(), 1);
    stream << some_list;
    BOOST_CHECK(stream.is_equal("[1, 0]"));

    some_list.insert(some_list.begin(), 4, 2);
    stream << some_list;
    BOOST_CHECK(stream.is_equal("[2, 2, 2, 2, 1, 0]"));

    int little_array[3] = { 3, 4, 5 };
    some_list.insert(some_list.end(), little_array, little_array + 3);
    BOOST_CHECK(stream.is_equal("[2, 2, 2, 2, 1, 0, 3, 4, 5]"));
  
}

}

}

#endif

// EOF
