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



/** @class lass::meta::IsConvertibleType
 *  @brief meta function that evaluates true if a type is convertible to another
 *  @author Bram de Greve [BdG]
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_IS_DERIVED_TYPE_H
#define LASS_GUARDIAN_OF_INCLUSION_META_IS_DERIVED_TYPE_H

#include "meta_common.h"
#include "is_convertible_type.h"
#include "is_same_type.h"

namespace lass
{
namespace meta
{

template
<
	typename DerivedType, 
	typename BaseType
>
struct IsDerivedType
{
    enum 
    { 
        value = IsConvertibleType<const DerivedType*, const BaseType*>::value &&
                !IsSameType<const BaseType*, const void*>::value
    };                   
};

template
<
	typename DerivedType, 
	typename BaseType
>
struct IsDerivedTypeStrict
{
    enum 
    { 
        value = IsDerivedType<DerivedType, BaseType>::value &&
                !IsSameType<const DerivedType, const BaseType>::value
    };     
};

}

}

#endif

// EOF