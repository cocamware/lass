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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_VARIOUS_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_VARIOUS_INL

#include "test_common.h"

#include "../meta/is_same.h"
#include "../meta/is_convertible.h"
#include "../meta/is_derived.h"

namespace lass
{
namespace test
{

class Spam {};
class Ham: public Spam {};

void testMetaIsConvertible()
{
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<float, int>::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<int, float>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<std::string, const char*>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<const char*, std::string>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<float*, int*>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<float*, void*>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<const float*, float*>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<float*, const float*>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<Spam, int>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<int, Spam>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<Spam, Ham>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<Spam*, Ham*>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<Ham*, Spam*>::value), true);
}



void testMetaIsDerivedType()
{
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<float, int>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<float, int>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<float, float>::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<float, float>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Spam, void>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Spam, void>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<void, void>::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<void, void>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Spam, Ham>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Spam, Ham>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Ham, Spam>::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Ham, Spam>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Ham*, Spam*>::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Ham*, Spam*>::value), true);
}

}

}

#endif

// EOF
