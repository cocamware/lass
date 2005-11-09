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

	LASS_TEST_CHECK_EQUAL(trueBool, true);
	LASS_TEST_CHECK_EQUAL(falseBool, false);
	LASS_TEST_CHECK((unknownBool == unknown).isUnknown());

	LASS_TEST_CHECK(trueBool.isTrue());
	LASS_TEST_CHECK(!falseBool.isTrue());
	LASS_TEST_CHECK(!unknownBool.isTrue());

	LASS_TEST_CHECK(!trueBool.isFalse());
	LASS_TEST_CHECK(falseBool.isFalse());
	LASS_TEST_CHECK(!unknownBool.isFalse());

	LASS_TEST_CHECK(!trueBool.isUnknown());
	LASS_TEST_CHECK(!falseBool.isUnknown());
	LASS_TEST_CHECK(unknownBool.isUnknown());


	LASS_TEST_CHECK(trueBool);
	LASS_TEST_CHECK(!falseBool);
	LASS_TEST_CHECK((!unknownBool).isUnknown());

	LASS_TEST_CHECK((trueBool == trueBool).isTrue());
	LASS_TEST_CHECK((trueBool == falseBool).isFalse());
	LASS_TEST_CHECK((trueBool == unknownBool).isUnknown());
	LASS_TEST_CHECK((falseBool == trueBool).isFalse());
	LASS_TEST_CHECK((falseBool == falseBool).isTrue());
	LASS_TEST_CHECK((falseBool == unknownBool).isUnknown());
	LASS_TEST_CHECK((unknownBool == trueBool).isUnknown());
	LASS_TEST_CHECK((unknownBool == falseBool).isUnknown());
	LASS_TEST_CHECK((unknownBool == unknownBool).isUnknown());

	LASS_TEST_CHECK((trueBool != trueBool).isFalse());
	LASS_TEST_CHECK((trueBool != falseBool).isTrue());
	LASS_TEST_CHECK((trueBool != unknownBool).isUnknown());
	LASS_TEST_CHECK((falseBool != trueBool).isTrue());
	LASS_TEST_CHECK((falseBool != falseBool).isFalse());
	LASS_TEST_CHECK((falseBool != unknownBool).isUnknown());
	LASS_TEST_CHECK((unknownBool != trueBool).isUnknown());
	LASS_TEST_CHECK((unknownBool != falseBool).isUnknown());
	LASS_TEST_CHECK((unknownBool != unknownBool).isUnknown());

	LASS_TEST_CHECK((trueBool && trueBool).isTrue());
	LASS_TEST_CHECK((trueBool && falseBool).isFalse());
	LASS_TEST_CHECK((trueBool && unknownBool).isUnknown());
	LASS_TEST_CHECK((falseBool && trueBool).isFalse());
	LASS_TEST_CHECK((falseBool && falseBool).isFalse());
	LASS_TEST_CHECK((falseBool && unknownBool).isFalse());
	LASS_TEST_CHECK((unknownBool && trueBool).isUnknown());
	LASS_TEST_CHECK((unknownBool && falseBool).isFalse());
	LASS_TEST_CHECK((unknownBool && unknownBool).isUnknown());

	LASS_TEST_CHECK(trueBool || trueBool);
	LASS_TEST_CHECK(trueBool || falseBool);
	LASS_TEST_CHECK(trueBool || unknownBool);
	LASS_TEST_CHECK(falseBool || trueBool);
	LASS_TEST_CHECK((falseBool || falseBool).isFalse());
	LASS_TEST_CHECK((falseBool || unknownBool).isUnknown());
	LASS_TEST_CHECK(unknownBool || trueBool);
	LASS_TEST_CHECK((unknownBool || falseBool).isUnknown());
	LASS_TEST_CHECK((unknownBool || unknownBool).isUnknown());

	TestStream test;
	test << std::boolalpha << trueBool << ", " << falseBool << ", " << unknownBool;
	LASS_TEST_CHECK(test.isEqual("true, false, unknown"));
	test << std::noboolalpha << trueBool << ", " << falseBool << ", " << unknownBool;
	LASS_TEST_CHECK(test.isEqual("1, 0, ?"));
}



}

}

#endif

// EOF
