/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/
/** @internal
 *  @file test/test_io_file_attributes.inl
 *  @brief testbed for lass/io/file_attribute.h
 *  @author [BdG] Ghent University - INTEC (acoustics)
 *  @date 2004
 */

#include "test_common.h"

#include "../io/file_attribute.h"
#include <fstream>

namespace lass
{
namespace test
{

void testIoFileAttributes()
{
	std::ofstream test("this_file_does_exist");
	test << "foobar";
	test.close();

	BOOST_CHECK_EQUAL(io::fileDoesExist("this_file_does_exist"), true);
	BOOST_CHECK_EQUAL(io::fileDoesExist("this_file_does_not_exist"), false);

	BOOST_CHECK_EQUAL(io::fileExtension(""), "");
	BOOST_CHECK_EQUAL(io::fileExtension("foo"), "");
	BOOST_CHECK_EQUAL(io::fileExtension("foo."), "");
	BOOST_CHECK_EQUAL(io::fileExtension("foo.bar"), "bar");
	BOOST_CHECK_EQUAL(io::fileExtension("foo.bar."), "");	
	BOOST_CHECK_EQUAL(io::fileExtension("foo.bar.fun"), "fun");	
}

}

}

// EOF
