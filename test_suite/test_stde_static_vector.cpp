/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2004-2023 the Initial Developer.
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
namespace
{


class object
{
public:
	object(int value = 0): value_(value) { ++count_; }
#if LASS_HAVE_ARM && LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_GCC
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Warray-bounds"
#endif
	object(const object& other): value_(other.value_) { ++count_; }
#if LASS_HAVE_ARM && LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_GCC
#	pragma GCC diagnostic pop
#endif
	~object() { --count_; }
	object& operator=(const object& other) = default;
	int value() const { return value_; }
	static int count() { return count_; }
private:
	static int count_;
	int value_;
};

int object::count_ = 0;

constexpr size_t max_size = 10;
using vector_type = stde::static_vector<object, max_size>;
using string_vector_type = stde::static_vector<std::string, max_size>;
using object_ptr = std::unique_ptr<object>;
using ptr_vector_type = stde::static_vector<object_ptr, max_size>;
object* const null = nullptr;

}

void testStdeStaticVectorConstructors()
{
	{
		// default constructor
		vector_type vector;
		LASS_ASSERT(vector.empty());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(0));
	}

	{
		// count constructor
		vector_type vector(5);
		LASS_TEST_CHECK(!vector.empty());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(5));
		LASS_TEST_CHECK_EQUAL(object::count(), 5);
		for (size_t k = 0; k < 5; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), 0);
		}
		LASS_TEST_CHECK_THROW(vector.at(5), std::out_of_range);

		// exceeding max size
		LASS_TEST_CHECK_THROW(vector_type(666), std::length_error);
	}

	{
		// count&value constructor
		vector_type vector(5, object{ 37 });
		LASS_TEST_CHECK(!vector.empty());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(5));
		LASS_TEST_CHECK_EQUAL(object::count(), 5);
		for (size_t k = 0; k < 5; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), 37);
		}
		LASS_TEST_CHECK_THROW(vector.at(5), std::out_of_range);

		// exceeding max size
		LASS_TEST_CHECK_THROW(vector_type(666, object{ 37 }), std::length_error);
	}

	{
		// count&value constructor using input iterator overload
		vector_type vector(static_cast<int>(5), static_cast<int>(37));
		LASS_TEST_CHECK(!vector.empty());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(5));
		LASS_TEST_CHECK_EQUAL(object::count(), 5);
		for (size_t k = 0; k < 5; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), 37);
		}
		LASS_TEST_CHECK_THROW(vector.at(5), std::out_of_range);

		// exceeding max size
		LASS_TEST_CHECK_THROW(vector_type(static_cast<int>(666), static_cast<int>(37)), std::length_error);
	}

	{
		// input iterators constructor
		std::list<int> list{ { 4, 3, 6, 1, 0, 2, 5 } };
		vector_type vector(list.begin(), list.end());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(7));
		LASS_TEST_CHECK_EQUAL(object::count(), 7);
		size_t k = 0;
		for (auto x : list)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k++).value(), x);
		}
		LASS_TEST_CHECK_THROW(vector.at(k), std::out_of_range);
	}

	{
		// input iterators constructor exceeding max size
		std::list<int> list{ { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 } };
		LASS_TEST_CHECK_THROW(vector_type(list.begin(), list.end()), std::length_error);
	}

	{
		// initializer list & copy constructor
		int array[] = { 4, 3, 6, 1, 0, 2, 5 };
		vector_type vector({ 4, 3, 6, 1, 0, 2, 5 });
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(7));
		LASS_TEST_CHECK_EQUAL(object::count(), 7);
		vector_type copy(vector);
		LASS_TEST_CHECK_EQUAL(copy.size(), static_cast<size_t>(7));
		LASS_TEST_CHECK_EQUAL(object::count(), 14);
		for (size_t k = 0; k < 7; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), array[k]);
			LASS_TEST_CHECK_EQUAL(copy.at(k).value(), array[k]);
		}
		LASS_TEST_CHECK_THROW(vector.at(7), std::out_of_range);
		LASS_TEST_CHECK_THROW(copy.at(7), std::out_of_range);

		// exceeding max size
		LASS_TEST_CHECK_THROW(vector_type({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }), std::length_error);
	}

	{
		// move constructor, destructor, and move push_back
		object_ptr a(new object{ 1 });
		object_ptr b(new object{ 2 });
		object* aa = a.get();
		object* bb = b.get();
		LASS_TEST_CHECK_EQUAL(object::count(), 2);

		{
			const size_t size = 2;
			ptr_vector_type vector;
			vector.push_back(std::move(a));
			vector.push_back(std::move(b));
			LASS_TEST_CHECK_EQUAL(object::count(), 2);
			LASS_TEST_CHECK_EQUAL(vector.size(), size);
			LASS_TEST_CHECK_EQUAL(vector.at(0).get(), aa);
			LASS_TEST_CHECK_EQUAL(vector.at(1).get(), bb);

			ptr_vector_type moved(std::move(vector));
			LASS_TEST_CHECK_EQUAL(object::count(), 2);
			LASS_TEST_CHECK_EQUAL(moved.size(), size);
			LASS_TEST_CHECK_EQUAL(moved.at(0).get(), aa);
			LASS_TEST_CHECK_EQUAL(moved.at(1).get(), bb);

			LASS_TEST_CHECK_EQUAL(vector.size(), size);
			LASS_TEST_CHECK_EQUAL(vector.at(0).get(), null);
			LASS_TEST_CHECK_EQUAL(vector.at(1).get(), null);
		}

		LASS_TEST_CHECK_EQUAL(object::count(), 0);
	}
}

void testStdeStaticVectorAssignment()
{
	{
		// copy assignment
		vector_type vector({ 4, 3, 6, 1, 0, 2, 5 });
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(7));
		LASS_TEST_CHECK_EQUAL(object::count(), 7);

		vector_type copy({ 8, 7, 9 });
		LASS_TEST_CHECK_EQUAL(copy.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(object::count(), 10);

		copy = vector;
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(7));
		LASS_TEST_CHECK_EQUAL(copy.size(), static_cast<size_t>(7));
		LASS_TEST_CHECK_EQUAL(object::count(), 14);
		for (size_t k = 0; k < 7; ++k)
		{
			LASS_TEST_CHECK_EQUAL(copy.at(k).value(), vector.at(k).value());
		}
		LASS_TEST_CHECK_THROW(vector.at(8), std::out_of_range);
		LASS_TEST_CHECK_THROW(copy.at(8), std::out_of_range);

		vector_type other({ 10, 11 });
		LASS_TEST_CHECK_EQUAL(other.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 16);
		copy = other;
		LASS_TEST_CHECK_EQUAL(other.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(copy.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 11);
		for (size_t k = 0; k < 2; ++k)
		{
			LASS_TEST_CHECK_EQUAL(copy.at(k).value(), other.at(k).value());
		}
		LASS_TEST_CHECK_THROW(other.at(3), std::out_of_range);
		LASS_TEST_CHECK_THROW(copy.at(3), std::out_of_range);
	}

	{
		// move assignment
		object_ptr a(new object{ 1 });
		object_ptr b(new object{ 2 });
		object_ptr c(new object{ 3 });
		object* aa = a.get();
		object* bb = b.get();
		object* cc = c.get();
		LASS_TEST_CHECK_EQUAL(object::count(), 3);

		ptr_vector_type moved;
		moved.push_back(std::move(c));
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(moved.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(moved.at(0).get(), cc);

		{
			ptr_vector_type vector;
			vector.push_back(std::move(a));
			vector.push_back(std::move(b));
			LASS_TEST_CHECK_EQUAL(object::count(), 3);
			LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
			LASS_TEST_CHECK_EQUAL(vector.at(0).get(), aa);
			LASS_TEST_CHECK_EQUAL(vector.at(1).get(), bb);

			moved = std::move(vector);
			LASS_TEST_CHECK_EQUAL(object::count(), 2);
			LASS_TEST_CHECK_EQUAL(moved.size(), static_cast<size_t>(2));
			LASS_TEST_CHECK_EQUAL(moved.at(0).get(), aa);
			LASS_TEST_CHECK_EQUAL(moved.at(1).get(), bb);

			LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
			LASS_TEST_CHECK_EQUAL(vector.at(0).get(), null);
			LASS_TEST_CHECK_EQUAL(vector.at(1).get(), null);
		}

		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		LASS_TEST_CHECK_EQUAL(moved.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(moved.at(0).get(), aa);
		LASS_TEST_CHECK_EQUAL(moved.at(1).get(), bb);
	}

	{
		// move assignment 2
		object_ptr a(new object{ 1 });
		object_ptr b(new object{ 2 });
		object_ptr c(new object{ 3 });
		object* aa = a.get();
		object* bb = b.get();
		object* cc = c.get();
		LASS_TEST_CHECK_EQUAL(object::count(), 3);

		ptr_vector_type moved;
		moved.push_back(std::move(a));
		moved.push_back(std::move(b));
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(moved.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(moved.at(0).get(), aa);
		LASS_TEST_CHECK_EQUAL(moved.at(1).get(), bb);

		{
			ptr_vector_type vector;
			vector.push_back(std::move(c));
			LASS_TEST_CHECK_EQUAL(object::count(), 3);
			LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(1));
			LASS_TEST_CHECK_EQUAL(vector.at(0).get(), cc);

			moved = std::move(vector);
			LASS_TEST_CHECK_EQUAL(object::count(), 1);
			LASS_TEST_CHECK_EQUAL(moved.size(), static_cast<size_t>(1));
			LASS_TEST_CHECK_EQUAL(moved.at(0).get(), cc);

			LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(1));
			LASS_TEST_CHECK_EQUAL(vector.at(0).get(), null);
		}

		LASS_TEST_CHECK_EQUAL(object::count(), 1);
		LASS_TEST_CHECK_EQUAL(moved.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(moved.at(0).get(), cc);
	}

	{
		// assign count/value
		vector_type vector(3, object{ 1 });
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(object::count(), static_cast<int>(3));

		// to larger size
		vector.assign(5, object{ 2 });
		LASS_TEST_CHECK(!vector.empty());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(5));
		LASS_TEST_CHECK_EQUAL(object::count(), 5);
		for (size_t k = 0; k < 5; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), 2);
		}
		LASS_TEST_CHECK_THROW(vector.at(5), std::out_of_range);

		// to smaller size
		vector.assign(2, object{ 3 });
		LASS_TEST_CHECK(!vector.empty());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		for (size_t k = 0; k < 2; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), 3);
		}
		LASS_TEST_CHECK_THROW(vector.at(2), std::out_of_range);

		// exceeding max size
		LASS_TEST_CHECK_THROW(vector.assign(666, object{ 4 }), std::length_error);
	}

	{
		// assign count/value using input iterator overload
		vector_type vector(3);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));

		// to larger size
		vector.assign(5, 37);
		LASS_TEST_CHECK(!vector.empty());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(5));
		LASS_TEST_CHECK_EQUAL(object::count(), 5);
		for (size_t k = 0; k < 5; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), 37);
		}
		LASS_TEST_CHECK_THROW(vector.at(5), std::out_of_range);

		// to smaller size
		vector.assign(2, 42);
		LASS_TEST_CHECK(!vector.empty());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		for (size_t k = 0; k < 2; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), 42);
		}
		LASS_TEST_CHECK_THROW(vector.at(2), std::out_of_range);

		// exceeding max size
		LASS_TEST_CHECK_THROW(vector.assign(666, 37), std::length_error);
	}

	{
		// assign initializer_list
		vector_type vector(3);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(object::count(), 3);

		// to larger size
		int array[] = {4, 3, 6, 1, 0, 2, 5};
		vector.assign({ 4, 3, 6, 1, 0, 2, 5 });
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(7));
		LASS_TEST_CHECK_EQUAL(object::count(), 7);
		for (size_t k = 0; k < 7; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), array[k]);
		}
		LASS_TEST_CHECK_THROW(vector.at(7), std::out_of_range);

		// to smaller size
		int array2[] = { 7, 8, 9 };
		vector.assign({ 7, 8, 9 });
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		for (size_t k = 0; k < 3; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), array2[k]);
		}
		LASS_TEST_CHECK_THROW(vector.at(3), std::out_of_range);

		// exceeding max size
		LASS_TEST_CHECK_THROW(vector.assign({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }), std::length_error);
	}

	{
		// assign iterator
		vector_type vector(3);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(object::count(), 3);

		// to larger size
		std::list<int> list{ { 4, 3, 6, 1, 0, 2, 5 } };
		LASS_TEST_CHECK_EQUAL(list.size(), static_cast<size_t>(7));
		vector.assign(list.begin(), list.end());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(7));
		LASS_TEST_CHECK_EQUAL(object::count(), 7);
		size_t k = 0;
		for (auto x : list)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k++).value(), x);
		}
		LASS_TEST_CHECK_EQUAL(k, static_cast<size_t>(7));
		LASS_TEST_CHECK_THROW(vector.at(k), std::out_of_range);

		// to smaller size
		std::list<int> list2{ { 7, 8, 9 } };
		LASS_TEST_CHECK_EQUAL(list2.size(), static_cast<size_t>(3));
		vector.assign(list2.begin(), list2.end());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		k = 0;
		for (auto x : list2)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k++).value(), x);
		}
		LASS_TEST_CHECK_EQUAL(k, static_cast<size_t>(3));
		LASS_TEST_CHECK_THROW(vector.at(k), std::out_of_range);

		// exceeding max size
		std::list<int> list3{ { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 } };
		LASS_TEST_CHECK_THROW(vector.assign(list3.begin(), list3.end()), std::length_error);
	}
}

void testStdeStaticVectorIterators()
{
	{
		// iterators empty vector
		vector_type vector;
		LASS_TEST_CHECK(vector.begin() == vector.end());
		LASS_TEST_CHECK(static_cast<const vector_type&>(vector).begin() == static_cast<const vector_type&>(vector).end());
		LASS_TEST_CHECK(vector.cbegin() == vector.cend());
		LASS_TEST_CHECK(vector.rbegin() == vector.rend());
		LASS_TEST_CHECK(static_cast<const vector_type&>(vector).rbegin() == static_cast<const vector_type&>(vector).rend());
		LASS_TEST_CHECK(vector.crbegin() == vector.crend());
	}

	{
		// iterators non-empty vector
		vector_type vector{ { 1, 2 }};
		LASS_TEST_CHECK(vector.begin() != vector.end());
		LASS_TEST_CHECK(static_cast<const vector_type&>(vector).begin() != static_cast<const vector_type&>(vector).end());
		LASS_TEST_CHECK(vector.cbegin() != vector.cend());
		LASS_TEST_CHECK(vector.rbegin() != vector.rend());
		LASS_TEST_CHECK(static_cast<const vector_type&>(vector).rbegin() != static_cast<const vector_type&>(vector).rend());
		LASS_TEST_CHECK(vector.crbegin() != vector.crend());
		{
			auto it = vector.begin();
			LASS_TEST_CHECK_EQUAL((it++)->value(), 1);
			LASS_TEST_CHECK_EQUAL((it++)->value(), 2);
			LASS_TEST_CHECK(it == vector.end());
		}
		{
			auto it = static_cast<const vector_type&>(vector).begin();
			LASS_TEST_CHECK_EQUAL((it++)->value(), 1);
			LASS_TEST_CHECK_EQUAL((it++)->value(), 2);
			LASS_TEST_CHECK(it == static_cast<const vector_type&>(vector).end());
		}
		{
			auto it = vector.cbegin();
			LASS_TEST_CHECK_EQUAL((it++)->value(), 1);
			LASS_TEST_CHECK_EQUAL((it++)->value(), 2);
			LASS_TEST_CHECK(it == vector.cend());
		}
		{
			auto it = vector.rbegin();
			LASS_TEST_CHECK_EQUAL((it++)->value(), 2);
			LASS_TEST_CHECK_EQUAL((it++)->value(), 1);
			LASS_TEST_CHECK(it == vector.rend());
		}
		{
			auto it = static_cast<const vector_type&>(vector).rbegin();
			LASS_TEST_CHECK_EQUAL((it++)->value(), 2);
			LASS_TEST_CHECK_EQUAL((it++)->value(), 1);
			LASS_TEST_CHECK(it == static_cast<const vector_type&>(vector).rend());
		}
		{
			auto it = vector.crbegin();
			LASS_TEST_CHECK_EQUAL((it++)->value(), 2);
			LASS_TEST_CHECK_EQUAL((it++)->value(), 1);
			LASS_TEST_CHECK(it == vector.crend());
		}
	}
}

void testStdeStaticVectorSize()
{
	{
		// size etc ...
		vector_type vector;
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(0));
		LASS_TEST_CHECK_EQUAL(vector.max_size(), static_cast<size_t>(10));
		LASS_TEST_CHECK_EQUAL(vector.capacity(), static_cast<size_t>(10));
		LASS_TEST_CHECK(vector.empty());

		vector.resize(2, 37);
		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		vector.reserve(5);
		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(vector.max_size(), static_cast<size_t>(10));
		LASS_TEST_CHECK_EQUAL(vector.capacity(), static_cast<size_t>(10));
		LASS_TEST_CHECK(!vector.empty());
		for (size_t k = 0; k < 2; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), 37);
		}
		LASS_TEST_CHECK_THROW(vector.at(2), std::out_of_range);

		vector.resize(4, 42);
		LASS_TEST_CHECK_EQUAL(object::count(), 4);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(4));
		LASS_TEST_CHECK_EQUAL(vector.max_size(), static_cast<size_t>(10));
		LASS_TEST_CHECK_EQUAL(vector.capacity(), static_cast<size_t>(10));
		LASS_TEST_CHECK(!vector.empty());
		for (size_t k = 0; k < 4; ++k)
		{
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), k < 2 ? 37 : 42);
		}
		LASS_TEST_CHECK_THROW(vector.at(4), std::out_of_range);

		LASS_TEST_CHECK_THROW(vector.reserve(666), std::length_error);
		LASS_TEST_CHECK_THROW(vector.resize(666, 69), std::length_error);
	}
}

void testStdeStaticVectorAccess()
{
	{
		// operator[], at(), front() and back()
		vector_type vector({ 1, 2, 3});
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		const vector_type& const_vector = vector;
		for (size_t k = 0; k < 3; ++k)
		{
			const int value = static_cast<int>(k) + 1;
			LASS_TEST_CHECK_EQUAL(vector.at(k).value(), value);
			LASS_TEST_CHECK_EQUAL(const_vector.at(k).value(), value);
			LASS_TEST_CHECK_EQUAL(vector[k].value(), value);
			LASS_TEST_CHECK_EQUAL(const_vector[k].value(), value);
		}
		LASS_TEST_CHECK_EQUAL(vector.front().value(), 1);
		LASS_TEST_CHECK_EQUAL(const_vector.front().value(), 1);
		LASS_TEST_CHECK_EQUAL(vector.back().value(), 3);
		LASS_TEST_CHECK_EQUAL(const_vector.back().value(), 3);

		vector[0] = 4;
		vector.at(1) = 5;
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 4);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 5);
		LASS_TEST_CHECK_EQUAL(vector.at(2).value(), 3);
	}
}

void testStdeStaticVectorPushPop()
{
	{
		// push_back
		vector_type vector;
		vector.push_back(1);
		LASS_TEST_CHECK_EQUAL(object::count(), 1);
		{
			object b{ 2 };
			LASS_TEST_CHECK_EQUAL(object::count(), 2);
			vector.push_back(b);
			LASS_TEST_CHECK_EQUAL(object::count(), 3);
		}
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 1);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 2);

		vector.resize(vector.max_size());
		LASS_TEST_CHECK_THROW(vector.push_back(3), std::length_error);
	}

	{
		// push_back move
		ptr_vector_type vector;
		object_ptr a{ new object{ 1 } };
		object* aa = a.get();
		LASS_TEST_CHECK_EQUAL(object::count(), 1);
		vector.push_back(std::move(a));
		LASS_TEST_CHECK_EQUAL(object::count(), 1);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(vector.at(0).get(), aa);

		{
			object_ptr b{ new object{ 2 } };
			object* bb = b.get();
			LASS_TEST_CHECK_EQUAL(object::count(), 2);
			vector.push_back(std::move(b));
			LASS_TEST_CHECK_EQUAL(object::count(), 2);
			LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
			LASS_TEST_CHECK_EQUAL(vector.at(0).get(), aa);
			LASS_TEST_CHECK_EQUAL(vector.at(1).get(), bb);
		}

		for (int k = 2; k < 10; ++k)
		{
			object_ptr c{ new object{ k + 1 } };
			vector.push_back(std::move(c));
		}
		LASS_TEST_CHECK_EQUAL(object::count(), 10);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(10));

		object_ptr d{ new object{ 3 } };
		LASS_TEST_CHECK_THROW(vector.push_back(std::move(d)), std::length_error);
	}

	{
		// pop_back
		vector_type vector({1,2,3});
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(2).value(), 3);

		vector.pop_back();
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 2);

		vector.pop_back();
		vector.pop_back();
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(0));
		LASS_TEST_CHECK_EQUAL(object::count(), 0);

		// calling pop_back on empty container is considered UB
	}
}

void testStdeStaticVectorInsertEmplace()
{
	{
		// insert
		vector_type vector;

		const object a(1); // rvalue
		LASS_TEST_CHECK_EQUAL(object::count(), 1);
		vector_type::iterator i = vector.insert(vector.begin(), a);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		LASS_TEST_CHECK_EQUAL(i, vector.begin());
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 1);

		const object b(2); // rvalue
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		i = vector.insert(vector.begin(), b);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 4);
		LASS_TEST_CHECK_EQUAL(i, vector.begin());
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 2);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 1);
	
		const object c(3); // rvalue
		LASS_TEST_CHECK_EQUAL(object::count(), 5);
		i = vector.insert(vector.end(), c);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(object::count(), 6);
		LASS_TEST_CHECK_EQUAL(i, vector.begin() + 2);
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 2);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 1);
		LASS_TEST_CHECK_EQUAL(vector.at(2).value(), 3);
	
		const object d(4); // rvalue
		LASS_TEST_CHECK_EQUAL(object::count(), 7);
		i = vector.insert(vector.begin() + 1, d);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(4));
		LASS_TEST_CHECK_EQUAL(object::count(), 8);
		LASS_TEST_CHECK_EQUAL(i, vector.begin() + 1);
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 2);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 4);
		LASS_TEST_CHECK_EQUAL(vector.at(2).value(), 1);
		LASS_TEST_CHECK_EQUAL(vector.at(3).value(), 3);
	}

	{
		// insert move
		ptr_vector_type vector;

		object_ptr a{ new object(1) };
		object* aa = a.get();
		LASS_TEST_CHECK_EQUAL(object::count(), 1);
		ptr_vector_type::iterator i = vector.insert(vector.begin(), std::move(a));
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(object::count(), 1);
		LASS_TEST_CHECK_EQUAL(i, vector.begin());
		LASS_TEST_CHECK_EQUAL(vector.at(0).get(), aa);

		object_ptr b{ new object(2) };
		object* bb = b.get();
		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		i = vector.insert(vector.begin(), std::move(b));
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		LASS_TEST_CHECK_EQUAL(i, vector.begin());
		LASS_TEST_CHECK_EQUAL(vector.at(0).get(), bb);
		LASS_TEST_CHECK_EQUAL(vector.at(1).get(), aa);
	
		object_ptr c{ new object(3) };
		object* cc = c.get();
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		i = vector.insert(vector.end(), std::move(c));
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(i, vector.begin() + 2);
		LASS_TEST_CHECK_EQUAL(vector.at(0).get(), bb);
		LASS_TEST_CHECK_EQUAL(vector.at(1).get(), aa);
		LASS_TEST_CHECK_EQUAL(vector.at(2).get(), cc);
	
		object_ptr d{ new object(4) };
		object* dd = d.get();
		LASS_TEST_CHECK_EQUAL(object::count(), 4);
		i = vector.insert(vector.begin() + 1, std::move(d));
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(4));
		LASS_TEST_CHECK_EQUAL(object::count(), 4);
		LASS_TEST_CHECK_EQUAL(i, vector.begin() + 1);
		LASS_TEST_CHECK_EQUAL(vector.at(0).get(), bb);
		LASS_TEST_CHECK_EQUAL(vector.at(1).get(), dd);
		LASS_TEST_CHECK_EQUAL(vector.at(2).get(), aa);
		LASS_TEST_CHECK_EQUAL(vector.at(3).get(), cc);
	}

	{
		// insert n
		vector_type vector({1, 2});
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 2);

		const object a(3); // rvalue
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		auto i = vector.insert(vector.begin() + 1, 3, a);
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(5));
		LASS_TEST_CHECK_EQUAL(object::count(), 6);
		LASS_TEST_CHECK_EQUAL(i, vector.begin() + 1);
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 1);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(2).value(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(3).value(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(4).value(), 2);
	}

	{
		// insert iterators
		vector_type vector({ 1, 2 });
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 2);

		std::list<int> list({ 3, 4, 5 });

		auto i = vector.insert(vector.begin() + 1, list.begin(), list.end());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(5));
		LASS_TEST_CHECK_EQUAL(object::count(), 5);
		LASS_TEST_CHECK_EQUAL(i, vector.begin() + 1);
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 1);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(2).value(), 4);
		LASS_TEST_CHECK_EQUAL(vector.at(3).value(), 5);
		LASS_TEST_CHECK_EQUAL(vector.at(4).value(), 2);
	}

	{
		// insert initializer_list
		vector_type vector({ 1, 2 });
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 2);

		auto i = vector.insert(vector.begin() + 1, { 3, 4, 5 });
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(5));
		LASS_TEST_CHECK_EQUAL(object::count(), 5);
		LASS_TEST_CHECK_EQUAL(i, vector.begin() + 1);
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 1);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(2).value(), 4);
		LASS_TEST_CHECK_EQUAL(vector.at(3).value(), 5);
		LASS_TEST_CHECK_EQUAL(vector.at(4).value(), 2);
	}

	{
		// emplace and emplace_back

		string_vector_type vector;
		auto& a = vector.emplace_back(static_cast<size_t>(3), 'a');
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(vector.at(0), std::string{ "aaa" });
		LASS_TEST_CHECK_EQUAL(&a, &vector.at(0));

		auto& b = vector.emplace_back(static_cast<size_t>(2), 'b');
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(vector.at(0), std::string{ "aaa" });
		LASS_TEST_CHECK_EQUAL(vector.at(1), std::string{ "bb" });
		LASS_TEST_CHECK_EQUAL(&b, &vector.at(1));

		auto c = vector.emplace(vector.begin(), static_cast<size_t>(4), 'c');
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(3));
		LASS_TEST_CHECK_EQUAL(vector.at(0), std::string{ "cccc" });
		LASS_TEST_CHECK_EQUAL(vector.at(1), std::string{ "aaa" });
		LASS_TEST_CHECK_EQUAL(vector.at(2), std::string{ "bb" });
		LASS_TEST_CHECK_EQUAL(c, vector.begin());

		auto d = vector.emplace(vector.end(), static_cast<size_t>(3), 'd');
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(4));
		LASS_TEST_CHECK_EQUAL(vector.at(0), std::string{ "cccc" });
		LASS_TEST_CHECK_EQUAL(vector.at(1), std::string{ "aaa" });
		LASS_TEST_CHECK_EQUAL(vector.at(2), std::string{ "bb" });
		LASS_TEST_CHECK_EQUAL(vector.at(3), std::string{ "ddd" });
		LASS_TEST_CHECK_EQUAL(d, vector.end() - 1);

		auto e = vector.emplace(vector.begin() + 2, static_cast<size_t>(4), 'e');
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(5));
		LASS_TEST_CHECK_EQUAL(vector.at(0), std::string{ "cccc" });
		LASS_TEST_CHECK_EQUAL(vector.at(1), std::string{ "aaa" });
		LASS_TEST_CHECK_EQUAL(vector.at(2), std::string{ "eeee" });
		LASS_TEST_CHECK_EQUAL(vector.at(3), std::string{ "bb" });
		LASS_TEST_CHECK_EQUAL(vector.at(4), std::string{ "ddd" });
		LASS_TEST_CHECK_EQUAL(e, vector.begin() + 2);
	}
}

void testStdeStaticVectorEraseClear()
{
	{
		// erase

		vector_type vector{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(10));
		LASS_TEST_CHECK_EQUAL(object::count(), 10);

		auto i = vector.erase(vector.begin() + 2);
		// 0, 1, 3, 4, 5, 6, 7, 8, 9
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(9));
		LASS_TEST_CHECK_EQUAL(object::count(), 9);
		LASS_TEST_CHECK_EQUAL(i, vector.begin() + 2);
		LASS_TEST_CHECK_EQUAL(i->value(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 0);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 1);
		LASS_TEST_CHECK_EQUAL(vector.at(2).value(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(3).value(), 4);
		LASS_TEST_CHECK_EQUAL(vector.at(8).value(), 9);

		i = vector.erase(vector.begin() + 4, vector.begin() + 6);
		// 0, 1, 3, 4, 7, 8, 9
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(7));
		LASS_TEST_CHECK_EQUAL(object::count(), 7);
		LASS_TEST_CHECK_EQUAL(i, vector.begin() + 4);
		LASS_TEST_CHECK_EQUAL(i->value(), 7);
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 0);
		LASS_TEST_CHECK_EQUAL(vector.at(3).value(), 4);
		LASS_TEST_CHECK_EQUAL(vector.at(4).value(), 7);
		LASS_TEST_CHECK_EQUAL(vector.at(5).value(), 8);
		LASS_TEST_CHECK_EQUAL(vector.at(6).value(), 9);

		i = vector.erase(vector.begin() + 4, vector.end());
		// 0, 1, 3, 4
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(4));
		LASS_TEST_CHECK_EQUAL(object::count(), 4);
		LASS_TEST_CHECK_EQUAL(i, vector.end());
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 0);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 1);
		LASS_TEST_CHECK_EQUAL(vector.at(2).value(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(3).value(), 4);

		i = vector.erase(vector.begin(), vector.begin() + 2);
		// 3, 4
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(object::count(), 2);
		LASS_TEST_CHECK_EQUAL(i, vector.begin());
		LASS_TEST_CHECK_EQUAL(i->value(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(0).value(), 3);
		LASS_TEST_CHECK_EQUAL(vector.at(1).value(), 4);

		i = vector.erase(vector.begin(), vector.end());
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(0));
		LASS_TEST_CHECK_EQUAL(object::count(), 0);
		LASS_TEST_CHECK_EQUAL(i, vector.end());
	}

	{
		// clear

		vector_type vector{ { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(10));
		LASS_TEST_CHECK_EQUAL(object::count(), 10);
		vector.clear();
		LASS_TEST_CHECK_EQUAL(vector.size(), static_cast<size_t>(0));
		LASS_TEST_CHECK_EQUAL(object::count(), 0);
	}
}

void testStdeStaticVectorSwap()
{
	{
		// swap & std::swap
		object_ptr a(new object{ 1 });
		object_ptr b(new object{ 2 });
		object_ptr c(new object{ 3 });
		object* aa = a.get();
		object* bb = b.get();
		object* cc = c.get();
		LASS_TEST_CHECK_EQUAL(object::count(), 3);

		ptr_vector_type left;
		left.push_back(std::move(a));
		left.push_back(std::move(b));
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(left.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(left.at(0).get(), aa);
		LASS_TEST_CHECK_EQUAL(left.at(1).get(), bb);

		ptr_vector_type right;
		right.push_back(std::move(c));
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(right.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(right.at(0).get(), cc);

		left.swap(right);
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(left.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(left.at(0).get(), cc);
		LASS_TEST_CHECK_EQUAL(right.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(right.at(0).get(), aa);
		LASS_TEST_CHECK_EQUAL(right.at(1).get(), bb);

		left.swap(right);
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(left.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(left.at(0).get(), aa);
		LASS_TEST_CHECK_EQUAL(left.at(1).get(), bb);
		LASS_TEST_CHECK_EQUAL(right.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(right.at(0).get(), cc);

		std::swap(left, right);
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(left.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(left.at(0).get(), cc);
		LASS_TEST_CHECK_EQUAL(right.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(right.at(0).get(), aa);
		LASS_TEST_CHECK_EQUAL(right.at(1).get(), bb);

		std::swap(left, right);
		LASS_TEST_CHECK_EQUAL(object::count(), 3);
		LASS_TEST_CHECK_EQUAL(left.size(), static_cast<size_t>(2));
		LASS_TEST_CHECK_EQUAL(left.at(0).get(), aa);
		LASS_TEST_CHECK_EQUAL(left.at(1).get(), bb);
		LASS_TEST_CHECK_EQUAL(right.size(), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(right.at(0).get(), cc);
	}
}

TUnitTest test_stde_static_vector()
{
	return TUnitTest({
		LASS_TEST_CASE(testStdeStaticVectorConstructors),
		LASS_TEST_CASE(testStdeStaticVectorAssignment),
		LASS_TEST_CASE(testStdeStaticVectorIterators),
		LASS_TEST_CASE(testStdeStaticVectorSize),
		LASS_TEST_CASE(testStdeStaticVectorAccess),
		LASS_TEST_CASE(testStdeStaticVectorPushPop),
		LASS_TEST_CASE(testStdeStaticVectorInsertEmplace),
		LASS_TEST_CASE(testStdeStaticVectorEraseClear),
		LASS_TEST_CASE(testStdeStaticVectorSwap),
		});
}

}

}

// EOF
