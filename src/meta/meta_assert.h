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
 *	Copyright (C) 2004-2009 the Initial Developer.
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



/** @def LASS_META_ASSERT
 *  @brief complite time static check
 *  @author Bram de Greve [BdG]
 *
 *  @param expression__ the meta expression to be evaluated, should evaluate to @c bool value.
 *  @param message__ the_message_to_be_displayed_if_the_assertion_failed.  This message should
 *                  obey the rules of a valid type name.  You must concatenate different words
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
 *  // error C2087: 'LASS_META_ASSERT_FAILURE_it_s_the_end_of_the_world' : missing subscript
 *
 *  // Intel 700 compiler error:
 *  // error: an array may not have elements of this type LASS_META_ASSERT(1 + 1 == 3, it_s_the_end_of_the_world);
 *  @endcode
 *
 *  @par original code by Andrei Alexandrescu:
 *  The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *  This code (Loki) accompanies the book:\n
 *  Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied".
 *  Copyright (c) 2001. Addison-Wesley.\n
 *  <i>Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is
 *  hereby granted without fee, provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear in supporting documentation.\n
 *  The author or Addison-Wesley Longman make no representations about the suitability of this
 *  software (Loki) for any purpose. It is provided "as is" without express or implied warranty.</i>
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

template<bool x> struct MetaAssertor;
template<> struct MetaAssertor<false> {};

template<int x> struct MetaAssertTest {};

}
}
}


#define LASS_META_ASSERT(expression__, message__)\
	typedef lass::meta::impl::MetaAssertTest<\
		sizeof(lass::meta::impl::MetaAssertor<((expression__) == 0)>)>\
	LASS_META_ASSERT_##message__

#endif

// EOF
