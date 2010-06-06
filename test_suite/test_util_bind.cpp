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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

#include "../lass/util/bind.h"

namespace lass
{
namespace test
{
namespace bind_test
{
	bool functionIsCalled = false;

	void fun(const std::string& a)
	{
		LASS_COUT << "fun(a): " << a << std::endl;
		functionIsCalled = true;
	}

	int moreFun(int a, int b)
	{
		functionIsCalled = true;
		return a * b;
	}

	class Spam
	{
	public:
		Spam() {}
		void ham(const std::string& something)
		{
			std::cout << "Spam with ham and " << something << std::endl;
			functionIsCalled = true;
		}

		std::string eggs(int num) const
		{
			functionIsCalled = true;
			return util::stringCast<std::string>(num) + " eggs and spam";
		}
	};
}

void testUtilBind()
{
	using namespace util;

	Callback0 fun = bind(bind_test::fun, "hello world!");
	bind_test::functionIsCalled = false;
	fun();
	LASS_TEST_CHECK(bind_test::functionIsCalled);

	CallbackR0<int> moreFun = bind(bind_test::moreFun, 2, 3);
	bind_test::functionIsCalled = false;
	LASS_TEST_CHECK_EQUAL(moreFun(), 6);
	LASS_TEST_CHECK(bind_test::functionIsCalled);

	using bind_test::Spam;
	
	Spam spam1;	
	Callback0 spamAndHam = bind(&Spam::ham, &spam1, "spam");
	bind_test::functionIsCalled = false;
	spamAndHam();
	LASS_TEST_CHECK(bind_test::functionIsCalled);

	SharedPtr<Spam> spam2(new Spam);
	Callback0 spamAndEggs = bind(&Spam::eggs, spam2, 3); // ignore return value
	bind_test::functionIsCalled = false;
	spamAndEggs();	
	LASS_TEST_CHECK(bind_test::functionIsCalled);
}

TUnitTest test_util_bind()
{
	return TUnitTest(1, LASS_TEST_CASE(testUtilBind));
}

}
}

// EOF
