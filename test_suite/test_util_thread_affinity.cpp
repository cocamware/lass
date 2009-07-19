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

#include "../lass/util/thread_fun.h"

namespace lass
{
namespace test
{
namespace thread_affinity
{

volatile bool stop = false;
void fun()
{
	while (!stop)
	{
	}
}

}

void testUtilNumberOfProcessors()
{
	LASS_COUT << "numberOfProcessors: " << util::numberOfProcessors() << "\n";
	LASS_COUT << "numberOfAvailableProcessors: " << util::numberOfAvailableProcessors() << "\n";
	LASS_COUT << "availableProcessors: " << util::availableProcessors() << "\n";

	const util::TCpuSet cpuSet = util::availableProcessors();

	LASS_TEST_CHECK(util::numberOfProcessors() > 0);
	LASS_TEST_CHECK(util::numberOfAvailableProcessors() > 0);
	LASS_TEST_CHECK(util::numberOfAvailableProcessors() <= util::numberOfProcessors());
	LASS_TEST_CHECK_EQUAL(util::numberOfProcessors(), cpuSet.size());
	LASS_TEST_CHECK_EQUAL(util::numberOfAvailableProcessors(), static_cast<size_t>(std::count(cpuSet.begin(), cpuSet.end(), true)));
}

void testUtilThreadAffinity()
{
	typedef util::SharedPtr<util::Thread> TThreadPtr;

	const size_t n = util::numberOfProcessors();
	std::vector<TThreadPtr> threads(n);
	for (size_t i = 0; i < n; ++i)
	{
		if (util::isAvailableProcessor(i))
		{
			threads[i].reset(util::threadFun(thread_affinity::fun, util::threadJoinable));
			threads[i]->run();
		}
	}

	for (size_t i = 0; i < n; ++i)
	{
		if (threads[i])
		{
			LASS_TEST_CHECK_EQUAL(threads[i]->affinity(), util::availableProcessors());
		}
	}

	for (size_t i = 0; i < n; ++i)
	{
		if (threads[i])
		{
			threads[i]->bind(i);
		}
	}

	for (size_t i = 0; i < n; ++i)
	{
		if (threads[i])
		{
			util::TCpuSet expectedAffinity(n, false);
			expectedAffinity[i] = true;
			LASS_TEST_CHECK_EQUAL(threads[i]->affinity(), expectedAffinity);
		}
	}

	thread_affinity::stop = true;
	
	for (size_t i = 0; i < n; ++i)
	{
		if (threads[i])
		{
			threads[i]->join();
		}
	}
}

TUnitTest test_util_thread_affinity()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testUtilNumberOfProcessors));
	result.push_back(LASS_TEST_CASE(testUtilThreadAffinity));
	return result;
}

}

}

// EOF
