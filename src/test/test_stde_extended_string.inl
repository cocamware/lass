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


 *  @author Bram de Greve [Bramz]
 *  @date 2004
 *  
 *  @par last commit:
 *       $Revision$\n
 *       $Date$\n
 *       $Author$\n
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
	BOOST_CHECK_EQUAL(stde::tolower(testString), "this is a mixed case string");
}

}

}

#endif

/*
 * $Log$
 * Revision 1.3  2004/09/06 13:24:46  tgdemuer
 * *** empty log message ***
 *
 * Revision 1.2  2004/09/06 13:08:44  tgdemuer
 * *** empty log message ***
 *
 * Revision 1.1  2004/06/23 08:32:55  bdegreve
 * stde::tolower to transform strings to lowercase (hacked till bugfix stlport)
 *
 */

// EOF
