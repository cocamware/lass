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
 *      @arg @c T must implement prefix increment, @c operator++
 *      @arg @c T must be less-than comparable, @c operator<
 *      @arg @c T must be copy-constructible
 */




#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_ID_GENERATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_ID_GENERATOR_H



#include "util_common.h"
#include "call_traits.h"

#include <atomic>


namespace lass
{

namespace util
{

template<typename T>
class IdGenerator
{
public:

	IdGenerator( typename CallTraits<T>::TParam iFirstId = 0 );

	typename CallTraits<T>::TValue operator()();

	void setNext( typename CallTraits<T>::TParam iNextId );

private:

	std::atomic<typename CallTraits<T>::TValue> nextId_;
};



}

}

#include "id_generator.inl"

#endif
