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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

#include "../lass/num/tri_bool.h"

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

TUnitTest test_num_tri_bool()
{
	return TUnitTest(1, LASS_TEST_CASE(testNumTriBool));
}



}

}

// EOF
