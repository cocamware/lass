/**	@file
 *  @internal
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_VARIOUS_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_VARIOUS_INL

#include "test_common.h"

#include "../meta/is_same_type.h"
#include "../meta/is_convertible_type.h"
#include "../meta/is_derived_type.h"
 
namespace lass
{
namespace test
{

class Spam {};
class Ham: public Spam {};

void testMetaIsConvertibleType()
{
	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<float, int>::value), true);
	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<int, float>::value), true);

	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<std::string, const char*>::value), false);
	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<const char*, std::string>::value), true);

	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<float*, int*>::value), false);
	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<float*, void*>::value), true);

	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<const float*, float*>::value), false);
	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<float*, const float*>::value), true);

	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<Spam, int>::value), false);
	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<int, Spam>::value), false);
	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<Spam, Ham>::value), false);

	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<Spam*, Ham*>::value), false);
	BOOST_CHECK_EQUAL(bool(meta::IsConvertibleType<Ham*, Spam*>::value), true);
}



void testMetaIsDerivedType()
{
	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<float, int>::value), false);
	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<float, int>::value), false);

	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<float, float>::value), true);
	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<float, float>::value), true);

	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<Spam, void>::value), false);
	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<Spam, void>::value), false);

	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<void, void>::value), true);
	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<void, void>::value), true);

	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<Spam, Ham>::value), false);
	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<Spam, Ham>::value), false);

	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<Ham, Spam>::value), true);
	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<Ham, Spam>::value), true);

	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<Ham*, Spam*>::value), true);
	BOOST_CHECK_EQUAL(bool(meta::IsDerivedType<Ham*, Spam*>::value), true);
}

}

}

#endif

// EOF
