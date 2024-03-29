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
 *	Copyright (C) 2004-2022 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_ID_GENERATOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_ID_GENERATOR_INL

#include "id_generator.h"
#include "../num/num_traits.h"



namespace lass
{

namespace util
{

/** Construct an ID generator of which @a iFirstId is the first ID to be generated.
 */
template <typename T>
IdGenerator<T>::IdGenerator( typename CallTraits<T>::TParam iFirstId ):
	nextId_(iFirstId)
{
}



/** Generate an ID.
 *  Each call to this operator will give you a new ID incremented by one.
 *  @throw an exception is thrown if the generator can no longer increment the id field.
 *      Since the generator stores the id that will be returned on the next call, this means
 *      the maximum id returned will be @c num::NumTraits<T>::max-1
 */
template <typename T>
typename CallTraits<T>::TValue IdGenerator<T>::operator()()
{
	using TValue = typename CallTraits<T>::TValue;
	TValue currentId = nextId_.load(std::memory_order_acquire);
	TValue nextId {};
	do
	{
		if (currentId == num::NumTraits<TValue>::max)
		{
			LASS_THROW("ID overflow.  Can no longer increment ID field.");
		}
		nextId = static_cast<TValue>(currentId + 1);
	}
	while (!nextId_.compare_exchange_weak(currentId, nextId,
		std::memory_order_release, std::memory_order_relaxed));
	return currentId;
}



/** set the next ID to be drawn.
 */
template <typename T>
void IdGenerator<T>::setNext( typename CallTraits<T>::TParam iNextId )
{
	nextId_ = iNextId;
}



}

}

#endif
