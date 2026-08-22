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
 *	Copyright (C) 2022-2026 the Initial Developer.
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

#pragma once

#include "meta_common.h"
#include "null_type.h"

namespace lass
{
namespace meta
{

/** An ordered list of types
 * 
 *  The type tuple does not store any values, it's only a list of types that exists at compile type.
 * 
 *  @tparam T... list of N types
 * 
 *  Namespace type_tuple contains some operations to manipulate the TypeTuple
 *
 *  @par Example:
 *
 *  ```cpp
 *  using TFloats = TypeTuple<float, double, long double>;
 *  constexpr size = type_tuple::Size<TFloats>::value;
 *  using SecondType = type_tuple::At<TFloats, 1>::Type;
 *  using FifthType = type_tuple::AtNonStrict<TFloats, 4, NullType>::Type;
 *  constexpr bool hasInt = type_tuple::Contains<TFloats, int>::value;
 *  constexpr size_t index = type_tuple::Find<TFloats, double>::value;
 *  ```
 */
template <typename... T>
struct TypeTuple
{
	using Type = TypeTuple<T...>; ///< The TypeTuple itself.
};


/** Meta-operations on TypeTuple
 */
namespace type_tuple
{

/** Construct a TypeTuple
 * 
 *  This is equivalent to `TypeTuple<T...>`
 * 
 *  The result is available as `Make::Type` type alias.
 * 
 *  @tparam T... list of N types
 *
 *  @par Example:
 *
 *  ```cpp
 *  using TFloats = Make<float, double, long double>::Type;
 *  ```
 */
template <typename... T> struct Make
{
	using Type = TypeTuple<T...>; ///< The constructed TypeTuple
};



template <typename Ts> struct Size;

/** Evaluate number of types in TypeTuple
 * 
 *  The result is available as the static `Size::value` constant .
 * 
 *  @tparam TypeTuple<T...> TypeTuple with N types
 *
 *  @par Example:
 *
 *  ```cpp
 *  using TFloats = TypeTuple<float, double, long double>;
 *  constexpr size_t size = type_tuple::Size<TFloats>::value;
 *  ```

 */
template <typename... T>
struct Size< TypeTuple<T...> >
{
	static constexpr size_t value = sizeof...(T); ///< Number of types in TypeTuple
};



template <typename Ts, typename X> struct Contains;

/** Check if TypeTyple contains a type
 * 
 *  This evaluates to True if TypeTuple contains type X, False otherwise.
 *  The result is avaiable as the `Contains::Type` type alias.
 * 
 *  @tparam TypeTuple<T...> TypeTuple with N types
 *  @tparam X type to be searched for
 *
 *  @par Example:
 *
 *  ```cpp
 *  using TFloats = TypeTuple<float, double, long double>;
 *  constexpr bool hasInt = type_tuple::Contains<TFloats, int>::value;
 *  ```
 */
template <typename H, typename... T, typename X>
struct Contains<TypeTuple<H, T...>, X>:
	public Contains<TypeTuple<T...>, X>
{
};

template <typename... T, typename X>
struct Contains<TypeTuple<X, T...>, X> : public True {};

template <typename X>
struct Contains<TypeTuple<>, X> : public False {};



template <typename Ts, size_t i> struct At;


/** Extract type from TypeTuple by index
 * 
 *  The result is available as the `At::Type` type alias
 * 
 *  @tparam TypeTuple<T...> TypeTuple with N types
 *  @tparam i index of type to be extract from Ts, with 0 <= `i` < N
 * 
 *  In case `i` > N, then this will fail to compile.
 *
 *  @par Example:
 *
 *  ```cpp
 *  using TFloats = TypeTuple<float, double, long double>;
 *  using SecondType = type_tuple::At<TFloats, 1>::Type;
 *  ```
 */
template <typename H, typename... T, size_t i>
struct At<TypeTuple<H, T...>, i>:
	public At<TypeTuple<T...>, i - 1>
{
};

template <typename H, typename... T>
struct At<TypeTuple<H, T...>, 0>
{
	using Type = H; ///< Alias to the `i`th type in the TypeTuple.
};



template <typename Ts, size_t i, typename Default=NullType> struct AtNonStrict;

/** Extract type from TypeTuple by index, with default type
 * 
 *  The result is available as the `AtNonStrict::Type` type alias
 * 
 *  @tparam TypeTuple<T...> TypeTuple with N types
 *  @tparam i index of type to be extracted from TypeTuple, with 0 <= `i`
 *  @tparam Default default type this will evaluate to if `i` > N
 * 
 *  In case `i` > N, then this evaluates to the `Default` type.
 *
 *  @par Example:
 *
 *  ```cpp
 *  using TFloats = TypeTuple<float, double, long double>;
 *  using FifthType = type_tuple::AtNonStrict<TFloats, 4, NullType>::Type;
 *  ```
 */
template <typename H, typename... T, size_t i, typename Default>
struct AtNonStrict<TypeTuple<H, T...>, i, Default>:
	public AtNonStrict<TypeTuple<T...>, i - 1, Default>
{
};

template <typename H, typename... T, typename Default>
struct AtNonStrict<TypeTuple<H, T...>, 0, Default>
{
	using Type = H;
};

template <size_t i, typename Default>
struct AtNonStrict<TypeTuple<>, i, Default>
{
	using Type = Default;
};



template <typename Ts, typename X> struct Find;

/** Get index of type in TypeTuple
 * 
 *  The result is available as the statkc `Find::Type` constant
 * 
 *  @tparam TypeTuple<T...> TypeTuple with N types
 *  @tparam X type to be searched for
 * 
 *  If `X` is not a type in the TypeTuple, then this fails to compile
 *
 *  @par Example:
 *
 *  ```cpp
 *  using TFloats = TypeTuple<float, double, long double>;
 *  constexpr size_t index = type_tuple::Find<TFloats, double>::value;
 *  ```
 */
template <typename H, typename... T, typename X>
struct Find<TypeTuple<H, T...>, X>
{
	static constexpr int value = Find<TypeTuple<T...>, X>::value + 1; ///< index of X in TypeTuple
};

template <typename... T, typename X>
struct Find<TypeTuple<X, T...>, X>
{
	static constexpr int value = 0;
};

template <typename X>
struct Find<TypeTuple<>, X>
{
	static constexpr int value = 0;
};

}
}
}

// EOF
