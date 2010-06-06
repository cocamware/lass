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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

#include "lass_common.h"
#include "floating_point_consistency.h"

namespace lass
{
namespace num
{

#define LASS_NUM_FLOATING_POINT_CONSISTENCY_DEFINE_NUMTRAITS(type)\
	const int NumTraits< Consistent< type > >::memorySize = sizeof(Consistent< type >);\
	const Consistent< type > NumTraits< Consistent< type > >::one = Consistent< type >(NumTraits< type >::one);\
	const Consistent< type > NumTraits< Consistent< type > >::zero = Consistent< type >(NumTraits< type >::zero);\
	const Consistent< type > NumTraits< Consistent< type > >::qNaN = Consistent< type >(NumTraits< type >::qNaN);\
	const Consistent< type > NumTraits< Consistent< type > >::sNaN = Consistent< type >(NumTraits< type >::sNaN);\
	const Consistent< type > NumTraits< Consistent< type > >::infinity = Consistent< type >(NumTraits< type >::infinity);\
	const Consistent< type > NumTraits< Consistent< type > >::epsilon = Consistent< type >(NumTraits< type >::epsilon);\
	const Consistent< type > NumTraits< Consistent< type > >::min = Consistent< type >(NumTraits< type >::min);\
	const Consistent< type > NumTraits< Consistent< type > >::max = Consistent< type >(NumTraits< type >::max);\
	const Consistent< type > NumTraits< Consistent< type > >::minStrictPositive = Consistent< type >(NumTraits< type >::minStrictPositive);\
	const Consistent< type > NumTraits< Consistent< type > >::pi = Consistent< type >(NumTraits< type >::pi);\
	const Consistent< type > NumTraits< Consistent< type > >::e = Consistent< type >(NumTraits< type >::e);\
	const Consistent< type > NumTraits< Consistent< type > >::sqrt2 = Consistent< type >(NumTraits< type >::sqrt2);\
	const Consistent< type > NumTraits< Consistent< type > >::sqrtPi = Consistent< type >(NumTraits< type >::sqrtPi);
 
LASS_NUM_FLOATING_POINT_CONSISTENCY_DEFINE_NUMTRAITS(float)
LASS_NUM_FLOATING_POINT_CONSISTENCY_DEFINE_NUMTRAITS(double)
LASS_NUM_FLOATING_POINT_CONSISTENCY_DEFINE_NUMTRAITS(long double)

}
}

// EOF
