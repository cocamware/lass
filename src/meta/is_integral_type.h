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



/** @class lass::meta::IsSameType
 *  @brief meta function that evaluates true a type is a standard integral type.
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_IS_INTEGRAL_TYPE_H
#define LASS_GUARDIAN_OF_INCLUSION_META_IS_INTEGRAL_TYPE_H

#include "meta_common.h"
#include "bool.h"
#include "type_list.h"

namespace lass
{
namespace meta
{

typedef type_list::Make
<
	signed char, signed short, signed int, signed long, 
	unsigned char, unsigned short, unsigned int, unsigned long, 
	bool, char, wchar_t
>
::Type TIntegralTypes;

template <typename T>
struct IsIntegralType
{
	enum { value = (type_list::Find<TIntegralTypes, T>::value != -1) };
	typedef typename Bool<value>::Type Type;
};



}

}

#endif

// EOF
