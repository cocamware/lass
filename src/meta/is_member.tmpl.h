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

$[
template <typename R, typename C, $(typename P$x)$ > struct IsMember<R (C::*) ( $(P$x)$ )> : meta::True {};
template <typename R, typename C, $(typename P$x)$ > struct IsMember<R (C::*) ( $(P$x)$ ) const > : meta::True {};
template <typename R, typename C, $(typename P$x)$ > struct IsConstMember<R (C::*) ( $(P$x)$ ) const > : meta::True {};
]$

// following _functions_ work on instances

template <typename R, typename C > inline bool isMember(R (C::*iFunction)() ) { return true; }
template <typename R, typename C > inline bool isMember(R (C::*iFunction)() const ) { return true; }
template <typename R, typename C > inline bool isConstMember(R (C::*iFunction)()  ) { return false; }
template <typename R, typename C > inline bool isConstMember(R (C::*iFunction)() const ) { return true; }


$[
	template <typename R, typename C, $(typename P$x)$ > inline bool isMember(R (C::*iFunction)( $(P$x)$ ) ) { return true; }
	template <typename R, typename C, $(typename P$x)$ > inline bool isMember(R (C::*iFunction)( $(P$x)$ ) const ) { return true; }
	template <typename R, typename C, $(typename P$x)$ > inline bool isConstMember(R (C::*iFunction)( $(P$x)$ ) ) { return false; }
	template <typename R, typename C, $(typename P$x)$ > inline bool isConstMember(R (C::*iFunction)( $(P$x)$ ) const ) { return true; }
]$


}
}

#endif

// EOF
