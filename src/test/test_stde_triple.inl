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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_TRIPLE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_TRIPLE_INL

#include "test_common.h"

#pragma warning(push)
#pragma warning(disable: 4244)

#include "../stde/triple.h"



namespace lass
{
namespace test
{

void testStdeTriple()
{
	typedef stde::triple<char, float, std::string> triple1_type;
	typedef stde::triple<int, double, std::string> triple2_type;

	triple1_type triple1 = stde::make_triple(true, 1, std::string("hello world!"));
	triple2_type triple2(triple1);
	BOOST_CHECK_EQUAL(triple1, triple2);

	triple2.first = 666;
	BOOST_CHECK(triple1 != triple2);

	triple2 = triple1;
	BOOST_CHECK_EQUAL(triple1, triple2);

	triple2.second = 6.66;
	BOOST_CHECK(triple1 != triple2);

	triple2 = triple1;
	triple2.third = "foo";
	BOOST_CHECK(triple1 != triple2);
}



}

}

#pragma warning(pop)

#endif

// EOF