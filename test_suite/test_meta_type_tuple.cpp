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
 *	Copyright (C) 2022 the Initial Developer.
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

#include "../lass/meta/type_tuple.h"
#include "../lass/meta/is_same.h"

namespace lass
{
namespace test
{

// --- public --------------------------------------------------------------------------------------

void testMetaTypeTuple()
{
	using namespace meta;

	using TFloats = TypeTuple<float, double, long double>;
	using TSignedIntegers = type_tuple::Make<signed char, short, int, long>::Type;

	LASS_TEST_CHECK((IsSame<TSignedIntegers, TypeTuple<signed char, short, int, long>>::value));

	LASS_TEST_CHECK(type_tuple::Size<TFloats>::value == 3);
	LASS_TEST_CHECK(type_tuple::Size<TSignedIntegers>::value == 4);
	LASS_TEST_CHECK(type_tuple::Size<TypeTuple<>>::value == 0);

	LASS_TEST_CHECK((type_tuple::Contains<TFloats, double>::value));
	LASS_TEST_CHECK(!(type_tuple::Contains<TFloats, int>::value));

	using TFloat1 = type_tuple::At<TFloats, 1>::Type; // double
	LASS_TEST_CHECK((IsSame<TFloat1, double>::value));

	using TFloat2 = type_tuple::AtNonStrict<TFloats, 2>::Type; // long double
	LASS_TEST_CHECK((IsSame<TFloat2, long double>::value));
	using TFloat4 = type_tuple::AtNonStrict<TFloats, 4>::Type; // NullType
	LASS_TEST_CHECK((IsSame<TFloat4, NullType>::value));
	using TFloat5 = type_tuple::AtNonStrict<TFloats, 5, float>::Type; // float
	LASS_TEST_CHECK((IsSame<TFloat5, float>::value));

	LASS_TEST_CHECK((type_tuple::Find<TFloats, float>::value == 0));
	LASS_TEST_CHECK((type_tuple::Find<TFloats, double>::value == 1));
	LASS_TEST_CHECK((type_tuple::Find<TFloats, long double>::value == 2));
	// if type is not found, you get the length of the tuple as result
	LASS_TEST_CHECK((type_tuple::Find<TFloats, int>::value == type_tuple::Size<TFloats>::value));
}

TUnitTest test_meta_type_tuple()
{
	return TUnitTest({
		LASS_TEST_CASE(testMetaTypeTuple),
		});
}

}

}

// EOF
