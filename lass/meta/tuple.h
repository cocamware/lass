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

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_TUPLE_H
#define LASS_GUARDIAN_OF_INCLUSION_META_TUPLE_H

#include "meta_common.h"
#include "type_list.h"
#include "empty_type.h"

namespace lass
{
namespace meta
{

template <typename TList> class Tuple;

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
class Tuple<meta::NullType>
{
public:
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



namespace impl
{
	template <typename TupleType> struct ForEach;
	template <typename H, typename Ts>
	struct ForEach< Tuple< TypeList<H, Ts> > >
	{
		template <typename Functor> static void call(Tuple< TypeList<H, Ts> >& tuple, Functor& fun)
		{
			fun(tuple.value());
			ForEach< Tuple<Ts> >::call(tuple, fun);
		}
		template <typename Functor> static void call(const Tuple< TypeList<H, Ts> >& tuple, Functor& fun)
		{
			fun(tuple.value());
			ForEach< Tuple<Ts> >::call(tuple, fun);
		}
	};
	template <typename Ts>
	struct ForEach< Tuple< TypeList<meta::EmptyType, Ts> > >
	{
		template <typename Functor> static void call(Tuple< TypeList<meta::EmptyType, Ts> >& tuple, Functor& fun)
		{
			ForEach< Tuple<Ts> >::call(tuple, fun);
		}
		template <typename Functor> static void call(const Tuple< TypeList<meta::EmptyType, Ts> >& tuple, Functor& fun)
		{
			ForEach< Tuple<Ts> >::call(tuple, fun);
		}
	};
	template <>
	struct ForEach< Tuple< meta::NullType> >
	{
		template <typename Functor> static void call(Tuple<meta::NullType>& tuple, Functor& fun)
		{
		}
		template <typename Functor> static void call(const Tuple<meta::NullType>& tuple, Functor& fun)
		{
		}
	};
}

template <typename TupleType, typename Functor>
void forEach(TupleType& tuple, Functor& fun)
{
	impl::ForEach<TupleType>::call(tuple, fun);
}

template <typename TupleType, typename Functor>
void forEach(const TupleType& tuple, Functor& fun)
{
	impl::ForEach<TupleType>::call(tuple, fun);
}



}

}
}

#endif

// EOF
