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



template <typename Ts, typename X> struct PushBack;

template <typename H, typename Ts, typename X>
struct PushBack<TypeList<H, Ts>, X>
{
	typedef TypeList<H, typename PushBack<Ts, X>::Type> Type;
};

template <typename X>
struct PushBack<NullType, X>
{
	typedef TypeList<X, NullType> Type;
};



template <typename Ts, typename Tail = NullType> struct Reverse;

template <typename H, typename Ts, typename Tail>
struct Reverse<TypeList<H, Ts>, Tail>: public Reverse< Ts, TypeList<H, Tail> > {};

template <typename Tail>
struct Reverse<NullType, Tail>
{
	typedef Tail Type;
};


}
}
}

#endif

// EOF
