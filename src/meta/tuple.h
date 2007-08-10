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

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_TUPLE_H
#define LASS_GUARDIAN_OF_INCLUSION_META_TUPLE_H

#include "meta_common.h"
#include "type_list.h"
#include "empty_type.h"

namespace lass
{
namespace meta
{

template <typename TList> struct Tuple;

template <typename H, typename Ts>
class Tuple< TypeList<H, Ts> >: public Tuple<Ts>
{
public:
	typedef H TValue;
	typedef Tuple<Ts> TTail;
	typedef TypeList<H, Ts> TList;
	Tuple(): value_() {}
	const H& value() const { return value_; }
	H& value() { return value_; }
private:
	H value_;
};

template <typename Ts>
class Tuple< TypeList<meta::EmptyType, Ts> >: public Tuple<Ts>
{
public:
	typedef meta::EmptyType TValue;
	typedef Tuple<Ts> TTail;
	typedef TypeList<meta::EmptyType, Ts> TList;
	const meta::EmptyType& value() const { return meta::EmptyType::instance(); }
	meta::EmptyType& value() { return meta::EmptyType::instance(); }
};

template <>
struct Tuple<meta::NullType>
{
	typedef meta::NullType TValue;
	typedef meta::NullType TTail;
	typedef meta::NullType TList;
};



namespace tuple
{

template <typename TupleType, size_t index>	
struct Field
{
	typedef typename type_list::At<typename TupleType::TList, index>::Type Type;
};

template <typename TupleType, size_t index>	
struct Field<const TupleType, index>
{
	typedef const typename type_list::At<typename TupleType::TList, index>::Type Type;
};



template <typename TupleType, size_t index> struct SubType;

template <typename H, typename Ts, size_t index> 
struct SubType< Tuple< TypeList<H, Ts> >, index >: public SubType< Tuple<Ts>, index - 1 >
{
};

template <typename H, typename Ts> 
struct SubType< Tuple< TypeList<H, Ts> >, 0 >
{
	typedef Tuple< TypeList<H, Ts> > Type;
};

template <typename H, typename Ts, size_t index> 
struct SubType< const Tuple< TypeList<H, Ts> >, index >: public SubType< const Tuple<Ts>, index - 1 >
{
};

template <typename H, typename Ts> 
struct SubType< const Tuple< TypeList<H, Ts> >, 0 >
{
	typedef const Tuple< TypeList<H, Ts> > Type;
};



template <size_t index, typename TupleType> 
typename Field<TupleType, index>::Type& field(TupleType& tuple)
{
	typedef typename SubType<TupleType, index>::Type TSubType;
	return static_cast<TSubType&>(tuple).value();
}

}

}
}

#endif

// EOF
