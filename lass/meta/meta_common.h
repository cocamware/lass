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



/** @namespace lass::meta
 *  @brief library for template meta programming
 *
 *  This library (and namespace) contains tools for meta programming.
 *
 *  @section overview
 *
 *  This library obeys the grand rules of lass' code style.  Types start with a capital, values
 *  don't.  They do so regardless if the type is in fact a meta value, a meta type or a meta
 *  function.  And that's a good thing.  That way Bool, True, False, If, ... start with a capital
 *  so they can exists in peace next to @c bool, @c true, @c false, @c if ... .
 *
 *  @subsection meta-constants
 *
 *  - True
 *  - False
 *
 *  @subsection meta-types
 *
 *  - NullType: the @e i'm-not-a-type type.
 *  - TypeList: A meta container of types
 *
 *  @subsection meta-functions
 *
 *  The result of a meta function can be a type or a value.  The former will be presented by
 *  a @c typedef called @c Type.  The latter will be an @c enum called @c value.  Again, style
 *  rules are obeyed: types start with a capital, values don't.
 *
 *  @code
 *  typedef typename AMetaFunctionReturningAType<Foo, Bar>::Type Type;
 *  enum { value = AMetaFunctionReturningAValue<Fun, Spam>::value };
 *  @endcode
 *
 *  - Bool: converts a @c bool value the the meta constants True and False.
 *  - If: executes one of two metafunctions based on the value of a compile time @c bool.
 *  - Int2Type: converts an integral value to a type.
 *  - IsConvertible: evaluates true if one type is convertible to another.
 *  - IsDerived: evaluates true if one type is derived from (or equal to) another.
 *  - IsDerivedTypeStruct: evaluates true if one type is derived from another and is not the same.
 *  - IsIntegral: evaluates true if a type is a standard integral type.
 *  - IsSame: evaluates true if two types are identical.
 *  - Select: selects one of two types based on a compile time @c bool.
 *  - Wrap: converts a type to a unique empty type.
 *
 *  @subscection traits
 *
 *  - TypeTraits: compile time information and operations on a single type
 *
 *  @subsection macros
 *
 *  - @ref LASS_META_ASSERT: compile time assert
 *
 *  @note Currently, this libary is headers only.  This means no static library needs to be
 *        build.  Hence, the meta project is skipped by the build and the automatic link
 *        to the lib is commented. [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_META_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_META_META_COMMON_H

#include "../lass_common.h"

#endif

// EOF
