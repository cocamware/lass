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


 *  @author Bram de Greve [Bramz]
 *  @date 2004
 *  
 *  @par last commit:
 *       $Revision$\n
 *       $Date$\n
 *       $Author$\n
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_EXTENDED_IO_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_EXTENDED_IO_INL

#include "test_common.h"
#include "../stde/extended_io.h"

namespace lass
{
namespace test
{

void testStdeExtendedIo()
{
	boost::test_toolbox::output_test_stream stream;

	std::pair<int, int> pair(1, 2);
	stream << pair;
	BOOST_CHECK(stream.is_equal("(1, 2)"));

	std::vector<int> vector;
	vector.push_back(1);
	vector.push_back(2);
	vector.push_back(3);
	stream << vector;
	BOOST_CHECK(stream.is_equal("[1, 2, 3]"));

	std::list<int> list;
	list.push_back(1);
	list.push_back(2);
	list.push_back(3);
	stream << list;
	BOOST_CHECK(stream.is_equal("[1, 2, 3]"));

	std::deque<int> deque;
	deque.push_back(1);
	deque.push_back(2);
	deque.push_back(3);
	stream << deque;
	BOOST_CHECK(stream.is_equal("[1, 2, 3]"));

	std::map<std::string, int> map;
	map["foo"] = 1;
	map["bar"] = 2;
	map["spam"] = 3;
	stream << map;
	BOOST_CHECK(stream.is_equal("{bar: 2, foo: 1, spam: 3}")); // alphabetic!

	std::multimap<std::string, int> multimap;
	multimap.insert(std::pair<std::string, int>("foo", 1));
	multimap.insert(std::pair<std::string, int>("bar", 2));
	multimap.insert(std::pair<std::string, int>("foo", 3));
	stream << multimap;
	BOOST_CHECK(stream.is_equal("{bar: 2, foo: 1, foo: 3}")); // alphabetic!

	std::set<std::string> set;
	set.insert("foo");
	set.insert("bar");
	set.insert("spam");
	stream << set;
	BOOST_CHECK(stream.is_equal("{bar, foo, spam}")); // alphabetic!

	std::multiset<std::string> multiset;
	multiset.insert("foo");
	multiset.insert("bar");
	multiset.insert("foo");
	stream << multiset;
	BOOST_CHECK(stream.is_equal("{bar, foo, foo}")); // alphabetic!

#ifdef _STLP_SLIST
	std::slist<int> slist;
	slist.push_back(1);
	slist.push_back(2);
	slist.push_back(3);
	stream << slist;
	BOOST_CHECK(stream.is_equal("[1, 2, 3]"));
#endif

}

}

}

#endif

/*
 * $Log$
 * Revision 1.3  2004/09/06 13:24:46  tgdemuer
 * *** empty log message ***
 *
 * Revision 1.2  2004/09/06 13:08:44  tgdemuer
 * *** empty log message ***
 *
 * Revision 1.1  2004/06/21 16:53:19  bdegreve
 * testing extended io (and bug fixes :)
 *
 */

// EOF
