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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_IO_BINARY_STREAM_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_IO_BINARY_STREAM_INL

#include "test_common.h"

#include "../io/binary_i_file.h"
#include "../io/binary_o_file.h"

namespace lass
{
namespace test
{

void testIoBinaryStream()
{
	using namespace io;

	long a = 5;
	std::string b = "This is a test";

	{
	BinaryOFile testO("temp.txt");
	testO << a;
	testO << b;
	}

	{
	BinaryIFile testI("temp.txt");
	long t;
	std::string tstr;

	testI >> t;
	testI >> tstr;

	LASS_TEST_CHECK_EQUAL(t, a);
	LASS_TEST_CHECK_EQUAL(tstr, b);
	}
}

}

}

#endif

// EOF
