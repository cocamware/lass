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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_DICTIONARY_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_DICTIONARY_INL

#include "test_common.h"

#include "../util/dictionary.h"

namespace lass
{
namespace test
{
namespace dictionary
{
}

void testUtilDictionary()
{
	enum FooBar 
	{ 
		fbInvalid = 0, 
		fbEggs = 1, 
		fbSpam = 2, 
		fbHam = 3 
	};

	typedef lass::util::Dictionary<std::string, int> TDictionary;

	TDictionary dictionary;
	dictionary.add("eggs", fbEggs);
	dictionary.add("spam", fbSpam);
	dictionary.add("ham", fbHam);
	dictionary.setDefault("invalid", fbInvalid);

	BOOST_CHECK_EQUAL(dictionary["eggs"], int(fbEggs));
	BOOST_CHECK_EQUAL(dictionary["spam"], int(fbSpam));
	BOOST_CHECK_EQUAL(dictionary["ham"], int(fbHam));
	BOOST_CHECK_EQUAL(dictionary["foobar"], int(fbInvalid));

	BOOST_CHECK_EQUAL(dictionary.key(fbEggs), "eggs");
	BOOST_CHECK_EQUAL(dictionary.key(fbSpam), "spam");
	BOOST_CHECK_EQUAL(dictionary.key(fbHam), "ham");
	BOOST_CHECK_EQUAL(dictionary.key(666), "invalid");

	boost::test_toolbox::output_test_stream stream;
	stream << dictionary.keys();
	BOOST_CHECK(stream.is_equal("{eggs, ham, spam}"));
	stream << dictionary.values();
	BOOST_CHECK(stream.is_equal("{1, 2, 3}"));

	dictionary.add("EGGS", fbEggs);
	BOOST_CHECK_EQUAL(dictionary["eggs"], int(fbEggs));
	BOOST_CHECK_EQUAL(dictionary["EGGS"], int(fbEggs));
	BOOST_CHECK(dictionary.key(fbEggs) == "eggs" || dictionary.key(fbEggs) == "EGGS");
	stream << dictionary.keys();
	BOOST_CHECK(stream.is_equal("{EGGS, eggs, ham, spam}"));
	stream << dictionary.keys(fbEggs);
	BOOST_CHECK(stream.is_equal("{EGGS, eggs}"));
	stream << dictionary.values();
	BOOST_CHECK(stream.is_equal("{1, 2, 3}"));

	dictionary.remove("EGGS", fbEggs);
	stream << dictionary.keys();
	BOOST_CHECK(stream.is_equal("{eggs, ham, spam}"));

	dictionary.add("eggs", 12);
	BOOST_CHECK(dictionary["eggs"] == fbEggs || dictionary["eggs"] == 12);
	BOOST_CHECK_EQUAL(dictionary.key(fbEggs), "eggs");
	BOOST_CHECK_EQUAL(dictionary.key(12), "eggs");
	stream << dictionary.keys();
	BOOST_CHECK(stream.is_equal("{eggs, ham, spam}"));
	stream << dictionary.values();
	BOOST_CHECK(stream.is_equal("{1, 2, 3, 12}"));
	stream << dictionary.values("eggs");
	BOOST_CHECK(stream.is_equal("{1, 12}"));

	dictionary.remove("eggs", 12);
	stream << dictionary.values();
	BOOST_CHECK(stream.is_equal("{1, 2, 3}"));

}

}

}

#endif

// EOF
