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



/** @class lass::meta::IsSameType
 *  @brief meta function that evaluates true if two types are the same
 *  @author Bram de Greve [BdG]
 *
 *  @code
 *  bool a = lass::meta::IsSameType<int, int>::value; // a = true;
 *  bool b = lass::meta::IsSameType<int, float>::value; // b = false;
 *
 *  // you can also evaluate to meta::True and meta::False
 *  typedef lass::meta::IsSameType<std::string, std::string>::Type C; // C = meta::True
 *  @encode
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_IS_SAME_TYPE_H
#define LASS_GUARDIAN_OF_INCLUSION_META_IS_SAME_TYPE_H

#include "meta_common.h"
#include "bool.h"

namespace lass
{
namespace meta
{

#if !defined(LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template
<
	typename A,
	typename B
>
struct IsSameType
{
	enum { value = false };
    typedef False Type;
};

template
<
	typename A
>
struct IsSameType<A, A>
{
	enum { value = true };
    typedef True Type;
};

#else

template
<
	typename A,
	typename B
>
struct IsSameType
{
private:

	template <typename T>
	struct IsSameAsA
	{
		enum { value = false };
	};

	template <>
	struct IsSameAsA<A>
	{
		enum { value = true };
	};

public:

	enum { value = IsSameAsA<B>::value };
    typedef typename Bool<value>::Type Type;
};

#endif

}

}

#endif

// EOF
