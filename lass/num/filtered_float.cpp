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
 *	Copyright (C) 2004-2011 the Initial Developer.
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
#include "filtered_float.h"

namespace lass
{
namespace num
{

/*
#define LASS_NUM_EXTENDED_PRECISION_DEFINE_NUMTRAITS(type)\
	const int NumTraits< FilteredFloat< type > >::memorySize = sizeof(FilteredFloat< type >);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::one = FilteredFloat< type >(NumTraits< type >::one);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::zero = FilteredFloat< type >(NumTraits< type >::zero);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::qNaN = FilteredFloat< type >(NumTraits< type >::qNaN);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sNaN = FilteredFloat< type >(NumTraits< type >::sNaN);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::infinity = FilteredFloat< type >(NumTraits< type >::infinity);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::epsilon = FilteredFloat< type >(NumTraits< type >::epsilon);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::min = FilteredFloat< type >(NumTraits< type >::min);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::max = FilteredFloat< type >(NumTraits< type >::max);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::minStrictPositive = FilteredFloat< type >(NumTraits< type >::minStrictPositive);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::pi = FilteredFloat< type >(NumTraits< type >::pi);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::e = FilteredFloat< type >(NumTraits< type >::e);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sqrt2 = FilteredFloat< type >(NumTraits< type >::sqrt2);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sqrtPi = FilteredFloat< type >(NumTraits< type >::sqrtPi);
 */
#define LASS_NUM_EXTENDED_PRECISION_DEFINE_NUMTRAITS(type)\
	const int NumTraits< FilteredFloat< type > >::memorySize = sizeof(FilteredFloat< type >);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::one = FilteredFloat< type >(NumTraits< type >::one);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::zero = FilteredFloat< type >(NumTraits< type >::zero);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::qNaN = FilteredFloat< type >(NumTraits< type >::qNaN);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sNaN = FilteredFloat< type >(NumTraits< type >::sNaN);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::infinity = FilteredFloat< type >(NumTraits< type >::infinity);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::epsilon = FilteredFloat< type >(NumTraits< type >::epsilon);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::min = FilteredFloat< type >(NumTraits< type >::min);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::max = FilteredFloat< type >(NumTraits< type >::max);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::minStrictPositive = FilteredFloat< type >(NumTraits< type >::minStrictPositive);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::pi = FilteredFloat< type >(NumTraits< type >::pi);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::e = FilteredFloat< type >(NumTraits< type >::e);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sqrt2 = FilteredFloat< type >(NumTraits< type >::sqrt2);\
	const FilteredFloat< type > NumTraits< FilteredFloat< type > >::sqrtPi = FilteredFloat< type >(NumTraits< type >::sqrtPi);

LASS_NUM_EXTENDED_PRECISION_DEFINE_NUMTRAITS(float)
LASS_NUM_EXTENDED_PRECISION_DEFINE_NUMTRAITS(double)
LASS_NUM_EXTENDED_PRECISION_DEFINE_NUMTRAITS(long double)

}
}

// EOF
