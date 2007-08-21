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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_COMPOSE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_STDE_COMPOSE_INL

#include "test_common.h"
#include "../stde/compose.h"
#include "../stde/select.h"
#include "../stde/integral_range.h"
#include "../stde/range_algorithm.h"

namespace lass
{
namespace test
{

void testStdeCompose()
{
	TestStream stream;

	std::vector<int> a;
	stde::transform_r(stde::integral_range(5), std::back_inserter(a),
		stde::compose_f_gx(
			std::negate<int>(),
			std::bind2nd(std::multiplies<int>(), 2)));	
	LASS_TEST_CHECK_LEXICAL(a, "[0, -2, -4, -6, -8]");

	typedef std::pair<int, int> pair_type;
	std::vector<pair_type> c;
	std::vector<int> d;
	for (int i = 0; i < 5; ++i)
	{
		c.push_back(std::make_pair(10 * i, i));
	}
	stde::transform_r(c, std::back_inserter(d),
		stde::compose_f_gx_hx(
			std::plus<int>(),
			stde::select_1st<pair_type>(),
			stde::select_2nd<pair_type>()));
	LASS_TEST_CHECK_LEXICAL(d, "[0, 11, 22, 33, 44]");
}

}

}

#endif

// EOF
