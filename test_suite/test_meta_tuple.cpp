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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#include "../lass/meta/tuple.h"
#include "../lass/meta/is_same.h"

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

TUnitTest test_meta_tuple()
{
	return TUnitTest(1, LASS_TEST_CASE(testMetaTuple));
}

}
}

// EOF
