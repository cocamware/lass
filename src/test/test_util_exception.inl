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




#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_EXCEPTION_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_EXCEPTION_INL

#include "test_common.h"

#ifdef _DEBUG
#	define LASS_CHECK_THROW_IN_DEBUG(x, e) BOOST_CHECK_THROW(x, e)
#else
#	define LASS_CHECK_THROW_IN_DEBUG(x, e) BOOST_CHECK_NO_THROW(x)
#endif

namespace lass
{
namespace test
{

void testUtilException()
{
	BOOST_CHECK_NO_THROW(
		try
		{
			LASS_THROW("this is a lass exception");
		}
		catch (util::Exception& lassException)
		{
			BOOST_CHECK_EQUAL(lassException.message(), lassException.what());
		}
	);

	BOOST_CHECK_THROW(LASS_THROW("this is a lass exception"), util::Exception);
	BOOST_CHECK_THROW(LASS_THROW("this is a lass exception"), std::exception);

}



}

}

#endif

// EOF
