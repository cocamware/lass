/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#include "test_common.h"
#include "test_stde.h"

#include "test_stde_compose.inl"
#include "test_stde_extended_io.inl"
#include "test_stde_extended_string.inl"
#include "test_stde_slist.inl"
#include "test_stde_small_object_allocator.inl"
#include "test_stde_static_vector.inl"
#include "test_stde_triple.inl"

namespace lass
{
namespace test
{

TUnitTests testStde()
{
	TUnitTests result;

	result.push_back(LASS_UNIT_TEST(testStdeCompose));
	result.push_back(LASS_UNIT_TEST(testStdeExtendedIo));
	result.push_back(LASS_UNIT_TEST(testStdeExtendedString));
	result.push_back(LASS_UNIT_TEST(testStdeSlist));
	result.push_back(LASS_UNIT_TEST(testStdeSmallObjectAllocator));
	result.push_back(LASS_UNIT_TEST(testStdeStaticVector));
	result.push_back(LASS_UNIT_TEST(testStdeTriple));

	return result;
}

}

}

// EOF
