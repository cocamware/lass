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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_SMALL_OBJECT_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_SMALL_OBJECT_INL

#include "test_common.h"
#include "../util/small_object.h"



namespace lass
{
namespace test
{
namespace small_object
{
	class Small: public util::SmallObject<>
	{
	public:
		Small() { std::cout << "Small::Small()\n"; }
		~Small() { std::cout << "Small::~Small()\n"; }
		int filling;
	};
}

void testUtilSmallObject()
{
	using namespace small_object;

	LASS_COUT << "sizeof Small: " << sizeof Small << std::endl;

	LASS_COUT << "a\n";
	Small* a = new Small;
	delete a;

	LASS_COUT << "b\n";
	Small* b = new (std::nothrow) Small;
	delete b;

	LASS_COUT << "c\n";
	char buffer[sizeof Small];
	Small* c = new (buffer) Small;
	c->~Small();
}



}

}

#endif

// EOF
