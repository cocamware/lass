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
#include "../lass/stde/iterator_range.h"

namespace lass
{
namespace test
{

void testStdeIteratorRange()
{
	typedef std::vector<int> TContainer;
	typedef stde::iterator_range<TContainer::iterator> TRange;

	TRange range;

#if LASS_COMPILER_TYPE != LASS_COMPILER_TYPE_MSVC
	// MSVC's _HAS_ITERATOR_DEBUGGING doesn't like containerless iterators
	LASS_TEST_CHECK(range.begin() == range.end());
	LASS_TEST_CHECK_EQUAL(range.size(), 0);
	LASS_TEST_CHECK(range.empty());
	LASS_TEST_CHECK(!range);
	LASS_TEST_CHECK(range ? false : true);
#endif

	TContainer container;
	const int n = 10;
	for (int i = 0; i < n; ++i)
	{
		container.push_back(i);
	}

	range = TRange(container.begin(), container.end());
	LASS_TEST_CHECK(range.begin() == container.begin());
	LASS_TEST_CHECK(range.end() == container.end());
	LASS_TEST_CHECK_EQUAL(range.size(), n);
	LASS_TEST_CHECK(!range.empty());
	LASS_TEST_CHECK_EQUAL(!range, false);
	LASS_TEST_CHECK(range);

	for (int i = 0; i < n; ++i)
	{
		LASS_TEST_CHECK_EQUAL(range[i], i);
	}

	for (int i = 0; range; ++range, ++i)
	{
		LASS_TEST_CHECK_EQUAL(*range, i);
	}
	LASS_TEST_CHECK(range.empty());

	TContainer other_container;
	for (int i = 100; i < 200; ++i)
	{
		other_container.push_back(i);
	}
	TRange other(other_container.begin(), other_container.end());

#if LASS_COMPILER_TYPE != LASS_COMPILER_TYPE_MSVC
	// MSVC's _HAS_ITERATOR_DEBUGGING doesn't like comparing cross-container iterators ... booh!
	LASS_TEST_CHECK(range != other);
#endif

	range = other++;
	LASS_TEST_CHECK(range.end() == other.end());
	LASS_TEST_CHECK_EQUAL(range.size(), other.size() + 1);
	
	range.swap(other);
	LASS_TEST_CHECK_EQUAL(range.size() + 1, other.size());
}

TUnitTest test_stde_iterator_range()
{
	return TUnitTest(1, LASS_TEST_CASE(testStdeIteratorRange));
}

}

}

// EOF
