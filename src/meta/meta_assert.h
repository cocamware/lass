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
 *
 *	@par original code by Andrei Alexandrescu:
 *	The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *	This code (Loki) accompanies the book:\n
 *	Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied". 
 *	Copyright (c) 2001. Addison-Wesley.\n
 *	<i>Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is 
 *	hereby granted without fee, provided that the above copyright notice appear in all copies and 
 *	that both that copyright notice and this permission notice appear in supporting documentation.\n
 *	The author or Addison-Wesley Longman make no representations about the suitability of this 
 *	software (Loki) for any purpose. It is provided "as is" without express or implied warranty.</i>
 */



/** @def LASS_META_ASSERT
 *  @brief complite time static check
 *  @author Bram de Greve [BdG]
 *
 *  @param expression__ the meta expression to be evaluated, should evaluate to @c bool value.
 *  @param message__ the_message_to_be_displayed_if_the_assertion_failed.  This message should 
 *					obey the rules of a valid type name.  You must concatenate different words
 *                  with underscores.
 *
 *  This macro checks if an expression evaluates true at compile time.  If so, nothing happens.
 *  But if it fails, the compiler will generate some error mentioning @a iMessage.  In fact,
 *  it will complain about something else (causing the compiler error), but while it does so,
 *  it will generate you message.  Cunning huh?
 *  
 *  Here's how it looks like:
 *
 *  @code
 *  LASS_META_ASSERT(1 + 1 == 3, it_s_the_end_of_the_world);
 *
 *  // MSVC 7.0 compiler error:
 *  // error C2087: 'LASS_META_ERROR_it_s_the_end_of_the_world' : missing subscript
 *
 *  // Intel 700 compiler error:
 *  // error: an array may not have elements of this type LASS_META_ASSERT(1 + 1 == 3, it_s_the_end_of_the_world);
 *  @endcode
 *
 *	@par original code by Andrei Alexandrescu:
 *	The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *	This code (Loki) accompanies the book:\n
 *	Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied". 
 *	Copyright (c) 2001. Addison-Wesley.\n
 *	<i>Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is 
 *	hereby granted without fee, provided that the above copyright notice appear in all copies and 
 *	that both that copyright notice and this permission notice appear in supporting documentation.\n
 *	The author or Addison-Wesley Longman make no representations about the suitability of this 
 *	software (Loki) for any purpose. It is provided "as is" without express or implied warranty.</i>
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_META_ASSERT_H
#define LASS_GUARDIAN_OF_INCLUSION_META_META_ASSERT_H

#include "meta_common.h"

namespace lass
{
namespace meta
{
namespace impl
{

template<int> struct MetaAssertor;
template<> struct MetaAssertor<true> {};

}
}
}

#define LASS_META_ASSERT(expression__, message__) \
{\
	lass::meta::impl::MetaAssertor<((expression__) != 0)> ERROR_##message__;\
	(void)ERROR_##message__;\
} 

/*
#define LASS_META_ASSERT(iExpression, iMessage) \
	typedef char LASS_META_ERROR_##iMessage[1][(iExpression)]
*/

#endif

// EOF
