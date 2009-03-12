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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_SHADOWEE_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_SHADOWEE_TRAITS_H

#include "python_common.h"
#include "../meta/bool.h"

namespace lass
{
namespace python
{

/** @ingroup Python
 *  @internal
 *  Tells whether a type has a shadow class, and its type TShadow
 *  By default, this is false.  Must be specialized by shadow classes ...
 *
 *  ShadoweeTraits shall be (partially) specialized for all shadwowee types.
 *	It shall derive from meta::True and define the following types:
 *	@arg TShadow: the shadow type (obvious)
 *	@arg TPointerTraits: ...
 *
 *  ShadoweeTraits for non-shadowee types shall derive from meta::False and
 *	won't define these types.
 */
template <typename T> 
struct ShadoweeTraits: public meta::False 
{
	typedef T TShadow;
	// typedef ... TShadowTraits;
	// typedef ... TPointerTraits;
};

namespace impl
{

template <typename T, bool isShadowee>
struct ConstShadoweeTraits: ShadoweeTraits<T>
{
	typedef typename ShadoweeTraits<T>::TShadow TShadow;
	//typedef typename ShadoweeTraits<T>::TShadowTraits TShadowTraits;
	typedef typename ShadoweeTraits<T>::TPointerTraits::template Rebind<const T>::Type TPointerTraits;
};
template <typename T>
struct ConstShadoweeTraits<T, false>: public meta::False
{
};

}

/** @ingroup Python
 *  @internal
 *  const types have the same shadowee traits as non const types ...
 */
template <typename T> 
struct ShadoweeTraits<const T>: public impl::ConstShadoweeTraits<T, ShadoweeTraits<T>::value>
{
};

}
}

#endif

