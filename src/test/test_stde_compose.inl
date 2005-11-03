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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_COMPOSE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_COMPOSE_INL

#include "test_common.h"
#include "../stde/compose.h"
#include "../stde/select.h"

namespace lass
{
namespace test
{

void testStdeCompose()
{
	boost::test_toolbox::output_test_stream stream;

	std::vector<int> a, b;
	for (int i = 0; i < 5; ++i)
	{
		a.push_back(i);
	}
	std::transform(a.begin(), a.end(), std::back_inserter(b),
		stde::compose_f_gx(
			std::negate<int>(),
			std::bind2nd(std::multiplies<int>(), 2)));	
	stream << b;
	BOOST_CHECK(stream.is_equal("[0, -2, -4, -6, -8]"));

	typedef std::pair<int, int> pair_type;
	std::vector<pair_type> c;
	std::vector<int> d;
	for (int i = 0; i < 10; ++i)
	{
		c.push_back(std::make_pair(10 * i, i));
	}
	std::transform(c.begin(), c.end(), std::back_inserter(d),
		stde::compose_f_gx_hx(
			std::plus<int>(),
			stde::select_1st<pair_type>(),
			stde::select_2nd<pair_type>()));
	stream << d;
	BOOST_CHECK(stream.is_equal("[0, 11, 22, 33, 44]"));
}

}

}

#endif

// EOF
