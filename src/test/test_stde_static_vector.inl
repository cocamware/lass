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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_STATIC_VECTOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_STATIC_VECTOR_INL

#include "test_common.h"
#include "../stde/static_vector.h"

namespace lass
{
namespace test
{

void testStdeStaticVector()
{
	boost::test_toolbox::output_test_stream stream;

	typedef stde::static_vector<int, 10> static_vector_type;

	// constructors

	static_vector_type empty_vector;
	BOOST_CHECK(empty_vector.empty());
	BOOST_CHECK_EQUAL(empty_vector.size(), 0);
	BOOST_CHECK(empty_vector.begin() == empty_vector.end());

	const static_vector_type::size_type n = 5;
	static_vector_type n_vector(n, 37);
	BOOST_CHECK(!n_vector.empty());
	BOOST_CHECK_EQUAL(n_vector.size(), n);
	BOOST_CHECK(n_vector.begin() != n_vector.end());
	static_vector_type::const_iterator i = n_vector.begin();
	for (static_vector_type::size_type k = 0; k < n; ++k)
	{
		BOOST_CHECK_NO_THROW(i != n_vector.end());
		if (i != n_vector.end()) // just a safety
		{
			BOOST_CHECK_EQUAL(*i, 37);
			BOOST_CHECK_NO_THROW(++i);
		}
	}

	const static_vector_type::size_type array_size = 7;
	int array[array_size] = {4, 3, 6, 1, 0, 2, 5};
	static_vector_type array_vector(array, array + array_size);
	BOOST_CHECK_EQUAL(array_vector.size(), array_size);
	i = array_vector.begin();
	for (static_vector_type::size_type k = 0; k < array_size; ++k)
	{
		BOOST_CHECK_NO_THROW(i != array_vector.end());
		if (i != array_vector.end()) // just a safety
		{
			BOOST_CHECK_EQUAL(*i, array[k]);
			BOOST_CHECK_NO_THROW(++i);
		}
	}

	static_vector_type copy_vector(array_vector);
	BOOST_CHECK_EQUAL(copy_vector.size(), array_size);
	BOOST_CHECK_EQUAL(array_vector, copy_vector);

	// assignment

	static_vector_type assigned_vector;
	assigned_vector = array_vector;
	BOOST_CHECK_EQUAL(assigned_vector, array_vector);

	assigned_vector.assign(n, 37);
	BOOST_CHECK_EQUAL(assigned_vector, n_vector);

	assigned_vector.assign(array, array + array_size);
	BOOST_CHECK_EQUAL(assigned_vector, array_vector);

	// iterators

	const static_vector_type const_vector = array_vector;
	BOOST_CHECK_EQUAL(*array_vector.begin(), array[0]);
	BOOST_CHECK_EQUAL(*const_vector.begin(), array[0]);
	BOOST_CHECK_EQUAL(*(array_vector.end() - 1), array[array_size - 1]);
	BOOST_CHECK_EQUAL(*(const_vector.end() - 1), array[array_size - 1]);
	BOOST_CHECK_EQUAL(std::distance(array_vector.begin(), array_vector.end()), array_vector.size());
	BOOST_CHECK_EQUAL(std::distance(const_vector.begin(), const_vector.end()), const_vector.size());

	// resize

	static_vector_type::size_type m = 5;
	BOOST_CHECK_NO_THROW(n_vector.resize(n + m, 69));
	BOOST_CHECK_EQUAL(n_vector.size(), n + m);
	i = n_vector.begin();
	for (static_vector_type::size_type k = 0; k < n; ++k)
	{
		BOOST_CHECK_NO_THROW(i != n_vector.end());
		if (i != n_vector.end()) // just a safety
		{
			BOOST_CHECK_EQUAL(*i, 37);
			BOOST_CHECK_NO_THROW(++i);
		}
	}
	for (static_vector_type::size_type k = 0; k < m; ++k)
	{
		BOOST_CHECK_NO_THROW(i != n_vector.end());
		if (i != n_vector.end()) // just a safety
		{
			BOOST_CHECK_EQUAL(*i, 69);
			BOOST_CHECK_NO_THROW(++i);
		}
	}

	BOOST_CHECK_NO_THROW(n_vector.resize(n));
	BOOST_CHECK_EQUAL(n_vector.size(), n);
	i = n_vector.begin();
	for (static_vector_type::size_type k = 0; k < n; ++k)
	{
		BOOST_CHECK_NO_THROW(i != n_vector.end());
		if (i != n_vector.end()) // just a safety
		{
			BOOST_CHECK_EQUAL(*i, 37);
			BOOST_CHECK_NO_THROW(++i);
		}
	}

	BOOST_CHECK_THROW(n_vector.resize(n_vector.max_size() + 1), std::length_error);

	// back operations

	static_vector_type some_vector;
	BOOST_CHECK_NO_THROW(some_vector.push_back(0));
	BOOST_CHECK(!some_vector.empty());
	BOOST_CHECK_EQUAL(some_vector.size(), 1);
	BOOST_CHECK_EQUAL(some_vector.back(), 0);

	BOOST_CHECK_NO_THROW(some_vector.push_back(1));
	BOOST_CHECK_EQUAL(some_vector.size(), 2);
	BOOST_CHECK_EQUAL(some_vector.back(), 1);

	some_vector.pop_back();
	BOOST_CHECK_EQUAL(some_vector.size(), 1);
	BOOST_CHECK_EQUAL(some_vector.back(), 0);


	// insert

	some_vector.insert(some_vector.begin(), 1);
	stream << some_vector;
	BOOST_CHECK(stream.is_equal("[1, 0]"));

	some_vector.insert(some_vector.begin(), 4, 2);
	stream << some_vector;
	BOOST_CHECK(stream.is_equal("[2, 2, 2, 2, 1, 0]"));

	int little_array[3] = { 3, 4, 5 };
	some_vector.insert(some_vector.end(), little_array, little_array + 3);
	stream << some_vector;
	BOOST_CHECK(stream.is_equal("[2, 2, 2, 2, 1, 0, 3, 4, 5]"));


	int yet_another_array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	static_vector_type yet_another_vector(yet_another_array, yet_another_array + 10);
	BOOST_CHECK_EQUAL(yet_another_vector.size(), 10);
	stream << yet_another_vector;
	BOOST_CHECK(stream.is_equal("[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]"));

	// fixed size limits

	BOOST_CHECK_THROW(yet_another_vector.push_back(666), std::length_error);
	BOOST_CHECK_THROW(yet_another_vector.insert(yet_another_vector.begin(), 666), std::length_error);
	BOOST_CHECK_THROW(yet_another_vector.insert(yet_another_vector.begin(), 5, 666), std::length_error);
	BOOST_CHECK_THROW(yet_another_vector.insert(yet_another_vector.begin(), yet_another_array, yet_another_array + 10), std::length_error);

	// erase

	i = yet_another_vector.erase(yet_another_vector.begin());
	stream << yet_another_vector;
	BOOST_CHECK(stream.is_equal("[1, 2, 3, 4, 5, 6, 7, 8, 9]"));
	BOOST_CHECK(i == yet_another_vector.begin());

	i = yet_another_vector.erase(yet_another_vector.begin() + 5, yet_another_vector.begin() + 7);
	stream << yet_another_vector;
	BOOST_CHECK(stream.is_equal("[1, 2, 3, 4, 5, 8, 9]"));
	BOOST_CHECK(i == yet_another_vector.begin() + 5);
	
	BOOST_CHECK_EQUAL(n_vector.size(), n);
	n_vector.clear();
	BOOST_CHECK_EQUAL(n_vector.size(), 0);
}

}

}

#endif

// EOF
