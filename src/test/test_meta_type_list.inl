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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_TYPE_LIST_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_TYPE_LIST_INL

#include "test_common.h"

#include "../meta/type_list.h"
#include "../meta/is_same_type.h"

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

	LASS_TEST_CHECK_EQUAL(int(type_list::Size<TFloats>::value), 3);
	LASS_TEST_CHECK_EQUAL(int(type_list::Size<TSignedIntegers>::value), 4);
	LASS_TEST_CHECK_EQUAL(int(type_list::Size<NullType>::value), 0);

	typedef type_list::At<TFloats, 1>::Type TFloat1; // double
	LASS_TEST_CHECK((IsSameType<TFloat1, double>::value));

	typedef type_list::AtNonStrict<TFloats, 4>::Type TFloat4; // NullType
	LASS_TEST_CHECK((IsSameType<TFloat4, NullType>::value));
	typedef type_list::AtNonStrict<TFloats, 5, float>::Type TFloat5; // float
	LASS_TEST_CHECK((IsSameType<TFloat5, float>::value));

	LASS_TEST_CHECK_EQUAL(int(type_list::Find<TFloats, float>::value), 0);
	LASS_TEST_CHECK_EQUAL(int(type_list::Find<TFloats, double>::value), 1);
	LASS_TEST_CHECK_EQUAL(int(type_list::Find<TFloats, long double>::value), 2);
	LASS_TEST_CHECK_EQUAL(int(type_list::Find<TFloats, NullType>::value), -1);

	LASS_TEST_CHECK_EQUAL(type_list::find<TFloats>(typeid(float)), 0);
	LASS_TEST_CHECK_EQUAL(type_list::find<TFloats>(typeid(double)), 1);
	LASS_TEST_CHECK_EQUAL(type_list::find<TFloats>(typeid(long double)), 2);
	LASS_TEST_CHECK_EQUAL(type_list::find<TFloats>(typeid(NullType)), -1);

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
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
