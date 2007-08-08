/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_TYPE_LIST_H
#define LASS_GUARDIAN_OF_INCLUSION_TYPE_LIST_H

#include "meta_common.h"
#include "null_type.h"

namespace lass
{
namespace meta
{

template <typename Head, typename Tail>
struct TypeList
{
	typedef Head THead;
	typedef Tail TTail;
	typedef TypeList<Head, Tail> Type;
};

namespace type_list
{

template 
<
	typename T1 = NullType, typename T2 = NullType, typename T3 = NullType, typename T4 = NullType, 
	typename T5 = NullType, typename T6 = NullType, typename T7 = NullType, typename T8 = NullType,
	typename T9 = NullType, typename T10 = NullType, typename T11 = NullType, typename T12 = NullType,
	typename T13 = NullType, typename T14 = NullType, typename T15 = NullType, typename T16 = NullType
>
struct Make
{
	typedef TypeList
	<
		T1, 
		typename Make<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::Type
	> Type;
};

template<>
struct Make<>
{
	typedef NullType Type;
};



template <typename Ts, typename X> struct Contains;

template <typename H, typename Ts, typename X> 
struct Contains<TypeList<H, Ts>, X>: public Contains<Ts, X> {};

template <typename Ts, typename X>
struct Contains<TypeList<X, Ts>, X>: public True {};

template <typename X>
struct Contains<NullType, X>: public False {};



template <typename Ts, size_t i> struct At;

template <typename H, typename Ts, size_t i>
struct At<TypeList<H, Ts>, i>: public At<Ts, i - 1> {};

template <typename H, typename Ts>
struct At<TypeList<H, Ts>, 0>
{
	typedef H Type;
};


}
}
}

#endif

// EOF
