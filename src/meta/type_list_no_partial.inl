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
 *
 *  This file contains a @e hacked implementation of lass::meta::TypeList for compilers that
 *  don't support partial template specialisation like MSVC70
 *
 *  @bug this whole no partial template specialisation is really screwed up.  don't try to use it!
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_TYPE_LIST_INL
#define LASS_GUARDIAN_OF_INCLUSION_META_TYPE_LIST_INL

#include "meta_common.h"
#include "type_list.h"
#include "select.h"
#include "is_same_type.h"
#include "type_2_type.h"

#define LASS_META_ASSERT_TYPE_LIST(Type)\
    typedef char foo[true]

    //LASS_META_ASSERT(impl::IsTypeList<Type>::value, argument_is_not_a_TypeList)

namespace lass
{
namespace meta
{
namespace type_list
{
namespace impl
{

struct TypeListTag {};
struct NullTypeTag {};
struct NoneListTag {};

enum 
{
    TypeList_ID = 1,
    NullType_ID = 2,
    NoneList_ID = 3
};

template <typename T>
struct IsTypeList
{
private:
    typedef char (&yesTypeList)[1];
    typedef char (&yesNullType)[2];
    typedef char (&no) [3];

    template <typename HeadType, typename TailType> 
	static yesTypeList check(Type2Type<TypeList<HeadType, TailType> >);

    static yesNullType check(Type2Type<NullType>);

    static no check(...);

public:

    enum { value = sizeof(check(Type2Type<T>())) != sizeof(no) };
};

}

template
<
	typename T0 = NullType, typename T1 = NullType, typename T2 = NullType, 
	typename T3 = NullType, typename T4 = NullType, typename T5 = NullType, 
	typename T6 = NullType, typename T7 = NullType, typename T8 = NullType, 
	typename T9 = NullType,	typename T10 = NullType, typename T11 = NullType,
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
struct Make
<
	NullType, NullType, NullType, NullType, NullType, NullType, NullType, 
	NullType, NullType, NullType, NullType, NullType, NullType, NullType, 
	NullType, NullType, NullType, NullType, NullType, NullType, NullType 
>
{
	typedef NullType Type;
};




template <typename TypeListType> 
struct Size
{
private:
    LASS_META_ASSERT_TYPE_LIST(TypeListType);
public:
    enum { value = Size<typename TypeListType::TTail>::value + 1 };
};

template <>
struct Size<NullType>
{
    enum { value = 0 };
};



template <typename TypeListType, unsigned index>
struct At
{
private:
    LASS_META_ASSERT_TYPE_LIST(TypeListType);

	template <unsigned i>
	struct Impl
	{
		typedef At<typename TypeListType::TTail, i - 1>::Type Type;
	};

	template <>
	struct Impl<0>
	{
		typedef typename TypeListType::THead Type;
	};

public:
    typedef typename Impl<index>::Type Type;
};

struct FooType {};

template <typename TypeListType, unsigned index, typename DefaultType = NullType> 
struct AtNonStrict
{
private:
    LASS_META_ASSERT_TYPE_LIST(TypeListType);

	template <typename TList>
	struct Impl
	{
	private:
		LASS_META_ASSERT_TYPE_LIST(TList);

		template <unsigned i>
		struct Detail
		{
			typedef AtNonStrict<typename TList::TTail, i - 1, DefaultType>::Type Type;
		};

		template <>
		struct Detail<0>
		{
			typedef typename TList::THead Type;
		};

	public:
		typedef typename Detail<index>::Type Type;
	};

	template <>
	struct Impl<NullType>
	{
		typedef DefaultType Type;
	};
public:
    typedef typename Impl<TypeListType>::Type Type;
};



template <typename TypeListType, typename T> 
struct Find
{
private:
    LASS_META_ASSERT_TYPE_LIST(TypeListType);

    template <typename TList>
    struct Impl
    {
    private:
		LASS_META_ASSERT_TYPE_LIST(TList);

		template <typename HeadType>
		struct Detail
		{
		private:
			enum { temp = Find<typename TList::TTail, T>::value };
		public:
	        enum { value = temp == -1 ? -1 : temp + 1 };
		};

		template <>
		struct Detail<T>
		{
			enum { value = 0 };
		};

    public:
		enum { value = Detail<typename TList::THead>::value };
    };

    template <>
    struct Impl<NullType>
    {
        enum { value = -1 };
    };

public:

    enum { value = Impl<TypeListType>::value };
};



template <typename TypeListType, typename T> 
struct Merge
{
private:
    LASS_META_ASSERT_TYPE_LIST(TypeListType);

    template <typename TList>
    struct Impl
    {
        typedef TypeList
        <
            typename TList::THead,
            typename Merge<typename TList::TTail, T>::Type
        >
        Type;
    };

    template <>
    struct Impl<NullType>
    {
        typedef typename Select
        <
            impl::IsTypeList<T>::value,
            T,
            TypeList<T, NullType>
        >::Type
        Type;
    };

public:
    typedef typename Impl<TypeListType>::Type Type;
};


template <typename TypeListType, typename T> 
struct Remove
{
private:
    LASS_META_ASSERT_TYPE_LIST(TypeListType);


    template <typename TList>
    struct Impl
    {
    private:
        typedef typename Remove<typename TList::TTail, T>::Type TCleanTail;

    public:
        typedef typename Select
        <
            IsSameType<typename TList::THead, T>::value,
            TCleanTail,
            TypeList<typename TList::THead, TCleanTail>
        >::Type
        Type;
    };

    template <>
    struct Impl<NullType>
    {
        typedef NullType Type;
    };

public:
    typedef typename Impl<TypeListType>::Type Type;
};




template <typename TypeListType, typename OldType, typename NewType> 
struct Replace
{
private:
    LASS_META_ASSERT_TYPE_LIST(TypeListType);

    template <typename TList>
    struct Impl
    {
        typedef TypeList
        <
            typename Select
            <
                IsSameType<typename TList::THead, OldType>::value,
                NewType,
                typename TList::THead
            >::Type,
            typename Replace<typename TList::TTail, OldType, NewType>::Type
        >
        Type;
    };

    template <>
    struct Impl<NullType>
    {
        typedef NullType Type;
    };

public:
    typedef typename Impl<TypeListType>::Type Type;
};



template <typename TypeListType> struct Unique
{
private:
    LASS_META_ASSERT_TYPE_LIST(TypeListType);

    typedef typename Unique<typename TypeListType::TTail>::Type TUniqueTail;
    typedef typename Remove<TUniqueTail, typename TypeListType::THead>::Type TCleanTail;
public:
    typedef TypeList<typename TypeListType::THead, TCleanTail> Type;
};

template <> 
struct Unique<NullType>
{
    typedef NullType Type;
};



template <typename TypeListType>
struct Reverse
{
private:
    LASS_META_ASSERT_TYPE_LIST(TypeListType);

    typedef typename Reverse<typename TypeListType::TTail>::Type TTempHead;

public:
    typedef typename Merge<TTempHead, typename TypeListType::THead>::Type Type;
};

template <>
struct Reverse<NullType>
{
    typedef NullType Type;
};



}

}

}

#endif

// EOF
