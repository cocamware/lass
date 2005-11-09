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
	TestStream stream;

	typedef stde::slist<int> slist_type;

	// constructors

	slist_type empty_list;
	LASS_TEST_CHECK(empty_list.empty());
	LASS_TEST_CHECK_EQUAL(empty_list.size(), 0);
	LASS_TEST_CHECK(empty_list.begin() == empty_list.end());

	const slist_type::size_type n = 5;
	slist_type n_list(n, 37);
	LASS_TEST_CHECK(!n_list.empty());
	LASS_TEST_CHECK_EQUAL(n_list.size(), n);
	LASS_TEST_CHECK(n_list.begin() != n_list.end());
	slist_type::const_iterator i = n_list.begin();
	for (slist_type::size_type k = 0; k < n; ++k)
	{
		LASS_TEST_CHECK_NO_THROW(i != n_list.end());
		if (i != n_list.end()) // just a safety
		{
			LASS_TEST_CHECK_EQUAL(*i, 37);
			LASS_TEST_CHECK_NO_THROW(++i);
		}
	}

	const slist_type::size_type array_size = 7;
	int array[array_size] = {4, 3, 6, 1, 0, 2, 5};
	slist_type array_list(array, array + array_size);
	LASS_TEST_CHECK_EQUAL(array_list.size(), array_size);
	i = array_list.begin();
	for (slist_type::size_type k = 0; k < array_size; ++k)
	{
		LASS_TEST_CHECK_NO_THROW(i != array_list.end());
		if (i != array_list.end()) // just a safety
		{
			LASS_TEST_CHECK_EQUAL(*i, array[k]);
			LASS_TEST_CHECK_NO_THROW(++i);
		}
	}

	slist_type copy_list(array_list);
	LASS_TEST_CHECK_EQUAL(copy_list.size(), array_size);
	LASS_TEST_CHECK_EQUAL(array_list, copy_list);

	// assignment

	slist_type assigned_list;
	assigned_list = array_list;
	LASS_TEST_CHECK_EQUAL(assigned_list, array_list);

	assigned_list.assign(n, 37);
	LASS_TEST_CHECK_EQUAL(assigned_list, n_list);

	assigned_list.assign(array, array + array_size);
	LASS_TEST_CHECK_EQUAL(assigned_list, array_list);

	// iterators

	const slist_type const_list = array_list;
	LASS_TEST_CHECK_EQUAL(*array_list.begin(), array[0]);
	LASS_TEST_CHECK_EQUAL(*const_list.begin(), array[0]);
	LASS_TEST_CHECK_EQUAL(*array_list.prior(array_list.end()), array[array_size - 1]);
	LASS_TEST_CHECK_EQUAL(*const_list.prior(const_list.end()), array[array_size - 1]);
	LASS_TEST_CHECK(stde::next(array_list.before_begin()) == array_list.begin());
	LASS_TEST_CHECK(stde::next(const_list.before_begin()) == const_list.begin());
	LASS_TEST_CHECK_EQUAL(std::distance(array_list.begin(), array_list.end()), array_list.size());
	LASS_TEST_CHECK_EQUAL(std::distance(const_list.begin(), const_list.end()), const_list.size());

	// resize

	slist_type::size_type m = 5;
	n_list.resize(n + m, 69);
	LASS_TEST_CHECK_EQUAL(n_list.size(), n + m);
	i = n_list.begin();
	for (slist_type::size_type k = 0; k < n; ++k)
	{
		LASS_TEST_CHECK_NO_THROW(i != n_list.end());
		if (i != n_list.end()) // just a safety
		{
			LASS_TEST_CHECK_EQUAL(*i, 37);
			LASS_TEST_CHECK_NO_THROW(++i);
		}
	}
	for (slist_type::size_type k = 0; k < m; ++k)
	{
		LASS_TEST_CHECK_NO_THROW(i != n_list.end());
		if (i != n_list.end()) // just a safety
		{
			LASS_TEST_CHECK_EQUAL(*i, 69);
			LASS_TEST_CHECK_NO_THROW(++i);
		}
	}

	n_list.resize(n);
	LASS_TEST_CHECK_EQUAL(n_list.size(), n);
	i = n_list.begin();
	for (slist_type::size_type k = 0; k < n; ++k)
	{
		LASS_TEST_CHECK_NO_THROW(i != n_list.end());
		if (i != n_list.end()) // just a safety
		{
			LASS_TEST_CHECK_EQUAL(*i, 37);
			LASS_TEST_CHECK_NO_THROW(++i);
		}
	}

	// front operations

	slist_type some_list;
	some_list.push_front(0);
	LASS_TEST_CHECK(!some_list.empty());
	LASS_TEST_CHECK_EQUAL(some_list.size(), 1);
	LASS_TEST_CHECK_EQUAL(some_list.front(), 0);

	some_list.push_front(1);
	LASS_TEST_CHECK_EQUAL(some_list.size(), 2);
	LASS_TEST_CHECK_EQUAL(some_list.front(), 1);

	some_list.pop_front();
	LASS_TEST_CHECK_EQUAL(some_list.size(), 1);
	LASS_TEST_CHECK_EQUAL(some_list.front(), 0);

	// insert

	some_list.insert(some_list.begin(), 1);
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[1, 0]"));

	some_list.insert(some_list.begin(), 4, 2);
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[2, 2, 2, 2, 1, 0]"));

	int little_array[3] = { 3, 4, 5 };
	some_list.insert(some_list.end(), little_array, little_array + 3);
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[2, 2, 2, 2, 1, 0, 3, 4, 5]"));

	// insert_after

	i = some_list.insert_after(some_list.begin(), 6);
	LASS_TEST_CHECK_EQUAL(*i, 6);
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[2, 6, 2, 2, 2, 1, 0, 3, 4, 5]"));

	some_list.insert_after(some_list.prior(some_list.end()), 3, 7);
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[2, 6, 2, 2, 2, 1, 0, 3, 4, 5, 7, 7, 7]"));

	int another_little_array[] = { 8, 9, 10, 11 };
	some_list.insert_after(some_list.before_begin(), another_little_array, another_little_array + 4);
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[8, 9, 10, 11, 2, 6, 2, 2, 2, 1, 0, 3, 4, 5, 7, 7, 7]"));

	// erase

	int yet_another_array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	slist_type yet_another_list(yet_another_array, yet_another_array + 16);
	LASS_TEST_CHECK_EQUAL(yet_another_list.size(), 16);
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]"));

	i = yet_another_list.erase(yet_another_list.begin());
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]"));
	LASS_TEST_CHECK(i == yet_another_list.begin());

	i = yet_another_list.erase(stde::next(yet_another_list.begin(), 12), yet_another_list.end());
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]"));
	LASS_TEST_CHECK(i == yet_another_list.end());

	yet_another_list.erase_after(yet_another_list.begin());
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]"));

	yet_another_list.erase_after(stde::next(yet_another_list.begin(), 8), yet_another_list.end());
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[1, 3, 4, 5, 6, 7, 8, 9, 10]"));

	some_list.swap(yet_another_list);
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[1, 3, 4, 5, 6, 7, 8, 9, 10]"));
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[8, 9, 10, 11, 2, 6, 2, 2, 2, 1, 0, 3, 4, 5, 7, 7, 7]"));

	LASS_TEST_CHECK_EQUAL(n_list.size(), n);
	n_list.clear();
	LASS_TEST_CHECK_EQUAL(n_list.size(), 0);

	yet_another_list.unique();
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[8, 9, 10, 11, 2, 6, 2, 1, 0, 3, 4, 5, 7]"));

	yet_another_list.remove(2);
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[8, 9, 10, 11, 6, 1, 0, 3, 4, 5, 7]"));

	yet_another_list.remove_if(std::bind2nd(std::greater_equal<int>(), 10));
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[8, 9, 6, 1, 0, 3, 4, 5, 7]"));

	yet_another_list.sort();
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[0, 1, 3, 4, 5, 6, 7, 8, 9]"));

	// splice

	slist_type a_small_list(array, array + array_size);
	stream << a_small_list;
	LASS_TEST_CHECK(stream.isEqual("[4, 3, 6, 1, 0, 2, 5]"));

	yet_another_list.splice(stde::next(yet_another_list.begin()), a_small_list, a_small_list.begin());
	stream << a_small_list;
	LASS_TEST_CHECK(stream.isEqual("[3, 6, 1, 0, 2, 5]"));
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[0, 4, 1, 3, 4, 5, 6, 7, 8, 9]"));

	yet_another_list.splice(yet_another_list.begin(), a_small_list, a_small_list.begin(),
		stde::next(a_small_list.begin(), 3));
	stream << a_small_list;
	LASS_TEST_CHECK(stream.isEqual("[0, 2, 5]"));
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[3, 6, 1, 0, 4, 1, 3, 4, 5, 6, 7, 8, 9]"));

	yet_another_list.splice(yet_another_list.end(), a_small_list);
	LASS_TEST_CHECK(a_small_list.empty());
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[3, 6, 1, 0, 4, 1, 3, 4, 5, 6, 7, 8, 9, 0, 2, 5]"));

	// splice after

	slist_type another_small_list(another_little_array, another_little_array + 4);
	stream << another_small_list;
	LASS_TEST_CHECK(stream.isEqual("[8, 9, 10, 11]"));

	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[1, 3, 4, 5, 6, 7, 8, 9, 10]"));

	some_list.splice_after(some_list.begin(), another_small_list, another_small_list.begin());
	stream << another_small_list;
	LASS_TEST_CHECK(stream.isEqual("[8, 10, 11]"));
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[1, 9, 3, 4, 5, 6, 7, 8, 9, 10]"));

	some_list.splice_after(some_list.before_begin(), another_small_list, another_small_list.before_begin(),
		another_small_list.begin());
	stream << another_small_list;
	LASS_TEST_CHECK(stream.isEqual("[10, 11]"));
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[8, 1, 9, 3, 4, 5, 6, 7, 8, 9, 10]"));

	some_list.splice_after(some_list.prior(some_list.end()), another_small_list);
	LASS_TEST_CHECK(another_small_list.empty());
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[8, 1, 9, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11]"));

	// merge

	yet_another_list.sort();
	stream << yet_another_list;
	LASS_TEST_CHECK(stream.isEqual("[0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8, 9]"));

	some_list.sort();
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[1, 3, 4, 5, 6, 7, 8, 8, 9, 9, 10, 10, 11]"));

	some_list.merge(yet_another_list);
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[0, 0, 1, 1, 1, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11]"));

	some_list.unique();
	stream << some_list;
	LASS_TEST_CHECK(stream.isEqual("[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]"));
}

}

}

#endif

// EOF
