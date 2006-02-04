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



/** @namespace lass::meta::type_list
 *  @brief meta members of TypeList
 *  @sa TypeList
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_TYPE_LIST_INL
#define LASS_GUARDIAN_OF_INCLUSION_META_TYPE_LIST_INL

#include "meta_common.h"
#include "type_list.h"
#include "../util/type_info.h"

namespace lass
{
namespace meta
{
namespace type_list
{

/** @class Make
 *  @brief maker meta funtion of a TypeList
 *  @sa TypeList
 */
template
<
	typename T0 = NullType, typename T1 = NullType, typename T2 = NullType,
	typename T3 = NullType, typename T4 = NullType, typename T5 = NullType,
	typename T6 = NullType, typename T7 = NullType, typename T8 = NullType,
	typename T9 = NullType, typename T10 = NullType, typename T11 = NullType,
	typename T12 = NullType, typename T13 = NullType, typename T14 = NullType,
	typename T15 = NullType, typename T16 = NullType, typename T17 = NullType,
	typename T18 = NullType, typename T19 = NullType, typename T20 = NullType
>
struct Make
{
	typedef TypeList
	<
		T0,
		typename Make
		<
			T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,
			T12, T13, T14, T15, T16, T17, T18, T19, T20
		>::Type
	>
	Type;
};

template <>
struct Make<>
{
	typedef NullType Type;
};



/** number of types in TypeList
 *  @sa TypeList
 */
template <typename TypeListType> struct Size;

template <>
struct Size<NullType>
{
	enum { value = 0 };
};

template <typename HeadType, typename TailType>
struct Size<TypeList<HeadType, TailType> >
{
	enum { value = Size<TailType>::value + 1 };
};



/** acces types by index
 *  @sa TypeList
 */
template <typename TypeListType, unsigned index> struct At;

template <typename HeadType, typename TailType>
struct At<TypeList<HeadType, TailType>, 0>
{
	typedef HeadType Type;
};

template <typename HeadType, typename TailType, unsigned index>
struct At<TypeList<HeadType, TailType>, index>
{
	typedef typename At<TailType, index - 1>::Type Type;
};



/** same as type_list::At but returns a @c DefaultType if index is out of range.
 *  @sa TypeList
 */
template <typename TypeListType, unsigned index, typename DefaultType = NullType>
struct AtNonStrict
{
	typedef DefaultType Type;
};

template <typename HeadType, typename TailType, typename DefaultType>
struct AtNonStrict<TypeList<HeadType, TailType>, 0, DefaultType>
{
	typedef HeadType Type;
};

template <typename HeadType, typename TailType, unsigned index, typename DefaultType>
struct AtNonStrict<TypeList<HeadType, TailType>, index, DefaultType>
{
	typedef typename AtNonStrict<TailType, index - 1, DefaultType>::Type Type;
};



/** returns index of type in typelist, or -1 if not present
 *  @sa TypeList
 */
template <typename TypeListType, typename T> struct Find;

template <typename T>
struct Find<NullType, T>
{
	enum { value = -1 };
};

template <typename TailType, typename T>
struct Find<TypeList<T, TailType>, T>
{
	enum { value = 0 };
};

template <typename HeadType, typename TailType, typename T>
struct Find<TypeList<HeadType, TailType>, T>
{
private:
	enum { temp = Find<TailType, T>::value };
public:
	enum { value = temp == -1 ? -1 : temp + 1 };
};



/** merges two type lists.
 *  @sa TypeList
 */
template <typename TypeListType, typename T> struct Merge;

template <>
struct Merge<NullType, NullType>
{
	typedef NullType Type;
};

template <typename T>
struct Merge<NullType, T>
{
	typedef TypeList<T, NullType> Type;
};

template <typename HeadType, typename TailType>
struct Merge<NullType, TypeList<HeadType, TailType> >
{
	typedef TypeList<HeadType, TailType> Type;
};

template <typename HeadType, typename TailType, typename T>
struct Merge<TypeList<HeadType, TailType>, T>
{
	typedef TypeList<HeadType, typename Merge<TailType, T>::Type> Type;
};



/** removes all occurences of a type in a TypeList
 *  @sa TypeList
 */
template <typename TypeListType, typename T> struct Remove;

template <typename T>
struct Remove<NullType, T>
{
	typedef NullType Type;
};

template <typename TailType, typename T>
struct Remove<TypeList<T, TailType>, T>
{
	typedef typename Remove<TailType, T>::Type Type;
};

template <typename HeadType, typename TailType, typename T>
struct Remove<TypeList<HeadType, TailType>, T>
{
	typedef TypeList<HeadType, typename Remove<TailType, T>::Type> Type;
};



/** Replace in a TypeList all @c OldType by @c NewType
 */
template <typename TypeListType, typename OldType, typename NewType> struct Replace;

template <typename OldType, typename NewType>
struct Replace<NullType, OldType, NewType>
{
	typedef NullType Type;
};

template <typename TailType, typename OldType, typename NewType>
struct Replace<TypeList<OldType, TailType>, OldType, NewType>
{
	typedef TypeList<NewType, typename Replace<TailType, OldType, NewType>::Type> Type;
};

template <typename HeadType, typename TailType, typename OldType, typename NewType>
struct Replace<TypeList<HeadType, TailType>, OldType, NewType>
{
	typedef TypeList<HeadType, typename Replace<TailType, OldType, NewType>::Type> Type;
};



/** Removes all duplicate types from a TypeList
 *  @sa TypeList
 */
template <typename TypeListType> struct Unique;

template <>
struct Unique<NullType>
{
	typedef NullType Type;
};

template <typename HeadType, typename TailType>
struct Unique<TypeList<HeadType, TailType> >
{
private:
	typedef typename Remove<TailType, HeadType>::Type TStrippedTail;
public:
	typedef TypeList<HeadType, typename Unique<TStrippedTail>::Type> Type;
};



/** fronts becomes back and back becomes front
 *  @sa TypeList
 */
template <typename TypeList> struct Reverse;

template <>
struct Reverse<NullType>
{
	typedef NullType Type;
};

template <typename HeadType, typename TailType>
struct Reverse<TypeList<HeadType, TailType> >
{
private:
	typedef typename Reverse<TailType>::Type TTempHead;
public:
	typedef typename Merge<TTempHead, HeadType>::Type Type;
};




// --- runtime algorithms --------------------------------------------------------------------------

template <typename TypeListType> struct RunTimeFinder {};

/** returns index of type in typelist at runtime, or -1 if not present
 *
 *  @sa TypeList
 */
template <typename TypeListType> int find(const util::TypeInfo& iType)
{
	return RunTimeFinder<TypeListType>::result(iType);
}


template <>
struct RunTimeFinder<NullType>
{
	static int result(const util::TypeInfo& /*iType*/)
	{
		return -1;
	}
};

template <typename HeadType, typename TailType>
struct RunTimeFinder<TypeList<HeadType, TailType> >
{
	static int result(const util::TypeInfo& iType)
	{
		if (typeid(HeadType) == iType.type_info())
		{
			return 0;
		}
		const int result = find< TailType >(iType);
		return result == -1 ? -1 : result + 1;
	}
};

}

}

}

#endif

// EOF
