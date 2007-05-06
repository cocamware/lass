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

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_BIND_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_BIND_INL

#include "test_common.h"

#include "../util/bind.h"

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

}
}

#endif

// EOF
