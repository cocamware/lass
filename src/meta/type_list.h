/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



/** @class lass::meta::TypeList
 *  @brief meta list of types
 *  @author Bram de Greve [BdG]
 *
 *  @section members meta member functions
 *
 *  All @e member functions are encapsulated in the namespace type_list
 *
 *  @par template <typename T1 = NullType, ..., typename T20 = NullType> struct Make
 *  makes a TypeList in a convenient way.  A common mistake is to forget the @c ::Type at the
 *  end.  It's the @c ::Type that resolves to the actual typelist.  If you forget this one,
 *  you end up with a @c typedef to the @c Make function instead of the resulting TypeList.
 *  Anyway, if you need more than 21 types straight in one TypeList, feel free to exand the
 *  template parameters list =)
 *
 *  @code
 *  typedef type_list::Make<float, double, long double>::Type TFloats;
 *  typedef type_list::Make<signed char, short, int, long>::Type TSignedIntegers;
 *  @endcode
 *
 *  @par template <typename TypeListType> struct Size
 *  Returns the length of @a TypeListType, i.e. the number of types that is stored in the list.
 *
 *  @code
 *  const int numberOfTypes = type_list::Size<TFloats>::value;
 *  @endcode
 *
 *  @par template <typename TypeListType, unsigned index> struct At
 *  Access a type in @a TypeListType by its position @a index.  The first type in the list has
 *  index 0 and the last one has index @c type_list::Size<TypeListType>::value-1.  If you pass
 *  an out of range index, you'll get a compiler error.
 *
 *  @code
 *  typedef type_list::At<TFloats, 1>::Type TFloat1; // double.
 *  @endcode
 *
 *  @par template <typename TypeListType, unsigned index, typename DefaultType = NullType> struct AtNonStrict
 *  same as type_list::At but will return a @a DefaultType instead of a compiler error if
 *  the index is out of range.  By default this @a DefaultType is the NullType.
 *
 *  @code
 *  typedef type_list::AtNonStrict<TFloats, 4>::Type TFloat4; // NullType
 *  typedef type_list::AtNonStrict<TFloats, 5, float>::Type TFloat5; // float
 *  @endcode
 *
 *  @par template <typename TypeListType, typename T> struct Find
 *  returns the position of the first occurence of type @a T in @a TypeListType, and -1 if @a T is
 *  not found.
 *
 *  @code
 *  const int indexOfDouble = type_list::Find<TFloats, double>::value;
 *  @endcode
 *
 *  @par template <typename A, typename B> struct Merge
 *  merges two type lists @a A and @a B in one, or appends a type @a B to an existing TypeList @a A.
 *
 *  @code
 *  typedef type_list::Merge<TSignedIntegers, signed __int64>::Type TAllSignedIntegers;
 *  typedef type_list::Merge<TFloats, TAllSignedIntegers>::Type TAllSignedTypes;
 *  @endcode
 *
 *  @par template <typename TypeListType, typename T> struct Remove
 *  removes all occurences of @a T from a @a TypeListType
 *
 *  @code
 *  typedef type_list::Remove<TFloats, long double>::Type TCommonFloats;
 *  @endcode
 *
 *  @par template <typename TypeListType, typename OldType, typename NewType> struct Replace
 *  replaces in @a TypeListType all occurences of @a OldType by @a NewType.
 *
 *  @code
 *  typedef type_list::Replace<TAllSignedIntegers, signed __int64, long>::Type TLongIs64;
 *  @endcode
 *
 *  @par template <typename TypeListType> struct Unique
 *  removes all duplicate types from @a TypeListType.  Of course, it leaves exactly one occurence
 *  of duplicate types in the TypeList.
 *
 *  @code
 *  typedef type_list::Unique<TAllSignedIntegers>::Type TUniqueSignedIntegers;
 *  @endcode
 *
 *  @par template <typename TypeListType> struct Reverse
 *  reverses the order of the types in @a TypeListType
 *
 *  @code
 *  typedef type_list::Reverse<TFloats>::Type TReverseFloats;
 *  @endcode
 *
 *  @section legal legal notes
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_TYPE_LIST_H
#define LASS_GUARDIAN_OF_INCLUSION_META_TYPE_LIST_H

#include "meta_common.h"
#include "null_type.h"

namespace lass
{
namespace meta
{

template
<
	typename HeadType,
	typename TailType
>
struct TypeList
{
	typedef HeadType THead;
	typedef TailType TTail;
};

}

}

#define LASS_TYPE_LIST_1(T1) lass::meta::TypeList< T1 ,  lass::meta::NullType >

#define LASS_TYPE_LIST_2(T1, T2) lass::meta::TypeList<T1, LASS_TYPE_LIST_1(T2) >

#define LASS_TYPE_LIST_3(T1, T2, T3) lass::meta::TypeList<T1, LASS_TYPE_LIST_2(T2, T3) >

#define LASS_TYPE_LIST_4(T1, T2, T3, T4) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_3(T2, T3, T4) >

#define LASS_TYPE_LIST_5(T1, T2, T3, T4, T5) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_4(T2, T3, T4, T5) >

#define LASS_TYPE_LIST_6(T1, T2, T3, T4, T5, T6) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_5(T2, T3, T4, T5, T6) >

#define LASS_TYPE_LIST_7(T1, T2, T3, T4, T5, T6, T7) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_6(T2, T3, T4, T5, T6, T7) >

#define LASS_TYPE_LIST_8(T1, T2, T3, T4, T5, T6, T7, T8) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_7(T2, T3, T4, T5, T6, T7, T8) >

#define LASS_TYPE_LIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >

#define LASS_TYPE_LIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >

#define LASS_TYPE_LIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >

#define LASS_TYPE_LIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12) >

#define LASS_TYPE_LIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13) >

#define LASS_TYPE_LIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14) >

#define LASS_TYPE_LIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15) >

#define LASS_TYPE_LIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16) >

#define LASS_TYPE_LIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17) >

#define LASS_TYPE_LIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18) >

#define LASS_TYPE_LIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19) >

#define LASS_TYPE_LIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) >

#define LASS_TYPE_LIST_21(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_20(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) >

#define LASS_TYPE_LIST_22(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_21(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) >

#define LASS_TYPE_LIST_23(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_22(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) >

#define LASS_TYPE_LIST_24(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_23(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) >

#define LASS_TYPE_LIST_25(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_24(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25) >

#define LASS_TYPE_LIST_26(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_25(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26) >

#define LASS_TYPE_LIST_27(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_26(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27) >

#define LASS_TYPE_LIST_28(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_27(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28) >

#define LASS_TYPE_LIST_29(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_28(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29) >

#define LASS_TYPE_LIST_30(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_29(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) >

#define LASS_TYPE_LIST_31(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_30(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31) >

#define LASS_TYPE_LIST_32(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_31(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32) >

#define LASS_TYPE_LIST_33(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_32(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33) >

#define LASS_TYPE_LIST_34(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33, T34) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_33(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33, T34) >

#define LASS_TYPE_LIST_35(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_34(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35) >

#define LASS_TYPE_LIST_36(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_35(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36) >

#define LASS_TYPE_LIST_37(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_36(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37) >

#define LASS_TYPE_LIST_38(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_37(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38) >

#define LASS_TYPE_LIST_39(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_38(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39) >

#define LASS_TYPE_LIST_40(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_39(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40) >

#define LASS_TYPE_LIST_41(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_40(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41) >

#define LASS_TYPE_LIST_42(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_41(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42) >

#define LASS_TYPE_LIST_43(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_42(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43) >

#define LASS_TYPE_LIST_44(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43, T44) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_43(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43, T44) >

#define LASS_TYPE_LIST_45(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_44(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45) >

#define LASS_TYPE_LIST_46(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45, T46) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_45(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45, T46) >

#define LASS_TYPE_LIST_47(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45, T46, T47) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_46(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45, T46, T47) >

#define LASS_TYPE_LIST_48(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45, T46, T47, T48) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_47(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45, T46, T47, T48) >

#define LASS_TYPE_LIST_49(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45, T46, T47, T48, T49) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_48(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45, T46, T47, T48, T49) >

#define LASS_TYPE_LIST_50(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45, T46, T47, T48, T49, T50) \
	lass::meta::TypeList<T1, LASS_TYPE_LIST_49(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
		T41, T42, T43, T44, T45, T46, T47, T48, T49, T50) >

#ifdef LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#	include "type_list_no_partial.inl"
#else
#	include "type_list.inl"
#endif

#endif

// EOF
