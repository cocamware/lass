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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_TRI_BOOL_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_TRI_BOOL_INL

#include "test_common.h"

#include "../num/tri_bool.h"

namespace lass
{
namespace test
{

void testNumTriBool()
{
	using namespace num;

	const TriBool unknownBool; // trying default constructor!
	const TriBool trueBool(true);
	const TriBool falseBool(false); 

	BOOST_CHECK_EQUAL(trueBool, true);
	BOOST_CHECK_EQUAL(falseBool, false);
    BOOST_CHECK((unknownBool == unknown).isUnknown());

    BOOST_CHECK(trueBool.isTrue());
    BOOST_CHECK(!falseBool.isTrue());
    BOOST_CHECK(!unknownBool.isTrue());

    BOOST_CHECK(!trueBool.isFalse());
    BOOST_CHECK(falseBool.isFalse());
    BOOST_CHECK(!unknownBool.isFalse());

    BOOST_CHECK(!trueBool.isUnknown());
    BOOST_CHECK(!falseBool.isUnknown());
    BOOST_CHECK(unknownBool.isUnknown());


	BOOST_CHECK(trueBool);
	BOOST_CHECK(!falseBool);
    BOOST_CHECK((!unknownBool).isUnknown());

	BOOST_CHECK((trueBool == trueBool).isTrue());
	BOOST_CHECK((trueBool == falseBool).isFalse());
	BOOST_CHECK((trueBool == unknownBool).isUnknown());
	BOOST_CHECK((falseBool == trueBool).isFalse());
	BOOST_CHECK((falseBool == falseBool).isTrue());
	BOOST_CHECK((falseBool == unknownBool).isUnknown());
	BOOST_CHECK((unknownBool == trueBool).isUnknown());
	BOOST_CHECK((unknownBool == falseBool).isUnknown());
	BOOST_CHECK((unknownBool == unknownBool).isUnknown());

	BOOST_CHECK((trueBool != trueBool).isFalse());
	BOOST_CHECK((trueBool != falseBool).isTrue());
	BOOST_CHECK((trueBool != unknownBool).isUnknown());
	BOOST_CHECK((falseBool != trueBool).isTrue());
	BOOST_CHECK((falseBool != falseBool).isFalse());
	BOOST_CHECK((falseBool != unknownBool).isUnknown());
	BOOST_CHECK((unknownBool != trueBool).isUnknown());
	BOOST_CHECK((unknownBool != falseBool).isUnknown());
	BOOST_CHECK((unknownBool != unknownBool).isUnknown());

	BOOST_CHECK((trueBool && trueBool).isTrue());
	BOOST_CHECK((trueBool && falseBool).isFalse());
	BOOST_CHECK((trueBool && unknownBool).isUnknown());
	BOOST_CHECK((falseBool && trueBool).isFalse());
	BOOST_CHECK((falseBool && falseBool).isFalse());
	BOOST_CHECK((falseBool && unknownBool).isFalse());
	BOOST_CHECK((unknownBool && trueBool).isUnknown());
	BOOST_CHECK((unknownBool && falseBool).isFalse());
	BOOST_CHECK((unknownBool && unknownBool).isUnknown());

	BOOST_CHECK(trueBool || trueBool);
	BOOST_CHECK(trueBool || falseBool);
	BOOST_CHECK(trueBool || unknownBool);
	BOOST_CHECK(falseBool || trueBool);
	BOOST_CHECK((falseBool || falseBool).isFalse());
	BOOST_CHECK((falseBool || unknownBool).isUnknown());
	BOOST_CHECK(unknownBool || trueBool);
	BOOST_CHECK((unknownBool || falseBool).isUnknown());
	BOOST_CHECK((unknownBool || unknownBool).isUnknown());

	boost::test_toolbox::output_test_stream test;
	test << std::boolalpha << trueBool << ", " << falseBool << ", " << unknownBool;
	BOOST_CHECK(test.is_equal("true, false, unknown"));
	test << std::noboolalpha << trueBool << ", " << falseBool << ", " << unknownBool;
	BOOST_CHECK(test.is_equal("1, 0, ?"));
}



}

}

#endif

// EOF
