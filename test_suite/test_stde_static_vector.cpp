/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	*** BEGIN LICENSE INFORMATION ***
 *	
 *	The contents of this file are subject to the Common Public Attribution License 
 *	Version 1.0 (the "License"); you may not use this file except in compliance with 
 *	the License. You may obtain a copy of the License at 
 *	http://lass.sourceforge.net/cpal-license. The License is based on the 
 *	Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *	use of software over a computer network and provide for limited attribution for 
 *	the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *	with Exhibit B.
 *	
 *	Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *	WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *	language governing rights and limitations under the License.
 *	
 *	The Original Code is LASS - Library of Assembled Shared Sources.
 *	
 *	The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *	The Original Developer is the Initial Developer.
 *	
 *	All portions of the code written by the Initial Developer are:
 *	Copyright (C) 2004-2007 the Initial Developer.
 *	All Rights Reserved.
 *	
 *	Contributor(s):
 *
 *	Alternatively, the contents of this file may be used under the terms of the 
 *	GNU General Public License Version 2 or later (the GPL), in which case the 
 *	provisions of GPL are applicable instead of those above.  If you wish to allow use
 *	of your version of this file only under the terms of the GPL and not to allow 
 *	others to use your version of this file under the CPAL, indicate your decision by 
 *	deleting the provisions above and replace them with the notice and other 
 *	provisions required by the GPL License. If you do not delete the provisions above,
 *	a recipient may use your version of this file under either the CPAL or the GPL.
 *	
 *	*** END LICENSE INFORMATION ***
 */

#include "test_common.h"
#include "../lass/stde/static_vector.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4552) // '!=' : operator has no effect; expected operator with side-effect
#endif

namespace lass
{
namespace test
{

void testStdeStaticVector()
{
	TestStream stream;

	typedef stde::static_vector<int, 10> static_vector_type;

	// constructors

	static_vector_type empty_vector;
	LASS_TEST_CHECK(empty_vector.empty());
	LASS_TEST_CHECK_EQUAL(empty_vector.size(), size_t(0));
	LASS_TEST_CHECK(empty_vector.begin() == empty_vector.end());

	const size_t n = 5;
	static_vector_type n_vector(n, 37);
	LASS_TEST_CHECK(!n_vector.empty());
	LASS_TEST_CHECK_EQUAL(n_vector.size(), n);
	LASS_TEST_CHECK(n_vector.begin() != n_vector.end());
	static_vector_type::const_iterator i = n_vector.begin();
	for (size_t k = 0; k < n; ++k)
	{
		LASS_TEST_CHECK_NO_THROW(bool LASS_UNUSED(a) = i != n_vector.end());
		if (i != n_vector.end()) // just a safety
		{
			LASS_TEST_CHECK_EQUAL(*i, 37);
			LASS_TEST_CHECK_NO_THROW(++i);
		}
	}

	const size_t array_size = 7;
	int array[array_size] = {4, 3, 6, 1, 0, 2, 5};
	static_vector_type array_vector(array, array + array_size);
	LASS_TEST_CHECK_EQUAL(array_vector.size(), array_size);
	i = array_vector.begin();
	for (size_t k = 0; k < array_size; ++k)
	{
		LASS_TEST_CHECK_NO_THROW(bool LASS_UNUSED(a) = i != array_vector.end());
		if (i != array_vector.end()) // just a safety
		{
			LASS_TEST_CHECK_EQUAL(*i, array[k]);
			LASS_TEST_CHECK_NO_THROW(++i);
		}
	}

	static_vector_type copy_vector(array_vector);
	LASS_TEST_CHECK_EQUAL(copy_vector.size(), array_size);
	LASS_TEST_CHECK_EQUAL(array_vector, copy_vector);

	// assignment

	static_vector_type assigned_vector;
	assigned_vector = array_vector;
	LASS_TEST_CHECK_EQUAL(assigned_vector, array_vector);

	assigned_vector.assign(n, 37);
	LASS_TEST_CHECK_EQUAL(assigned_vector, n_vector);

	assigned_vector.assign(array, array + array_size);
	LASS_TEST_CHECK_EQUAL(assigned_vector, array_vector);

	// iterators

	const static_vector_type const_vector = array_vector;
	LASS_TEST_CHECK_EQUAL(*array_vector.begin(), array[0]);
	LASS_TEST_CHECK_EQUAL(*const_vector.begin(), array[0]);
	LASS_TEST_CHECK_EQUAL(*(array_vector.end() - 1), array[array_size - 1]);
	LASS_TEST_CHECK_EQUAL(*(const_vector.end() - 1), array[array_size - 1]);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(std::distance(array_vector.begin(), array_vector.end())), array_vector.size());
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(std::distance(const_vector.begin(), const_vector.end())), const_vector.size());

	// resize

	const size_t m = 5;
	LASS_TEST_CHECK_NO_THROW(n_vector.resize(n + m, 69));
	LASS_TEST_CHECK_EQUAL(n_vector.size(), n + m);
	i = n_vector.begin();
	for (size_t k = 0; k < n; ++k)
	{
		LASS_TEST_CHECK_NO_THROW(bool LASS_UNUSED(a) = i != n_vector.end());
		if (i != n_vector.end()) // just a safety
		{
			LASS_TEST_CHECK_EQUAL(*i, 37);
			LASS_TEST_CHECK_NO_THROW(++i);
		}
	}
	for (size_t k = 0; k < m; ++k)
	{
		LASS_TEST_CHECK_NO_THROW(bool LASS_UNUSED(a) = i != n_vector.end());
		if (i != n_vector.end()) // just a safety
		{
			LASS_TEST_CHECK_EQUAL(*i, 69);
			LASS_TEST_CHECK_NO_THROW(++i);
		}
	}

	LASS_TEST_CHECK_NO_THROW(n_vector.resize(n));
	LASS_TEST_CHECK_EQUAL(n_vector.size(), n);
	i = n_vector.begin();
	for (size_t k = 0; k < n; ++k)
	{
		LASS_TEST_CHECK_NO_THROW(bool LASS_UNUSED(a) = i != n_vector.end());
		if (i != n_vector.end()) // just a safety
		{
			LASS_TEST_CHECK_EQUAL(*i, 37);
			LASS_TEST_CHECK_NO_THROW(++i);
		}
	}

	LASS_TEST_CHECK_THROW(n_vector.resize(n_vector.max_size() + 1), std::length_error);

	// back operations

	static_vector_type some_vector;
	LASS_TEST_CHECK_NO_THROW(some_vector.push_back(0));
	LASS_TEST_CHECK(!some_vector.empty());
	LASS_TEST_CHECK_EQUAL(some_vector.size(), size_t(1));
	LASS_TEST_CHECK_EQUAL(some_vector.back(), 0);

	LASS_TEST_CHECK_NO_THROW(some_vector.push_back(1));
	LASS_TEST_CHECK_EQUAL(some_vector.size(), size_t(2));
	LASS_TEST_CHECK_EQUAL(some_vector.back(), 1);

	some_vector.pop_back();
	LASS_TEST_CHECK_EQUAL(some_vector.size(), size_t(1));
	LASS_TEST_CHECK_EQUAL(some_vector.back(), 0);


	// insert

	some_vector.insert(some_vector.begin(), 1);
	stream << some_vector;
	LASS_TEST_CHECK(stream.isEqual("[1, 0]"));

	some_vector.insert(some_vector.begin(), 4, 2);
	stream << some_vector;
	LASS_TEST_CHECK(stream.isEqual("[2, 2, 2, 2, 1, 0]"));

	int little_array[3] = { 3, 4, 5 };
	some_vector.insert(some_vector.end(), little_array, little_array + 3);
	stream << some_vector;
	LASS_TEST_CHECK(stream.isEqual("[2, 2, 2, 2, 1, 0, 3, 4, 5]"));


	int yet_another_array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	static_vector_type yet_another_vector(yet_another_array, yet_another_array + 10);
	LASS_TEST_CHECK_EQUAL(yet_another_vector.size(), size_t(10));
	stream << yet_another_vector;
	LASS_TEST_CHECK(stream.isEqual("[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]"));

	// fixed size limits

	LASS_TEST_CHECK_THROW(yet_another_vector.push_back(666), std::length_error);
	LASS_TEST_CHECK_THROW(yet_another_vector.insert(yet_another_vector.begin(), 666), std::length_error);
	LASS_TEST_CHECK_THROW(yet_another_vector.insert(yet_another_vector.begin(), 5, 666), std::length_error);
	LASS_TEST_CHECK_THROW(yet_another_vector.insert(yet_another_vector.begin(), yet_another_array, yet_another_array + 10), std::length_error);

	// erase

	i = yet_another_vector.erase(yet_another_vector.begin());
	stream << yet_another_vector;
	LASS_TEST_CHECK(stream.isEqual("[1, 2, 3, 4, 5, 6, 7, 8, 9]"));
	LASS_TEST_CHECK(i == yet_another_vector.begin());

	i = yet_another_vector.erase(yet_another_vector.begin() + 5, yet_another_vector.begin() + 7);
	stream << yet_another_vector;
	LASS_TEST_CHECK(stream.isEqual("[1, 2, 3, 4, 5, 8, 9]"));
	LASS_TEST_CHECK(i == yet_another_vector.begin() + 5);
	
	LASS_TEST_CHECK_EQUAL(n_vector.size(), n);
	n_vector.clear();
	LASS_TEST_CHECK_EQUAL(n_vector.size(), size_t(0));
}

TUnitTest test_stde_static_vector()
{
	return TUnitTest(1, LASS_TEST_CASE(testStdeStaticVector));
}

}

}

// EOF
