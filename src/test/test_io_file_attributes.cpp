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
	std::ofstream test("this_file_does_exist.txt");
	test << "foobar";
	test.close();

	LASS_TEST_CHECK_EQUAL(io::fileDoesExist("this_file_does_exist.txt"), true);
	LASS_TEST_CHECK_EQUAL(io::fileDoesExist("this_file_does_not_exist.txt"), false);

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

TUnitTests test_io_file_attributes()
{
	return TUnitTests(1, LASS_UNIT_TEST(testIoFileAttributes));
}

}

}

// EOF
