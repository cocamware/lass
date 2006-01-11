/** @file
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
	const typename CallTraits<T>::TValue result = nextId_;
	++nextId_;
	if (nextId_ < result)
	{
		LASS_THROW("ID overflow.  Can no longer increment ID field.");
	}
	return result;
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
