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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_EXTENDED_STRING_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_EXTENDED_STRING_INL

#include "test_common.h"
#include "../stde/extended_string.h"

namespace lass
{
namespace test
{

void testStdeExtendedString()
{
	const std::string testString = "This IS a MiXed CaSE stRINg";
    
    const std::string lower = stde::tolower(testString);
	BOOST_CHECK_EQUAL(stde::tolower(testString), "this is a mixed case string");
	BOOST_CHECK_EQUAL(stde::toupper(testString), "THIS IS A MIXED CASE STRING");
	BOOST_CHECK_EQUAL(stde::replace_all(testString, "IS", "was"), "This was a MiXed CaSE stRINg");
}

}

}

#endif

// EOF
