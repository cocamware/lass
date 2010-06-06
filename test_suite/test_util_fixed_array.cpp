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

#include "../lass/util/fixed_array.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4267) //  'argument' : conversion from 'size_t' to 'const lass::num::Tuint32', possible loss of data
#endif

namespace lass
{
namespace test
{

template <typename T>
void testUtilFixedArray()
{
	using namespace util;

	typedef FixedArray<T, 5> TFixedArray;

	TFixedArray array;

	LASS_TEST_CHECK_EQUAL(array.size(), size_t(5));
	LASS_TEST_CHECK_EQUAL(array.max_size(), size_t(5));
	LASS_TEST_CHECK_EQUAL(array.empty(), false);

	T value(1);
	for (typename TFixedArray::iterator it = array.begin(); it != array.end(); ++it)
	{
		*it = value;
		value += value;
	}

	LASS_TEST_CHECK_EQUAL(array[0], 1);
	LASS_TEST_CHECK_EQUAL(array[1], 2);
	LASS_TEST_CHECK_EQUAL(array[2], 4);
	LASS_TEST_CHECK_EQUAL(array[3], 8);
	LASS_TEST_CHECK_EQUAL(array[4], 16);

	for (unsigned i = 0; i < array.size(); ++i)
	{
		LASS_TEST_CHECK_EQUAL(array[i], array.at(i));
	}

	LASS_TEST_CHECK_EQUAL(array[0], array.front());
	LASS_TEST_CHECK_EQUAL(*array.begin(), array.front());
	LASS_TEST_CHECK_EQUAL(*array.rbegin(), array.back());

	LASS_TEST_CHECK_THROW(array.at(static_cast<typename TFixedArray::size_type>(-1)) = 37, std::exception);
	LASS_TEST_CHECK_THROW(array.at(5) = 37, std::exception);

	FixedArray<T, 0> nullArray;

	LASS_TEST_CHECK_EQUAL(nullArray.size(), size_t(0));
	LASS_TEST_CHECK_EQUAL(nullArray.max_size(), size_t(0));
	LASS_TEST_CHECK_EQUAL(nullArray.empty(), true);
	LASS_TEST_CHECK(nullArray.begin() == nullArray.end());
	LASS_TEST_CHECK(nullArray.rbegin() == nullArray.rend());

	LASS_TEST_CHECK_THROW(nullArray.at(0) = 37, std::exception);
}

TUnitTest test_util_fixed_array()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testUtilFixedArray<int>));
	result.push_back(LASS_TEST_CASE(testUtilFixedArray<float>));
	return result;
}

}

}

// EOF
