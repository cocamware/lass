/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_IS_MEMBER_H
#define LASS_GUARDIAN_OF_INCLUSION_META_IS_MEMBER_H

#include "meta_common.h"
#include "bool.h"
#include "type_list.h"

namespace lass
{
namespace meta
{

// following _templates_ work on types

template <typename R> struct IsMember : meta::False {};
template <typename R> struct IsConstMember : meta::False {};

template <typename R, typename C > struct IsMember<R (C::*) ()> : meta::True {};
template <typename R, typename C > struct IsMember<R (C::*) () const > : meta::True {};
template <typename R, typename C > struct IsConstMember<R (C::*) () const > : meta::True {};


template <typename R, typename C, typename P1 > struct IsMember<R (C::*) ( P1 )> : meta::True {};
template <typename R, typename C, typename P1 > struct IsMember<R (C::*) ( P1 ) const > : meta::True {};
template <typename R, typename C, typename P1 > struct IsConstMember<R (C::*) ( P1 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2 > struct IsMember<R (C::*) ( P1, P2 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2 > struct IsMember<R (C::*) ( P1, P2 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2 > struct IsConstMember<R (C::*) ( P1, P2 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3 > struct IsMember<R (C::*) ( P1, P2, P3 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3 > struct IsMember<R (C::*) ( P1, P2, P3 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3 > struct IsConstMember<R (C::*) ( P1, P2, P3 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4 > struct IsMember<R (C::*) ( P1, P2, P3, P4 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4 > struct IsMember<R (C::*) ( P1, P2, P3, P4 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5, P6 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14 ) const > : meta::True {};

template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15 )> : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15 > struct IsMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15 ) const > : meta::True {};
template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15 > struct IsConstMember<R (C::*) ( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15 ) const > : meta::True {};


// following _functions_ work on instances

template <typename R, typename C > inline bool isMember(R (C::*iFunction)() ) { return true; }
template <typename R, typename C > inline bool isMember(R (C::*iFunction)() const ) { return true; }
template <typename R, typename C > inline bool isConstMember(R (C::*iFunction)()  ) { return false; }
template <typename R, typename C > inline bool isConstMember(R (C::*iFunction)() const ) { return true; }



	template <typename R, typename C, typename P1 > inline bool isMember(R (C::*iFunction)( P1 ) ) { return true; }
	template <typename R, typename C, typename P1 > inline bool isMember(R (C::*iFunction)( P1 ) const ) { return true; }
	template <typename R, typename C, typename P1 > inline bool isConstMember(R (C::*iFunction)( P1 ) ) { return false; }
	template <typename R, typename C, typename P1 > inline bool isConstMember(R (C::*iFunction)( P1 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2 > inline bool isMember(R (C::*iFunction)( P1, P2 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2 > inline bool isMember(R (C::*iFunction)( P1, P2 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2 > inline bool isConstMember(R (C::*iFunction)( P1, P2 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2 > inline bool isConstMember(R (C::*iFunction)( P1, P2 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3 > inline bool isMember(R (C::*iFunction)( P1, P2, P3 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3 > inline bool isMember(R (C::*iFunction)( P1, P2, P3 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14 ) const ) { return true; }

	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15 ) ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15 > inline bool isMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15 ) const ) { return true; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15 ) ) { return false; }
	template <typename R, typename C, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15 > inline bool isConstMember(R (C::*iFunction)( P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15 ) const ) { return true; }



}
}

#endif

// EOF
