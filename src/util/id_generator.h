/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @class lass::util::IdGenerator
 *  @brief Generates sequential and unique ID's
 *  @author Bram de Greve
 *  @date 2003
 *
 *  Pretty simple.  This object generates unique ID numbers, each incremented by one, starting from
 *  a given ID.
 *
 *  @code
 *  IdGenerator<unsigned> generator;
 *  LASS_COUT << generator() << "\n"; // will print '0'
 *  LASS_COUT << generator() << "\n"; // will print '1'
 *  LASS_COUT << generator() << "\n"; // will print '2'
 *  LASS_COUT << generator() << "\n"; // will print '3'
 *  ...
 *  @endcode
 *
 *  @par requirements for @c T:
 *		@arg @c T must implement prefix increment, @c operator++
 *		@arg @c T must be less-than comparable, @c operator<
 *		@arg @c T must be copy-constructible
 */




#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_ID_GENERATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_ID_GENERATOR_H
#pragma once


#include "util_common.h"
#include "call_traits.h"


namespace lass
{

namespace util
{

template<typename T>
class LASS_DLL_EXPORT IdGenerator
{
public:

	IdGenerator( typename CallTraits<T>::TParam iFirstId = 0 ) throw();

	typename CallTraits<T>::TValue operator()();

	void setNext( typename CallTraits<T>::TParam iNextId ) throw();

private:

	typename CallTraits<T>::TValue nextId_;
};



}

}

#include "id_generator.inl"

#endif
