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
 *	Copyright (C) 2004-2025 the Initial Developer.
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

#include "../lass/util/rw_lock.h"
#include <thread>
#include <chrono>

namespace lass
{
namespace test
{

void testUtilRWLock()
{
	const int maxCores = std::min<int>(static_cast<int>(std::thread::hardware_concurrency()), 16);
	const int numProducers = std::max<int>(maxCores / 8, 2);
	const int numConsumers = std::max<int>(maxCores - numProducers, 2);
	LASS_COUT << "#producers = " << numProducers << ", #consumers = " << numConsumers << std::endl;

	util::RWLock lock(std::max(numConsumers / 2, 1));

	auto producer = [&lock]()
	{
		using namespace std::chrono_literals;
		for (int j = 0; j < 4; ++j)
		{
			std::this_thread::sleep_for(150ms);
			for (int i = 0; i < 16; ++i)
			{
				std::this_thread::sleep_for(5ms);
				lock.lockw();
				std::cout << "(W)";
				lock.unlockw();
			}
		}
	};

	auto consumer = [&lock]()
	{
		using namespace std::chrono_literals;
		for (int i = 0; i < 1000; ++i)
		{
			std::this_thread::sleep_for(1ms);
			lock.lockr();
			std::cout << "R";
			lock.unlockr();
		}
	};

	LASS_COUT << "thread spam ...\n";
	std::vector<std::thread> workers;
	for (int i = 0; i < numConsumers; ++i)
	{
		workers.emplace_back(consumer);
		if (i < numProducers)
			workers.emplace_back(producer);
	}
	LASS_COUT << "joining\n";
	for (auto& worker : workers)
	{
		worker.join();
	}
	LASS_COUT << "\njoined\n";

}

TUnitTest test_util_rw_lock()
{
	return TUnitTest(1, LASS_TEST_CASE(testUtilRWLock));
}

}

}

// EOF
