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

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_TUPLE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_TUPLE_INL

#include "test_common.h"

#include "../meta/tuple.h"
#include "../meta/is_same.h"

namespace lass
{
namespace test
{

void testMetaTuple()
{
	using namespace meta;

	typedef Tuple< type_list::Make<int, std::string, meta::EmptyType, int>::Type > MyTuple;

	// check to see if the NullType fields doesn't take any place
	//
	LASS_TEST_CHECK_EQUAL(sizeof(MyTuple), sizeof(Tuple< type_list::Make<int, std::string, int>::Type >));
	
	LASS_TEST_CHECK((IsSame<tuple::Field<MyTuple, 0>::Type, int>::value));
	LASS_TEST_CHECK((IsSame<tuple::Field<MyTuple, 1>::Type, std::string>::value));
	LASS_TEST_CHECK((IsSame<tuple::Field<MyTuple, 2>::Type, EmptyType>::value));
	LASS_TEST_CHECK((IsSame<tuple::Field<MyTuple, 3>::Type, int>::value));

	MyTuple myTuple;

	tuple::field<0>(myTuple) = 15;
	tuple::field<1>(myTuple) = "Hello World!";
	tuple::field<2>(myTuple) = EmptyType::instance();
	tuple::field<3>(myTuple) = 42;

	const MyTuple yourTuple = myTuple;

	LASS_TEST_CHECK_EQUAL(tuple::field<0>(yourTuple), 15);
	LASS_TEST_CHECK_EQUAL(tuple::field<1>(yourTuple), "Hello World!");
	LASS_TEST_CHECK_EQUAL(tuple::field<3>(yourTuple), 42);
}

}
}

#endif

// EOF
