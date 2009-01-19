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

#include "../meta/type_list.h"
#include "../meta/is_same.h"

namespace lass
{
namespace test
{

struct AVeryLongInt {}; // with linux port, __int64 does fit this job no more, some fake it.

// --- public --------------------------------------------------------------------------------------

void testMetaTypeList()
{

	using namespace meta;

	typedef type_list::Make<float, double, long double>::Type TFloats;
	typedef type_list::Make<signed char, short, int, long>::Type TSignedIntegers;

	/*
	LASS_TEST_CHECK_EQUAL(int(type_list::Size<TFloats>::value), 3);
	LASS_TEST_CHECK_EQUAL(int(type_list::Size<TSignedIntegers>::value), 4);
	LASS_TEST_CHECK_EQUAL(int(type_list::Size<NullType>::value), 0);

	typedef type_list::At<TFloats, 1>::Type TFloat1; // double
	LASS_TEST_CHECK((IsSame<TFloat1, double>::value));

	typedef type_list::AtNonStrict<TFloats, 4>::Type TFloat4; // NullType
	LASS_TEST_CHECK((IsSame<TFloat4, NullType>::value));
	typedef type_list::AtNonStrict<TFloats, 5, float>::Type TFloat5; // float
	LASS_TEST_CHECK((IsSame<TFloat5, float>::value));

	LASS_TEST_CHECK_EQUAL(int(type_list::Find<TFloats, float>::value), 0);
	LASS_TEST_CHECK_EQUAL(int(type_list::Find<TFloats, double>::value), 1);
	LASS_TEST_CHECK_EQUAL(int(type_list::Find<TFloats, long double>::value), 2);
	LASS_TEST_CHECK_EQUAL(int(type_list::Find<TFloats, NullType>::value), -1);

	typedef type_list::Merge<TSignedIntegers, AVeryLongInt>::Type TAllSignedIntegers;
	LASS_TEST_CHECK_EQUAL(int(type_list::Size<TAllSignedIntegers>::value), 5);
	LASS_TEST_CHECK_EQUAL(int(type_list::Find<TAllSignedIntegers, AVeryLongInt>::value), 4);
	typedef type_list::Merge<TFloats, TAllSignedIntegers>::Type TAllSignedTypes;
	LASS_TEST_CHECK_EQUAL(int(type_list::Size<TAllSignedTypes>::value), 8);

	typedef type_list::Remove<TFloats, long double>::Type TCommonFloats;
	LASS_TEST_CHECK_EQUAL(int(type_list::Size<TCommonFloats>::value), 2);

	typedef type_list::Replace<TAllSignedIntegers, AVeryLongInt, long>::Type TLongIs64;
	LASS_TEST_CHECK_EQUAL(int(type_list::Find<TLongIs64, AVeryLongInt>::value), -1);

	typedef type_list::Unique<TLongIs64>::Type TUniqueSignedIntegers;
	LASS_TEST_CHECK_EQUAL(int(type_list::Size<TUniqueSignedIntegers>::value), 4);

	typedef type_list::Reverse<TFloats>::Type TReverseFloats;
	LASS_TEST_CHECK_EQUAL(int(type_list::Size<TReverseFloats>::value),
					  int(type_list::Size<TFloats>::value));
  */
}

TUnitTest test_meta_type_list()
{
	return TUnitTest(1, LASS_TEST_CASE(testMetaTypeList));
}

}

}

// EOF
