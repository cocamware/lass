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



/** @defgroup extended_iterator ExtendedIterator
 *  @brief extra iterator functionality
 *  @author Bram de Greve [BdG]
 *
 *  @section next_prior next() and prior()
 *
 *  These are copied from the boost libary, and are original written by Dave Abrahams and 
 *  Daniel Walker (boost/next_prior.hpp)
 *
 *  <i>(C) Copyright Boost.org 1999-2003. Permission to copy, use, modify, sell
 *  and distribute this software is granted provided this copyright
 *  notice appears in all copies. This software is provided "as is" without
 *  express or implied warranty, and with no claim as to its suitability for
 *  any purpose.</i>
 *
 *  @subsection next_prior_doc documentation
 *
 *  This documentation is copied (and adapted) from boost 
 *  (http://www.boost.org/libs/utility/utility.htm)
 *
 *  Certain data types, such as the C++ Standard Library's forward and bidirectional iterators, 
 *  do not provide addition and subtraction via operator+() or operator-().  This means that 
 *  non-modifying computation of the next or prior value requires a temporary, even though 
 *  operator++() or operator--() is provided.  It also means that writing code like itr+1 inside a 
 *  template restricts the iterator category to random access iterators.
 *
 *  The next() and prior() functions provide a simple way around these problems.  Usage is simple:
 *
 *  @code
 *	const std::list<T>::iterator p = get_some_iterator();
 *	const std::list<T>::iterator prev = stde::prior(p);
 *	const std::list<T>::iterator next = stde::next(prev, 2);
 *  @endcode
 *
 *	The distance from the given iterator should be supplied as an absolute value. For example, the 
 *  iterator four iterators prior to the given iterator p may be obtained by prior(p, 4).
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_ITERATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_ITERATOR_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

template <typename Iterator> Iterator next(Iterator iterator);
template <typename Iterator> Iterator prior(Iterator iterator);
template <typename Iterator, typename Distance> Iterator next(Iterator iterator, Distance distance);
template <typename Iterator, typename Distance> Iterator prior(Iterator iterator, Distance distance);



}

}

#include "extended_iterator.inl"

#endif

// EOF
