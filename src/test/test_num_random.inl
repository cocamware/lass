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


 *  @date 2004
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_RANDOM_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_RANDOM_INL

#include "test_common.h"

#include "../num/random.h"

namespace lass
{
namespace test
{

void testNumRandomMersenne()
{
    // we use a test file provided by the mersenne twister authors.
    //
    boost::test_toolbox::output_test_stream output("mt19937ar.out");

    // just something in the file
    //
    output << "1000 outputs of genrand_int32()\n";
    BOOST_CHECK(output.match_pattern());       

    // initialize the generator with an array of 4 values
    //
    const num::RandomMT19937::TValue keys[4] = {0x123, 0x234, 0x345, 0x456}; 
    num::RandomMT19937 generator(keys, keys + 4);

    // test 1000 draws, 200 lines of 5.
    for (unsigned i = 0; i < 200; ++i)
    {
        for (unsigned j = 0; j < 5; ++j)
        {
            output << std::setw(10) << generator() << " ";
        }
        output << "\n";
    }
    BOOST_CHECK(output.match_pattern());            
}



}

}

#endif

// EOF
