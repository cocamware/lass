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
 *	Copyright (C) 2004-2022 the Initial Developer.
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
#include "../lass/stde/vector_map.h"

namespace
{
	std::unique_ptr<std::string> make_str(const std::string& s)
	{
		return std::make_unique<std::string>(s);
	}

	const std::string DUMMY = "[dummy]"; // dummy to avoid dereference crashes
	const std::string& deref(const std::unique_ptr<std::string>& p)
	{
		return p ? *p : DUMMY;
	}

}

namespace lass
{
namespace test
{

void testStdeVectorMap()
{
	using TVectorMap = stde::vector_map<std::string, std::string>;
	using size_type = TVectorMap::size_type;

	const std::string foo{ "foo" };
	const std::string FOO{ "FOO" };
	const TVectorMap::value_type foo_FOO{ foo, FOO };
	const std::string bar{ "bar" };
	const std::string BAR{ "BAR" };
	const TVectorMap::value_type bar_BAR{ bar, BAR };
	const std::string baz = "baz";
	const std::string BAZ = "BAZ";
	const TVectorMap::value_type baz_BAZ{ baz, BAZ };
	const TVectorMap::value_type spam_SPAM("spam", "SPAM");
	const TVectorMap::value_type ham_HAM("ham", "HAM");

	{
		// default constructor
		TVectorMap map;
		LASS_ASSERT(map.empty());
		LASS_TEST_CHECK_EQUAL(map.size(), static_cast<size_type>(0));
	}

	
	{
		// input iterators
		std::vector<TVectorMap::value_type> values{ { foo_FOO, bar_BAR } };
		TVectorMap map{ values.begin(), values.end() };
		LASS_ASSERT(!map.empty());
		LASS_TEST_CHECK_EQUAL(map.size(), static_cast<size_type>(2));
		LASS_TEST_CHECK_EQUAL(map[foo], FOO);
		LASS_TEST_CHECK_EQUAL(map[bar], BAR);
		LASS_TEST_CHECK_EQUAL(map.size(), static_cast<size_type>(2));
	}

	{
		// initializer list
		stde::vector_map<std::string, std::string> map{ { foo_FOO, bar_BAR } };
		LASS_ASSERT(!map.empty());
		LASS_TEST_CHECK_EQUAL(map.size(), static_cast<size_type>(2));
		LASS_TEST_CHECK_EQUAL(map[foo], FOO);
		LASS_TEST_CHECK_EQUAL(map[bar], BAR);
		LASS_TEST_CHECK_EQUAL(map.size(), static_cast<size_type>(2));
	}

	{
		// copy constructor
		const TVectorMap map{ { foo_FOO, bar_BAR } };
		TVectorMap copied{ map };
		LASS_TEST_CHECK_EQUAL(map.size(), static_cast<size_type>(2));
		LASS_TEST_CHECK_EQUAL(map.at(foo), FOO);
		LASS_TEST_CHECK_EQUAL(map.at(bar), BAR);
		LASS_TEST_CHECK_EQUAL(copied.size(), static_cast<size_type>(2));
		LASS_TEST_CHECK_EQUAL(copied[foo], FOO);
		LASS_TEST_CHECK_EQUAL(copied[bar], BAR);
	}

	{
		// move constructor 
		stde::vector_map<std::string, std::unique_ptr<std::string>> map;
		map.emplace(foo, make_str(FOO));
		map.emplace(bar, make_str(BAR));
		stde::vector_map<std::string, std::unique_ptr<std::string>> moved{ std::move(map) };
		LASS_ASSERT(map.empty());
		LASS_TEST_CHECK_EQUAL(moved.size(), static_cast<size_type>(2));
		LASS_TEST_CHECK_EQUAL(deref(moved[foo]), FOO);
		LASS_TEST_CHECK_EQUAL(deref(moved[bar]), BAR);
	}

	{
		// copy assignment
		const TVectorMap map{ { foo_FOO } };
		TVectorMap copied{ { {bar, BAR} } };
		LASS_TEST_CHECK_EQUAL(copied[bar], BAR);
		LASS_TEST_CHECK_EQUAL(copied.size(), static_cast<size_type>(1));
		copied = map;
		LASS_TEST_CHECK_EQUAL(map.size(), static_cast<size_type>(1));
		LASS_TEST_CHECK_EQUAL(map.at(foo), FOO);
		LASS_TEST_CHECK_EQUAL(copied.size(), static_cast<size_type>(1));
		LASS_TEST_CHECK_EQUAL(copied[foo], FOO);
	}

	{
		// move assignment
		stde::vector_map<std::string, std::unique_ptr<std::string>> map;
		map.emplace(foo, make_str(FOO));
		stde::vector_map<std::string, std::unique_ptr<std::string>> moved;
		moved.emplace(bar, make_str(BAR));
		LASS_TEST_CHECK_EQUAL(moved.size(), static_cast<size_type>(1));
		LASS_TEST_CHECK_EQUAL(deref(moved[bar]), BAR);
		moved = std::move(map);
		LASS_ASSERT(map.empty());
		LASS_TEST_CHECK_EQUAL(moved.size(), static_cast<size_type>(1));
		LASS_TEST_CHECK_EQUAL(deref(moved[foo]), FOO);
	}

	{
		// iterators empty map
		TVectorMap map;
		LASS_TEST_CHECK(map.begin() == map.end());
		LASS_TEST_CHECK(static_cast<const TVectorMap&>(map).begin() == static_cast<const TVectorMap&>(map).end());
		LASS_TEST_CHECK(map.cbegin() == map.cend());
		LASS_TEST_CHECK(map.rbegin() == map.rend());
		LASS_TEST_CHECK(static_cast<const TVectorMap&>(map).rbegin() == static_cast<const TVectorMap&>(map).rend());
		LASS_TEST_CHECK(map.crbegin() == map.crend());
	}

	{
		// iterators non-empty map
		TVectorMap map{ { foo_FOO, bar_BAR } };
		LASS_TEST_CHECK(map.begin() != map.end());
		LASS_TEST_CHECK(static_cast<const TVectorMap&>(map).begin() != static_cast<const TVectorMap&>(map).end());
		LASS_TEST_CHECK(map.cbegin() != map.cend());
		LASS_TEST_CHECK(map.rbegin() != map.rend());
		LASS_TEST_CHECK(static_cast<const TVectorMap&>(map).rbegin() != static_cast<const TVectorMap&>(map).rend());
		LASS_TEST_CHECK(map.crbegin() != map.crend());
		{
			auto it = map.begin();
			LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
			LASS_TEST_CHECK_EQUAL(*it++, foo_FOO);
			LASS_TEST_CHECK(it == map.end());
		}
		{
			auto it = static_cast<const TVectorMap&>(map).begin();
			LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
			LASS_TEST_CHECK_EQUAL(*it++, foo_FOO);
			LASS_TEST_CHECK(it == static_cast<const TVectorMap&>(map).end());
		}
		{
			auto it = map.cbegin();
			LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
			LASS_TEST_CHECK_EQUAL(*it++, foo_FOO);
			LASS_TEST_CHECK(it == map.cend());
		}
		{
			auto it = map.rbegin();
			LASS_TEST_CHECK_EQUAL(*it++, foo_FOO);
			LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
			LASS_TEST_CHECK(it == map.rend());
		}
		{
			auto it = static_cast<const TVectorMap&>(map).rbegin();
			LASS_TEST_CHECK_EQUAL(*it++, foo_FOO);
			LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
			LASS_TEST_CHECK(it == static_cast<const TVectorMap&>(map).rend());
		}
		{
			auto it = map.crbegin();
			LASS_TEST_CHECK_EQUAL(*it++, foo_FOO);
			LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
			LASS_TEST_CHECK(it == map.crend());
		}
	}

	{
		// at and operator[]
		TVectorMap map{ { foo_FOO } };
		LASS_TEST_CHECK_EQUAL(map.at(foo), FOO);
		LASS_TEST_CHECK_EQUAL(static_cast<const TVectorMap&>(map).at(foo), FOO);
		LASS_TEST_CHECK_THROW(map.at(bar), std::out_of_range);
		LASS_TEST_CHECK_THROW(static_cast<const TVectorMap&>(map).at(bar), std::out_of_range);
		LASS_TEST_CHECK_EQUAL(map[foo], FOO);
		LASS_TEST_CHECK_EQUAL(map[bar], std::string());
		map[bar] = BAR;
		map["baz"] = "BAZ";
		LASS_TEST_CHECK_EQUAL(map[bar], BAR);
		LASS_TEST_CHECK_EQUAL(map["baz"], "BAZ");
	}

	{
		// insert lvalue
		TVectorMap map;
		auto ret = map.insert(foo_FOO);
		LASS_TEST_CHECK_EQUAL(*ret.first, foo_FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, true);
		const TVectorMap::value_type foo_BAR{ foo, BAR };
		LASS_TEST_CHECK(foo_FOO != foo_BAR);
		ret = map.insert(foo_BAR);
		LASS_TEST_CHECK_EQUAL(*ret.first, foo_FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, false);
	}

	{
		// insert rvalue
		stde::vector_map<std::string, std::unique_ptr<std::string>> map;
		auto ret = map.insert(std::make_pair(foo, make_str(FOO)));
		LASS_TEST_CHECK_EQUAL(ret.first->first, foo);
		LASS_TEST_CHECK_EQUAL(*ret.first->second, FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, true);
		ret = map.insert(std::make_pair(foo, make_str(BAR)));
		LASS_TEST_CHECK_EQUAL(ret.first->first, foo);
		LASS_TEST_CHECK_EQUAL(*ret.first->second, FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, false);
	}

	{
		// insert with hint
		TVectorMap map{ { foo_FOO } };

		// an insert with the right hint
		auto it = map.insert(map.begin(), bar_BAR);
		LASS_TEST_CHECK_EQUAL(*it, std::make_pair(bar, BAR));
		LASS_TEST_CHECK(it == map.begin()); // should be inserted in first place
		LASS_TEST_CHECK_EQUAL(*std::next(it), std::make_pair(foo, FOO));

		// an insert with the wrong hint
		it = map.insert(map.end(), baz_BAZ);
		LASS_TEST_CHECK_EQUAL(*it, baz_BAZ);
		LASS_TEST_CHECK(it == map.begin() + 1);
		LASS_TEST_CHECK_EQUAL(*std::prev(it), bar_BAR);
		LASS_TEST_CHECK_EQUAL(*std::next(it), foo_FOO);
	}

	{
		// insert first to last
		TVectorMap map{ { foo_FOO, bar_BAR } };
		std::vector<TVectorMap::value_type> values{ baz_BAZ, spam_SPAM, ham_HAM };

		map.insert(values.begin(), values.end());
		auto it = map.begin();
		LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
		LASS_TEST_CHECK_EQUAL(*it++, baz_BAZ);
		LASS_TEST_CHECK_EQUAL(*it++, foo_FOO);
		LASS_TEST_CHECK_EQUAL(*it++, ham_HAM);
		LASS_TEST_CHECK_EQUAL(*it++, spam_SPAM);
		LASS_TEST_CHECK(it == map.end());
	}

	{
		// emplace
		TVectorMap map;
		auto ret = map.emplace(foo, FOO);
		LASS_TEST_CHECK_EQUAL(ret.first->first, foo);
		LASS_TEST_CHECK_EQUAL(ret.first->second, FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, true);
		ret = map.emplace(foo, BAR);
		LASS_TEST_CHECK_EQUAL(ret.first->first, foo);
		LASS_TEST_CHECK_EQUAL(ret.first->second, FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, false);
	}

	{
		// emplace too
		stde::vector_map<std::string, std::unique_ptr<std::string>> map;
		auto ret = map.emplace(foo, make_str(FOO));
		LASS_TEST_CHECK_EQUAL(ret.first->first, foo);
		LASS_TEST_CHECK_EQUAL(*ret.first->second, FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, true);
		ret = map.emplace(foo, make_str(BAR));
		LASS_TEST_CHECK_EQUAL(ret.first->first, foo);
		LASS_TEST_CHECK_EQUAL(*ret.first->second, FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, false);
	}

	{
		// emplace_hint
		stde::vector_map<std::string, std::unique_ptr<std::string>> map;
		map.emplace(foo, make_str(FOO));

		// an emplace with the right hint
		auto it = map.emplace_hint(map.begin(), bar, make_str(BAR));
		LASS_TEST_CHECK_EQUAL(it->first, bar);
		LASS_TEST_CHECK_EQUAL(*it->second, BAR);
		LASS_TEST_CHECK(it == map.begin()); // should be inserted in first place
		LASS_TEST_CHECK_EQUAL(std::next(it)->first, foo);

		// an emplace with the wrong hint
		it = map.emplace_hint(map.end(), baz, make_str(BAZ));
		LASS_TEST_CHECK_EQUAL(it->first, baz);
		LASS_TEST_CHECK_EQUAL(*it->second, BAZ);
		LASS_TEST_CHECK(it == map.begin() + 1);
		LASS_TEST_CHECK_EQUAL(std::prev(it)->first, bar);
		LASS_TEST_CHECK_EQUAL(std::next(it)->first, foo);
	}

	{
		// try_emplace with lvalue as key
		stde::vector_map<std::string, std::unique_ptr<std::string>> map;
		auto ret = map.try_emplace(foo, make_str(FOO));
		LASS_TEST_CHECK_EQUAL(ret.first->first, foo);
		LASS_TEST_CHECK_EQUAL(*ret.first->second, FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, true);
		ret = map.try_emplace(foo, make_str(BAR));
		LASS_TEST_CHECK_EQUAL(ret.first->first, foo);
		LASS_TEST_CHECK_EQUAL(*ret.first->second, FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, false);
	}

	{
		// try_emplace with rvalue as key
		stde::vector_map<std::string, std::unique_ptr<std::string>> map;
		auto ret = map.try_emplace("foo", make_str(FOO));
		LASS_TEST_CHECK_EQUAL(ret.first->first, foo);
		LASS_TEST_CHECK_EQUAL(*ret.first->second, FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, true);
		ret = map.try_emplace("foo", make_str(BAR));
		LASS_TEST_CHECK_EQUAL(ret.first->first, foo);
		LASS_TEST_CHECK_EQUAL(*ret.first->second, FOO);
		LASS_TEST_CHECK_EQUAL(ret.second, false);
	}

	{
		// try_emplace with hint and lvalue as key
		stde::vector_map<std::string, std::unique_ptr<std::string>> map;
		map.emplace(foo, make_str(FOO));

		// an emplace with the right hint
		auto it = map.try_emplace(map.begin(), bar, make_str(BAR));
		LASS_TEST_CHECK_EQUAL(it->first, bar);
		LASS_TEST_CHECK_EQUAL(*it->second, BAR);
		LASS_TEST_CHECK(it == map.begin()); // should be inserted in first place
		LASS_TEST_CHECK_EQUAL(std::next(it)->first, foo);

		// an emplace with the wrong hint
		it = map.try_emplace(map.end(), baz, make_str(BAZ));
		LASS_TEST_CHECK_EQUAL(it->first, baz);
		LASS_TEST_CHECK_EQUAL(*it->second, BAZ);
		LASS_TEST_CHECK(it == map.begin() + 1);
		LASS_TEST_CHECK_EQUAL(std::prev(it)->first, bar);
		LASS_TEST_CHECK_EQUAL(std::next(it)->first, foo);
	}

	{
		// try_emplace with hint and rvalue as key
		stde::vector_map<std::string, std::unique_ptr<std::string>> map;
		map.emplace(foo, make_str(FOO));

		// an emplace with the right hint
		auto it = map.try_emplace(map.begin(), "bar", make_str(BAR));
		LASS_TEST_CHECK_EQUAL(it->first, bar);
		LASS_TEST_CHECK_EQUAL(*it->second, BAR);
		LASS_TEST_CHECK(it == map.begin()); // should be inserted in first place
		LASS_TEST_CHECK_EQUAL(std::next(it)->first, foo);

		// an emplace with the wrong hint
		it = map.try_emplace(map.end(), "baz", make_str(BAZ));
		LASS_TEST_CHECK_EQUAL(it->first, baz);
		LASS_TEST_CHECK_EQUAL(*it->second, BAZ);
		LASS_TEST_CHECK(it == map.begin() + 1);
		LASS_TEST_CHECK_EQUAL(std::prev(it)->first, bar);
		LASS_TEST_CHECK_EQUAL(std::next(it)->first, foo);
	}

	{
		// erase by iterator
		TVectorMap map{ { foo_FOO, bar_BAR, baz_BAZ } };
		map.erase(stde::next(map.begin()));
		auto it = map.begin();
		LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
		LASS_TEST_CHECK_EQUAL(*it++, foo_FOO);
		LASS_TEST_CHECK(it == map.end());
	}

	{
		// erase by key
		TVectorMap map{ { foo_FOO, bar_BAR, baz_BAZ } };
		LASS_TEST_CHECK_EQUAL(map.erase(baz), static_cast<size_t>(1));
		auto it = map.begin();
		LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
		LASS_TEST_CHECK_EQUAL(*it++, foo_FOO);
		LASS_TEST_CHECK(it == map.end());

		LASS_TEST_CHECK_EQUAL(map.erase(baz), static_cast<size_t>(0));
		it = map.begin();
		LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
		LASS_TEST_CHECK_EQUAL(*it++, foo_FOO);
		LASS_TEST_CHECK(it == map.end());
	}

	{
		// erase first to last
		TVectorMap map{ { foo_FOO, bar_BAR, baz_BAZ } };
		map.erase(stde::next(map.begin()), map.end());
		auto it = map.begin();
		LASS_TEST_CHECK_EQUAL(*it++, bar_BAR);
		LASS_TEST_CHECK(it == map.end());
	}

	{
		// swap
		TVectorMap map1{ { foo_FOO, bar_BAR } };
		TVectorMap map2{ { spam_SPAM, ham_HAM } };
		map1.swap(map2);
		auto it1 = map1.begin();
		LASS_TEST_CHECK_EQUAL(*it1++, ham_HAM);
		LASS_TEST_CHECK_EQUAL(*it1++, spam_SPAM);
		LASS_TEST_CHECK(it1 == map1.end());
		auto it2 = map2.begin();
		LASS_TEST_CHECK_EQUAL(*it2++, bar_BAR);
		LASS_TEST_CHECK_EQUAL(*it2++, foo_FOO);
		LASS_TEST_CHECK(it2 == map2.end());
	}

	{
		// swap 2
		stde::vector_map<std::string, std::unique_ptr<std::string>> map1, map2;
		map1.emplace(foo, make_str(FOO));
		map2.emplace(bar, make_str(BAR));
		std::swap(map1, map2);
		LASS_TEST_CHECK_EQUAL(*map1[bar], BAR);
		LASS_TEST_CHECK_EQUAL(*map2[foo], FOO);
	}

	{
		TVectorMap map;
		LASS_TEST_CHECK(map.key_comp()(bar, foo));
		LASS_TEST_CHECK(!map.key_comp()(bar, bar));
		LASS_TEST_CHECK(!map.key_comp()(foo, bar));
		LASS_TEST_CHECK(map.value_comp()(bar_BAR, foo_FOO));
		LASS_TEST_CHECK(!map.value_comp()(bar_BAR, bar_BAR));
		LASS_TEST_CHECK(!map.value_comp()(foo_FOO, bar_BAR));
	}

	{
		// find, count, contains
		TVectorMap map{ { foo_FOO, bar_BAR } };
		{
			auto it = map.find(foo);
			LASS_TEST_CHECK(it != map.end());
			LASS_TEST_CHECK_EQUAL(*it, foo_FOO);
		}
		{
			auto it = static_cast<const TVectorMap&>(map).find(foo);
			LASS_TEST_CHECK(it != static_cast<const TVectorMap&>(map).end());
			LASS_TEST_CHECK_EQUAL(*it, foo_FOO);
		}
		LASS_TEST_CHECK(map.find(baz) == map.end());
		LASS_TEST_CHECK_EQUAL(map.count(foo), static_cast<size_t>(1));
		LASS_TEST_CHECK_EQUAL(map.count(baz), static_cast<size_t>(0));
		LASS_TEST_CHECK(map.contains(foo));
		LASS_TEST_CHECK(!map.contains(baz));
	}

	{
		// lower_bound
		TVectorMap map{ { foo_FOO, baz_BAZ } };
		LASS_TEST_CHECK(map.lower_bound(bar) == map.begin());
		LASS_TEST_CHECK(map.lower_bound(baz) == map.begin());
		LASS_TEST_CHECK(map.lower_bound("spam") == map.end());
		LASS_TEST_CHECK(map.lower_bound(foo) == stde::prev(map.end()));
	}

	{
		// lower_bound const
		const TVectorMap map{ { foo_FOO, baz_BAZ } };
		LASS_TEST_CHECK(map.lower_bound(bar) == map.begin());
		LASS_TEST_CHECK(map.lower_bound(baz) == map.begin());
		LASS_TEST_CHECK(map.lower_bound("spam") == map.end());
		LASS_TEST_CHECK(map.lower_bound(foo) == stde::prev(map.end()));
	}

	{
		// upper_bound
		TVectorMap map{ { foo_FOO, baz_BAZ } };
		LASS_TEST_CHECK(map.upper_bound(bar) == map.begin());
		LASS_TEST_CHECK(map.upper_bound(baz) == stde::next(map.begin()));
		LASS_TEST_CHECK(map.upper_bound("spam") == map.end());
		LASS_TEST_CHECK(map.upper_bound(foo) == map.end());
	}

	{
		// upper_bound const
		const TVectorMap map{ { foo_FOO, baz_BAZ } };
		LASS_TEST_CHECK(map.upper_bound(bar) == map.begin());
		LASS_TEST_CHECK(map.upper_bound(baz) == stde::next(map.begin()));
		LASS_TEST_CHECK(map.upper_bound("spam") == map.end());
		LASS_TEST_CHECK(map.upper_bound(foo) == map.end());
	}

	{
		// equal_range
		TVectorMap map{ { foo_FOO, baz_BAZ } };
		LASS_TEST_CHECK(map.equal_range(bar) == std::make_pair(map.begin(), map.begin()));
		LASS_TEST_CHECK(map.equal_range(baz) == std::make_pair(map.begin(), stde::next(map.begin())));
		LASS_TEST_CHECK(map.equal_range("spam") == std::make_pair(map.end(), map.end()));
		LASS_TEST_CHECK(map.equal_range(foo) == std::make_pair(stde::prev(map.end()), map.end()));
	}

	{
		// equal_range const
		const TVectorMap map{ { foo_FOO, baz_BAZ } };
		LASS_TEST_CHECK(map.equal_range(bar) == std::make_pair(map.begin(), map.begin()));
		LASS_TEST_CHECK(map.equal_range(baz) == std::make_pair(map.begin(), stde::next(map.begin())));
		LASS_TEST_CHECK(map.equal_range("spam") == std::make_pair(map.end(), map.end()));
		LASS_TEST_CHECK(map.equal_range(foo) == std::make_pair(stde::prev(map.end()), map.end()));
	}
}



TUnitTest test_stde_vector_map()
{
	return TUnitTest({
		LASS_TEST_CASE(testStdeVectorMap),
		});
}


}

}

// EOF
