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



/** @namespace lass::meta
 *	@brief library for template meta programming
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
 *  - If: selects a type based on the value of a compile time @c bool.
 *  - MakeTypeList: creates a TypeList
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
#pragma once



#include "../lass_common.h"

// Since no static lib is build, we shouldn't try to link to it.  
// Uncomment this if this changes [BdG]
//
///** name of library
// */
//#define LASS_LIB_META LASS_LIB_PREFIX "meta" LASS_LIB_SUFFIX
//#pragma comment(lib, LASS_LIB_META)



#endif

