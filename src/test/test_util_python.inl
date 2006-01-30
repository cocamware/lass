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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_PYTHON_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_PYTHON_INL

#include "test_common.h"
#include "bar.h"

#include "../util/scoped_ptr.h"
#include "../util/shared_ptr.h"
#include "../io/file_attribute.h"
#include "../stde/extended_string.h"

namespace lass
{
namespace test
{

void testUtilPython()
{
	std::string testFile = io::fileJoinPath(test::workPath(), "test_bar.py");
	
	std::string commandStr = "execfile('" + testFile + "')";
	//commandStr = "from code import interact\ninteract()";
		
	commandStr = stde::replace_all(commandStr, std::string("\\"), std::string("\\\\"));
	LASS_TEST_CHECK_EQUAL( PyRun_SimpleString( commandStr.c_str() ) , 0 );

	
	
	typedef std::vector<double> TV;
	TV vec;
	python::impl::PySequence pyseqtest(vec);
	LASS_TEST_CHECK_EQUAL( PySequence_Check(&pyseqtest) , 1);

}



}

}

#endif

// EOF
