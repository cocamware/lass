/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_EXTENDED_IO_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_EXTENDED_IO_INL

#include "test_common.h"
#include "../stde/extended_io.h"
#include "../util/string_cast.h"

namespace lass
{
namespace test
{
namespace extended_io
{

template <typename Container1, typename Container2>
bool safe_equal(const Container1& iA, const Container2& iB)
{
	if (iA.size() != iB.size())
	{
		return false;
	}
	return std::equal(iA.begin(), iA.end(), iB.begin());
}

}



void testStdeExtendedIo()
{
	TestStream stream;
	std::stringstream buffer;

	typedef std::pair<int, std::string> TPair;
	TPair pair(1, "spam & ham");
	const std::string& pairString = "(1, spam & ham)";
	stream << pair;
	LASS_TEST_CHECK(stream.isEqual(pairString));
	LASS_TEST_CHECK_EQUAL(util::stringCast<TPair>(pairString), pair);

	const std::string& sequencePattern = "[foo, bar, spam & ham]";

	typedef std::vector<std::string> TVector;
	TVector vector;
	vector.push_back("foo");
	vector.push_back("bar");
	vector.push_back("spam & ham");
	stream << vector;
	LASS_TEST_CHECK(stream.isEqual(sequencePattern));
	TVector vector2;
	//LASS_TEST_CHECK_NO_THROW(vector2 = util::stringCast<TVector>(sequencePattern));
		do 
	{
		try 
		{ 
			vector2 = util::stringCast<TVector>(sequencePattern); 
		} 
		catch (const ::lass::util::Exception& error)
		{ 
			++::lass::test::impl::errors(); 
			(lass::io::cout()) << "d:\\bram\\visual_studio_projects\\lass\\src\\test\\test_stde_extended_io.inl" << "(" << 79 << "): " << "exception was thrown by '" "vector2 = util::stringCast<TVector>(sequencePattern)" "'" << ": " << "::lass::util::Exception" << " (" << error.what() << ")." << std::endl; 
			::lass::test::impl::errorLog() << "d:\\bram\\visual_studio_projects\\lass\\src\\test\\test_stde_extended_io.inl" << "(" << 79 << "): " << "exception was thrown by '" "vector2 = util::stringCast<TVector>(sequencePattern)" "'" << ": " << "::lass::util::Exception" << " (" << error.what() << ")." << std::endl; 
		} 
		catch (const ::std::exception& error) 
		{ 
			++::lass::test::impl::errors(); 
			(lass::io::cout()) << "d:\\bram\\visual_studio_projects\\lass\\src\\test\\test_stde_extended_io.inl" << "(" << 79 << "): " << "exception was thrown by '" "vector2 = util::stringCast<TVector>(sequencePattern)" "'" << ": " << "::std::exception" << " (" << error.what() << ")." << std::endl; 
			::lass::test::impl::errorLog() << "d:\\bram\\visual_studio_projects\\lass\\src\\test\\test_stde_extended_io.inl" << "(" << 79 << "): " << "exception was thrown by '" "vector2 = util::stringCast<TVector>(sequencePattern)" "'" << ": " << "::std::exception" << " (" << error.what() << ")." << std::endl; 
		} 
		catch (...) 
		{ 
			++::lass::test::impl::errors(); 
			(lass::io::cout()) << "d:\\bram\\visual_studio_projects\\lass\\src\\test\\test_stde_extended_io.inl" << "(" << 79 << "): " << "exception was thrown by '" "vector2 = util::stringCast<TVector>(sequencePattern)" "'" << ": unknown exception." << std::endl; 
			::lass::test::impl::errorLog() << "d:\\bram\\visual_studio_projects\\lass\\src\\test\\test_stde_extended_io.inl" << "(" << 79 << "): " << "exception was thrown by '" "vector2 = util::stringCast<TVector>(sequencePattern)" "'" << ": unknown exception." << std::endl; 
		} 
	} 
	while (false);
	LASS_TEST_CHECK(extended_io::safe_equal(vector, vector2));

	typedef std::list<std::string> TList;
	TList list;
	list.push_back("foo");
	list.push_back("bar");
	list.push_back("spam & ham");
	stream << list;
	LASS_TEST_CHECK(stream.isEqual(sequencePattern));
	TList list2;
	LASS_TEST_CHECK_NO_THROW(list2 = util::stringCast<TList>(sequencePattern));
	LASS_TEST_CHECK(extended_io::safe_equal(list, list2));

	typedef std::deque<std::string> TDeque;
	TDeque deque;
	deque.push_back("foo");
	deque.push_back("bar");
	deque.push_back("spam & ham");
	stream << deque;
	LASS_TEST_CHECK(stream.isEqual(sequencePattern));
	TDeque deque2;
	LASS_TEST_CHECK_NO_THROW(deque2 = util::stringCast<TDeque>(sequencePattern));
	LASS_TEST_CHECK(extended_io::safe_equal(deque, deque2));

	const std::string mapPattern = "{bar: 2, foo: 1, spam: 3}"; // alphabetic!
	typedef std::map<std::string, int> TMap;
	TMap map;
	map["foo"] = 1;
	map["bar"] = 2;
	map["spam"] = 3;
	stream << map;
	LASS_TEST_CHECK(stream.isEqual(mapPattern));
	TMap map2;
	LASS_TEST_CHECK_NO_THROW(map2 = util::stringCast<TMap>(mapPattern));
	LASS_TEST_CHECK(extended_io::safe_equal(map, map2));

	const std::string multimapPattern = "{bar: 2, foo: 1, foo: 3}";
	typedef std::multimap<std::string, int> TMultimap;
	TMultimap multimap;
	multimap.insert(std::pair<std::string, int>("foo", 1));
	multimap.insert(std::pair<std::string, int>("bar", 2));
	multimap.insert(std::pair<std::string, int>("foo", 3));
	stream << multimap;
	LASS_TEST_CHECK(stream.isEqual(multimapPattern)); // alphabetic!
	TMultimap multimap2;
	LASS_TEST_CHECK_NO_THROW(multimap2 = util::stringCast<TMultimap>(multimapPattern));
	LASS_TEST_CHECK(extended_io::safe_equal(multimap, multimap2));

	const std::string setPattern = "{bar, foo, spam}";
	typedef std::set<std::string> TSet;
	TSet set;
	set.insert("foo");
	set.insert("bar");
	set.insert("spam");
	stream << set;
	LASS_TEST_CHECK(stream.isEqual(setPattern)); // alphabetic!
	TSet set2;
	LASS_TEST_CHECK_NO_THROW(set2 = util::stringCast<TSet>(setPattern));
	LASS_TEST_CHECK(extended_io::safe_equal(set, set2));

	const std::string multisetPattern = "{bar, foo, foo}";
	typedef std::multiset<std::string> TMultiset;
	TMultiset multiset;
	multiset.insert("foo");
	multiset.insert("bar");
	multiset.insert("foo");
	stream << multiset;
	LASS_TEST_CHECK(stream.isEqual(multisetPattern)); // alphabetic!
	TMultiset multiset2;
	LASS_TEST_CHECK_NO_THROW(multiset2 = util::stringCast<TMultiset>(multisetPattern));
	LASS_TEST_CHECK(extended_io::safe_equal(multiset, multiset2));

#ifdef _STLP_SLIST
	std::slist<int> slist;
	slist.push_back(1);
	slist.push_back(2);
	slist.push_back(3);
	stream << slist;
	LASS_TEST_CHECK(stream.isEqual("[1, 2, 3]"));
#endif

}

}

}

#endif

// EOF
