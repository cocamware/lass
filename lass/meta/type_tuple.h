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
 *	Copyright (C) 2022 the Initial Developer.
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

template <typename... T>
struct TypeTuple
{
	using Type = TypeTuple<T...>;
};


namespace type_tuple
{

template <typename... T> struct Make
{
	using Type = TypeTuple<T...>;
};



template <typename Ts> struct Size;

template <typename... T>
struct Size< TypeTuple<T...> >
{
	static constexpr size_t value = sizeof...(T);
};



template <typename Ts, typename X> struct Contains;

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

template <typename H, typename... T, size_t i>
struct At<TypeTuple<H, T...>, i>:
	public At<TypeTuple<T...>, i - 1>
{
};

template <typename H, typename... T>
struct At<TypeTuple<H, T...>, 0>
{
	using Type = H;
};



template <typename Ts, size_t i, typename Default=NullType> struct AtNonStrict;

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

template <typename H, typename... T, typename X>
struct Find<TypeTuple<H, T...>, X>
{
	static constexpr int value = Find<TypeTuple<T...>, X>::value + 1;
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
