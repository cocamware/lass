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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_FILTERED_FLOAT_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_FILTERED_FLOAT_INL

#include "test_common.h"

#include "../num/filtered_float.h"

namespace lass
{
namespace test
{


void testNumFilteredFloat()
{
	typedef lass::num::FilteredFloat<float> FloatFiltered;
	typedef lass::num::FilteredFloat<double> DoubleFiltered;

	for (int i=0;i<100;++i)
	{
		FloatFiltered	a(1.0);
		FloatFiltered	b(lass::num::pow(float(2.0),-float(i)));
		//std::cout << a.value() << "+" << b.value() << "=";
        a+=b;
		//std::cout << a.value() << "," << a.error() << "\n";
		LASS_ASSERT(a.value()>a.error());
	}
}



}

}

#endif

// EOF
