/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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
#include "../util/atomic.h"
#include "../num/basic_types.h"

namespace lass
{
namespace test
{
namespace atomic
{

template <typename T>
void testUtilAtomicType()
{
	std::cout << (8*sizeof(T)) << "bits\n";

	T multiplier = 0;
	for (size_t i = 0; i < sizeof(T); ++i)
	{
		multiplier = (multiplier << 8) + 1;
	}


	T old1 = 1;
	T new1 = 2;
	T a = old1;
	util::atomicIncrement(a);
	LASS_TEST_CHECK_EQUAL(a, new1);	
	util::atomicDecrement(a);
	LASS_TEST_CHECK_EQUAL(a, old1);

	a *= multiplier;
	old1 *= multiplier;
	new1 *= multiplier;
	T wrong1 = 10;

	LASS_TEST_CHECK(!util::atomicCompareAndSwap(a, wrong1, new1));
	LASS_TEST_CHECK_EQUAL(a, old1);
	LASS_TEST_CHECK(util::atomicCompareAndSwap(a, old1, new1));
	LASS_TEST_CHECK_EQUAL(a, new1);
}

template <typename T>
void testUtilAtomicAdjacentCas()
{
	T multiplier = 0;
	for (size_t i = 0; i < sizeof(T); ++i)
	{
		multiplier = (multiplier << 8) + 1;
	}

	const T old1 = 1 * multiplier, old2 = 2 * multiplier;
	const T new1 = 3 * multiplier, new2 = 4 * multiplier;
	const T wrong1 = 10 * multiplier, wrong2 = 11 * multiplier;

	T a[2] = { old1, old2 };
	LASS_TEST_CHECK(!util::atomicCompareAndSwap(a[0], wrong1, wrong2, new1, new2));
	LASS_TEST_CHECK_EQUAL(a[0], old1);
	LASS_TEST_CHECK_EQUAL(a[1], old2);
	LASS_TEST_CHECK(util::atomicCompareAndSwap(a[0], old1, old2, new1, new2));
	LASS_TEST_CHECK_EQUAL(a[0], new1);
	LASS_TEST_CHECK_EQUAL(a[1], new2);
}

}

TUnitTests test_util_atomic()
{
	using namespace atomic;

	TUnitTests result;
	
	result.push_back(LASS_UNIT_TEST(testUtilAtomicType<num::Tuint8>));	
	result.push_back(LASS_UNIT_TEST(testUtilAtomicType<num::Tuint16>));	
	result.push_back(LASS_UNIT_TEST(testUtilAtomicType<num::Tuint32>));	
	result.push_back(LASS_UNIT_TEST(testUtilAtomicType<num::Tuint64>));

	result.push_back(LASS_UNIT_TEST(testUtilAtomicType<num::Tint8>));	
	result.push_back(LASS_UNIT_TEST(testUtilAtomicType<num::Tint16>));	
	result.push_back(LASS_UNIT_TEST(testUtilAtomicType<num::Tint32>));	
	result.push_back(LASS_UNIT_TEST(testUtilAtomicType<num::Tint64>));

	result.push_back(LASS_UNIT_TEST(testUtilAtomicAdjacentCas<num::Tuint8>));
	result.push_back(LASS_UNIT_TEST(testUtilAtomicAdjacentCas<num::Tuint16>));
	result.push_back(LASS_UNIT_TEST(testUtilAtomicAdjacentCas<num::Tuint32>));

	result.push_back(LASS_UNIT_TEST(testUtilAtomicAdjacentCas<num::Tint8>));
	result.push_back(LASS_UNIT_TEST(testUtilAtomicAdjacentCas<num::Tint16>));
	result.push_back(LASS_UNIT_TEST(testUtilAtomicAdjacentCas<num::Tint32>));

	return result;
}

}
}

// EOF
 
