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

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_TUPLE_H
#define LASS_GUARDIAN_OF_INCLUSION_META_TUPLE_H

#include "meta_common.h"
#include "type_list.h"

namespace lass
{
namespace meta
{

template <typename TList> struct Tuple;

template <typename Head, typename Tail>
struct Tuple< TypeList<Head, Tail> >: Tuple<Tail>
{
	typedef TypeList<Head, Tail> TList;
	Head value;
	Tuple(): Tuple<Tail>(), value() {}
};

template <typename Tail>
struct Tuple< TypeList<meta::NullType, Tail> >: Tuple<Tail>
{
	typedef TypeList<meta::NullType, Tail> TList;
	Tuple(): Tuple<Tail>() {}
};

template <>
struct Tuple< meta::NullType >
{
	typedef meta::NullType TList;
};

namespace tuple
{
namespace impl
{
	template <typename TupleType, size_t i, typename ReturnType> struct Accessor;

	template <typename Head, typename Tail, size_t i, typename ReturnType>
	struct Accessor<Tuple< TypeList<Head, Tail> >, i, ReturnType>
	{
		typedef Tuple< TypeList<Head, Tail> > TTuple;
		static ReturnType& access(TTuple& tuple)
		{
			typedef Tuple<Tail> TTailTuple;
			TTailTuple& tail = static_cast<TTailTuple&>(tuple);
			return Accessor<TTailTuple, i - 1, ReturnType>::access(tuple);
		}
		static const ReturnType& access(const TTuple& tuple)
		{
			typedef Tuple<Tail> TTailTuple;
			const TTailTuple& tail = static_cast<const TTailTuple&>(tuple);
			return Accessor<TTailTuple, i - 1, ReturnType>::access(tuple);
		}
	};

	template <typename Head, typename Tail, typename ReturnType>
	struct Accessor<Tuple< TypeList<Head, Tail> >, 0, ReturnType>
	{
		typedef Tuple< TypeList<Head, Tail> > TTuple;
		static ReturnType& access(TTuple& tuple) { return tuple.value; }
		static const ReturnType& access(const TTuple& tuple) { return tuple.value; }
	};

	template <typename Tail, typename ReturnType>
	struct Accessor<Tuple< TypeList<NullType, Tail> >, 0, ReturnType>
	{
		typedef Tuple< TypeList<NullType, Tail> > TTuple;
		static ReturnType& access(TTuple& tuple) { return NullType::Null(); }
		static const ReturnType& access(const TTuple& tuple) { return NullType::Null(); }
	};
}

template <typename TupleType, size_t index>	struct Field:
	public type_list::At<typename TupleType::TList, index> {};

template <size_t index, typename TupleType> 
typename Field<TupleType, index>::Type& field(TupleType& tuple)
{
	typedef typename Field<TupleType, index>::Type TReturn;
	return impl::Accessor<TupleType, index, TReturn>::access(tuple);
}

template <size_t index, typename TupleType> 
const typename Field<TupleType, index>::Type& field(const TupleType& tuple)
{
	typedef typename Field<TupleType, index>::Type TReturn;
	return impl::Accessor<TupleType, index, TReturn>::access(tuple);
}


}

}
}

#endif

// EOF
