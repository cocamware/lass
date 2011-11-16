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



/** @defgroup extended_algorithm
 *  @brief extra algorithms
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_ALGORITHM_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_ALGORITHM_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

// --- pure iterator algorithms --------------------------------------------------------------------

/** copy @a count elements from sequence starting at @a first to sequence starting at @a result
 *	@ingroup extended_algorithm 
 *  @pre there should be enough room in output sequence to hold @a n elements
 */
template <class InputIterator, class Size, class OutputIterator>
OutputIterator copy_n(InputIterator first, Size count, 
                      OutputIterator result)
{
	for (Size i=0;i<count;++i)
		*result++ = *first++;
	return result;
}      

/** copy sequence @a first to @a last @a n times to sequence starting at @a output
 *	@ingroup extended_algorithm 
 *  @pre there should be enough room in output sequence to hold @a n times the input sequence.
 */
template <class InputIterator, class OutputIterator, class Size>
OutputIterator repeat(InputIterator first, InputIterator last, OutputIterator output, Size n)
{
	for (Size i = 0; i < n; ++i)
	{
		output = std::copy(first, last, output);
	}
	return output;
}


// --- container algorithms shortcuts --------------------------------------------------------------

/** @ingroup extended_algorithm 
 */ 
template <class Container, class Size> inline
Container repeat_c(const Container& iC, Size n)
{
	Container result;
	repeat(iC.begin(), iC.end(), std::back_inserter(result), n);
	return result;
}

/** @ingroup extended_algorithm 
 */ 
template <class Container, class Size> inline
Container& inplace_repeat_c(Container& iC, Size n)
{
	Container temp;
	repeat(iC.begin(), iC.end(), std::back_inserter(temp), n);
	iC.swap(temp);
	return iC;
}

}
}

#endif

// EOF
