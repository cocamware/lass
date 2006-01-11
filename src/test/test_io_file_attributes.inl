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

#include "../io/file_attribute.h"
#include <fstream>

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	define LASS_TEST_IO_FILE_ATTRIBUTES_HAVE_FORWARD_SLASHES
#	define LASS_TEST_IO_FILE_ATTRIBUTES_HAVE_BACKWARD_SLASHES
#else
#	define LASS_TEST_IO_FILE_ATTRIBUTES_HAVE_FORWARD_SLASHES
#endif

namespace lass
{
namespace test
{

void testIoFileAttributes()
{
	std::ofstream test("this_file_does_exist");
	test << "foobar";
	test.close();

	LASS_TEST_CHECK_EQUAL(io::fileDoesExist("this_file_does_exist"), true);
	LASS_TEST_CHECK_EQUAL(io::fileDoesExist("this_file_does_not_exist"), false);

	LASS_TEST_CHECK_EQUAL(io::fileExtension(""), "");
	LASS_TEST_CHECK_EQUAL(io::fileExtension("foo"), "");
	LASS_TEST_CHECK_EQUAL(io::fileExtension("foo."), "");
	LASS_TEST_CHECK_EQUAL(io::fileExtension("foo.bar"), "bar");
	LASS_TEST_CHECK_EQUAL(io::fileExtension("foo.bar."), "");
	LASS_TEST_CHECK_EQUAL(io::fileExtension("foo.bar.fun"), "fun");

	LASS_TEST_CHECK_EQUAL(io::fileWithoutExtension(""), "");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutExtension("foo"), "foo");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutExtension("foo."), "foo");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutExtension("foo.bar"), "foo");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutExtension("foo.bar."), "foo.bar");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutExtension("foo.bar.fun"), "foo.bar");

	#ifdef LASS_TEST_IO_FILE_ATTRIBUTES_HAVE_FORWARD_SLASHES
	LASS_TEST_CHECK_EQUAL(io::filePath(""), "");
	LASS_TEST_CHECK_EQUAL(io::filePath("foo"), "");
	LASS_TEST_CHECK_EQUAL(io::filePath("foo/"), "foo");
	LASS_TEST_CHECK_EQUAL(io::filePath("foo/bar"), "foo");
	LASS_TEST_CHECK_EQUAL(io::filePath("foo/bar/"), "foo/bar");
	LASS_TEST_CHECK_EQUAL(io::filePath("foo/bar/fun"), "foo/bar");
	#endif
	#ifdef LASS_TEST_IO_FILE_ATTRIBUTES_HAVE_BACKWARD_SLASHES
	LASS_TEST_CHECK_EQUAL(io::filePath(""), "");
	LASS_TEST_CHECK_EQUAL(io::filePath("foo"), "");
	LASS_TEST_CHECK_EQUAL(io::filePath("foo\\"), "foo");
	LASS_TEST_CHECK_EQUAL(io::filePath("foo\\bar"), "foo");
	LASS_TEST_CHECK_EQUAL(io::filePath("foo\\bar\\"), "foo\\bar");
	LASS_TEST_CHECK_EQUAL(io::filePath("foo\\bar\\fun"), "foo\\bar");
	#endif
	
	#ifdef LASS_TEST_IO_FILE_ATTRIBUTES_HAVE_FORWARD_SLASHES
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath(""), "");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath("foo"), "foo");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath("foo/"), "");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath("foo/bar"), "bar");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath("foo/bar/"), "");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath("foo/bar/fun"), "fun");
	#endif
	#ifdef LASS_TEST_IO_FILE_ATTRIBUTES_HAVE_BACKWARD_SLASHES
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath(""), "");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath("foo"), "foo");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath("foo\\"), "");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath("foo\\bar"), "bar");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath("foo\\bar\\"), "");
	LASS_TEST_CHECK_EQUAL(io::fileWithoutPath("foo\\bar\\fun"), "fun");
	#endif

}

}

}

// EOF
