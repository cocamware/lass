/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 */



/** @struct lass::util::CallTraits
 *  @brief defines typedefs for function usage
 *  @author Bram de Greve [BdG]
 *
 *  Pretty much a copy of boost::call_traits<> (boost/call_traits.hpp, http://www.boost.org).
 *
 *  <i>(C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
 *	Use, modification and distribution are subject to the Boost Software License,
 *	Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt ).</i>
 *
 *  @section documentation
 *
 *  To qoute boost (http://www.boost.org/libs/utility/call_traits.htm): 
 *
 *  <i>"The template class call_traits<T> encapsulates the "best" method to pass a
 *  parameter of some type T to or from a function, and consists of a collection of typedefs 
 *  defined as in the table below. The purpose of call_traits is to ensure that problems like 
 *  "references to references" never occur, and that parameters are passed in the most efficient 
 *  manner possible (see examples). In each case if your existing practice is to use the type 
 *  defined on the left, then replace it with the call_traits defined type on the right."</i>
 *
 *  @warning guess what, stupid MSVC70 doesn't support partial template specialization.  So we "could"
 *           work around it, but we're not gonna do that.  WHAT?  Don't worry, we do have a sollution
 *           for you: use Boost :)  OK, so depending on what you've defined in config.h, we'll 
 *           implement our call traits by using boost::call_traits<>.  Cool huh?  Well yeah, a little.
 *           But anyway, even if you don't have Boost, or you don't want to use it, you're not 
 *           completely lost. However, you'll only get the "default" implementation of CallTraits<>.
 *           This might not give you the best performance, but it's better that no performance at all ;)
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALL_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALL_TRAITS_H

#include "util_common.h"

// To boost or not to boost ...
#ifdef LASS_BOOST_ALWAYS
#	pragma LASS_NOTE("set to always use boost::call_traits<> implementation.")
#	define LASS_UTIL_CALL_TRAITS_BOOST
#else
#	ifdef LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#		ifdef LASS_BOOST_IF_BROKEN
#			pragma LASS_NOTE("broken compiler: fall back on boost::call_traits<> implementation.")
#			define LASS_UTIL_CALL_TRAITS_BOOST
#		else
#			pragma LASS_NOTE("broken compiler: no fall back, use suboptimal basic implementation.")
#		endif
#	endif
#endif

#ifdef LASS_UTIL_CALL_TRAITS_BOOST
#	include <boost/call_traits.hpp>
#endif


namespace lass
{

namespace util
{

#ifndef LASS_UTIL_CALL_TRAITS_BOOST 



// Let's roll our own call traits.

template<typename T>
struct CallTraits
{
	/** Defines a type that represents the "value" of type T. 
	 *  Use this for functions that return by value, or possibly for stored values of type T.
     *
     *  @note If T is an array type, then CallTraits defines TValue as a "constant pointer to 
     *  type" rather than an "array of type" (requires partial specialization, or boost). Note that
     *  if you are using TValue as a stored value then this will result in storing a "constant 
     *  pointer to an array" rather than the array itself. This may or may not be a good thing 
     *  depending upon what you actually need (in other words take care!). 
	 */
	typedef T TValue;

	/** Defines a type that represents a reference to type T. 
	 *  Use for functions that would normally return a T&.
     *  
     *  @note If T is already reference type, then CallTraits is defined such that "references to 
     *  references" do not occur (requires partial specialization, or boost). 
	 */
	typedef T& TReference;

	/** Defines a type that represents a constant reference to type T. 
	 *  Use for functions that would normally return a const T&.
     *  
     *  @note If T is already reference type, then CallTraits is defined such that "references to 
     *  references" do not occur (requires partial specialization, or boost). 
	 */
	typedef const T& TConstReference;

	/** Defines a type that represents the "best" way to pass a parameter of type T to a function.
     *
     *  @note If T is a small built in type or a pointer, then TParam is defined as T const, 
     *  instead of T const&. This can improve the ability of the compiler to optimize loops in the 
     *  body of the function if they depend upon the passed parameter, the semantics of the passed 
     *  parameter is otherwise unchanged (requires partial specialization). 
     *
     *  @note If T is already reference type, then CallTraits is defined such that "references to 
     *  references" do not occur (requires partial specialization, or boost). 
	 */
	typedef const T& TParam;
};



#else // !LASS_UTIL_CALL_TRAITS_BOOST



// steal Boost's call traits

template<typename T>
struct CallTraits
{
	/** Defines a type that represents the "value" of type T. 
	 *  Use this for functions that return by value, or possibly for stored values of type T.
	 */
	typedef boost::call_traits<T>::value_type TValue;

	/** Defines a type that represents a reference to type T. 
	 *  Use for functions that would normally return a T&.
	 */
	typedef boost::call_traits<T>::reference TReference;

	/** Defines a type that represents a constant reference to type T. 
	 *  Use for functions that would normally return a const T&.
	 */
	typedef boost::call_traits<T>::const_reference TConstReference;

	/** Defines a type that represents the "best" way to pass a parameter of type T to a function.
	 */
	typedef boost::call_traits<T>::param_type TParam;
};



#endif // !LASS_UTIL_CALL_TRAITS_BOOST

}

}

#include "call_traits.inl"	// specializations

#endif
