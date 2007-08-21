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
	LASS_TEST_CHECK_NO_THROW(vector2 = util::stringCast<TVector>(sequencePattern));
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
}

}

}

#endif

// EOF
