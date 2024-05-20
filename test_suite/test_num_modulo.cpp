/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2024 the Initial Developer.
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

#include "../lass/num/modulo.h"

namespace lass
{
namespace test
{

template <typename T>
void testNumModulo3()
{
    using TModulo = num::Modulo<3, T>;

    TModulo a, b;
    LASS_TEST_CHECK_EQUAL(a, 0);
    LASS_TEST_CHECK_EQUAL(a.value(), 0);

    ++a;
    LASS_TEST_CHECK_EQUAL(a, 1);
    LASS_TEST_CHECK_EQUAL(a.value(), 1);
    a++;
    LASS_TEST_CHECK_EQUAL(a, 2);
    a += 2;
    LASS_TEST_CHECK_EQUAL(a, 1);
    a += 3;
    LASS_TEST_CHECK_EQUAL(a, 1);
    a += 40;
    LASS_TEST_CHECK_EQUAL(a, 2);

    --a;
    LASS_TEST_CHECK_EQUAL(a, 1);
    a--;
    LASS_TEST_CHECK_EQUAL(a, 0);
    a -= 2;
    LASS_TEST_CHECK_EQUAL(a, 1);
    a -= 3;
    LASS_TEST_CHECK_EQUAL(a, 1);
    a -= 40;
    LASS_TEST_CHECK_EQUAL(a, 0);

    a = 5;
    LASS_TEST_CHECK_EQUAL(a, 2);

    LASS_TEST_CHECK_EQUAL(+a, 2);
    LASS_TEST_CHECK_EQUAL(-a, 1);

    //a = 2;
    //LASS_TEST_CHECK_EQUAL(a, 2);
    //a *= 19; // = 38%3 = 2
    //LASS_TEST_CHECK_EQUAL(a, 2);
    a = 2;
    a *= TModulo(19); // *= 1
    LASS_TEST_CHECK_EQUAL(a, 2);

    a = 1;
    b = 1;
    LASS_TEST_CHECK(a == b);
    LASS_TEST_CHECK(!(a != b));
    LASS_TEST_CHECK(!(a < b));
    LASS_TEST_CHECK(!(a > b));
    LASS_TEST_CHECK(a <= b);
    LASS_TEST_CHECK(a >= b);

    b = 2;
    LASS_TEST_CHECK(!(a == b));
    LASS_TEST_CHECK(a != b);
    LASS_TEST_CHECK(a < b);
    LASS_TEST_CHECK(!(a > b));
    LASS_TEST_CHECK(a <= b);
    LASS_TEST_CHECK(!(a >= b));

    b = 0;
    LASS_TEST_CHECK(!(a == b));
    LASS_TEST_CHECK(a != b);
    LASS_TEST_CHECK(!(a < b));
    LASS_TEST_CHECK(a > b);
    LASS_TEST_CHECK(!(a <= b));
    LASS_TEST_CHECK(a >= b);

    {
        a = 2;
        std::ostringstream os;
        os << a;
        LASS_TEST_CHECK(static_cast<bool>(os));
        LASS_TEST_CHECK_EQUAL(os.str(), "2");
    }

    {
        std::istringstream is("1");
        is >> a;
        LASS_TEST_CHECK(static_cast<bool>(is));
        LASS_TEST_CHECK_EQUAL(a, 1);
    }
}


TUnitTest test_num_modulo()
{
	return  TUnitTest{
        LASS_TEST_CASE(testNumModulo3<int>),
    };
}



}

}

// EOF
